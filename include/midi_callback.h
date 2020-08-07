#ifndef TOCCATA_CORE_MIDI_CALLBACK_H
#define TOCCATA_CORE_MIDI_CALLBACK_H

#include <Windows.h>

void CALLBACK Toccata_MIDI_Callback(
	HMIDIIN   hMidiIn,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
);

#endif /* TOCCATA_CORE_MIDI_CALLBACK_H */
