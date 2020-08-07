#ifndef TOCCATA_CORE_MATH_H
#define TOCCATA_CORE_MATH_H

namespace toccata {

    class Math {
    public:
        static constexpr double Epsilon = 1E-7;

        inline static bool IsZero(double a, double epsilon = Epsilon) { return Abs(a) < epsilon; }
        inline static double Abs(double a) { return (a < 0) ? -a : a; }
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MATH_H */
