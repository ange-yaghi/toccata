from decision_tree import Decision, DecisionTree
from decision_tree_v3 import DecisionTree_v3, Decision_v3
from rpm_utilities import Utilities
from rpm_midi import MidiNote, MidiSegment
from rpm_library import Library

import random

def generate_random_segment():
    note_count = random.randint(16, 32)
    random_bar = Utilities.create_random_bar_quantized(note_count, 1000, 16, 32)
    random_bar.sort()

    return random_bar

def generate_input_buffer(bars):
    input_buffer = []
    current_offset = 0

    for bar in bars:
        for note in bar.segment.notes:
            new_note = MidiNote(note.pitch, note.start + current_offset, note.length)
            input_buffer.append(new_note)

        current_offset += bar.segment.length

    new_segment = MidiSegment()
    new_segment.notes = input_buffer
    new_segment.length = current_offset

    return new_segment

def test_1():
    random.seed(1)

    library = Library()

    piece1 = library.new_piece()
    piece2 = library.new_piece()

    random_segments = []
    for _ in range(100):
        random_segments.append(generate_random_segment())

    bars_piece1 = []
    bars_piece1.append(library.new_bar(piece1, random_segments[0]))
    bars_piece1.append(library.new_bar(piece1, random_segments[1]))
    bars_piece1.append(library.new_bar(piece1, random_segments[2]))
    bars_piece1.append(library.new_bar(piece1, random_segments[3]))
    bars_piece1.append(library.new_bar(piece1, random_segments[4]))
    bars_piece1.append(library.new_bar(piece1, random_segments[5]))
    bars_piece1.append(library.new_bar(piece1, random_segments[6]))
    bars_piece1.append(library.new_bar(piece1, random_segments[7]))
    bars_piece1.append(library.new_bar(piece1, random_segments[8]))

    bars_piece1[0].next = [bars_piece1[1]]
    bars_piece1[1].next = [bars_piece1[2]]
    bars_piece1[2].next = [bars_piece1[3]]
    bars_piece1[3].next = [bars_piece1[4]]
    bars_piece1[4].next = [bars_piece1[5]]
    bars_piece1[5].next = [bars_piece1[6]]
    bars_piece1[6].next = [bars_piece1[7]]
    bars_piece1[7].next = [bars_piece1[8]]
    bars_piece1[8].next = []

    bars_piece2 = []
    bars_piece2.append(library.new_bar(piece2, random_segments[0]))
    bars_piece2.append(library.new_bar(piece2, random_segments[1]))
    bars_piece2.append(library.new_bar(piece2, random_segments[2]))
    bars_piece2.append(library.new_bar(piece2, random_segments[12]))
    bars_piece2.append(library.new_bar(piece2, random_segments[13]))

    bars_piece2[0].next = [bars_piece2[1]]
    bars_piece2[1].next = [bars_piece2[2]]
    bars_piece2[2].next = [bars_piece2[3]]
    bars_piece2[3].next = [bars_piece2[4]]
    bars_piece2[4].next = []

    input_buffer = generate_input_buffer(bars_piece1)
    original = input_buffer.create_copy()
    Utilities.jitter(input_buffer, 100)
    Utilities.add_note_omissions(input_buffer, 5)
    Utilities.add_nonsense_notes(input_buffer, 5, 5000, 32)
    Utilities.scale_segment(input_buffer, 2.0)
    input_buffer.sort()

    decision_tree = DecisionTree_v3()

    for start_clip in range(input_buffer.get_note_count()):
        decision_tree.seed_match(start_clip, input_buffer, library)

        print("{} Decisions".format(len(decision_tree.decisions)))

    print("{} Decisions".format(len(decision_tree.decisions)))

    index = 0
    for note in input_buffer.notes:
        print("{}: {} [{}]".format(index, note, original.notes[index]))
        index += 1

    for decision in decision_tree.decisions:
        decision_tree.get_depth(decision)
        print(decision)

if __name__ == "__main__":
    test_1()
