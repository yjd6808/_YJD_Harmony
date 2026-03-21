/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 10:58:34 PM
 * =====================
 *
 */


#include "AudioPlayer.h"

#include "bass.h"

USING_NS_JC;

// =============================================================
//					플레이어 기능
// =============================================================
void AudioPlayer::PlayOnce()
{
	jc_assert_msg(handle_, "핸들이 설정되지 않았습니다.");

	if (playing_)
		Stop(handle_);

	if (Play(handle_, false))
	{
		paused_ = false;
		playing_ = true;
		repeat_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AudioPlayer::PlayNew(const SoundDataPtr& _pSoundData, bool _repeat)
{
	if (handle_ != 0)
	{
		Stop(handle_);
		Close(handle_);
	}

	handle_ = Create(_pSoundData.GetPtr(), _pSoundData.Length());

	if (handle_ != 0)
	{
		soundData_ = _pSoundData;

		if (_repeat)
			PlayRepeat();
		else
			PlayOnce();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AudioPlayer::PlayRepeat()
{
	jc_assert_msg(handle_, "핸들이 설정되지 않았습니다.");

	if (playing_)
		Stop(handle_);

	if (Play(handle_, true))
	{
		paused_ = false;
		playing_ = true;
		repeat_ = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AudioPlayer::PlayNew(_u8* _pMem, int _len, bool _repeat)
{
	if (handle_ != 0)
	{
		Stop(handle_);
		Close(handle_);
	}

	handle_ = Create(_pMem, _len);

	if (handle_ != 0)
	{
		soundData_ = nullptr;

		if (_repeat)
			PlayRepeat();
		else
			PlayOnce();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AudioPlayer::Pause()
{
	if (!playing_)
		return;

	if (Pause(handle_))
	{
		paused_ = true;
		playing_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AudioPlayer::Stop()
{
	if (!playing_)
		return;

	if (Stop(handle_))
	{
		paused_ = false;
		playing_ = false;
	}
}

// =============================================================
//					라이브러리 래핑
// =============================================================
int AudioPlayer::Create(void* _pMem, int _len)
{
	return static_cast<int>(BASS_StreamCreateFile(TRUE, _pMem, 0, _len, 0));
}

bool AudioPlayer::Play(int _handle, bool _restart)
{
	return BASS_ChannelPlay(_handle, !_restart) != 0;
}

bool AudioPlayer::Stop(int _handle)
{
	return BASS_ChannelStop(_handle) != 0;
}

bool AudioPlayer::Pause(int _handle)
{
	return BASS_ChannelPause(_handle) != 0;
}

bool AudioPlayer::Close(int _handle)
{
	return BASS_StreamFree(_handle) != 0;
}

// https://www.un4seen.com/doc/#bass/BASS_ChannelGetPosition.html
_u64 AudioPlayer::GetPosition(int _handle)
{
	return BASS_ChannelGetPosition(_handle, BASS_POS_BYTE);
}

bool AudioPlayer::Initilize()
{
	if (BASS_Init(-1, 44100, 0, 0, 0))
		IsInitialized = true;

	return IsInitialized;
}

bool AudioPlayer::Finalize()
{
	if (!BASS_Stop())
	{
		jc_assert(false);
		return false;
	}

	if (!BASS_Free())
	{
		jc_assert(false);
		return false;
	}

	IsInitialized = false;
	return true;
}
