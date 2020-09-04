#ifndef TOCCATA_CORE_TRANSFORM_H
#define TOCCATA_CORE_TRANSFORM_H

#include "music_point.h"

#include <cmath>

namespace toccata {

    struct Transform {
        timestamp r(timestamp t) const {
            return t - t_coarse;
        }

        double f(double x) const {
            return x * s + this->t;
        }

        double Scale(double x) const {
            return x * s;
        }

        double inv_f(double y) const {
            return (y - t) / s;
        }

        timestamp Local(timestamp t) const {
            return t - t_coarse;
        }

        double s;
        double t;
        timestamp t_coarse = 0;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_TRANSFORM_H */
