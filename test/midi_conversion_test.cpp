#include <pch.h>

#include "../include/segment_generator.h"
#include "../include/midi_file.h"

TEST(MidiConversionTest, SanityCheck) {
	const std::string path = "../../../test/midi/basic_midi.midi";

	toccata::MidiStream stream;
	toccata::MidiFile midiFile;
	midiFile.Read(path.c_str(), &stream);

	toccata::Library library;

	toccata::SegmentGenerator::Convert(&stream, &library, 0);

	EXPECT_EQ(library.GetBarCount(), 4);
}
