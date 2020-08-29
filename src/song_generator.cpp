#include "../include/song_generator.h"

#include "../include/segment_generator.h"

toccata::SongGenerator::SongGenerator() {
    /* void */
}

toccata::SongGenerator::~SongGenerator() {
    /* void */
}

void toccata::SongGenerator::GenerateSong(Library *library, int sections, int sectionBars) {    
    Bar *previous = nullptr;

    std::uniform_int_distribution<int> randomNoteCount(1, 32);
    int id = library->GetBarCount();

    for (int i = 0; i < sections; ++i) {
        Bar *sectionStart = nullptr;
        for (int j = 0; j < sectionBars; ++j) {
            const int n = randomNoteCount(m_engine);

            MusicSegment *newSegment = library->NewSegment();
            m_generator.CreateRandomSegmentQuantized(newSegment, n, 16, 1.0, 100);

            Bar *newBar = library->NewBar();
            newBar->SetSegment(newSegment);
            newBar->SetId(id++);

            if (j == 0) sectionStart = newBar;

            if (previous != nullptr) {
                previous->AddNext(newBar);
            }

            previous = newBar;
        }

        previous->AddNext(sectionStart);
    }
}

void toccata::SongGenerator::Seed(unsigned int seed) {
    m_generator.Seed(seed);
    m_engine.seed(seed);
}
