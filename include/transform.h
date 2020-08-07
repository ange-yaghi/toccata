#ifndef TOCCATA_CORE_TRANSFORM_H
#define TOCCATA_CORE_TRANSFORM_H

namespace toccata {

    class Transform {
    public:
        static double f(double x, double s, double t) {
            return x * s + t;
        }

        static double inv_f(double x, double s, double t) {
            return (x - t) / s;
        }
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_TRANSFORM_H */
