from rpm_midi import MidiSegment
from rpm_library import Library, Piece
from rpm_utilities import Utilities

import random


class BarMatch(object):
    def __init__(self):
        self.bar = None
        self.missing_notes = 0
        self.nonsense_notes = 0
        self.t = 0
        self.s = 0
        self.err = 0.0


class Recording(object):
    def __init__(self):
        self.bars = []
        self.segment = MidiSegment()


class Matcher(object):
    def __init__(self, library: Library):
        self.input_buffer = MidiSegment()
        self.recording = Recording()
        self.library = library

    def push(self, note):
        self.input_buffer.add_note(note)
        self.input_buffer.sort()

    def commit_note(self, note):
        self.input_buffer.remove_note(note)
        self.recording.segment.add_note(note)

    def clip_beginning(self, clip, threshold=50):
        clipped_notes = []
        for note in self.input_buffer.notes:
            if note.start < clip - threshold:
                clipped_notes.append(note)

        for note in clipped_notes:
            self.input_buffer.remove_note(note)

    def commit_bar(self, bar, s, t, threshold=10):
        committed_notes = []
        mapping = self.input_buffer.get_mapping(bar.segment, s, t)
        for r, p in mapping.items():
            if p is not None:
                note = self.input_buffer.notes[p]
                committed_notes.append(note)

        for note in committed_notes:
            self.commit_note(note)

        begin = MidiSegment.inverse_transform(0, s, t)
        end = MidiSegment.inverse_transform(bar.segment.length, s, t)

        clipped_notes = []
        committed_notes = []
        for note in self.input_buffer.notes:
            if note.start < begin - threshold:
                clipped_notes.append(note)
            elif note.start < end - threshold:
                committed_notes.append(note)

        for note in clipped_notes:
            self.input_buffer.remove_note(note)

        for note in committed_notes:
            self.commit_note(note)

    def find_complete_bar(self, current_timestamp):
        candidates = []
        for bar in self.library.bars:
            test_pattern = bar.segment.find_test_pattern(4)
            err = self.input_buffer.fast_check(test_pattern, bar.segment)

            if err is None:
                continue
            elif err < 100:
                candidates.append((err, bar))

        refined_candidates = []
        for err0, bar in candidates:
            err, s, t = self.input_buffer.find_best_solution(test_pattern, bar.segment)
            if err is not None:
                refined_candidates.append((err, s, t, bar))

        best_candidate = None
        smallest_err = None
        best_s, best_t = None, None
        for err, s, t, bar in refined_candidates:
            adjusted_timestamp = MidiSegment.transform(current_timestamp, s, t)
            complete = adjusted_timestamp > bar.segment.length

            if complete:
                if smallest_err is None or err < smallest_err:
                    best_candidate = bar
                    best_s = s
                    best_t = t

        return best_candidate, best_s, best_t


if __name__ == "__main__":
    Utilities.seed_random(14)
    library = Library()

    piece = Piece(0)

    BAR_COUNT = 1000
    for i in range(BAR_COUNT):
        reference_segment = Utilities.create_random_bar_quantized(16, 1000, 16)
        new_bar = library.new_bar(piece, reference_segment)

    SELECTED_BAR0 = random.randint(0, BAR_COUNT)
    SELECTED_BAR1 = random.randint(0, BAR_COUNT)
    played_segment = Utilities.concatenate_segments(library.bars[SELECTED_BAR0].segment, library.bars[SELECTED_BAR1].segment)
    Utilities.add_nonsense_notes(played_segment, 4, 2000)
    Utilities.add_note_omissions(played_segment, 3)
    Utilities.add_nonsense_notes_range(played_segment, 30, 2000, 3000)
    Utilities.scale_segment(played_segment, 6)
    Utilities.jitter(played_segment, 200)

    matcher = Matcher(library)

    print("Expecting bar {}, {}".format(SELECTED_BAR0, SELECTED_BAR1))
    
    for note in played_segment.notes:
        matcher.push(note)
        detected_bar, s, t = matcher.find_complete_bar(note.start)

        if detected_bar is not None:
            print("bar = {}".format(detected_bar.id))
            matcher.commit_bar(detected_bar, s, t)

    print(len(matcher.recording.segment.notes))
