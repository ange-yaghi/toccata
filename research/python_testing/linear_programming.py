from structures import MidiNote

class MidiChannel(object):
    def __init__(self, pitch):
        self.notes = []
        self.pitch = pitch

    def add_note(self, start, length):
        new_note = MidiNote(self.pitch, start, length)
        self.notes.append(new_note)

    def find_closest_note(self, position):
        smallest_err = None
        closest = None

        for note in self.notes:
            err = abs(closest.start - position)
            if closest is None or err < smallest_err:
                smallest_err = err
                closest = note

        return closest
        