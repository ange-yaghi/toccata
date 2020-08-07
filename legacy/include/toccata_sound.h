#ifndef TOCCATA_SOUND_H
#define TOCCATA_SOUND_H

#include <yds_windows_audio_wave_file.h>
#include <yds_audio_buffer.h>

#include <yds_base.h>

class Toccata_Sound : public ysObject
{

public:

	Toccata_Sound();
	~Toccata_Sound();

	ysWindowsAudioWaveFile m_soundFile;
	ysAudioBuffer *m_soundBuffer;

	void Play();

};

#endif