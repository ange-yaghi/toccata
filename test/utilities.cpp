#include <pch.h>

#include "utilities.h"

#include "../include/segment_generator.h"

int GenerateInput(
	toccata::Bar *start, toccata::MusicSegment *target, int barCount,
	double jitter, double scale, int missingNotesPerBar, int addedNotesPerBar) 
{
	toccata::Bar *current = start;
	int n = 0;

	toccata::SegmentGenerator utilities;
	utilities.Seed(0);

	while (current != nullptr && n < barCount) {
		toccata::MusicSegment segment;
		toccata::SegmentGenerator::Copy(current->GetSegment(), &segment);

		utilities.Jitter(&segment, jitter);
		utilities.AddRandomNotes(&segment, addedNotesPerBar, 64);
		utilities.RemoveRandomNotes(&segment, missingNotesPerBar);
		toccata::SegmentGenerator::Scale(&segment, scale);

		toccata::SegmentGenerator::Append(target, &segment);
		++n;

		if (current->GetNextCount() > 1) {
			current = current->GetNext(1);
		}
		else if (current->GetNextCount() == 1) {
			current = current->GetNext(0);
		}
	}

	return n;
}
