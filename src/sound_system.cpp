#include "../include/sound_system.h"

toccata::SoundSystem::SoundSystem() {
	ysAudioSystem::CreateAudioSystem(&AudioSystem, ysAudioSystemObject::API::DirectSound8);
	AudioSystem->EnumerateDevices();
	AudioDevice = AudioSystem->GetPrimaryDevice();

	AudioSystem->ConnectDeviceConsole(AudioDevice);
}

toccata::SoundSystem::~SoundSystem() {
	/* void */
}

toccata::Sound *toccata::SoundSystem::CreateSound(const char *fname) {
	toccata::Sound *newSound = m_sounds.New();

	newSound->m_soundFile.OpenFile(fname);

	newSound->m_soundBuffer = AudioDevice->CreateBuffer(newSound->m_soundFile.GetAudioParameters(), newSound->m_soundFile.GetSampleCount());

	newSound->m_soundFile.AttachExternalBuffer(newSound->m_soundBuffer);
	newSound->m_soundFile.FillBuffer(0);

	return newSound;
}
