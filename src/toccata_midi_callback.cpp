#include "toccata_midi_callback.h"

#include "toccata_core.h"
#include "toccata_set_tempo_fsm.h"
#include "toccata_set_time_fsm.h"

#include <atomic>

#define DEBUG_PRINT_DISABLED 0

void CALLBACK Toccata_MIDI_Callback(
	HMIDIIN   hMidiIn,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
	)
{

	//printf("MIDI message received: 0x%X\n", wMsg);

	uint64_t programTimeStamp = Toccata.GetTimeStamp();

	Toccata.GetRecorder()->GetRawOutput()->WriteLine(wMsg, dwParam1, dwParam2);

	if (wMsg == MIM_DATA)
	{

		DWORD dwMidiMessage = dwParam1;
		DWORD dwTimestamp = dwParam2;

		WORD hiWord = HIWORD(dwMidiMessage);
		WORD loWord = LOWORD(dwMidiMessage);

		BYTE midiByte1 = HIBYTE(loWord);
		BYTE midiByte2 = LOBYTE(hiWord);
		BYTE midiStatus = LOBYTE(loWord);

		if (midiStatus == 0x80)
		{

			// Note off

			int key = midiByte1;
			int velocity = midiByte2;

#ifdef DEBUG_PRINT
			printf("Key off at %d: %d v=%d\n", dwTimestamp, midiByte1, velocity);
#endif // DEBUG_PRINT

		}

		else if (midiStatus == 0x90)
		{

			// Note on

			int key = midiByte1;
			int velocity = midiByte2;

			if (velocity > 0)
			{

#ifdef DEBUG_PRINT
				printf("Key on at %d: %d v=%d\n", dwTimestamp, midiByte1, velocity);
#endif // DEBUG_PRINT

			}

			if (velocity == 0)
			{

#ifdef DEBUG_PRINT
				printf("Key off at %d: %d v=%d\n", dwTimestamp, midiByte1, velocity);
#endif // DEBUG_PRINT

			}

			bool processed = false;

			if (!processed) processed = Toccata.SetTimeFSM()->Run(key, velocity);
			if (!processed) processed = Toccata.SetTempoFSM()->Run(key, velocity);
			if (!processed) Toccata.GetRecorder()->ProcessNote(key, velocity, dwTimestamp, programTimeStamp);

		}

		else if (midiStatus == 0xF8)
		{

			//SoundSystem.Test();

			// Midi tick
			Toccata.GetRecorder()->ProcessMidiTick(dwTimestamp);

		}

		else if (midiStatus != 0xF8 && midiStatus != 0xFE)
		{

			int a = 0;

		}

	}

}