#include <pch.h>

#include "../include/munkres_solver.h"
#include "../include/memory.h"

TEST(MunkresSolverTest, SanityCheck) {
	double **cost = toccata::Memory::Allocate2d<double>(3, 3);
	bool **disallowed = toccata::Memory::Allocate2d<bool>(3, 3);

	cost[0][0] = 1.0;
	cost[0][1] = 2.0;
	cost[0][2] = 3.0;
	cost[1][0] = 2.0;
	cost[1][1] = 4.0;
	cost[1][2] = 6.0;
	cost[2][0] = 3.0;
	cost[2][1] = 6.0;
	cost[2][2] = 9.0;

	disallowed[0][0] = disallowed[0][1] = disallowed[0][2] = false;
	disallowed[1][0] = disallowed[1][1] = disallowed[1][2] = false;
	disallowed[2][0] = disallowed[2][1] = disallowed[2][2] = false;

	toccata::MunkresSolver::Request request;
	request.m = 3;
	request.n = 3;
	request.Target = new int[3];
	request.Costs = cost;
	request.DisallowedMappings = disallowed;
	toccata::MunkresSolver::AllocateMemorySpace(&request.Memory, 3, 3);
	toccata::MunkresSolver::InitializeRequest(&request);

	int *mapping = toccata::MunkresSolver::Solve(&request);

	toccata::MunkresSolver::FreeMemorySpace(&request.Memory);

	EXPECT_EQ(mapping[0], 2);
	EXPECT_EQ(mapping[1], 1);
	EXPECT_EQ(mapping[2], 0);
}

TEST(MunkresSolverTest, Rectangle1x2) {
	double **cost = toccata::Memory::Allocate2d<double>(1, 2);
	bool **disallowed = toccata::Memory::Allocate2d<bool>(1, 2);

	cost[0][0] = 1.0;
	cost[0][1] = 2.0;

	disallowed[0][0] = false;
	disallowed[0][1] = false;

	toccata::MunkresSolver::Request request;
	request.m = 2;
	request.n = 1;
	request.Target = new int[1];
	request.Costs = cost;
	request.DisallowedMappings = disallowed;
	toccata::MunkresSolver::AllocateMemorySpace(&request.Memory, 1, 2);
	toccata::MunkresSolver::InitializeRequest(&request);

	int *mapping = toccata::MunkresSolver::Solve(&request);

	toccata::MunkresSolver::FreeMemorySpace(&request.Memory);

	EXPECT_EQ(mapping[0], 0);
}

TEST(MunkresSolverTest, Rectangle1x2WithDisallowed) {
	double **cost = toccata::Memory::Allocate2d<double>(1, 2);
	bool **disallowed = toccata::Memory::Allocate2d<bool>(1, 2);

	cost[0][0] = 1.0;
	cost[0][1] = 2.0;

	disallowed[0][0] = true;
	disallowed[0][1] = false;

	toccata::MunkresSolver::Request request;
	request.m = 2;
	request.n = 1;
	request.Target = new int[1];
	request.Costs = cost;
	request.DisallowedMappings = disallowed;
	toccata::MunkresSolver::AllocateMemorySpace(&request.Memory, 1, 2);
	toccata::MunkresSolver::InitializeRequest(&request);

	int *mapping = toccata::MunkresSolver::Solve(&request);

	toccata::MunkresSolver::FreeMemorySpace(&request.Memory);

	EXPECT_EQ(mapping[0], 1);
}

TEST(MunkresSolverTest, Rectangle2x1) {
	double **cost = toccata::Memory::Allocate2d<double>(2, 2);
	bool **disallowed = toccata::Memory::Allocate2d<bool>(2, 2);

	cost[0][0] = 2.0;
	cost[1][0] = 1.0;

	// Padding
	cost[0][1] = 0.0;
	cost[1][1] = 0.0;

	disallowed[0][0] = false;
	disallowed[1][0] = false;

	// Padding
	disallowed[0][1] = true;
	disallowed[1][1] = true;

	toccata::MunkresSolver::Request request;
	request.m = 2;
	request.n = 2;
	request.Target = new int[2];
	request.Costs = cost;
	request.DisallowedMappings = disallowed;
	toccata::MunkresSolver::AllocateMemorySpace(&request.Memory, 2, 2);
	toccata::MunkresSolver::InitializeRequest(&request);

	int *mapping = toccata::MunkresSolver::Solve(&request);

	toccata::MunkresSolver::FreeMemorySpace(&request.Memory);

	EXPECT_EQ(mapping[1], 0);
	EXPECT_EQ(mapping[0], -1);
}

TEST(MunkresSolverTest, LargeData) {
	double **cost = toccata::Memory::Allocate2d<double>(16, 16);
	bool **disallowed = toccata::Memory::Allocate2d<bool>(16, 16);

	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 16; ++j) {
			cost[i][j] = (double)i + j;
			disallowed[i][j] = false;
		}
	}

	toccata::MunkresSolver::Request request;
	request.m = 16;
	request.n = 16;
	request.Target = new int[16];
	request.Costs = cost;
	request.DisallowedMappings = disallowed;
	toccata::MunkresSolver::AllocateMemorySpace(&request.Memory, 16, 16);
	toccata::MunkresSolver::InitializeRequest(&request);

	int *mapping = toccata::MunkresSolver::Solve(&request);

	toccata::MunkresSolver::FreeMemorySpace(&request.Memory);
}
