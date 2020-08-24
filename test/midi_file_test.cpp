#include <pch.h>

#include "../include/midi_file.h"
#include "../include/midi_stream.h"

TEST(MidiFileTest, SanityCheck) {
	std::string path = "../../../test/midi/basic_midi.midi";

	toccata::MidiStream stream;
	toccata::MidiFile midiFile;
	bool result = midiFile.Read(path.c_str(), &stream);

	EXPECT_TRUE(result);
	EXPECT_EQ(stream.GetNoteCount(), 4);
	EXPECT_NEAR(stream.GetTempo(), 120.0, 1E-4);
	EXPECT_EQ(stream.GetTimeSignatureNumerator(), 3);
	EXPECT_EQ(stream.GetTimeSignatureDenominator(), 4);
}
