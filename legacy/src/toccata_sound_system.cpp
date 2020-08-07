#include "toccata_sound_system.h"

Toccata_SoundSystem::Toccata_SoundSystem()
{

	ysAudioSystem::CreateAudioSystem(&AudioSystem, ysAudioSystemObject::API_DIRECT_SOUND8);
	AudioSystem->EnumerateDevices();
	AudioDevice = AudioSystem->GetPrimaryDevice();

	AudioSystem->ConnectDeviceConsole(AudioDevice);

}

Toccata_SoundSystem::~Toccata_SoundSystem()
{



}

Toccata_Sound *Toccata_SoundSystem::CreateSound(char *fname)
{

	Toccata_Sound *newSound = m_sounds.New();

	newSound->m_soundFile.OpenFile(fname);

	newSound->m_soundBuffer = AudioDevice->CreateBuffer(newSound->m_soundFile.GetAudioParameters(), newSound->m_soundFile.GetSampleCount());

	newSound->m_soundFile.AttachExternalBuffer(newSound->m_soundBuffer);
	newSound->m_soundFile.FillBuffer(0);

	newSound->m_soundBuffer->SetMode(ysAudioBuffer::MODE_STOP);

	return newSound;

}