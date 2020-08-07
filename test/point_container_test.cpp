#include <pch.h>

#include "../include/music_point_container.h"

TEST(PointContainerTest, SanityCheck) {
	toccata::MusicPointContainer container;
	
	EXPECT_EQ(container.GetCount(), 0);
}

TEST(PointContainerTest, AddTest) {
	toccata::MusicPointContainer container;
	container.AddPoint({ 0.0 });
	container.AddPoint({ 1.0 });

	EXPECT_EQ(container.GetCount(), 2);
}

TEST(PointContainerTest, RemoveTest) {
	toccata::MusicPointContainer container;
	container.AddPoint({ 0.0 });
	container.AddPoint({ 2.0 });
	container.AddPoint({ 1.0 });

	EXPECT_DOUBLE_EQ(container.GetPoints()[0].Timestamp, 0.0);
	EXPECT_DOUBLE_EQ(container.GetPoints()[1].Timestamp, 1.0);
	EXPECT_DOUBLE_EQ(container.GetPoints()[2].Timestamp, 2.0);

	container.RemovePoint(1);
	EXPECT_EQ(container.GetCount(), 2);

	EXPECT_DOUBLE_EQ(container.GetPoints()[0].Timestamp, 0.0);
	EXPECT_DOUBLE_EQ(container.GetPoints()[1].Timestamp, 2.0);

	container.RemovePoint(0);
	container.RemovePoint(0);

	EXPECT_EQ(container.GetCount(), 0);
}
