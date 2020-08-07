#ifndef TOCCATA_CORE_SOUND_H
#define TOCCATA_CORE_SOUND_H

#include "delta.h"

namespace toccata {

    class Sound : public ysObject {
    public:
        Sound();
        ~Sound();

        ysWindowsAudioWaveFile m_soundFile;
        ysAudioBuffer *m_soundBuffer;

        void Play();
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SOUND_H */
