#include <pch.h>

#include "../include/segment_utilities.h"
#include "../include/memory.h"

TEST(SegmentUtilitiesTest, SanityCheck) {
	toccata::MusicSegment segment;
	segment.PulseUnit = 1.0f;
	segment.NoteContainer.AddPoint({ 0, 0 });
	segment.NoteContainer.AddPoint({ 1, 1 });
	segment.NoteContainer.AddPoint({ 2, 2 });

	int **target = toccata::Memory::Allocate2d<int>(3, 4);

	toccata::SegmentUtilities::SortByPitch(&segment, 0, 2, 3, target);

	EXPECT_EQ(target[0][0], 0);
	EXPECT_EQ(target[0][1], -1);
	EXPECT_EQ(target[0][2], -1);
	EXPECT_EQ(target[0][3], -1);

	EXPECT_EQ(target[1][0], 1);
	EXPECT_EQ(target[1][1], -1);
	EXPECT_EQ(target[1][2], -1);
	EXPECT_EQ(target[1][3], -1);

	EXPECT_EQ(target[2][0], 2);
	EXPECT_EQ(target[2][1], -1);
	EXPECT_EQ(target[2][2], -1);
	EXPECT_EQ(target[2][3], -1);
}
