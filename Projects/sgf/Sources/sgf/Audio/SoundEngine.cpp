/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 사운드 엔진 구현부 (XAudio2)
 *
 * [전체 흐름]
 * Initialize: XAudio2 엔진 + 마스터보이스 생성
 * Play2d: WAV 로딩(캐시) -> 빈 슬롯에 SourceVoice 생성 -> 버퍼 제출 -> 재생
 * Update: 매 프레임 끝난 재생기 회수 (Application이 호출)
 * Finalize: 모두 정지 후 역순 해제
 */

#include "Core.h"
#include "sgf/Audio/SoundEngine.h"
#include "jc/Math.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
SoundEngine::SoundEngine()
	: pXAudio_(nullptr)
	, pMasterVoice_(nullptr)
	, initialized_(false)
	, comInitialized_(false)
	, nextAudioId_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SoundEngine::~SoundEngine()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// XAudio2 엔진을 켬다.
// [생성 순서] COM 초기화 -> 엔진(IXAudio2) -> 마스터보이스(스피커 출구)
bool SoundEngine::Initialize()
{
	if (initialized_)
	{
		return true;
	}

	// XAudio2는 COM 기반이므로 COM을 먼저 초기화한다.
	// S_FALSE = 이미 초기화됨(이 경우 CoUninitialize를 부르면 안 됨),
	// RPC_E_CHANGED_MODE = 다른 모드로 초기화됨. 둘 다 계속 진행 가능.
	const HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (hrCom == S_OK)
	{
		comInitialized_ = true;	// 이번 호출이 초기화했으므로 Finalize에서 정리한다.
	}
	else if (FAILED(hrCom) && hrCom != RPC_E_CHANGED_MODE)
	{
		return false;
	}

	if (FAILED(XAudio2Create(&pXAudio_, 0, XAUDIO2_DEFAULT_PROCESSOR)))
	{
		pXAudio_ = nullptr;
		ShutdownCom();
		return false;
	}

	if (FAILED(pXAudio_->CreateMasteringVoice(&pMasterVoice_)))
	{
		pXAudio_->Release();
		pXAudio_ = nullptr;
		pMasterVoice_ = nullptr;
		ShutdownCom();
		return false;
	}

	initialized_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 이번 인스턴스가 초기화한 COM만 정리한다. (S_FALSE로 넘어온 경우 책임 없음)
void SoundEngine::ShutdownCom()
{
	if (comInitialized_)
	{
		CoUninitialize();
		comInitialized_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모든 재생 정지 -> 캐시 비우기 -> 마스터보이스 -> 엔진 순서로 해제한다.
// (생성의 역순. SourceVoice가 살아있는 채로 엔진을 부수면 크래시한다)
void SoundEngine::Finalize()
{
	if (!initialized_)
	{
		return;
	}
	initialized_ = false;

	StopAll();
	UncacheAll();

	if (pMasterVoice_ != nullptr)
	{
		pMasterVoice_->DestroyVoice();
		pMasterVoice_ = nullptr;
	}
	if (pXAudio_ != nullptr)
	{
		pXAudio_->Release();
		pXAudio_ = nullptr;
	}
	ShutdownCom();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 호출: 재생이 끝난 슬롯을 회수한다.
// [왜 필요한가?] SourceVoice는 재생이 끝나도 스스로 사라지지 않는다.
// 방치하면 32개 슬롯이 전부 \"다 끝난 소리\"로 가득 차서 새 소리를 못 틀게 된다.
void SoundEngine::Update()
{
	if (!initialized_)
	{
		return;
	}

	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		VoiceSlot& slot = voices_[i];
		if (slot.pVoice == nullptr || slot.paused_)
		{
			continue;	// 빈 슬롯/일시정지 중은 건너맄 (일시정지는 끝난 게 아니다)
		}

		XAUDIO2_VOICE_STATE state;
		slot.pVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
		if (state.BuffersQueued == 0)
		{
			releaseSlot(slot);	// 대기 중인 버퍼가 없다 = 재생 완료
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 빈 슬롯 찾기. 꽉 차있으면 nullptr. (호출 전 Update로 회수를 마친 상태라고 가정)
SoundEngine::VoiceSlot* SoundEngine::findFreeSlot()
{
	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		if (voices_[i].pVoice == nullptr)
		{
			return &voices_[i];
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// audioId로 재생 중인 슬롯을 찾는다. 이미 끝나 회수된 소리면 nullptr.
SoundEngine::VoiceSlot* SoundEngine::findSlot(_s32 _audioId)
{
	if (_audioId == INVALID_AUDIO_ID)
	{
		return nullptr;
	}
	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		if (voices_[i].pVoice != nullptr && voices_[i].audioId_ == _audioId)
		{
			return &voices_[i];
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 슬롯을 완전히 비운다. 재생기 파괴 + PlayTone이 만든 임시 데이터 회수.
void SoundEngine::releaseSlot(VoiceSlot& _slot)
{
	if (_slot.pVoice != nullptr)
	{
		_slot.pVoice->Stop(0);
		_slot.pVoice->FlushSourceBuffers();
		_slot.pVoice->DestroyVoice();
		_slot.pVoice = nullptr;
	}
	if (_slot.pOwnedTone != nullptr)
	{
		delete _slot.pOwnedTone;	// PlayTone 전용 데이터 (캐시와 무관)
		_slot.pOwnedTone = nullptr;
	}
	_slot.audioId_ = INVALID_AUDIO_ID;
	_slot.paused_ = false;
	_slot.loop_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// WAV 파일을 읽어 캐시에서 찾거나 새로 올린다.
// [WAV 파일 구조 = RIFF 컸테이너]
// 파일 맨 앞에 RIFF/WAVE 표시가 있고, 그 뒤로 (청크 ID 4바이트 + 크기 4바이트 + 본문)이
// 반복된다. 우리에게 필요한 것은 fmt 청크(파형 설명서)와 data 청크(실제 소리)뿐이다.
// [파일 IO는 jc 라이브러리 사용 - std:: 금지 규칙]
SoundEngine::WavData* SoundEngine::loadWav(const jc::String& _path)
{
	// 1. 캐시 확인: 같은 파일을 두 번 읽지 않는다.
	WavData** ppCached = cache_.Find(_path);
	if (ppCached != nullptr)
	{
		return *ppCached;
	}

	// 2. 파일 존재/크기 확인 (jc::File)
	if (!jc::File::Exist(_path))
	{
		jc::Console::WriteLine("[sgf] SoundEngine: WAV file not found");
		return nullptr;
	}
	const _s64 fileSize = jc::File::Size(_path);
	if (fileSize < 44) // RIFF 헤더(12) + fmt(8+16) + data 헤더(8) 최소 크기
	{
		return nullptr;
	}

	// 3. 파일 전체를 메모리로 읽는다. (jc::FileStream)
	jc::Vector<_u8> bytes;
	bytes.Resize((_s32)fileSize);
	jc::FileStream stream(_path, jc::FileAccess::eRead, jc::FileMode::eOpen);
	stream.Read(bytes.Source(), 0, (_s32)fileSize);
	stream.Close();

	// 4. RIFF/WAVE 식별자 검사
	const _u8* pData = bytes.Source();
	if (memcmp(pData, "RIFF", 4) != 0 || memcmp(pData + 8, "WAVE", 4) != 0)
	{
		jc::Console::WriteLine("[sgf] SoundEngine: not a WAV(RIFF) file");
		return nullptr;
	}

	WavData* pWav = new WavData();
	memset(&pWav->format_, 0, sizeof(WAVEFORMATEX));
	bool hasFmt = false;
	bool hasData = false;

	// 5. 청크들을 순회하며 fmt / data를 찾는다.
	_s64 offset = 12;
	while (offset + 8 <= fileSize)
	{
		_u8 chunkId[4];
		memcpy(chunkId, pData + offset, 4);
		_u32 chunkSize = 0;
		memcpy(&chunkSize, pData + offset + 4, 4);	// 크기는 리틀엔디안 4바이트

		const _s64 body = offset + 8;
		if (body + (_s64)chunkSize > fileSize)
		{
			break;	// 손상된 파일 보호
		}

		if (memcmp(chunkId, "fmt ", 4) == 0)
		{
			// 파형 설명서: 샘플레이트/채널 수/비트 수 등. WAVEFORMATEX보다 짧을 수 있어 크기를 맞춰 복사.
			const _u32 copySize = (chunkSize < (_u32)sizeof(WAVEFORMATEX)) ? chunkSize : (_u32)sizeof(WAVEFORMATEX);
			memcpy(&pWav->format_, pData + body, copySize);
			pWav->format_.cbSize = 0;
			hasFmt = true;
		}
		else if (memcmp(chunkId, "data", 4) == 0)
		{
			// 실제 소리 데이터(PCM 샘플)
			// u32 -> s32 변환 시 2GB 이상 청크가 음수로 Overflow 되는 것을 방지한다.
			const _s64 sampleSize = (_s64)chunkSize;
			if (sampleSize > (_s64)INT_MAX)
			{
				jc::Console::WriteLine("[sgf] SoundEngine: WAV data chunk too large");
				delete pWav;
				return nullptr;
			}
			pWav->samples.Resize((_s32)sampleSize);
			memcpy(pWav->samples.Source(), pData + body, chunkSize);
			hasData = true;
		}

		offset = body + chunkSize + (chunkSize & 1);	// 홀수 크기 청크는 1바이트 패딩
	}

	// 6. 필수 청크 검사 + 비압축 PCM만 지원 (압축 포맷은 튜토리얼 범위 밖)
	if (!hasFmt || !hasData || pWav->format_.wFormatTag != WAVE_FORMAT_PCM)
	{
		jc::Console::WriteLine("[sgf] SoundEngine: unsupported WAV format (PCM only)");
		delete pWav;
		return nullptr;
	}

	// 7. 캐시 등록
	cache_.Insert(_path, pWav);
	cacheKeys_.PushBack(_path);
	return pWav;
}

//////////////////////////////////////////////////////////////////////////////////////////
// WAV를 미리 메모리에 올려둔다. 첫 재생 순간의 디스크 읽기 덩퀌임 방지.
bool SoundEngine::Preload(const jc::String& _path)
{
	if (!initialized_)
	{
		return false;
	}
	return loadWav(_path) != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 특정 파일을 캐시에서 내린다.
// 주의: 그 데이터로 재생 중인 소리가 있을 수 있으므로 먼저 모두 멈춘다.
void SoundEngine::Uncache(const jc::String& _path)
{
	WavData** ppCached = cache_.Find(_path);
	if (ppCached == nullptr)
	{
		return;
	}

	StopAll();	// 샘플 메모리를 참조 중인 재생기가 없도록 안전하게 정지

	delete *ppCached;
	cache_.Remove(_path);

	// 키 목록에서도 제거 (마지막 원소를 리거 자리로 당겨오고 길이를 1 줄인다)
	for (_s32 i = 0; i < cacheKeys_.Size(); ++i)
	{
		if (cacheKeys_[i] == _path)
		{
			cacheKeys_[i] = cacheKeys_[cacheKeys_.Size() - 1];
			cacheKeys_.Resize(cacheKeys_.Size() - 1);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 캐시 전체를 비운다. (Finalize에서 호출)
void SoundEngine::UncacheAll()
{
	StopAll();

	for (_s32 i = 0; i < cacheKeys_.Size(); ++i)
	{
		WavData** ppCached = cache_.Find(cacheKeys_[i]);
		if (ppCached != nullptr)
		{
			delete *ppCached;
			cache_.Remove(cacheKeys_[i]);
		}
	}
	cacheKeys_.Resize(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// WAV 파일 재생. Cocos2d-x의 AudioEngine::play2d 대응.
// [흐름] WAV 로딩(캐시) -> 빈 슬롯 확보 -> 재생기 생성 -> 버퍼 제출 -> 재생 시작
_s32 SoundEngine::Play2d(const jc::String& _path, bool _loop, _f32 _volume)
{
	if (!initialized_)
	{
		return INVALID_AUDIO_ID;
	}

	WavData* pWav = loadWav(_path);
	if (pWav == nullptr)
	{
		return INVALID_AUDIO_ID;
	}

	Update();	// 끝난 슬롯을 먼저 회수해 자리를 확보한다.
	VoiceSlot* pSlot = findFreeSlot();
	if (pSlot == nullptr)
	{
		return INVALID_AUDIO_ID;	// 32개 동시 재생 초과
	}

	// 이 WAV의 포맷에 맞는 재생기(SourceVoice)를 만든다.
	if (FAILED(pXAudio_->CreateSourceVoice(&pSlot->pVoice, &pWav->format_)))
	{
		pSlot->pVoice = nullptr;
		return INVALID_AUDIO_ID;
	}

	// 재생할 데이터를 제출한다. (복사가 아니라 포인터 참조 - 그래서 캐시가 살아있어야 한다)
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = (UINT32)pWav->samples.Size();
	buffer.pAudioData = pWav->samples.Source();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = _loop ? XAUDIO2_LOOP_INFINITE : 0;

	if (FAILED(pSlot->pVoice->SubmitSourceBuffer(&buffer)))
	{
		pSlot->pVoice->DestroyVoice();
		pSlot->pVoice = nullptr;
		return INVALID_AUDIO_ID;
	}

	pSlot->pVoice->SetVolume(_volume);
	pSlot->pVoice->Start(0);

	pSlot->audioId_ = nextAudioId_++;
	pSlot->paused_ = false;
	pSlot->loop_ = _loop;
	pSlot->pOwnedTone = nullptr;
	return pSlot->audioId_;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 사인파 톤 재생: 파일 없이 코드로 소리를 만든다. (튜토리얼 효과음용)
// [원리] 소리 = 공기의 떨림. 사인 함수로 1초에 _frequency번 떨리는 파형을 숫자로 찍어낸다.
// 44100Hz = 1초를 44100칸으로 쪼개 기록한다는 뜻 (CD 음질).
_s32 SoundEngine::PlayTone(_s32 _frequency, _s32 _milliseconds, _f32 _volume)
{
	if (!initialized_ || _frequency <= 0 || _milliseconds <= 0)
	{
		return INVALID_AUDIO_ID;
	}

	Update();
	VoiceSlot* pSlot = findFreeSlot();
	if (pSlot == nullptr)
	{
		return INVALID_AUDIO_ID;
	}

	constexpr _s32 SAMPLE_RATE = 44100;	// 1초당 샘플 수
	const _s32 sampleCount = SAMPLE_RATE * _milliseconds / 1000;

	// 톤 전용 WavData를 만든다. (캐시와 무관, 슬롯이 소유하고 끝나면 삭제)
	WavData* pTone = new WavData();
	memset(&pTone->format_, 0, sizeof(WAVEFORMATEX));
	pTone->format_.wFormatTag = WAVE_FORMAT_PCM;
	pTone->format_.nChannels = 1;							// 모노
	pTone->format_.nSamplesPerSec = SAMPLE_RATE;
	pTone->format_.wBitsPerSample = 16;						// 샘플 하나 = 16비트 정수
	pTone->format_.nBlockAlign = 2;							// 채널 수 * 바이트 수
	pTone->format_.nAvgBytesPerSec = SAMPLE_RATE * 2;

	pTone->samples.Resize(sampleCount * 2);	// 16비트 = 샘플당 2바이트
	_s16* pSamples = (_s16*)pTone->samples.Source();

	// 5ms 페이드 인/아웃: 파형이 갑자기 시작/종료되면 뚝 하는 잡음(클릭 노이즈)이 난다.
	// 아주 짧은 톤(페이드 2개가 겹칠 정도)은 페이드 기간을 절반으로 줄여 겹침(음수 배율)을 막는다.
	_f32 fadeSamples = (_f32)(SAMPLE_RATE * 5) / 1000.0f;
	if (fadeSamples * 2.0f > (_f32)sampleCount)
	{
		fadeSamples = (_f32)sampleCount * 0.5f;
	}

	for (_s32 i = 0; i < sampleCount; ++i)
	{
		const _f32 t = (_f32)i / (_f32)SAMPLE_RATE;
		_f32 amplitude = sinf(jc_math_pi2 * (_f32)_frequency * t);

		if (i < (_s32)fadeSamples)
		{
			amplitude *= (_f32)i / fadeSamples;							// 서서히 커짐
		}
		if (i >= sampleCount - (_s32)fadeSamples)
		{
			amplitude *= (_f32)(sampleCount - i) / fadeSamples;			// 서서히 작아짐
		}

		pSamples[i] = (_s16)(amplitude * 32000.0f);	// 16비트 범위(+-32767)로 확대
	}

	if (FAILED(pXAudio_->CreateSourceVoice(&pSlot->pVoice, &pTone->format_)))
	{
		pSlot->pVoice = nullptr;
		delete pTone;
		return INVALID_AUDIO_ID;
	}

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = (UINT32)pTone->samples.Size();
	buffer.pAudioData = pTone->samples.Source();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (FAILED(pSlot->pVoice->SubmitSourceBuffer(&buffer)))
	{
		pSlot->pVoice->DestroyVoice();
		pSlot->pVoice = nullptr;
		delete pTone;
		return INVALID_AUDIO_ID;
	}

	pSlot->pVoice->SetVolume(_volume);
	pSlot->pVoice->Start(0);

	pSlot->audioId_ = nextAudioId_++;
	pSlot->paused_ = false;
	pSlot->loop_ = false;
	pSlot->pOwnedTone = pTone;	// 슬롯이 소유 -> releaseSlot에서 delete
	return pSlot->audioId_;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 정지 / 일시정지 / 재개 / 볼륨 / 상태 조회
void SoundEngine::Stop(_s32 _audioId)
{
	VoiceSlot* pSlot = findSlot(_audioId);
	if (pSlot != nullptr)
	{
		releaseSlot(*pSlot);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::StopAll()
{
	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		if (voices_[i].pVoice != nullptr)
		{
			releaseSlot(voices_[i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::Pause(_s32 _audioId)
{
	VoiceSlot* pSlot = findSlot(_audioId);
	if (pSlot != nullptr && !pSlot->paused_)
	{
		pSlot->pVoice->Stop(0);	// Stop(0)은 파괴가 아니라 \"멈춤\" (버퍼는 그대로 남는다)
		pSlot->paused_ = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::PauseAll()
{
	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		if (voices_[i].pVoice != nullptr && !voices_[i].paused_)
		{
			voices_[i].pVoice->Stop(0);
			voices_[i].paused_ = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::Resume(_s32 _audioId)
{
	VoiceSlot* pSlot = findSlot(_audioId);
	if (pSlot != nullptr && pSlot->paused_)
	{
		pSlot->pVoice->Start(0);	// 멈췄던 지점부터 이어서 재생
		pSlot->paused_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::ResumeAll()
{
	for (_s32 i = 0; i < MAX_VOICES; ++i)
	{
		if (voices_[i].pVoice != nullptr && voices_[i].paused_)
		{
			voices_[i].pVoice->Start(0);
			voices_[i].paused_ = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SoundEngine::SetVolume(_s32 _audioId, _f32 _volume)
{
	VoiceSlot* pSlot = findSlot(_audioId);
	if (pSlot != nullptr)
	{
		pSlot->pVoice->SetVolume(_volume);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SoundEngine::IsPlaying(_s32 _audioId)
{
	VoiceSlot* pSlot = findSlot(_audioId);
	return pSlot != nullptr && !pSlot->paused_;
}

NS_SGF_END
