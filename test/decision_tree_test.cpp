#include <pch.h>

#include "../include/decision_tree.h"

TEST(DecisionTreeTest, SanityCheck) {
	toccata::DecisionTree tree;
	tree.Initialize(1);
	tree.SpawnThreads();

	for (int i = 0; i < 1; ++i) {
		tree.Process();
	}

	tree.KillThreads();
	tree.Destroy();
}

TEST(DecisionTreeTest, MultipleThreads) {
	toccata::DecisionTree tree;
	tree.Initialize(24);
	tree.SpawnThreads();

	for (int i = 0; i < 1000; ++i) {
		tree.Process();
	}

	tree.KillThreads();
	tree.Destroy();
}
