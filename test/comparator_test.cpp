#include <pch.h>

#include "../include/comparator.h"

TEST(ComparatorTest, SanityCheck) {
	toccata::MusicSegment reference;
	reference.PulseUnit = 1.0;
	reference.NoteContainer.AddPoint({ 0 });
	reference.NoteContainer.AddPoint({ 1 });
	reference.NoteContainer.AddPoint({ 2 });

	toccata::MusicSegment segment;
	segment.PulseUnit = 1.0;
	segment.NoteContainer.AddPoint({ 0 });
	segment.NoteContainer.AddPoint({ 1 });
	segment.NoteContainer.AddPoint({ 2 });

	int mapping[] = { 0, 1, 2 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.T.s = 1.0;
	comparatorRequest.T.t = 0.0;
	comparatorRequest.T.t_coarse = 0;
	const bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(result.AverageError, 0.0, 1E-4);
}

TEST(ComparatorTest, MissingMappings) {
	toccata::MusicSegment reference;
	reference.PulseUnit = 100.0;
	reference.NoteContainer.AddPoint({ 0 });
	reference.NoteContainer.AddPoint({ 100 });
	reference.NoteContainer.AddPoint({ 200 });

	toccata::MusicSegment segment;
	segment.PulseUnit = 100.0;
	segment.NoteContainer.AddPoint({ 50 });
	segment.NoteContainer.AddPoint({ 150 });
	segment.NoteContainer.AddPoint({ 275 });

	int mapping[] = { 0, -1, 2 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.T.s = 1.0;
	comparatorRequest.T.t = 0.0;
	comparatorRequest.T.t_coarse = 0;
	const bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(result.AverageError, 1.25 / 2, 1E-4);
}

TEST(ComparatorTest, NoMappings) {
	toccata::MusicSegment reference;
	reference.PulseUnit = 1.0;
	reference.NoteContainer.AddPoint({ 0 });
	reference.NoteContainer.AddPoint({ 10 });
	reference.NoteContainer.AddPoint({ 20 });

	toccata::MusicSegment segment;
	segment.PulseUnit = 1.0;
	segment.NoteContainer.AddPoint({ 5 });
	segment.NoteContainer.AddPoint({ 15 });
	segment.NoteContainer.AddPoint({ 28 });

	int mapping[] = { -1, -1, -1 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.T.s = 1.0;
	comparatorRequest.T.t = 0.0;
	comparatorRequest.T.t_coarse = 0;
	const bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_FALSE(valid);
}
