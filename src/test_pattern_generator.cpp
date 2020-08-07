#include "../include/test_pattern_generator.h"

toccata::TestPatternGenerator::TestPatternGenerator() {
    /* void */
}

toccata::TestPatternGenerator::~TestPatternGenerator() {
    /* void */
}

void toccata::TestPatternGenerator::Seed(unsigned int seed) {
    m_generator.seed(seed);
}

int toccata::TestPatternGenerator::FindRandomTestPattern(
    TestPatternRequest &request) 
{
    const int noteCount = request.NoteCount;
    const int requestedSize = request.RequestedPatternSize;

    int patternSize = requestedSize;
    if (patternSize > noteCount) patternSize = noteCount;
    if (patternSize > MaxPatternSize) patternSize = MaxPatternSize;

    for (int i = 0; i < noteCount; ++i) {
        request.Buffer[i] = i;
    }

    int currentPatternSize = noteCount;
    while (currentPatternSize > patternSize) {
        std::uniform_int_distribution<int> distribution(0, currentPatternSize - 1);
        int randomEntryToDelete = distribution(m_generator);

        request.Buffer[randomEntryToDelete] = request.Buffer[currentPatternSize - 1];
        --currentPatternSize;
    }

    return patternSize;
}

int toccata::TestPatternGenerator::FindExtremeTestPattern(TestPatternRequest &request) {
    const int noteCount = request.NoteCount;
    const int requestedSize = request.RequestedPatternSize;

    int patternSize = requestedSize;
    if (patternSize > noteCount) patternSize = noteCount;
    if (patternSize > MaxPatternSize) patternSize = MaxPatternSize;

    int l = 0, r = noteCount - 1;
    bool left = true;
    for (int i = 0; i < patternSize; ++i) {
        request.Buffer[i] = left
            ? l++
            : r--;

        left = !left;
    }

    return patternSize; 
}
