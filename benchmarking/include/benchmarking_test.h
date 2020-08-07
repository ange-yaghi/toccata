#ifndef TOCCATA_BENCHMARKING_BENCHMARKING_TEST_H
#define TOCCATA_BENCHMARKING_BENCHMARKING_TEST_H

#include <string>

namespace toccata {

    class BenchmarkingTest {
    public:
        BenchmarkingTest();
        ~BenchmarkingTest();

        std::string GetName() const { return m_name; }
        void SetName(const std::string &name) { m_name = name; }

        virtual void Run() = 0;

    protected:
        std::string m_name;
    };

} /* namespace toccata */

#endif /* TOCCATA_BENCHMARKING_BENCHMARKING_TEST_H */
