/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 10:58:34 PM
 * =====================
 * Bass 라이브러리 래퍼 및 플레이어
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/SmartPtr.h>


class BassPlayer
{
	using SoundDataPtr = JCore::SharedPtr<Byte[]>;

public:
	BassPlayer()
		: m_iHandle(0)
		, m_bPlaying(false)
		, m_bPaused(false)
		, m_bRepeat(false) {}
	~BassPlayer() {
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


