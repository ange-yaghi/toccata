#ifndef TOCCATA_CORE_FULL_SOLVER_H
#define TOCCATA_CORE_FULL_SOLVER_H

#include "test_pattern_generator.h"
#include "test_pattern_evaluator.h"
#include "comparator.h"

namespace toccata {

    class FullSolver {
    public:
        static constexpr int MaxPitches = 256;
        static constexpr int NoteBufferSize = 100;

        static constexpr int DefaultTestPatternLength = 4;
        static constexpr double DefaultMissingNoteThreshold = 0.25;
        static constexpr double DefaultCorrelationThreshold = 0.1;

    public:
        struct Request {
            const MusicSegment *Reference = nullptr;
            const MusicSegment *Segment = nullptr;

            double MissingNoteThreshold = DefaultMissingNoteThreshold;
            double CorrelationThreshold = DefaultCorrelationThreshold;
            int PatternLength = DefaultTestPatternLength;

            int StartIndex = -1;
            int EndIndex = -1;
        };

        struct Result {
            Comparator::Result Fit;
            bool Singular;
            double s;
            double t;
        };

    public:
        FullSolver();
        ~FullSolver();

        void Initialize();
        void Release();

        bool Solve(const Request &request, Result *result);

    protected:
        TestPatternGenerator m_testPatternGenerator;
        TestPatternEvaluator::Request::MemorySpace m_memorySpace;
        int **m_notesByPitchBuffer;
        int *m_testPatternBuffer;

    protected:
        int m_testPatternLength = DefaultTestPatternLength;
        double m_missingNoteThreshold = DefaultMissingNoteThreshold;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_FULL_SOLVER_H */
