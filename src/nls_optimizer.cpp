#include "../include/nls_optimizer.h"

#include "../include/transform.h"
#include "../include/math.h"

bool toccata::NlsOptimizer::Solve(const Problem &problem, Solution *solution) {
    const Hessian H = CostHessian(problem);
    const double det = Determinant(H);

    if (Math::IsZero(det)) return false;

    const Gradient gradient = CostGradient(problem);
    const Hessian inv_H = Invert(H, det);

    const Gradient delta = Transform(gradient, inv_H);

    const double s_identity = 1.0;
    const double t_identity = 0.0;

    solution->s = s_identity - delta.df_s;
    solution->t = t_identity - delta.df_t;

    return true;
}

toccata::NlsOptimizer::Gradient toccata::NlsOptimizer::CostGradient(
    const Problem &problem) 
{
    Gradient gradient = { 0.0, 0.0 };
    for (int i = 0; i < problem.N; ++i) {
        double p = problem.p_set[i];
        double r = problem.r_set[i];
        gradient.df_s += 2 * p * (p - r);
        gradient.df_t += 2 * (p - r);
    }

    return gradient;
}

toccata::NlsOptimizer::Hessian toccata::NlsOptimizer::CostHessian(
    const Problem &problem) 
{
    Hessian H = { 
        0.0, 0.0, 
        0.0, 0.0
    };

    for (int i = 0; i < problem.N; ++i) {
        double p = problem.p_set[i];

        H.ds2 += 2 * p * p;
        H.dsdt += 2 * p;
    }

    H.dtds = H.dsdt;
    H.dt2 = 2.0 * problem.N;

    return H;
}

double toccata::NlsOptimizer::Determinant(const Hessian &H) {
    return H.ds2 * H.dt2 - H.dsdt * H.dtds;
}

toccata::NlsOptimizer::Hessian toccata::NlsOptimizer::Invert(const Hessian &H, double det) {
    const double inv_det = 1 / det;

    return {
        inv_det * H.dt2, inv_det  * -H.dsdt,
        inv_det  * -H.dtds, inv_det * H.ds2
    };
}

toccata::NlsOptimizer::Gradient toccata::NlsOptimizer::Transform(
    const Gradient &gradient, const Hessian &H) 
{
    return {
        H.ds2 * gradient.df_s + H.dsdt * gradient.df_t,
        H.dtds * gradient.df_s + H.dt2 * gradient.df_t
    };
}
