#ifndef TOCCATA_CORE_SOUND_SYSTEM_H
#define TOCCATA_CORE_SOUND_SYSTEM_H

#include "sound.h"

#include "delta.h"

namespace toccata {

    class SoundSystem {
    public:
        SoundSystem();
        ~SoundSystem();

        Sound *CreateSound(const char *fname);

    protected:
        ysDynamicArray<Sound, 4> m_sounds;

        ysAudioSystem *AudioSystem;
        ysAudioDevice *AudioDevice;

        ysWindowsAudioWaveFile m_ambientSoundFile;
        ysAudioBuffer *m_ambientSoundBuffer;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SOUND_SYSTEM_H */
