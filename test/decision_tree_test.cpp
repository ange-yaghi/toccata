#include <pch.h>

#include "../include/decision_tree.h"
#include "../include/library.h"
#include "../include/music_segment.h"
#include "../include/song_generator.h"

int GenerateInput(toccata::Bar *start, toccata::MusicSegment *target, int barCount) {
	toccata::Bar *current = start;
	int n = 0;

	while (current != nullptr && n < barCount) {
		toccata::SegmentGenerator::Append(target, current->GetSegment());
		++n;

		if (current->GetNextCount() > 0) {
			toccata::Bar *nextBar = current->GetNext(0);
			current = nextBar;
		}
		else current = nullptr;
	}

	return n;
}

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

	songGenerator.GenerateSong(&library, 4, 8);
	songGenerator.GenerateSong(&library, 4, 8);

	toccata::MusicSegment inputSegment;
	inputSegment.Length = 0.0;

	GenerateInput(library.GetBar(0), &inputSegment, 36);

	toccata::DecisionTree tree;
	tree.SetLibrary(&library);
	tree.SetInputSegment(&inputSegment);
	tree.Initialize(12);
	tree.SpawnThreads();

	const int n = inputSegment.NoteContainer.GetCount();
	for (int i = 0; i < n; ++i) {
		tree.Process(i);
	}

	tree.KillThreads();
	tree.Destroy();
}
