#ifndef TOCCATA_CORE_MIDI_HANDLER_H
#define TOCCATA_CORE_MIDI_HANDLER_H

#include "midi_stream.h"

#include <mutex>

namespace toccata {

    class MidiHandler {
    protected:
        MidiHandler();
        ~MidiHandler();

        static MidiHandler *s_handler;

    public:
        static MidiHandler *Get();

        void Extract(MidiStream *targetBuffer);

        void ProcessEvent(int status, int midiByte1, int midiByte2, timestamp timestamp);
        void ProcessMidiTick(unsigned long timestamp);
        void AlignTimestampOffset();

    protected:
        std::mutex m_bufferLock;
        MidiStream m_buffer;

        timestamp m_timestampOffset;
        timestamp m_lastTimestamp;
    };

} /* namespace toccata */

#endif /* TOCCATA_MIDI_HANDLER_H */
