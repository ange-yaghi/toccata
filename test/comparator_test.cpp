#include <pch.h>

#include "../include/comparator.h"

TEST(ComparatorTest, SanityCheck) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0 });
	reference.NoteContainer.AddPoint({ 1.0 });
	reference.NoteContainer.AddPoint({ 2.0 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.0 });
	segment.NoteContainer.AddPoint({ 1.0 });
	segment.NoteContainer.AddPoint({ 2.0 });

	int mapping[] = { 0, 1, 2 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.s = 1.0;
	comparatorRequest.t = 0.0;
	bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(result.AverageError, 0.0, 1E-4);
}

TEST(ComparatorTest, MissingMappings) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0 });
	reference.NoteContainer.AddPoint({ 1.0 });
	reference.NoteContainer.AddPoint({ 2.0 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.5 });
	segment.NoteContainer.AddPoint({ 1.5 });
	segment.NoteContainer.AddPoint({ 2.75 });

	int mapping[] = { 0, -1, 2 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.s = 1.0;
	comparatorRequest.t = 0.0;
	bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(result.AverageError, 1.25 / 2, 1E-4);
}

TEST(ComparatorTest, NoMappings) {
	toccata::MusicSegment reference;
	reference.Length = 1.0f;
	reference.NoteContainer.AddPoint({ 0.0 });
	reference.NoteContainer.AddPoint({ 1.0 });
	reference.NoteContainer.AddPoint({ 2.0 });

	toccata::MusicSegment segment;
	segment.Length = 1.0f;
	segment.NoteContainer.AddPoint({ 0.5 });
	segment.NoteContainer.AddPoint({ 1.5 });
	segment.NoteContainer.AddPoint({ 2.75 });

	int mapping[] = { -1, -1, -1 };

	toccata::Comparator::Result result;
	toccata::Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = mapping;
	comparatorRequest.Reference = &reference;
	comparatorRequest.Segment = &segment;
	comparatorRequest.s = 1.0;
	comparatorRequest.t = 0.0;
	bool valid = toccata::Comparator::CalculateError(comparatorRequest, &result);

	EXPECT_FALSE(valid);
}
