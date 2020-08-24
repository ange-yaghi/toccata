#include "../include/nls_optimizer.h"

#include "../include/transform.h"
#include "../include/math.h"

#include <limits>

bool toccata::NlsOptimizer::Solve(const Problem &problem, Solution *solution) {
    if (IsSingular(problem)) return SolveSingular(problem, solution);

    const Hessian H = CostHessian(problem);
    const double det = Determinant(H);

    if (Math::IsZero(det)) return false;

    const Gradient gradient = CostGradient(problem);
    const Hessian inv_H = Invert(H, det);

    const Gradient delta = Transform(gradient, inv_H);

    solution->s = ScaleIdentity - delta.df_s;
    solution->t = TranslateIdentity - delta.df_t;
    solution->Singularity = false;

    return true;
}

bool toccata::NlsOptimizer::SolveSingular(const Problem &problem, Solution *solution) {
    const double df_t2 = CostHessian(problem).dt2;
    const double df_t = CostGradient(problem).df_t;

    if (Math::IsZero(df_t2)) return false;
    
    solution->s = 1.0;
    solution->t = TranslateIdentity - (df_t / df_t2);
    solution->Singularity = true;

    return true;
}

bool toccata::NlsOptimizer::IsSingular(const Problem &problem) {
    constexpr double Threshold = 1E-4;

    double minR = DBL_MAX, maxR = DBL_MIN;
    double minP = DBL_MAX, maxP = DBL_MIN;
    for (int i = 0; i < problem.N; ++i) {
        if (problem.p_set[i] < minP) minP = problem.p_set[i];
        if (problem.p_set[i] > maxP) maxP = problem.p_set[i];

        if (problem.r_set[i] < minR) minR = problem.r_set[i];
        if (problem.r_set[i] > maxR) maxR = problem.r_set[i];
    }

    return (maxR - minR) < Threshold || (maxP - minP) < Threshold;
}

toccata::NlsOptimizer::Gradient toccata::NlsOptimizer::CostGradient(
    const Problem &problem) 
{
    Gradient gradient = { 0.0, 0.0 };
    for (int i = 0; i < problem.N; ++i) {
        const double p = problem.p_set[i];
        const double r = problem.r_set[i];
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
