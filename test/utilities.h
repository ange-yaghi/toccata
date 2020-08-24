#ifndef TOCCATA_TEST_UTILITIES_H
#define TOCCATA_TEST_UTILITIES_H

#include "../include/bar.h"

int GenerateInput(
	toccata::Bar *start, toccata::MusicSegment *target, int barCount,
	double jitter, double scale, int missingNotesPerBar, int addedNotesPerBar);

#endif /* TOCCATA_TEST_UTILITIES_H */
