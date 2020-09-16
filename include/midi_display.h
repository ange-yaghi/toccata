#ifndef TOCCATA_UI_MIDI_DISPLAY_H
#define TOCCATA_UI_MIDI_DISPLAY_H

#include "timeline_element.h"

#include "heat_map.h"
#include "music_segment.h"
#include "decision_tree.h"

#include "delta.h"

#include <map>

namespace toccata {

    class MidiDisplay : public TimelineElement {
    public:
        enum class PracticeMode {
            Default,
            Velocity,
            Timing
        };

    public:
        MidiDisplay();
        ~MidiDisplay();

        void Process();
        void Render();

        void SetKeyRangeStart(int start) { m_keyStart = start; }
        int GetKeyRangeStart() const { return m_keyStart; }

        void SetKeyRangeEnd(int end) { m_keyEnd = end; }
        int GetKeyRangeEnd() const { return m_keyEnd; }

        bool IsAccidental(int key) const;

        void FindUnmappedNotes(std::set<int> &mapped) const;

        void SetShowReferenceNotes(bool show) { m_showReferenceNotes = show; }
        bool GetShowReferenceNotes() const { return m_showReferenceNotes; }

        void SetShowPlayedNotes(bool show) { m_showPlayedNotes = show; }
        bool GetShowPlayedNotes() const { return m_showPlayedNotes; }

        void SetPracticeMode(PracticeMode mode) { m_mode = mode; }
        PracticeMode GetPracticeMode() const { return m_mode; }

        ysVector GetColor(const Analyzer::NoteInformation &noteInfo) const;
        ysVector GetVelocityColor(unsigned short velocity) const;
        ysVector GetTimingColor(double error) const;

        void SetTargetVelocity(double velocity) { m_targetVelocity = velocity; }
        double GetTargetVelocity() const { return m_targetVelocity; }

        void SetVelocityErrorThreshold(double threshold) { m_velocityErrorThreshold = threshold; }
        double GetVelocityErrorThreshold() const { return m_velocityErrorThreshold; }

        void SetTimingErrorThreshold(double threshold) { m_timingErrorThreshold = threshold; }
        double GetTimingErrorThreshold() const { return m_timingErrorThreshold; }

    protected:
        void RenderReferenceNotes();
        void RenderPlayedNotes();

        void RenderCursor();

    protected:
        int m_keyStart;
        int m_keyEnd;

        bool m_showReferenceNotes;
        bool m_showPlayedNotes;

        PracticeMode m_mode;

        // Velocity practice parameters
        double m_targetVelocity;
        double m_velocityErrorThreshold;

        // Timing practice parameters
        double m_timingErrorThreshold;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_MIDI_DISPLAY_H */
