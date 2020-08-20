#ifndef TOCCATA_CORE_SEGMENT_GENERATOR_H
#define TOCCATA_CORE_SEGMENT_GENERATOR_H

#include "music_segment.h"

#include <random>

namespace toccata {

    class SegmentGenerator {
    public:
        SegmentGenerator();
        ~SegmentGenerator();

        void Seed(unsigned int seed);

        static void Copy(const MusicSegment *reference, MusicSegment *segment);
        static void Append(MusicSegment *target, const MusicSegment *segment);

        void CreateRandomSegment(MusicSegment *segment, int noteCount, double length, int notes);
        void CreateRandomSegmentQuantized(MusicSegment *segment, int noteCount, int gridSpaces, double length, int notes);

        void AddRandomNotes(MusicSegment *segment, int count, int notes);
        void AddRandomNotes(MusicSegment *segment, int count, int notes, double start, double end);
        void RemoveRandomNotes(MusicSegment *segment, int count);

        void Jitter(MusicSegment *segment, double amplitude);
        static void Scale(MusicSegment *segment, double s);
        static void Shift(MusicSegment *segment, double t);

    protected:
        std::default_random_engine m_generator;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SEGMENT_GENERATOR_H */
