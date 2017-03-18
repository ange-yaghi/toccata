#ifndef TOCCATA_SOUND_SYSTEM_H
#define TOCCATA_SOUND_SYSTEM_H

#include <yds_audio_system.h>
#include <yds_audio_device.h>

#include <yds_windows_audio_wave_file.h>
#include <yds_audio_buffer.h>

#include "toccata_sound.h"

class Toccata_SoundSystem
{

public:

	Toccata_SoundSystem();
	~Toccata_SoundSystem();

	Toccata_Sound *CreateSound(char *fname);

protected:

	ysDynamicArray<Toccata_Sound, 4> m_sounds;

	ysAudioSystem *AudioSystem;
	ysAudioDevice *AudioDevice;

	ysWindowsAudioWaveFile m_ambientSoundFile;
	ysAudioBuffer *m_ambientSoundBuffer;

};

#endif