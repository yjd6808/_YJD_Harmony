/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 10:58:34 PM
 * =====================
 * Bass 라이브러리 래퍼 및 플레이어
 * TODO: 여러 사운드 겹치게 재생가능하도록 구현, 플레이시 핸들을 맵에 기록하는 식으로 처리해야할듯? ex) Play(SOUND_BGM_LOGIN, mem)
 */


#pragma once

#include <jc/Type.h>
#include <jc/Primitives/SmartPtr.h>


class AudioPlayer
{
	using SoundDataPtr = jc::SharedPtr<Byte[]>;

public:
	AudioPlayer()
	: handle_(0)
	, playing_(false)
	, paused_(false)
	, repeat_(false)
	{
	}

	~AudioPlayer()
	{
		Stop();
		Close(handle_);
	}

public:
	bool IsPlaying() const { return playing_; }
	bool IsPaused() const { return paused_; }
	bool IsRepeat() const { return repeat_; }

	void PlayOnce();
	void PlayNew(const SoundDataPtr& _pSoundData, bool _repeat = false);
	void PlayNew(Byte* _pMem, int _len, bool _repeat);
	void PlayRepeat();
	void Pause();
	void Stop();

public:
	static int Create(void* _pMem, int _len);
	static bool Play(int _handle, bool _restart = false);
	static bool Stop(int _handle);
	static bool Pause(int _handle);
	static bool Close(int _handle);
	static Int64U GetPosition(int _handle);

	static bool Initilize();
	static bool Finalize();

private:
	SoundDataPtr soundData_;
	int handle_;
	bool playing_;
	bool paused_;
	bool repeat_;

	inline static bool IsInitialized;
};
