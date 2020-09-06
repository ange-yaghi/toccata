#include "../include/segment_generator.h"

#include "../include/midi_file.h"

toccata::SegmentGenerator::SegmentGenerator() {
    /* void */
}

toccata::SegmentGenerator::~SegmentGenerator() {
    /* void */
}

void toccata::SegmentGenerator::Seed(unsigned int seed) {
    m_generator.seed(seed);
}

void toccata::SegmentGenerator::Convert(const MidiStream *midi, Library *target, int leading8thRests) {
    const unsigned int eigthNoteLength = midi->GetTicksPerQuarterNote() / 2;
    const unsigned int barLength = midi->GetBarLength();

    const int n = midi->GetNoteCount();

    const unsigned int offset = leading8thRests * eigthNoteLength;
    int barIndex = 0;
    MusicSegment *currentSegment = nullptr;
    Bar *currentBar = nullptr;

    for (int i = 0; i < n; ++i) {
        const MidiNote &note = midi->GetNote(i);

        const timestamp t = note.Timestamp + offset;
        const timestamp barBoundary = (barIndex + 1) * barLength;
        if (note.Timestamp >= barBoundary) {
            Bar *previous = currentBar;
            currentSegment = target->NewSegment();
            currentBar = target->NewBar();

            if (previous != nullptr) {
                previous->AddNext(currentBar);
            }

            currentSegment->PulseUnit = (double)midi->GetTicksPerQuarterNote();
            currentSegment->PulseRate = 120 / 60.0; // 120 bpm
            currentSegment->Length = barLength;
            currentBar->SetSegment(currentSegment);

            ++barIndex;
        }

        if (currentSegment == nullptr) {
            currentSegment = target->NewSegment();
            currentBar = target->NewBar();

            currentSegment->PulseUnit = (double)midi->GetTicksPerQuarterNote();
            currentSegment->Length = barLength;
            currentSegment->PulseRate = 120 / 60.0;
            currentBar->SetSegment(currentSegment);
        }

        const timestamp barStart = barIndex * barLength;
        const timestamp noteStart = t - barStart;

        MusicPoint point;
        point.Timestamp = noteStart;
        point.Pitch = note.MidiKey;
        point.Velocity = note.Velocity;
        point.Length = note.NoteLength;
        point.Part = note.AssignedHand;
        currentSegment->NoteContainer.AddPoint(point);
    }
}

void toccata::SegmentGenerator::Copy(const MusicSegment *reference, MusicSegment *segment) {
    const MusicPoint *points = reference->NoteContainer.GetPoints();
    const int n = reference->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        segment->NoteContainer.AddPoint(points[i]);
    }

    segment->Length = reference->Length;
    segment->PulseUnit = reference->PulseUnit;
}

void toccata::SegmentGenerator::Append(MusicSegment *target, const MusicSegment *segment) {
    const timestamp offset = target->Length;

    const MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        MusicPoint newPoint = points[i];
        newPoint.Timestamp += offset;

        target->NoteContainer.AddPoint(newPoint);
    }

    target->Length += segment->Length;
}

void toccata::SegmentGenerator::CreateRandomSegment(
    MusicSegment *segment, int noteCount, timestamp length, int notes) 
{
    segment->Length = length;
    AddRandomNotes(segment, noteCount, notes);
}

void toccata::SegmentGenerator::CreateRandomSegmentQuantized(
    MusicSegment *segment, int noteCount, int gridSpaces, int unitLength, int notes) 
{
    std::uniform_int_distribution<int> gridOffset(0, gridSpaces - 1);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    segment->Length = (timestamp)unitLength * gridSpaces;
    for (int i = 0; i < noteCount; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0;
        newPoint.Velocity = 1;
        newPoint.Pitch = noteDist(m_generator);
        newPoint.Timestamp = (timestamp)gridOffset(m_generator) * unitLength;

        segment->NoteContainer.AddPoint(newPoint);
    }
}

void toccata::SegmentGenerator::AddRandomNotes(
    MusicSegment *segment, int count, int notes) 
{
    std::uniform_int_distribution<timestamp> offsetDist(0, segment->Length);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    for (int i = 0; i < count; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0;
        newPoint.Velocity = 1;
        newPoint.Pitch = noteDist(m_generator);
        newPoint.Timestamp = offsetDist(m_generator);

        segment->NoteContainer.AddPoint(newPoint);
    }
}

void toccata::SegmentGenerator::AddRandomNotes(
    MusicSegment *segment, int count, int notes, timestamp start, timestamp end) 
{
    std::uniform_int_distribution<timestamp> offsetDist(start, end);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    for (int i = 0; i < count; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0;
        newPoint.Velocity = 1;
        newPoint.Pitch = noteDist(m_generator);
        newPoint.Timestamp = offsetDist(m_generator);

        segment->NoteContainer.AddPoint(newPoint);
    }
}

void toccata::SegmentGenerator::RemoveRandomNotes(MusicSegment *segment, int count) {
    for (int i = 0; i < count; ++i) {
        const int n = segment->NoteContainer.GetCount();
        if (n == 0) break;

        std::uniform_int_distribution<int> dist(0, n - 1);
        const int index = dist(m_generator);

        segment->NoteContainer.RemovePoint(index);
    }
}

void toccata::SegmentGenerator::Jitter(MusicSegment *segment, timestamp amplitude) {
    MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    std::uniform_int_distribution<timestamp> dist(-amplitude, amplitude);

    for (int i = 0; i < n; ++i) {
        points[i].Timestamp += dist(m_generator);
    }

    for (int i = 0; i < n; ++i) {
        int smallest = -1;
        for (int j = i; j < n; ++j) {
            if (smallest == -1 || points[j].Timestamp < points[smallest].Timestamp) {
                smallest = j;
            }
        }

        if (smallest == -1) break;

        MusicPoint temp = points[i];
        points[i] = points[smallest];
        points[smallest] = temp;
    }
}

void toccata::SegmentGenerator::Scale(MusicSegment *segment, double s) {
    MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        points[i].Timestamp = (timestamp)std::round(s * points[i].Timestamp);
    }

    segment->Length = (timestamp)std::round(s * segment->Length);
    segment->PulseUnit *= s;
}

void toccata::SegmentGenerator::Shift(MusicSegment *segment, timestamp t) {
    MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        points[i].Timestamp += t;
    }
}
