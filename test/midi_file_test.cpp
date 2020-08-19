#include <pch.h>

#include "../include/midi_file.h"

TEST(MidiFileTest, SanityCheck) {
	std::string path = "../../../test/midi/basic_midi.midi";

	toccata::MidiFile midiFile;
	bool result = midiFile.Read(path.c_str());

	EXPECT_TRUE(result);
	EXPECT_EQ(midiFile.GetNoteCount(), 4);
	EXPECT_NEAR(midiFile.GetTempo(), 120.0, 1E-4);
	EXPECT_EQ(midiFile.GetTimeSignatureNumerator(), 3);
	EXPECT_EQ(midiFile.GetTimeSignatureDenominator(), 4);
}
