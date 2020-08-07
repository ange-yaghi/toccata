#include <pch.h>

#include "../include/test_pattern_evaluator.h"

#include "../include/memory.h"
#include "../include/segment_utilities.h"
#include "../include/segment_generator.h"

TEST(TestPatternEvaluatorTest, SanityCheck) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0, 1 });
	reference.NoteContainer.AddPoint({ 1.0, 2 });
	reference.NoteContainer.AddPoint({ 2.0, 3 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.0, 1 });
	segment.NoteContainer.AddPoint({ 1.0, 2 });
	segment.NoteContainer.AddPoint({ 2.0, 3 });

	const int testPattern[] = { 0, 1 };

	int *const *notesByPitch = toccata::Memory::Allocate2d<int>(4, 2);
	notesByPitch[1][0] = 0; notesByPitch[1][1] = -1;
	notesByPitch[2][0] = 1; notesByPitch[2][1] = -1;
	notesByPitch[3][0] = 2; notesByPitch[3][1] = -1;

	toccata::TestPatternEvaluator::Output output;
	toccata::TestPatternEvaluator::Request request;
	request.Segment = &segment;
	request.ReferenceSegment = &reference;
	request.TestPattern = testPattern;
	request.TestPatternLength = 2;
	request.SegmentNotesByPitch = notesByPitch;
	
	toccata::TestPatternEvaluator::AllocateMemorySpace(&request.Memory, 2, 3, 3);
	bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
	toccata::TestPatternEvaluator::FreeMemorySpace(&request.Memory);

	EXPECT_TRUE(found);
	EXPECT_NEAR(output.s, 1.0, 1E-4);
	EXPECT_NEAR(output.t, 0.0, 1E-4);
}

TEST(TestPatternEvaluatorTest, TwoOptions) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0, 1 });
	reference.NoteContainer.AddPoint({ 1.0, 2 });
	reference.NoteContainer.AddPoint({ 2.0, 3 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.0, 1 });
	segment.NoteContainer.AddPoint({ 1.0, 2 });
	segment.NoteContainer.AddPoint({ 2.0, 3 });
	segment.NoteContainer.AddPoint({ 5.0, 1 });

	const int testPattern[] = { 0, 1 };

	int *const *notesByPitch = toccata::Memory::Allocate2d<int>(4, 3);
	notesByPitch[1][0] = 0; notesByPitch[1][1] = 3; notesByPitch[1][2] = -1;
	notesByPitch[2][0] = 1; notesByPitch[2][1] = -1;
	notesByPitch[3][0] = 2; notesByPitch[3][1] = -1;

	toccata::TestPatternEvaluator::Output output;
	toccata::TestPatternEvaluator::Request request;
	request.Segment = &segment;
	request.ReferenceSegment = &reference;
	request.TestPattern = testPattern;
	request.TestPatternLength = 2;
	request.SegmentNotesByPitch = notesByPitch;

	toccata::TestPatternEvaluator::AllocateMemorySpace(&request.Memory, 2, 3, 4);
	bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
	toccata::TestPatternEvaluator::FreeMemorySpace(&request.Memory);

	EXPECT_TRUE(found);
	EXPECT_NEAR(output.s, 1.0, 1E-4);
	EXPECT_NEAR(output.t, 0.0, 1E-4);
}

TEST(TestPatternEvaluatorTest, TwoCloseOptions) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0, 1 });
	reference.NoteContainer.AddPoint({ 1.0, 2 });
	reference.NoteContainer.AddPoint({ 2.0, 3 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ -0.1, 1 });
	segment.NoteContainer.AddPoint({ 0.0, 1 });
	segment.NoteContainer.AddPoint({ 2.0, 2 });
	segment.NoteContainer.AddPoint({ 4.0, 3 });

	const int testPattern[] = { 0, 1 };

	int *const *notesByPitch = toccata::Memory::Allocate2d<int>(4, 3);
	notesByPitch[1][0] = 0; notesByPitch[1][1] = 1; notesByPitch[1][2] = -1;
	notesByPitch[2][0] = 2; notesByPitch[2][1] = -1;
	notesByPitch[3][0] = 3; notesByPitch[3][1] = -1;

	toccata::TestPatternEvaluator::Output output;
	toccata::TestPatternEvaluator::Request request;
	request.Segment = &segment;
	request.ReferenceSegment = &reference;
	request.TestPattern = testPattern;
	request.TestPatternLength = 2;
	request.SegmentNotesByPitch = notesByPitch;

	toccata::TestPatternEvaluator::AllocateMemorySpace(&request.Memory, 2, 3, 4);
	bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
	toccata::TestPatternEvaluator::FreeMemorySpace(&request.Memory);

	EXPECT_TRUE(found);
	EXPECT_NEAR(output.s, 0.5, 1E-4);
	EXPECT_NEAR(output.t, 0.0, 1E-4);
}

TEST(TestPatternEvaluatorTest, OneBadPatternPoint) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0, 1 });
	reference.NoteContainer.AddPoint({ 1.0, 2 });
	reference.NoteContainer.AddPoint({ 2.0, 3 });
	reference.NoteContainer.AddPoint({ 3.0, 4 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.0, 1 });
	segment.NoteContainer.AddPoint({ 2.0, 2 });
	segment.NoteContainer.AddPoint({ 4.0, 3 });
	segment.NoteContainer.AddPoint({ 100.0, 4 });

	const int testPattern[] = { 0, 1, 3 };

	int *const *notesByPitch = toccata::Memory::Allocate2d<int>(5, 3);
	notesByPitch[1][0] = 0; notesByPitch[1][1] = -1;
	notesByPitch[2][0] = 1; notesByPitch[2][1] = -1;
	notesByPitch[3][0] = 2; notesByPitch[3][1] = -1;
	notesByPitch[4][0] = 3; notesByPitch[4][1] = -1;

	toccata::TestPatternEvaluator::Output output;
	toccata::TestPatternEvaluator::Request request;
	request.Segment = &segment;
	request.ReferenceSegment = &reference;
	request.TestPattern = testPattern;
	request.TestPatternLength = 3;
	request.SegmentNotesByPitch = notesByPitch;

	toccata::TestPatternEvaluator::AllocateMemorySpace(&request.Memory, 3, 4, 4);
	bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
	toccata::TestPatternEvaluator::FreeMemorySpace(&request.Memory);

	EXPECT_TRUE(found);
	EXPECT_NEAR(output.s, 0.5, 1E-4);
	EXPECT_NEAR(output.t, 0.0, 1E-4);
}

TEST(TestPatternEvaluatorTest, LargeData) {
	toccata::MusicSegment reference;
	toccata::MusicSegment segment;

	toccata::SegmentGenerator generator;
	generator.CreateRandomSegmentQuantized(&reference, 16, 16, 1.0, 256);
	generator.Copy(&reference, &segment);

	const int testPattern[] = { 3, 5, 10 };

	int **notesByPitch = toccata::Memory::Allocate2d<int>(256, 17);
	toccata::SegmentUtilities::SortByPitch(&segment, 256, notesByPitch);

	toccata::TestPatternEvaluator::Output output;
	toccata::TestPatternEvaluator::Request request;
	request.Segment = &segment;
	request.ReferenceSegment = &reference;
	request.TestPattern = testPattern;
	request.TestPatternLength = 3;
	request.SegmentNotesByPitch = notesByPitch;

	toccata::TestPatternEvaluator::AllocateMemorySpace(&request.Memory, 3, 16, 16);
	bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
	toccata::TestPatternEvaluator::FreeMemorySpace(&request.Memory);

	EXPECT_TRUE(found);
	EXPECT_NEAR(output.Error, 0.0, 1E-4);
	EXPECT_NEAR(output.s, 1.0, 1E-4);
	EXPECT_NEAR(output.t, 0.0, 1E-4);
}
