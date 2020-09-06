#include <pch.h>

#include "../include/search_thread.h"
#include "../include/segment_generator.h"

#include <random>

void GenerateRandomLibrary(toccata::Library *library, int n) {
	toccata::SegmentGenerator generator;
	generator.Seed(0);

	std::default_random_engine engine;
	engine.seed(0);

	std::uniform_int_distribution<int> noteCountDist(8, 32);

	for (int i = 0; i < n; ++i) {
		toccata::MusicSegment *segment = library->NewSegment();
		generator.CreateRandomSegmentQuantized(segment, noteCountDist(engine), 16, 16, 60);
	}
}

TEST(SearchThreadTest, SanityCheck) {
	constexpr int SegmentCount = 100;

	toccata::Library library;
	toccata::SearchThread searchThread;

	GenerateRandomLibrary(&library, SegmentCount);

	searchThread.Initialize(0, SegmentCount - 1);

	toccata::SegmentGenerator distortion;
	distortion.Seed(0);

	std::default_random_engine engine;
	std::uniform_int_distribution<int> selector(0, 99);
	engine.seed(0);

	int correctlyIdentified = 0;

	for (int i = 0; i < 100; ++i) {
		const int selectedIndex = selector(engine);
		const toccata::MusicSegment *reference = library.GetSegment(selectedIndex);
		toccata::MusicSegment played;
		distortion.Copy(reference, &played);
		distortion.Jitter(&played, 0);
		distortion.RemoveRandomNotes(&played, 2);
		distortion.AddRandomNotes(&played, 1, 256);
		distortion.Shift(&played, 10);
		distortion.Scale(&played, 2.0);

		toccata::SearchThread::Result result;
		searchThread.Search(&played, &library, &result);

		if (reference == result.MatchedSegment) ++correctlyIdentified;
		else {
			int breakHere = 0;
		}
	}

	EXPECT_GE(correctlyIdentified, 97);

	searchThread.Release();
}
