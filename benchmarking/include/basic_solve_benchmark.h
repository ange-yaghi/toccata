#ifndef TOCCATA_BENCHMARKING_BASIC_SOLVE_BENCHMARK_H
#define TOCCATA_BENCHMARKING_BASIC_SOLVE_BENCHMARK_H

#include "benchmarking_test.h"

namespace toccata {

    class BasicSolveBenchmark : public BenchmarkingTest {
    public:
        BasicSolveBenchmark();
        ~BasicSolveBenchmark();

        virtual void Run();
    };

} /* namespace toccata */

#endif /* TOCCATA_BENCHMARKING_BASIC_SOLVE_BENCHMARK_H */
