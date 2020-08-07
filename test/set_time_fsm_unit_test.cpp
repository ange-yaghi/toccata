#include <pch.h>

#include "../include/set_time_fsm.h"

void Procedure_SetMeter(toccata::SetTimeFsm *fsm, int meter, int tapCount = 1) {
	int digits[5];
	int digitCount = 0;

	do {
		int digit = meter % 10;
		meter /= 10;
		digits[digitCount] = digit;

		digitCount++;
	} while (meter > 0);

	// Activate using the highest B key
	fsm->Run(107, 10);

	for (int j = 0; j < tapCount; j++) {
		for (int i = digitCount - 1; i >= 0; i--) {
			// Press the key
			fsm->Run((digits[i] + j) % 10 + 65, 10);

			// Release the key
			fsm->Run((digits[i] + j) % 10 + 65, 0);
		}
	}

	// Release the B key
	fsm->Run(107, 0);
}

void Procedure_SwitchMetronome(toccata::SetTimeFsm *fsm) {
	// Activate using the highest B key
	fsm->Run(107, 10);

	// Press the key
	fsm->Run(64, 10);

	// Release the key
	fsm->Run(64, 0);

	// Release the B key
	fsm->Run(107, 0);
}

TEST(SetTimeFsmTest, InitializeCheck) {
	toccata::SetTimeFsm fsm;

	EXPECT_EQ(fsm.IsEnabled(), false);
	EXPECT_EQ(fsm.GetCurrentMeter(), 0);
	EXPECT_EQ(fsm.IsStateChanged(), false);
	EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
}

TEST(SetTimeFsmTest, EnableCheck) {
	toccata::SetTimeFsm fsm;

	// Check initial state
	EXPECT_EQ(fsm.IsEnabled(), false);
	
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	fsm.SetEnable(false);
	EXPECT_EQ(fsm.IsEnabled(), false);
}

TEST(SetTimeFsmTest, SimpleSetupTest) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	Procedure_SetMeter(&fsm, 4);

	EXPECT_EQ(fsm.IsStateChanged(), true);

	// Extract the data
	fsm.Lock();
	EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
	EXPECT_EQ(fsm.GetCurrentMeter(), 4);

	EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
	fsm.OnUpdate();
	fsm.Unlock();
}

TEST(SetTimeFsmTest, DisabledTest) {
	toccata::SetTimeFsm fsm;

	// Disable the FSM
	fsm.SetEnable(false);
	EXPECT_EQ(fsm.IsEnabled(), false);

	Procedure_SetMeter(&fsm, 4);

	EXPECT_EQ(fsm.IsStateChanged(), false);

	// Extract the data
	fsm.Lock();
	EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
	EXPECT_EQ(fsm.GetCurrentMeter(), 0);

	EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
	fsm.OnUpdate();
	fsm.Unlock();
}

TEST(SetTimeFsmTest, StressTest) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++) {
		int newMeter = i % 11;

		if (newMeter < 10) {
			Procedure_SetMeter(&fsm, newMeter);
			lastMeter = newMeter;

			if (newMeter == 0) {
				EXPECT_EQ(fsm.IsStateChanged(), true);

				// Extract the data
				fsm.Lock();
				EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
				EXPECT_EQ(fsm.GetCurrentMeter(), newMeter);

				EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
				fsm.OnUpdate();
				fsm.Unlock();
			}
			else {
				EXPECT_EQ(fsm.IsStateChanged(), true);

				// Extract the data
				fsm.Lock();
				EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
				EXPECT_EQ(fsm.GetCurrentMeter(), newMeter);

				EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
				fsm.OnUpdate();
				fsm.Unlock();
			}
		}
		else {
			Procedure_SwitchMetronome(&fsm);

			EXPECT_EQ(fsm.IsStateChanged(), true);

			// Extract the data
			fsm.Lock();
			EXPECT_EQ(fsm.GetMetronomeEnabled(), true);
			EXPECT_EQ(fsm.GetCurrentMeter(), lastMeter);

			EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
			fsm.OnUpdate();
			fsm.Unlock();
		}
	}
}

TEST(SetTimeFsmTest, DoubleTap) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++) {
		// User sets the meter three times before it's actually read
		Procedure_SetMeter(&fsm, i % 10);
		Procedure_SetMeter(&fsm, (i+1) % 10);
		Procedure_SetMeter(&fsm, (i+2) % 10);

		// Extract the data, make sure it's set to the last one set by the user
		fsm.Lock();
		EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
		EXPECT_EQ(fsm.GetCurrentMeter(), (i + 2) % 10);

		EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
		fsm.OnUpdate();
		fsm.Unlock();
	}
}

TEST(SetTimeFsmTest, DoubleTap_NoEnableKey) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++) {
		// User sets the meter three times before it's actually read
		Procedure_SetMeter(&fsm, i % 10, 3);

		// Extract the data, make sure it's set to the last one set by the user
		fsm.Lock();
		EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
		EXPECT_EQ(fsm.GetCurrentMeter(), (i + 2) % 10);

		EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
		fsm.OnUpdate();
		fsm.Unlock();
	}
}

TEST(SetTimeFsmTest, DoubleTap_Compound) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	int lastMeter = 0;

	// Press activate key
	fsm.Run(107, 10);

	// Enable metronome
	fsm.Run(64, 10);
	fsm.Run(64, 0);

	// Set meter to 4
	fsm.Run(65 + 4, 10);
	fsm.Run(65 + 4, 0);

	// Extract the data, make sure it's set to the last one set by the user
	fsm.Lock();
	EXPECT_EQ(fsm.GetMetronomeEnabled(), true);
	EXPECT_EQ(fsm.GetCurrentMeter(), 4);

	EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
	fsm.OnUpdate();
	fsm.Unlock();
}

TEST(SetTimeFsmTest, DoubleTap_CompoundReadMidle) {
	toccata::SetTimeFsm fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	EXPECT_EQ(fsm.IsEnabled(), true);

	int lastMeter = 0;

	// Press activate key
	fsm.Run(107, 10);

	// Enable metronome
	fsm.Run(64, 10);
	fsm.Run(64, 0);

	// Extract the data, this occurs before the entire operation is done
	fsm.Lock();
	EXPECT_EQ(fsm.GetMetronomeEnabled(), true);
	EXPECT_EQ(fsm.GetCurrentMeter(), 0);

	EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
	fsm.OnUpdate();
	fsm.Unlock();

	// Set meter to 4
	fsm.Run(65 + 4, 10);
	fsm.Run(65 + 4, 0);

	// Release activate key
	fsm.Run(107, 10);

	// Extract the data, this occurs after the entire operation is done
	fsm.Lock();
	EXPECT_EQ(fsm.GetMetronomeEnabled(), false);
	EXPECT_EQ(fsm.GetCurrentMeter(), 4);

	EXPECT_EQ(fsm.ClearFlag(), toccata::SetTimeFsm::ErrorCode::None);
	fsm.OnUpdate();
	fsm.Unlock();
}
