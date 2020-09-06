#ifndef TOCCATA_CORE_MIDI_NOTE_H
#define TOCCATA_CORE_MIDI_NOTE_H

#include "music_point.h"

namespace toccata {

    struct MidiNote {
        int MidiKey;
        int Velocity;

        int NoteLength;
        timestamp Timestamp;

        MusicPoint::Hand AssignedHand;

        bool Valid;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_NOTE_H */
