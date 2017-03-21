#include <boost/test/unit_test.hpp>

#include <toccata_set_time_fsm.h>

BOOST_AUTO_TEST_SUITE(Toccata_SetTimeUnitTesting);

void Procedure_SetMeter(Toccata_SetTimeFSM *fsm, int meter, int tapCount = 1)
{

	int digits[5];
	int digitCount = 0;

	do
	{

		int digit = meter % 10;
		meter /= 10;
		digits[digitCount] = digit;

		digitCount++;

	} while (meter > 0);

	// Activate using the highest B key
	fsm->Run(107, 10);

	for (int j = 0; j < tapCount; j++)
	{

		for (int i = digitCount - 1; i >= 0; i--)
		{

			// Press the key
			fsm->Run((digits[i] + j) % 10 + 65, 10);

			// Release the key
			fsm->Run((digits[i] + j) % 10 + 65, 0);

		}

	}

	// Release the B key
	fsm->Run(107, 0);

}

void Procedure_SwitchMetronome(Toccata_SetTimeFSM *fsm)
{

	// Activate using the highest B key
	fsm->Run(107, 10);

	// Press the key
	fsm->Run(64, 10);

	// Release the key
	fsm->Run(64, 0);

	// Release the B key
	fsm->Run(107, 0);

}

BOOST_AUTO_TEST_CASE(InitializeCheck)
{

	Toccata_SetTimeFSM fsm;

	BOOST_TEST(fsm.IsEnabled() == false);
	BOOST_TEST(fsm.GetCurrentMeter() == 0);
	BOOST_TEST(fsm.IsStateChanged() == false);
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);

}

BOOST_AUTO_TEST_CASE(EnableCheck)
{

	Toccata_SetTimeFSM fsm;

	// Check initial state
	BOOST_TEST(fsm.IsEnabled() == false);
	
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	fsm.SetEnable(false);
	BOOST_TEST(fsm.IsEnabled() == false);

}

BOOST_AUTO_TEST_CASE(SimpleSetupTest)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	Procedure_SetMeter(&fsm, 4);

	BOOST_TEST(fsm.IsStateChanged() == true);

	// Extract the data
	fsm.Lock();
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
	BOOST_TEST(fsm.GetCurrentMeter() == 4);

	BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
	fsm.OnUpdate();
	fsm.Unlock();

}

BOOST_AUTO_TEST_CASE(DisabledTest)
{

	Toccata_SetTimeFSM fsm;

	// Disable the FSM
	fsm.SetEnable(false);
	BOOST_TEST(fsm.IsEnabled() == false);

	Procedure_SetMeter(&fsm, 4);

	BOOST_TEST(fsm.IsStateChanged() == false);

	// Extract the data
	fsm.Lock();
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
	BOOST_TEST(fsm.GetCurrentMeter() == 0);

	BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
	fsm.OnUpdate();
	fsm.Unlock();

}

BOOST_AUTO_TEST_CASE(StressTest)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++)
	{

		int newMeter = i % 11;

		if (newMeter < 10)
		{

			Procedure_SetMeter(&fsm, newMeter);
			lastMeter = newMeter;

			if (newMeter == 0)
			{

				BOOST_TEST(fsm.IsStateChanged() == true);

				// Extract the data
				fsm.Lock();
				BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
				BOOST_TEST(fsm.GetCurrentMeter() == newMeter);

				BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
				fsm.OnUpdate();
				fsm.Unlock();

			}

			else
			{

				BOOST_TEST(fsm.IsStateChanged() == true);

				// Extract the data
				fsm.Lock();
				BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
				BOOST_TEST(fsm.GetCurrentMeter() == newMeter);

				BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
				fsm.OnUpdate();
				fsm.Unlock();

			}

		}

		else
		{

			Procedure_SwitchMetronome(&fsm);

			BOOST_TEST(fsm.IsStateChanged() == true);

			// Extract the data
			fsm.Lock();
			BOOST_TEST(fsm.SwitchMetronomeOnOff() == true);
			BOOST_TEST(fsm.GetCurrentMeter() == lastMeter);

			BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
			fsm.OnUpdate();
			fsm.Unlock();

		}

	}

}

BOOST_AUTO_TEST_CASE(DoubleTap)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++)
	{

		// User sets the meter three times before it's actually read
		Procedure_SetMeter(&fsm, i % 10);
		Procedure_SetMeter(&fsm, (i+1) % 10);
		Procedure_SetMeter(&fsm, (i+2) % 10);

		// Extract the data, make sure it's set to the last one set by the user
		fsm.Lock();
		BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
		BOOST_TEST(fsm.GetCurrentMeter() == (i + 2) % 10);

		BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
		fsm.OnUpdate();
		fsm.Unlock();

	}

}

BOOST_AUTO_TEST_CASE(DoubleTap_NoEnableKey)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	int lastMeter = 0;

	for (int i = 0; i < 10000; i++)
	{

		// User sets the meter three times before it's actually read
		Procedure_SetMeter(&fsm, i % 10, 3);

		// Extract the data, make sure it's set to the last one set by the user
		fsm.Lock();
		BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
		BOOST_TEST(fsm.GetCurrentMeter() == (i + 2) % 10);

		BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
		fsm.OnUpdate();
		fsm.Unlock();

	}

}

BOOST_AUTO_TEST_CASE(DoubleTap_Compound)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

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
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == true);
	BOOST_TEST(fsm.GetCurrentMeter() == 4);

	BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
	fsm.OnUpdate();
	fsm.Unlock();

}

BOOST_AUTO_TEST_CASE(DoubleTap_CompoundReadMidle)
{

	Toccata_SetTimeFSM fsm;

	// Enable the FSM
	fsm.SetEnable(true);
	BOOST_TEST(fsm.IsEnabled() == true);

	int lastMeter = 0;

	// Press activate key
	fsm.Run(107, 10);

	// Enable metronome
	fsm.Run(64, 10);
	fsm.Run(64, 0);

	// Extract the data, this occurs before the entire operation is done
	fsm.Lock();
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == true);
	BOOST_TEST(fsm.GetCurrentMeter() == 0);

	BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
	fsm.OnUpdate();
	fsm.Unlock();

	// Set meter to 4
	fsm.Run(65 + 4, 10);
	fsm.Run(65 + 4, 0);

	// Release activate key
	fsm.Run(107, 10);

	// Extract the data, this occurs after the entire operation is done
	fsm.Lock();
	BOOST_TEST(fsm.SwitchMetronomeOnOff() == false);
	BOOST_TEST(fsm.GetCurrentMeter() == 4);

	BOOST_TEST(fsm.ClearFlag() == Toccata_SetTimeFSM::ERROR_NONE);
	fsm.OnUpdate();
	fsm.Unlock();

}

BOOST_AUTO_TEST_SUITE_END();