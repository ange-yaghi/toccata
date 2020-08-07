#include "toccata_sound.h"

Toccata_Sound::Toccata_Sound() : ysObject("Toccata_Sound")
{
}

Toccata_Sound::~Toccata_Sound()
{
}

void Toccata_Sound::Play()
{

	m_soundBuffer->SetMode(ysAudioBuffer::MODE_PLAY);

}