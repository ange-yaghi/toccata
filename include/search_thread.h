#ifndef TOCCATA_CORE_SEARCH_THREAD_H
#define TOCCATA_CORE_SEARCH_THREAD_H

#include "test_pattern_evaluator.h"
#include "test_pattern_generator.h"
#include "transform.h"
#include "library.h"

namespace toccata {

    class SearchThread {
    public:
        static constexpr int MaxPitches = 256;
        static constexpr int NoteBufferSize = 100;

    public:
        struct Result {
            const MusicSegment *MatchedSegment;
            int MatchedNotes;
            double Error;
            Transform T;
        };

    public:
        SearchThread();
        ~SearchThread();

        void Initialize(int searchStart, int searchEnd);
        void Release();
        void Search(const MusicSegment *segment, const Library *library, Result *result);

    protected:
        int m_searchStart;
        int m_searchEnd;

    protected:
        TestPatternGenerator m_testPatternGenerator;
        TestPatternEvaluator::Request::MemorySpace m_memorySpace;
        int **m_notesByPitchBuffer;
        int *m_testPatternBuffer;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SEARCH_THREAD_H */
