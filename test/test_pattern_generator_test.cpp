#include <pch.h>

#include "../include/test_pattern_generator.h"

bool CheckPattern(const int *pattern, int patternSize) {
	for (int i = 0; i < patternSize; ++i) {
		for (int j = i + 1; j < patternSize; ++j) {
			if (pattern[i] == pattern[j]) return false;
		}
	}

	return true;
}

TEST(TestPatternGeneratorTest, SanityCheck) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);
	
	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[10];
	request.RequestedPatternSize = 4;
	request.NoteCount = 10;

	int patternSize = generator.FindRandomTestPattern(request);

	EXPECT_EQ(patternSize, 4);

	delete[] request.Buffer;
}

TEST(TestPatternGeneratorTest, NoRepititions) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);

	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[10];
	request.RequestedPatternSize = 4;
	request.NoteCount = 10;

	int patternSize = generator.FindRandomTestPattern(request);

	EXPECT_EQ(patternSize, 4);
	EXPECT_TRUE(CheckPattern(request.Buffer, patternSize));

	delete[] request.Buffer;
}

TEST(TestPatternGeneratorTest, ExtremeTestPattern) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);

	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[10];
	request.RequestedPatternSize = 4;
	request.NoteCount = 10;

	int patternSize = generator.FindExtremeTestPattern(request);

	EXPECT_EQ(patternSize, 4);
	EXPECT_TRUE(CheckPattern(request.Buffer, patternSize));

	EXPECT_EQ(request.Buffer[0], 0);
	EXPECT_EQ(request.Buffer[1], request.NoteCount - 1);

	delete[] request.Buffer;
}

TEST(TestPatternGeneratorTest, ClippedPatternExtreme) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);

	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[3];
	request.RequestedPatternSize = 4;
	request.NoteCount = 3;

	int patternSize = generator.FindExtremeTestPattern(request);

	EXPECT_EQ(patternSize, 3);
	EXPECT_TRUE(CheckPattern(request.Buffer, patternSize));

	delete[] request.Buffer;
}

TEST(TestPatternGeneratorTest, ClippedPatternRandom) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);

	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[3];
	request.RequestedPatternSize = 4;
	request.NoteCount = 3;

	int patternSize = generator.FindRandomTestPattern(request);

	EXPECT_EQ(patternSize, 3);
	EXPECT_TRUE(CheckPattern(request.Buffer, patternSize));

	delete[] request.Buffer;
}

TEST(TestPatternGeneratorTest, StressTest) {
	toccata::TestPatternGenerator generator;
	generator.Seed(0);

	toccata::TestPatternGenerator::TestPatternRequest request;
	request.Buffer = new int[10];
	request.RequestedPatternSize = 4;
	request.NoteCount = 10;

	for (int i = 0; i < 1000; ++i) {
		int patternSize = generator.FindRandomTestPattern(request);

		EXPECT_EQ(patternSize, 4);
		EXPECT_TRUE(CheckPattern(request.Buffer, patternSize));
	}

	delete[] request.Buffer;
}
