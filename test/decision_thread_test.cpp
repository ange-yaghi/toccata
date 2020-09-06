#include <pch.h>

#include "utilities.h"

#include "../include/decision_thread.h"
#include "../include/song_generator.h"

TEST(DecisionThreadTest, SanityCheck) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;
	songGenerator.Seed(0);

	songGenerator.GenerateSong(&library, 3, 8);
	songGenerator.GenerateSong(&library, 3, 8);

	toccata::MusicSegment inputSegment;
	GenerateInput(library.GetBar(0), &inputSegment, 3 * 8, 0.0, 1.0, 0, 0);

	toccata::DecisionThread decisionThread;
	decisionThread.Initialize(&library, 12, inputSegment.PulseUnit, inputSegment.PulseRate);
	decisionThread.StartThreads();

	std::default_random_engine engine;

	int i = 0;
	while (!decisionThread.IsComplete()) {
		const int n = inputSegment.NoteContainer.GetCount();

		if (n > 0) {
			std::uniform_int_distribution<int> index(0, n - 1);

			const int i = index(engine);
			decisionThread.AddNote(inputSegment.NoteContainer.GetPoints()[i]);
			inputSegment.NoteContainer.RemovePoint(i);
		}
	}

	decisionThread.KillThreads();
	decisionThread.Destroy();

	toccata::DecisionTree *tree = decisionThread.GetTree();

	int longest = -1;
	for (int i = 0; i < tree->GetDecisionCount(); ++i) {
		toccata::DecisionTree::Decision *d = tree->GetDecision(i);
		const int depth = tree->GetDepth(d);
		if (depth > longest) {
			longest = depth;
		}
	}

	EXPECT_EQ(longest, 3 * 8);
}
