#ifndef TOCCATA_CORE_COMPARATOR_H
#define TOCCATA_CORE_COMPARATOR_H

#include "music_segment.h"
#include "transform.h"

#include <random>
#include <set>

namespace toccata {

    class Comparator {
    public:
        struct Request {
            const MusicSegment *Reference;
            const MusicSegment *Segment;
            const int *Mapping;
            Transform T;
        };

        struct Result {
            double AverageError;
            int MappedNotes;
            int MappingStart;
            int MappingEnd;
            std::set<int> *Target = nullptr;
        };

        static bool CalculateError(
            const Request &request,
            Result *result
        );
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_COMPARATOR_H */
