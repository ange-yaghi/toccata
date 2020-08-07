#include <pch.h>

#include "../include/nls_optimizer.h"

TEST(NlsOptimizerTest, SanityCheck) {
	double r[] = { 0.0, 1.0 };
	double p[] = { 0.0, 5.0 };
	
	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 2;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(solution.s, 0.2, 1E-4);
	EXPECT_NEAR(solution.t, 0.0, 1E-4);
}

TEST(NlsOptimizerTest, Test3Points) {
	double r[] = { 0.0, 1.0, 10.0 };
	double p[] = { 0.0, 5.0, 50.0 };

	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 3;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(solution.s, 0.2, 1E-4);
	EXPECT_NEAR(solution.t, 0.0, 1E-4);
}

TEST(NlsOptimizerTest, Test3PointsScaleOffset) {
	double scale = 5.0;
	double offset = 10.0;

	double r[] = { 0.0, 1.0, 10.0 };
	double p[] = { 0.0 + offset, 1.0 * scale + offset, 10.0 * scale + offset };

	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 3;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(solution.s, 1 / scale, 1E-4);
	EXPECT_NEAR(solution.t, -offset / scale, 1E-4);
}

TEST(NlsOptimizerTest, TestBadReference) {
	double r[] = { 1.0, 1.0 };
	double p[] = { 2.0, 4.0 };

	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 2;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(solution.s, 0.0, 1E-4);
	EXPECT_NEAR(solution.t, 1.0, 1E-4);
}

TEST(NlsOptimizerTest, TestImperfectFit) {
	double r[] = { 0.0, 1.0, 2.0 };
	double p[] = { 0.0, 1.25, 2.5 };

	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 2;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_TRUE(valid);
	EXPECT_NEAR(solution.s, 0.8, 1E-4);
	EXPECT_NEAR(solution.t, 0.0, 1E-4);
}

TEST(NlsOptimizerTest, TestMalformed) {
	double r[] = { 0.0, 1.0, 2.0 };
	double p[] = { 0.0, 0.0, 0.0 };

	toccata::NlsOptimizer::Solution solution;
	toccata::NlsOptimizer::Problem problem;
	problem.r_set = r;
	problem.p_set = p;
	problem.N = 2;

	bool valid = toccata::NlsOptimizer::Solve(problem, &solution);

	EXPECT_FALSE(valid);
}
