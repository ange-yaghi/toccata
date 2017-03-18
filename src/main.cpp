#include <Windows.h>

#include <stdio.h>
#include <atomic>

#include <yds_timing.h>

#include "toccata_core.h"
#include "toccata_midi.h"
#include "toccata_sound_system.h"
#include "toccata_midi_callback.h"

#include "toccata_database.h"

#define DEBUG_PRINT_DISABLED 0

void CALLBACK MIDI_Output_Callback(
	HMIDIOUT  hmo,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
	)
{



}

void LatencyTest()
{

	UINT nDevs = midiOutGetNumDevs();

	MIDIOUTCAPS *deviceInfo = new MIDIOUTCAPS[nDevs];

	for (UINT i = 0; i < nDevs; i++)
	{

		MMRESULT result = midiOutGetDevCaps(i, &deviceInfo[i], sizeof(MIDIINCAPS));

		printf("Output device %d, %ls\n", i, deviceInfo[i].szPname);

	}

	UINT selectedDevice = 2;

	HMIDIOUT device;
	MMRESULT result = midiOutOpen(&device, selectedDevice, (DWORD_PTR)&MIDI_Output_Callback, NULL, CALLBACK_FUNCTION);

	if (result != MMSYSERR_NOERROR)
	{

		printf("*** Could not open device ***\n");

		switch (result)
		{

		case MMSYSERR_ALLOCATED:
			printf("Keyboard alredy allocated.\n");
			break;

		case MMSYSERR_BADDEVICEID:
			printf("Bad device ID\n");
			break;

		case MMSYSERR_INVALFLAG:
			printf("Invalid flag\n");
			break;

		case MMSYSERR_INVALPARAM:
			printf("Invalid parameter\n");
			break;

		case MMSYSERR_NOMEM:
			printf("System unable to allocate memory\n");
			break;

		default:
			printf("Unknown error\n");
			break;

		}

	}

	BYTE midiByte1 = 55; // High order
	BYTE midiByte2 = 70; // low order
	BYTE midiStatus = 0x90; // low order

	WORD hiWord = MAKEWORD(midiByte2, 0);
	WORD loWord = MAKEWORD(midiStatus, midiByte1);

	DWORD dwMidiMessage = MAKELONG(hiWord, loWord);

	WORD hiWord_ = HIWORD(dwMidiMessage);
	WORD loWord_ = LOWORD(dwMidiMessage);

	BYTE midiByte1_ = HIBYTE(loWord);
	BYTE midiByte2_ = LOBYTE(hiWord);
	BYTE midiStatus_ = LOBYTE(loWord);

	int result1 = midiOutShortMsg(device, dwMidiMessage);

	if (result1 != MMSYSERR_NOERROR)
	{

		int a = 0;

	}

	UINT volume;
	result = midiOutGetID(
		device,
		(LPUINT)&volume
		);

	int a = 0;

}

int main()
{

	Toccata.GetLogger()->LogUser(Toccata_Core::Line);
	Toccata.GetLogger()->LogUser("   Toccata\n");
	Toccata.GetLogger()->LogUser("   Ange Yaghi | 2017\n");
	Toccata.GetLogger()->LogUser(Toccata_Core::Line);
	Toccata.GetLogger()->LogUser("\n");

	Toccata.GetLogger()->LogUser("Initializing\n");
	Toccata.GetLogger()->Increment();

	//LatencyTest();

	Toccata.GetLogger()->LogUser("Searching for MIDI devices...\n");
	Toccata.GetLogger()->Increment();

	UINT nDevs = midiInGetNumDevs();
	Toccata.GetLogger()->LogUser("Found %d MIDI devic%s\n", nDevs, (nDevs == 0 || nDevs > 1) ? "es" : "e");

	MIDIINCAPS *deviceInfo = new MIDIINCAPS[nDevs];

	for (UINT i = 0; i < nDevs; i++)
	{

		MMRESULT result = midiInGetDevCaps(i, &deviceInfo[i], sizeof(MIDIINCAPS));

		Toccata.GetLogger()->LogUser("%d) %ls\n", i, deviceInfo[i].szPname);

	}

	Toccata.GetLogger()->Decrement();
	Toccata.GetLogger()->LogUser("\n");

	UINT selectedDevice = 1;

	Toccata.GetLogger()->LogUser("Connecting to MIDI device %d\n", selectedDevice);
	Toccata.GetLogger()->Increment();

	if (Toccata.GetMode() == Toccata_Core::MODE_PIANO_SOURCE)
	{

		HMIDIIN device;
		MMRESULT result = midiInOpen(&device, selectedDevice, (DWORD_PTR)&Toccata_MIDI_Callback, NULL, CALLBACK_FUNCTION | MIDI_IO_STATUS);

		if (result != MMSYSERR_NOERROR)
		{

			Toccata.GetLogger()->LogUser("*** Could not open device ***\n");
			Toccata.GetLogger()->Increment();

			switch (result)
			{

			case MMSYSERR_ALLOCATED:
				Toccata.GetLogger()->LogUser("Keyboard alredy allocated.\n");
				break;

			case MMSYSERR_BADDEVICEID:
				Toccata.GetLogger()->LogUser("Bad device ID\n");
				break;

			case MMSYSERR_INVALFLAG:
				Toccata.GetLogger()->LogUser("Invalid flag\n");
				break;

			case MMSYSERR_INVALPARAM:
				Toccata.GetLogger()->LogUser("Invalid parameter\n");
				break;

			case MMSYSERR_NOMEM:
				Toccata.GetLogger()->LogUser("System unable to allocate memory\n");
				break;

			default:
				Toccata.GetLogger()->LogUser("Unknown error\n");
				break;

			}

			Toccata.GetLogger()->Decrement();

		}

		midiInStart(device);

	}

	Toccata.Initialize();
	Toccata.MainLoop();

	return 0;

}