/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 사운드 엔진 (Cocos2d-x AudioEngine 구조 참고, XAudio2 기반)
 *
 * [구조 - Cocos2d-x AudioEngine과의 대응]
 *  cocos2d::AudioEngine::play2d(path, loop, volume) -> g_cSound.Play2d(path, loop, volume)
 *  재생마다 고유 번호(audioId)를 돌려주고, 그 번호로 정지/일시정지/볼륨을 제어한다.
 *
 * [XAudio2란?]
 *  Windows의 저수준 오디오 API. 구성은 세 층이다.
 *  1. IXAudio2            : 엔진 본체 (믹서 스레드를 내부에서 돌린다)
 *  2. MasteringVoice      : 최종 출력(스피커)으로 나가는 관문
 *  3. SourceVoice         : 사운드 하나를 재생하는 재생기 (동시에 여러 개 가능)
 *  PCM 데이터(압축 안 된 파형 숫자 배열)를 SourceVoice에 제출(Submit)하면 소리가 난다.
 *
 * [파일 IO는 jc 라이브러리 사용]
 *  WAV 파일 로딩은 jc::FileStream으로 읽는다. (std:: 사용 금지 규칙)
 */

#pragma once

#include <xaudio2.h>

#include "jc/Primitives/String.h"
#include "jc/Container/Vector.h"
#include "jc/Container/HashMap.h"
#include "jc/Pattern/Singleton.h"
#include "sgf/Namespace.h"

NS_SGF_BEGIN

class SoundEngine : public jc::SingletonStatic<SoundEngine>
{
	friend class jc::SingletonStatic<SoundEngine>;

private:
	SoundEngine();
	~SoundEngine();

public:
	static constexpr _s32 INVALID_AUDIO_ID = -1;	// 재생 실패 시 반환되는 번호
	static constexpr _s32 MAX_VOICES = 32;		// 동시 재생 가능한 소리 수

public:
	// 사운드 엔진을 켬다. 실패해도 게임은 계속 돌 수 있게 설계했다. (소리만 안 남)
	bool Initialize();

	// 모든 소리를 멈추고 XAudio2 객체를 해제한다.
	void Finalize();

	// 매 프레임 호출: 재생이 끝난 재생기(SourceVoice)를 회수한다.
	void Update();

	// WAV 파일을 재생한다. (Cocos2d-x의 AudioEngine::play2d와 같은 역할)
	// @param _path   : WAV 파일 경로 (처음 재생 시 자동으로 캐시에 올라간다)
	// @param _bLoop  : true면 무한 반복
	// @param _volume : 0.0(무음) ~ 1.0(원음) ~ 2.0(증폭)
	// @return 재생 제어용 audioId. 실패하면 INVALID_AUDIO_ID.
	_s32 Play2d(const jc::String& _path, bool _bLoop = false, _f32 _volume = 1.0f);

	// 사인파 톤을 즉석에서 만들어 재생한다. (파일 없이 효과음을 낼 때 - 튜토리얼용)
	// @param _frequency    : 주파수(Hz). 440 = "라"음. 높을수록 높은 음.
	// @param _milliseconds : 재생 길이 (밀리초)
	// @param _volume       : 볼륨
	_s32 PlayTone(_s32 _frequency, _s32 _milliseconds, _f32 _volume = 1.0f);

	// 특정 소리 정지 / 모두 정지
	void Stop(_s32 _audioId);
	void StopAll();

	// 일시정지 / 재개 (개별 + 전체. 전체 버전은 창 비활성화 시 Application이 호출한다)
	void Pause(_s32 _audioId);
	void PauseAll();
	void Resume(_s32 _audioId);
	void ResumeAll();

	// 볼륨 변경 / 재생 중 확인
	void SetVolume(_s32 _audioId, _f32 _volume);
	bool IsPlaying(_s32 _audioId);

	// WAV 파일을 미리 메모리에 올려둔다. (첫 재생 순간의 디스크 읽기 막힘 방지)
	bool Preload(const jc::String& _path);
	void Uncache(const jc::String& _path);
	void UncacheAll();

private:
	// 메모리에 올라온 WAV 한 개 (포맷 정보 + PCM 샘플 바이트들)
	struct WavData
	{
		WAVEFORMATEX format_;		// 샘플레이트/채널/비트수 등 파형 설명서
		jc::Vector<_u8> samples;	// 실제 소리 데이터 (PCM)
	};

	// 재생기 슬롯 하나 (SourceVoice + 상태)
	struct VoiceSlot
	{
		IXAudio2SourceVoice* pVoice = nullptr;	// 재생기 (nullptr면 빈 슬롯)
		_s32 audioId_ = INVALID_AUDIO_ID;		// 외부에 돌려준 제어 번호
		bool bPaused_ = false;					// 일시정지 상태인가
		bool bLoop_ = false;					// 반복 재생인가
		WavData* pOwnedTone = nullptr;			// PlayTone이 즉석에서 만든 데이터 (슬롯이 소유, 회수 시 delete)
	};

private:
	// WAV 파일을 읽어 캐시에서 찾거나 새로 올린다. 실패 시 nullptr.
	WavData* loadWav(const jc::String& _path);

	// 빈 슬롯을 찾는다. 없으면 nullptr. (먼저 Update로 끝난 슬롯을 회수해본다)
	VoiceSlot* findFreeSlot();

	// audioId로 슬롯을 찾는다. 없으면 nullptr. (이미 끝나서 회수된 소리일 수 있다)
	VoiceSlot* findSlot(_s32 _audioId);

	// 슬롯 하나를 완전히 비운다. (재생기 파괴 + 톤 데이터 회수)
	void releaseSlot(VoiceSlot& _slot);

	// 이 인스턴스가 초기화한 COM만 CoUninitialize로 정리한다.
	// (CoInitializeEx가 S_FALSE라면 다른 곳이 이미 초기화했으므로 건드리지 않는다)
	void ShutdownCom();

private:
	IXAudio2* pXAudio_;						// XAudio2 엔진 본체
	IXAudio2MasteringVoice* pMasterVoice_;	// 최종 출력 관문
	bool bInitialized_;						// 초기화 성공 여부
	bool bComInitialized_;					// 이 인스턴스가 COM을 초기화했는가 (Finalize에서 해제 대상)
	_s32 nextAudioId_;						// 다음에 발급할 audioId

	VoiceSlot voices_[MAX_VOICES];				// 재생기 슬롯들
	jc::HashMap<jc::String, WavData*> cache_;		// 경로 -> WAV 데이터 캐시
	jc::Vector<jc::String> cacheKeys_;				// 캐시된 경로 목록 (UncacheAll 순회용)
};

NS_SGF_END

// sgcl 스타일 전역 접근 매크로: g_cSound.Play2d(...) 처럼 사용
#define g_cSound (*sgf::SoundEngine::Get())
