#include <pch.h>

#include "utilities.h"

#include "../include/decision_tree.h"
#include "../include/library.h"
#include "../include/music_segment.h"
#include "../include/song_generator.h"

TEST(DecisionTreeTest, SanityCheck) {
	toccata::DecisionTree tree;
	tree.Initialize(1);
	tree.SpawnThreads();

	for (int i = 0; i < 1; ++i) {
		tree.Process(0);
	}

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, MultipleThreads) {
	toccata::DecisionTree tree;
	tree.Initialize(24);
	tree.SpawnThreads();

	for (int i = 0; i < 1000; ++i) {
		tree.Process(0);
	}

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, BasicIdentificationTest) {
	toccata::Library library;

	toccata::MusicSegment *segment0 = library.NewSegment();
	segment0->NoteContainer.AddPoint({ 1.0, 1 });
	segment0->NoteContainer.AddPoint({ 2.0, 0 });
	segment0->NoteContainer.AddPoint({ 3.0, 2 });
	segment0->Length = 4.0;

	toccata::MusicSegment *segment1 = library.NewSegment();
	segment1->NoteContainer.AddPoint({ 1.0, 1 });
	segment1->NoteContainer.AddPoint({ 2.0, 5 });
	segment1->NoteContainer.AddPoint({ 3.0, 2 });
	segment1->Length = 4.0;

	toccata::MusicSegment input;
	input.NoteContainer.AddPoint({ 1.0, 1 });
	input.NoteContainer.AddPoint({ 2.0, 0 });
	input.NoteContainer.AddPoint({ 3.0, 2 });
	input.NoteContainer.AddPoint({ 4.0, 1 });
	input.NoteContainer.AddPoint({ 5.0, 5 });
	input.NoteContainer.AddPoint({ 6.0, 2 });
	input.Length = 7.0;

	toccata::Bar *bar0 = library.NewBar();
	bar0->SetSegment(segment0);

	toccata::Bar *bar1 = library.NewBar();
	bar1->SetSegment(segment1);

	bar0->AddNext(bar1);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&input);
	tree.Initialize(1);
	tree.SpawnThreads();
	
	const int n = input.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, FullSong) {
	toccata::Library library;
	
	toccata::SongGenerator songGenerator;
	songGenerator.Seed(0);

	songGenerator.GenerateSong(&library, 4, 32);
	songGenerator.GenerateSong(&library, 4, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 64, 0.0, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	auto results = tree.GetPieces();

	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0].Bars.size(), 64);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, FullSongJitter) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;
	songGenerator.Seed(0);

	songGenerator.GenerateSong(&library, 4, 32);
	songGenerator.GenerateSong(&library, 4, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 64, 0.05, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	auto results = tree.GetPieces();

	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0].Bars.size(), 64);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, HighJitter) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;
	songGenerator.Seed(0);

	songGenerator.GenerateSong(&library, 4, 32);
	songGenerator.GenerateSong(&library, 4, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 64, 0.15, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	int longest = -1;
	for (int i = 0; i < tree.GetDecisionCount(); ++i) {
		toccata::DecisionTree::Decision *d = tree.GetDecision(i);
		int depth = tree.GetDepth(d);
		if (depth > longest) {
			longest = depth;
		}
	}

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, NoteMistakes) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;
	songGenerator.Seed(0);

	songGenerator.GenerateSong(&library, 4, 32);
	songGenerator.GenerateSong(&library, 4, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 64, 0.01, 2.0, 1, 2);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	int longest = -1;
	for (int i = 0; i < tree.GetDecisionCount(); ++i) {
		toccata::DecisionTree::Decision *d = tree.GetDecision(i);
		int depth = tree.GetDepth(d);
		if (depth > longest) {
			longest = depth;
		}
	}

	EXPECT_GE(longest, 30);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, SimilarSongs) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;

	songGenerator.Seed(0);
	songGenerator.GenerateSong(&library, 2, 32);

	songGenerator.Seed(0);
	songGenerator.GenerateSong(&library, 3, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(64), &inputSegment, 3 * 32, 0.0, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	auto results = tree.GetPieces();

	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0].Bars.size(), 3 * 32);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, Repeat) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;

	songGenerator.Seed(0);
	songGenerator.GenerateSong(&library, 2, 32);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 3 * 32, 0.0, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	int longest = -1;
	for (int i = 0; i < tree.GetDecisionCount(); ++i) {
		toccata::DecisionTree::Decision *d = tree.GetDecision(i);
		int depth = tree.GetDepth(d);
		if (depth > longest) {
			longest = depth;
		}
	}

	EXPECT_EQ(longest, 3 * 32);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, SimpleStructure) {
	const std::string path = "../../../test/midi/simple_passage.midi";

	toccata::Library library;

	toccata::MidiStream stream;
	toccata::MidiFile midiFile;
	midiFile.Read(path.c_str(), &stream);

	toccata::SegmentGenerator::Convert(&stream, &library, 0);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 3, 0.0, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(1);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	auto results = tree.GetPieces();

	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0].Bars.size(), 3);

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, SimilarSongs_2) {
	toccata::Library library;

	toccata::SongGenerator songGenerator;

	const std::string paths[] =
	{
		"../../../test/midi/simple_passage.midi",
		"../../../test/midi/simple_passage_2.mid"
	};

	for (const std::string &path : paths) {
		toccata::MidiStream stream;
		toccata::MidiFile midiFile;
		midiFile.Read(path.c_str(), &stream);

		toccata::SegmentGenerator::Convert(&stream, &library, 0);
	}

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 3, 0.0, 1.0, 0, 0);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(1);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	auto results = tree.GetPieces();

	EXPECT_EQ(results.size(), 1);
	EXPECT_EQ(results[0].Bars.size(), 3);
	EXPECT_EQ(results[0].Bars[0].MatchedBar->GetId(), 0);

	tree.KillThreads();
	tree.Destroy();
}
