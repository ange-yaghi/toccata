#include "..\include\segment_generator.h"

toccata::SegmentGenerator::SegmentGenerator() {
    /* void */
}

toccata::SegmentGenerator::~SegmentGenerator() {
    /* void */
}

void toccata::SegmentGenerator::Seed(unsigned int seed) {
    m_generator.seed(seed);
}

void toccata::SegmentGenerator::Copy(const MusicSegment *reference, MusicSegment *segment) {
    const MusicPoint *points = reference->NoteContainer.GetPoints();
    const int n = reference->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        segment->NoteContainer.AddPoint(points[i]);
    }

    segment->Length = reference->Length;
}

void toccata::SegmentGenerator::Append(MusicSegment *target, const MusicSegment *segment) {
    const double offset = target->Length;

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
    MusicSegment *segment, int noteCount, double length, int notes) 
{
    segment->Length = length;
    AddRandomNotes(segment, noteCount, notes);
}

void toccata::SegmentGenerator::CreateRandomSegmentQuantized(
    MusicSegment *segment, int noteCount, int gridSpaces, double length, int notes) 
{
    const double gridSize = length / gridSpaces;

    std::uniform_int_distribution<int> gridOffset(0, gridSpaces - 1);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    segment->Length = length;
    for (int i = 0; i < noteCount; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0.0;
        newPoint.Velocity = 1;
        newPoint.Pitch = noteDist(m_generator);
        newPoint.Timestamp = gridOffset(m_generator) * gridSize;

        segment->NoteContainer.AddPoint(newPoint);
    }
}

void toccata::SegmentGenerator::AddRandomNotes(
    MusicSegment *segment, int count, int notes) 
{
    std::uniform_real_distribution<double> offsetDist(0.0, segment->Length);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    for (int i = 0; i < count; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0.0;
        newPoint.Velocity = 1;
        newPoint.Pitch = noteDist(m_generator);
        newPoint.Timestamp = offsetDist(m_generator);

        segment->NoteContainer.AddPoint(newPoint);
    }
}

void toccata::SegmentGenerator::AddRandomNotes(
    MusicSegment *segment, int count, int notes, double start, double end) 
{
    std::uniform_real_distribution<double> offsetDist(start, end);
    std::uniform_int_distribution<int> noteDist(0, notes - 1);

    for (int i = 0; i < count; ++i) {
        MusicPoint newPoint;
        newPoint.Length = 0.0;
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

void toccata::SegmentGenerator::Jitter(MusicSegment *segment, double amplitude) {
    MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    std::uniform_real_distribution<double> dist(-amplitude, amplitude);

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
        points[i].Timestamp *= s;
    }
}

void toccata::SegmentGenerator::Shift(MusicSegment *segment, double t) {
    MusicPoint *points = segment->NoteContainer.GetPoints();
    const int n = segment->NoteContainer.GetCount();

    for (int i = 0; i < n; ++i) {
        points[i].Timestamp += t;
    }
}
