#ifndef TOCCATA_CORE_NLS_OPTIMIZER_H
#define TOCCATA_CORE_NLS_OPTIMIZER_H

namespace toccata {

    class NlsOptimizer {
    public:
        struct Problem {
            double *r_set;
            double *p_set;
            int N;
        };

        struct Solution {
            double s;
            double t;
        };

        static bool Solve(const Problem &problem, Solution *solution);

    private:
        struct Gradient {
            double df_s;
            double df_t;
        };

        struct Hessian {
            // ds2  dsdt 
            // dtds dt2

            double ds2;
            double dsdt;
            double dtds;
            double dt2;
        };

        static Gradient CostGradient(const Problem &problem);
        static Hessian CostHessian(const Problem &problem);

        static double Determinant(const Hessian &H);
        static Hessian Invert(const Hessian &H, double det);
        static Gradient Transform(const Gradient &gradient, const Hessian &H);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_NLS_OPTIMIZER_H */
