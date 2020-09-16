#ifndef TOCCATA_CORE_MIDI_HANDLER_H
#define TOCCATA_CORE_MIDI_HANDLER_H

#include "midi_stream.h"

#include <mutex>
#include <chrono>

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

        timestamp GetEstimatedTimestamp() const;

    protected:
        std::mutex m_bufferLock;
        MidiStream m_buffer;

        timestamp m_timestampOffset;
        timestamp m_lastTimestamp;

        std::chrono::time_point<std::chrono::system_clock> m_lastTimestampSystemTime;
    };

} /* namespace toccata */

#endif /* TOCCATA_MIDI_HANDLER_H */
