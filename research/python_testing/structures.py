import statistics

import copy

class MidiNote(object):
    def __init__(self, pitch, start, length):
        self.pitch = pitch
        self.start = start
        self.length = length

    def __repr__(self):
        return "({}, {})".format(self.pitch, self.start)


class MidiSegment(object):
    def __init__(self):
        self.notes = []
        self.notes_by_pitch = {}
        self.length = 0
        self.name = ''

    def add_note(self, note):
        self.notes.append(note)

    def get_note_count(self):
        return len(self.notes)

    def sort(self):
        self.notes = sorted(self.notes, key = lambda note: note.start)
        self.notes_by_pitch = {}

        for i in range(len(self.notes)):
            note = self.notes[i]

            if note.pitch not in self.notes_by_pitch.keys():
                self.notes_by_pitch[note.pitch] = []

            self.notes_by_pitch[note.pitch].append(i)

    @staticmethod
    def transform(x, scale, offset):
        return (x / scale) - offset

    def find_closest_note(self, pitch, start, start_index, s, offset):
        if pitch not in self.notes_by_pitch.keys():
            return None

        closest_diff = None
        index = None
        for i in self.notes_by_pitch[pitch]:
            if i < start_index:
                continue

            diff = abs(MidiSegment.transform(self.notes[i].start, s, offset) - start)
            if closest_diff is None or diff < closest_diff:
                closest_diff = diff
                index = i

        return index

    def find_note(self, pitch, start_index):
        if pitch not in self.notes_by_pitch.keys():
            return None

        for i in range(len(self.notes_by_pitch[pitch])):
            if i < start_index:
                continue
            else:
                return self.notes_by_pitch[pitch][i]
        
        return None

    def create_copy(self):
        return copy.deepcopy(self)

    def print(self):
        self.sort()

        print(self.notes)

    def write_to_csv(self, name):
        f = open("{}.csv".format(name), "w")

        for note in self.notes:
            f.write("{}, {}\n".format(note.start, note.pitch))

        f.close()


class NoteMapping(object):
    def __init__(self):
        self.note_mapping = dict()

    def map(self, index_reference, index_played):
        self.note_mapping[index_reference] = index_played

    def get(self, index_reference):
        return self.note_mapping[index_reference]


class Analyzer(object):
    def __init__(self):
        pass
    
    @staticmethod
    def stage_1_mapping(segment_ref, segment_played, start=0):
        mapping = NoteMapping()

        segment_ref.sort()
        segment_played.sort()

        missed_notes = 0
        mapped_notes = 0

        start_index_by_pitch = {}
        for pitch, note_indices in segment_ref.notes_by_pitch.items():
            if pitch not in start_index_by_pitch.keys():
                start_index_by_pitch[pitch] = start

            for note_index in note_indices:
                played_note = segment_played.find_note(pitch, start_index_by_pitch[pitch])

                if played_note is None:
                    mapping.map(note_index, None)
                    missed_notes += 1
                else:
                    mapping.map(note_index, played_note)
                    mapped_notes += 1
                    start_index_by_pitch[pitch] += 1

        #extra_notes = search_start - start - mapped_notes
        extra_notes = 0
        errors = missed_notes + extra_notes

        return errors, mapping

    @staticmethod
    def stage_2_calculate_tempo(segment_ref, segment_played, mapping):
        n = segment_ref.get_note_count()
        potential_scaling_factors = []

        for i in range(0, n):
            for j in range(i + 1, n):
                note0 = segment_ref.notes[i]
                note1 = segment_ref.notes[j]

                if note0.start == note1.start:
                    continue

                if mapping.get(i) is None or mapping.get(j) is None:
                    continue

                played0 = segment_played.notes[mapping.get(i)]
                played1 = segment_played.notes[mapping.get(j)]

                if played0.start == played1.start:
                    continue

                if note0.start > note1.start:
                    if played0.start < played0.start:
                        continue
                else:
                    if played0.start > played1.start:
                        continue

                nominal_delta = abs(note0.start - note1.start)
                played_delta = abs(played0.start - played1.start)

                potential_scaling_factors.append((played_delta / float(nominal_delta), i, j))

        smallest_err = None
        best_scaling = None
        optimal_offset = None
        for s in potential_scaling_factors:
            offset0, err0 = Analyzer.calculate_deviation(s[0], s[1], segment_ref, segment_played, mapping)
            offset1, err1 = Analyzer.calculate_deviation(s[0], s[2], segment_ref, segment_played, mapping)

            if err0 < err1:
                err = err0
                offset = offset0
            else:
                err = err1
                offset = offset1

            if smallest_err is None or err < smallest_err:
                smallest_err = err
                best_scaling = s[0]
                optimal_offset = offset

        if best_scaling is None:
            return None, None, None
        else:
            return (smallest_err / float(segment_ref.length)), optimal_offset, best_scaling

    @staticmethod
    def calculate_deviation(s, reference_note_index, segment_ref, segment_played, mapping):
        played_reference_note = segment_played.notes[mapping.get(reference_note_index)]
        reference_note = segment_ref.notes[reference_note_index]
        
        offset = played_reference_note.start / s - reference_note.start

        total_deviation = 0.0

        total_samples = 0

        for note in segment_ref.notes:
            closest = segment_played.find_closest_note(note.pitch, note.start, 0, s, offset)
            
            if closest is not None:
                closest_note = segment_played.notes[closest]
                total_deviation += abs(note.start - MidiSegment.transform(closest_note.start, s, offset))

                total_samples += 1

        return offset, total_deviation / float(total_samples)

    @staticmethod
    def detect_bar(played_segment: MidiSegment, library):
        note_count = played_segment.get_note_count()

        best_offset = None
        best_scale = None
        best_bar = None
        smallest_err = None

        for i in range(note_count):
            for segment in library:
                errors, mapping = Analyzer.stage_1_mapping(segment, played_segment, i)

                print(mapping.note_mapping)

                if errors >= 5:
                    continue

                err, offset, scale = Analyzer.stage_2_calculate_tempo(segment, played_segment, mapping)
                if scale is None:
                    continue

                if smallest_err is None or err < smallest_err:
                    smallest_err = err
                    best_scale = scale
                    best_offset = offset
                    best_bar = segment

        return (best_bar, best_offset, best_scale)
