#include "../include/basic_solve_benchmark.h"

#include "../../include/memory.h"
#include "../../include/test_pattern_evaluator.h"
#include "../../include/segment_utilities.h"
#include "../../include/segment_generator.h"
#include "../../include/test_pattern_generator.h"
#include "../../include/math.h"
#include "../../include/nls_optimizer.h"
#include "../../include/transform.h"

#include <chrono>
#include <iostream>

toccata::BasicSolveBenchmark::BasicSolveBenchmark() {
    /* void */
}

toccata::BasicSolveBenchmark::~BasicSolveBenchmark() {
    /* void */
}

void toccata::BasicSolveBenchmark::Run() {
	constexpr int Iterations = 10000;

	int **notesByPitch = Memory::Allocate2d<int>(256, 100);
	TestPatternEvaluator::Request::MemorySpace memorySpace;
	TestPatternEvaluator::AllocateMemorySpace(&memorySpace, 16, 100, 100);

	TestPatternGenerator testPatternGenerator;
	int *testPatternBuffer = Memory::Allocate<int>(100);

	SegmentGenerator generator;
	generator.Seed(0);

	MusicSegment reference;
	MusicSegment segment;

	double cumulativeScaleErrRough = 0.0;
	double cumulativeShiftErrRough = 0.0;

	double cumulativeScaleErrRefined = 0.0;
	double cumulativeShiftErrRefined = 0.0;

	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < Iterations; ++i) {
		if (i % 1000 == 0) {
			std::cout << "Iteration: " << i << "\n";
		}

		reference.NoteContainer.Clear();
		segment.NoteContainer.Clear();

		generator.CreateRandomSegmentQuantized(&reference, 16, 16, 1.0, 10);

		generator.Copy(&reference, &segment);
		generator.Jitter(&segment, 0.125);
		generator.Scale(&segment, 5.0);
		generator.Shift(&segment, 2.0);

		SegmentUtilities::SortByPitch(&segment, 0, 15, 10, notesByPitch);

		TestPatternGenerator::TestPatternRequest patternRequest;
		patternRequest.NoteCount = 16;
		patternRequest.Buffer = testPatternBuffer;
		patternRequest.RequestedPatternSize = 4;

		testPatternGenerator.FindRandomTestPattern(patternRequest);

		toccata::TestPatternEvaluator::Output output;
		toccata::TestPatternEvaluator::Request request;
		request.Segment = &segment;
		request.ReferenceSegment = &reference;
		request.TestPattern = testPatternBuffer;
		request.TestPatternLength = 4;
		request.SegmentNotesByPitch = notesByPitch;
		request.Memory = memorySpace;
		
		bool found = toccata::TestPatternEvaluator::Solve(request, &output);

		double current_s = output.s;
		double current_t = output.t;

		for (int j = 0; j < 1; ++j) {
			toccata::NoteMapper::InjectiveMappingRequest mappingRequest;
			mappingRequest.CorrelationThreshold = 0.2;
			mappingRequest.ReferenceSegment = &reference;
			mappingRequest.Segment = &segment;
			mappingRequest.Target = memorySpace.Mapping;
			mappingRequest.Memory = memorySpace.MappingMemory;
			mappingRequest.s = current_s;
			mappingRequest.t = current_t;

			const int *preciseMapping = toccata::NoteMapper::GetInjectiveMapping(&mappingRequest);

			int validPointCount = 0;
			const int n = reference.NoteContainer.GetCount();
			double *r = memorySpace.r;
			double *p = memorySpace.p;
			const MusicPoint *referencePoints = reference.NoteContainer.GetPoints();
			const MusicPoint *points = segment.NoteContainer.GetPoints();
			for (int i = 0; i < n; ++i) {
				if (preciseMapping[i] != -1) {
					const int noteIndex = preciseMapping[i];

					const MusicPoint &referencePoint = referencePoints[i];
					const MusicPoint &point = points[noteIndex];

					r[validPointCount] = referencePoint.Timestamp;
					p[validPointCount] = point.Timestamp;

					++validPointCount;
				}
			}

			toccata::NlsOptimizer::Solution refinedSolution;
			toccata::NlsOptimizer::Problem refineStepRequest;
			refineStepRequest.N = validPointCount;
			refineStepRequest.r_set = r;
			refineStepRequest.p_set = p;
			toccata::NlsOptimizer::Solve(refineStepRequest, &refinedSolution);

			current_s = refinedSolution.s;
			current_t = refinedSolution.t;
		}

		if (Math::Abs(output.s) < 1E-4) {
			continue;
		}
		
		cumulativeScaleErrRefined += Math::Abs(current_s - (1 / 5.0));
		cumulativeShiftErrRefined += Math::Abs(current_t - (-2.0 / 5.0));

		cumulativeScaleErrRough += Math::Abs(output.s - (1 / 5.0));
		cumulativeShiftErrRough += Math::Abs(output.t - (-2.0 / 5.0));
	}
	auto end = std::chrono::steady_clock::now();

	toccata::TestPatternEvaluator::FreeMemorySpace(&memorySpace);

	std::cout << "Scale err refined: " << cumulativeScaleErrRefined / Iterations << "\n";
	std::cout << "Shift err refined: " << cumulativeShiftErrRefined / Iterations << "\n";
	std::cout << "Scale err rough: " << cumulativeScaleErrRough / Iterations << "\n";
	std::cout << "Shift err rough: " << cumulativeShiftErrRough / Iterations << "\n";
	std::cout << "Test took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
		<< " ms for " << Iterations << " iterations\n";

	char e;
	std::cin >> e;
}
