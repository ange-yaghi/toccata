#ifndef TOCCATA_UI_MIDI_DISPLAY_H
#define TOCCATA_UI_MIDI_DISPLAY_H

#include "music_segment.h"
#include "decision_tree.h"

#include "delta.h"

namespace toccata {

    class MidiDisplay {
    public:
        struct MatchedBar {
            DecisionTree::MatchedBar Bar;
        };

    public:
        MidiDisplay();
        ~MidiDisplay();

        void Initialize(dbasic::DeltaEngine *engine);

        void Process();
        void Render();

        void SetPosition(const ysVector &pos) { m_position = pos; }
        ysVector GetPosition() const { return m_position; }

        void SetSize(const ysVector &size) { m_size = size; }
        ysVector GetSize() const { return m_size; }

        void SetKeyRangeStart(int start) { m_keyStart = start; }
        int GetKeyRangeStart() const { return m_keyStart; }

        void SetKeyRangeEnd(int end) { m_keyEnd = end; }
        int GetKeyRangeEnd() const { return m_keyEnd; }

        bool IsAccidental(int key) const;

        void SetInputSegment(MusicSegment *segment) { m_inputSegment = segment; }
        MusicSegment *GetInputSegment() const { return m_inputSegment; }

        void SetReferenceSegment(MusicSegment *segment) { m_referenceSegment = segment; }
        MusicSegment *GetReferenceSegment() const { return m_referenceSegment; }

        void SetTimeRange(double range) { m_timeRange = range; }
        double GetTimeRange() const { return m_timeRange; }

        void SetTimeOffset(double offset) { m_timeOffset = offset; }
        double GetTimeOffset() const { return m_timeOffset; }

        void ClearBars() { m_bars.clear(); }
        void AddBar(const DecisionTree::MatchedBar &bar) { m_bars.push_back({ bar }); }

    protected:
        ysVector m_position;
        ysVector m_size;

        int m_keyStart;
        int m_keyEnd;

        double m_timeRange;
        double m_timeOffset;

        dbasic::DeltaEngine *m_engine;

        MusicSegment *m_inputSegment;
        MusicSegment *m_referenceSegment;

        std::vector<MatchedBar> m_bars;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_MIDI_DISPLAY_H */
