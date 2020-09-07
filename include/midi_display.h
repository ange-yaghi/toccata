#ifndef TOCCATA_UI_MIDI_DISPLAY_H
#define TOCCATA_UI_MIDI_DISPLAY_H

#include "timeline_element.h"

#include "music_segment.h"
#include "decision_tree.h"

#include "delta.h"

#include <map>

namespace toccata {

    class MidiDisplay : public TimelineElement {
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

    protected:
        void RenderReferenceNotes();

    protected:
        int m_keyStart;
        int m_keyEnd;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_MIDI_DISPLAY_H */
