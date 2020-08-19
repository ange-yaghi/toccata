from structures import MidiNote, MidiSegment

import random


class ErrorSettings(object):
    def __init__(self, jitter_amplitude, nonsense_notes, note_omissions, scale):
        self.jitter_amplitude = jitter_amplitude
        self.nonsense_notes = nonsense_notes
        self.note_omissions = note_omissions
        self.scale = scale


class Utilities(object):
    def __init__(self):
        pass

    @staticmethod
    def seed_random(seed):
        random.seed(seed)

    @staticmethod
    def create_random_bar(note_count, bar_length, notes=256, name=""):
        new_segment = MidiSegment()
        new_segment.length = bar_length
        new_segment.name = name

        for _ in range(note_count):
            note = MidiNote(random.randint(0, notes), random.uniform(0, bar_length), 0.0)
            new_segment.add_note(note)

        return new_segment

    @staticmethod
    def create_random_bar_quantized(note_count, bar_length, grid_spaces, notes=256, name=""):
        new_segment = MidiSegment()
        new_segment.length = bar_length
        new_segment.name = name

        grid_size = bar_length / float(grid_spaces)

        for _ in range(note_count):
            note = MidiNote(random.randint(0, notes), random.randint(0, grid_spaces) * grid_size, 0.0)
            new_segment.add_note(note)

        return new_segment       

    @staticmethod
    def jitter(segment, jitter_amplitude):
        for note in segment.notes:
            jitter = random.uniform(-jitter_amplitude, jitter_amplitude)
            note.start += jitter

    @staticmethod
    def add_nonsense_notes(segment, error_count, bar_length, notes=256):
        for _ in range(error_count):
            note = MidiNote(random.randint(0, notes), random.uniform(0, bar_length), 0.0)
            segment.add_note(note)

    @staticmethod
    def add_nonsense_notes_range(segment, error_count, start, end, notes=256):
        for _ in range(error_count):
            note = MidiNote(random.randint(0, notes), random.uniform(start, end), 0.0)
            segment.add_note(note)

    @staticmethod
    def add_note_omissions(segment, error_count):
        omissions = random.sample(range(segment.get_note_count()), error_count)
        omissions.sort(reverse = True)

        for o in omissions:
            del segment.notes[o]

    @staticmethod
    def scale_segment(segment, s):
        for note in segment.notes:
            note.start *= s
            note.length *= s

    @staticmethod
    def shift_segment(segment, dx):
        for note in segment.notes:
            note.start += dx

    @staticmethod
    def roughen(segment, bar_length, settings):
        Utilities.jitter(segment, settings.jitter_amplitude)
        Utilities.add_note_omissions(segment, settings.note_omissions)
        Utilities.add_nonsense_notes(segment, settings.nonsense_notes, bar_length)
        Utilities.scale_segment(segment, settings.scale)
