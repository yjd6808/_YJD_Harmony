/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 10:58:34 PM
 * =====================
 *
 */


#include "Core.h"
#include "AudioPlayer.h"

#include <bass.h>

USING_NS_JC;

// =============================================================
//					플레이어 기능
// =============================================================


void AudioPlayer::PlayOnce() {
	DebugAssertMsg(m_iHandle, "핸들이 설정되지 않았습니다.");


	if (m_bPlaying)
		Stop(m_iHandle);

	if (Play(m_iHandle, false)) {
		m_bPaused = false;
		m_bPlaying = true;
		m_bRepeat = false; 
	}
}

void AudioPlayer::PlayNew(SoundDataPtr soundData, bool repeat) {
	if (m_iHandle != 0) {
		Stop(m_iHandle);
		Close(m_iHandle);
	}

	m_iHandle = Create(soundData.GetPtr(), soundData.Length());

	if (m_iHandle != 0) {
		m_spSoundData = soundData;
		if (repeat) PlayRepeat();
		else PlayOnce();
	}
}

void AudioPlayer::PlayRepeat() {
	DebugAssertMsg(m_iHandle, "핸들이 설정되지 않았습니다.");

	if (m_bPlaying)
		Stop(m_iHandle);

	if (Play(m_iHandle, true)) {
		m_bPaused = false;
		m_bPlaying = true;
		m_bRepeat = true;
	}
}

void AudioPlayer::PlayNew(Byte* mem, int len, bool repeat) {
	if (m_iHandle != 0) {
		Stop(m_iHandle);
		Close(m_iHandle);
	}

	m_iHandle = Create(mem, len);
	
	if (m_iHandle != 0) {
		m_spSoundData = nullptr;
		if (repeat) PlayRepeat();
		else PlayOnce();
	}
}

void AudioPlayer::Pause() {

	if (!m_bPlaying)
		return;

	if (Pause(m_iHandle)) {
		m_bPaused = true;
		m_bPlaying = false;
	}
}

void AudioPlayer::Stop() {

	if (!m_bPlaying)
		return;

	if (Stop(m_iHandle)) {
		m_bPaused = false;
		m_bPlaying = false;
	}
}

// =============================================================
//					라이브러리 래핑
// =============================================================

int AudioPlayer::Create(void* mem, int len) {
	return (int)BASS_StreamCreateFile(TRUE, mem, 0, len, 0);
}

bool AudioPlayer::Play(int handle, bool restart) {
	return BASS_ChannelPlay(handle, !restart) != 0;
}

bool AudioPlayer::Stop(int handle) {
	return BASS_ChannelStop(handle) != 0;
}

bool AudioPlayer::Pause(int handle) {
	return BASS_ChannelPause(handle);
}

bool AudioPlayer::Close(int handle) {
	return BASS_StreamFree(handle);
}


// https://www.un4seen.com/doc/#bass/BASS_ChannelGetPosition.html
Int64U AudioPlayer::GetPosition(int handle) {
	return BASS_ChannelGetPosition(handle, BASS_POS_BYTE);
}

bool AudioPlayer::Initilize() {
	if (BASS_Init(-1, 44100, 0, 0, 0))
		ms_bInitialized = true;

	return ms_bInitialized;
}


bool AudioPlayer::Finalize() {
	
	if (!BASS_Stop()) {
		DebugAssert(false);
		return false;
	}

	if (!BASS_Free()) {
		DebugAssert(false);
		return false;
	}

	ms_bInitialized = false;
	return true;
}



