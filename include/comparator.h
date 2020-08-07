#ifndef TOCCATA_CORE_COMPARATOR_H
#define TOCCATA_CORE_COMPARATOR_H

#include "music_segment.h"

#include <random>

namespace toccata {

    class Comparator {
    public:
        struct Request {
            const MusicSegment *Reference;
            const MusicSegment *Segment;
            const int *Mapping;
            double s;
            double t;
        };

        struct Result {
            double AverageError;
        };

        static bool CalculateError(
            const Request &request,
            Result *result
        );
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_COMPARATOR_H */
