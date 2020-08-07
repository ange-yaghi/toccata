#include "../include/basic_solve_benchmark.h"

#include "../../include/memory.h"
#include "../../include/test_pattern_evaluator.h"
#include "../../include/segment_utilities.h"
#include "../../include/segment_generator.h"
#include "../../include/test_pattern_generator.h"
#include "../../include/math.h"

#include <chrono>
#include <iostream>

toccata::BasicSolveBenchmark::BasicSolveBenchmark() {
    /* void */
}

toccata::BasicSolveBenchmark::~BasicSolveBenchmark() {
    /* void */
}

void toccata::BasicSolveBenchmark::Run() {
	constexpr int Iterations = 100000;

	int **notesByPitch = Memory::Allocate2d<int>(256, 100);
	TestPatternEvaluator::Request::MemorySpace memorySpace;
	TestPatternEvaluator::AllocateMemorySpace(&memorySpace, 5, 100, 100);

	TestPatternGenerator testPatternGenerator;
	int *testPatternBuffer = Memory::Allocate<int>(100);

	SegmentGenerator generator;
	generator.Seed(0);

	MusicSegment reference;
	MusicSegment segment;

	double cumulativeScaleErr = 0.0;
	double cumulativeShiftErr = 0.0;

	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < Iterations; ++i) {
		if (i % 1000 == 0) {
			std::cout << "Iteration: " << i << "\n";
		}

		reference.NoteContainer.Clear();
		segment.NoteContainer.Clear();

		generator.CreateRandomSegmentQuantized(&reference, 16, 16, 1.0, 10);

		generator.Copy(&reference, &segment);
		generator.Jitter(&segment, 0.05);
		generator.Scale(&segment, 5.0);
		generator.Shift(&segment, 2.0);

		SegmentUtilities::SortByPitch(&reference, 10, notesByPitch);

		TestPatternGenerator::TestPatternRequest patternRequest;
		patternRequest.NoteCount = 16;
		patternRequest.Buffer = testPatternBuffer;
		patternRequest.RequestedPatternSize = 3;
		
		testPatternGenerator.FindRandomTestPattern(patternRequest);

		toccata::TestPatternEvaluator::Output output;
		toccata::TestPatternEvaluator::Request request;
		request.Segment = &segment;
		request.ReferenceSegment = &reference;
		request.TestPattern = testPatternBuffer;
		request.TestPatternLength = 3;
		request.SegmentNotesByPitch = notesByPitch;
		request.Memory = memorySpace;
		
		bool found = toccata::TestPatternEvaluator::FindBestSolution(request, &output);
		
		cumulativeScaleErr += Math::Abs(output.s - (1 / 5.0));
		cumulativeShiftErr += Math::Abs(output.t - (-2.0 / 5.0));
	}
	auto end = std::chrono::steady_clock::now();

	toccata::TestPatternEvaluator::FreeMemorySpace(&memorySpace);

	std::cout << "Scale err: " << cumulativeScaleErr / Iterations << "\n";
	std::cout << "Shift err: " << cumulativeShiftErr / Iterations << "\n";
	std::cout << "Test took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		<< " ms for " << Iterations << " iterations\n";
}
