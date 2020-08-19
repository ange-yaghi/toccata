#include "../include/test_pattern_generator.h"

#include <map>

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

/*
int toccata::TestPatternGenerator::FindSafeTestPattern(SafeTestPatternRequest &request) {
    const int noteCount = request.Segment->NoteContainer.GetCount();
    const int requestedSize = request.RequestedPatternSize;
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    int patternSize = requestedSize;
    if (patternSize > noteCount) patternSize = noteCount;
    if (patternSize > MaxPatternSize) patternSize = MaxPatternSize;

    std::map<int, int> countByPitch;
    for (int i = 0; i < noteCount; ++i) {
        countByPitch.emplace(points[i].Pitch, 0);
        if (countByPitch[points[i].Pitch] < 2) {
            countByPitch[points[i].Pitch] += 1;
        }
    }

    int *mem = request.Buffer;
    for (int i = 0; i < patternSize; ++i) {
        mem[i] = i;
    }

    int choiceSize = noteCount;
    int currentPatternSize = 0;
    for (int i = 0; i < patternSize; ++i) {
        std::uniform_int_distribution<int> dist(currentPatternSize, currentPatternSize + choiceSize - 1);

        const int newIndex = mem[dist(m_generator)];

        mem[currentPatternSize] = newIndex;

        const int pitch = points[newIndex].Pitch;
        if (--countByPitch[pitch] == 0) {
            for (int j = currentPatternSize; j < currentPatternSize + choiceSize; ++j) {
                if (points[mem[j]].Pitch == pitch) {
                    mem[j] = mem[currentPatternSize + choiceSize - 1];
                    --choiceSize;
                }
            }
        }
    }
}*/
