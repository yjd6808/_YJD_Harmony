/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 10:58:34 PM
 * =====================
 * Bass 라이브러리 래퍼 및 플레이어
 * TODO: 여러 사운드 겹치게 재생가능하도록 구현, 플레이시 핸들을 맵에 기록하는 식으로 처리해야할듯? ex) Play(SOUND_BGM_LOGIN, mem)
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/SmartPtr.h>


class AudioPlayer
{
	using SoundDataPtr = JCore::SharedPtr<Byte[]>;

public:
	AudioPlayer()
		: m_iHandle(0)
		, m_bPlaying(false)
		, m_bPaused(false)
		, m_bRepeat(false) {}
	~AudioPlayer() {
		Stop();
		Close(m_iHandle);
	}
public:
	bool IsPlaying() { return m_bPlaying; }
	bool IsPaused() { return m_bPaused; }
	bool IsRepeat() { return m_bRepeat; }

	void PlayOnce();
	void PlayNew(SoundDataPtr soundData, bool repeat = false);
	void PlayNew(Byte* mem, int len, bool repeat);
	void PlayRepeat();
	void Pause();
	void Stop();
public:
	static int Create(void* mem, int len);
	static bool Play(int handle, bool restart = false);
	static bool Stop(int handle);
	static bool Pause(int handle);
	static bool Close(int handle);
	static Int64U GetPosition(int handle);
	
	static bool Initilize();
	static bool Finalize();
private:
	SoundDataPtr m_spSoundData;
	int m_iHandle;
	bool m_bPlaying;
	bool m_bPaused;
	bool m_bRepeat;

	inline static bool ms_bInitialized;
};


