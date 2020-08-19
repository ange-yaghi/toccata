from newtons_method import NewtonsMethod

import random
import copy
import numpy as np

class MidiNote:
    def __init__(self, pitch, start, length):
        self.pitch = pitch
        self.start = start
        self.length = length

        # For use in the Newton solver
        self.mapped = False

    def __repr__(self):
        return "({}, {})".format(self.pitch, self.start)


class MidiSegment:
    def __init__(self):
        self.notes = []
        self.channels = {}
        self.length = 0

    @staticmethod
    def transform(x, s, t):
        return (x * s) + t

    @staticmethod
    def inverse_transform(x, s, t):
        return (x - t) / s

    def sort(self):
        self.notes = sorted(self.notes, key = lambda note: note.start)
        self.channels = {}

        for i in range(len(self.notes)):
            note = self.notes[i]

            if note.pitch not in self.channels.keys():
                self.channels[note.pitch] = []

            self.channels[note.pitch].append(i)

    def add_note(self, note: MidiNote):
        if note.pitch not in self.channels.keys():
            self.channels[note.pitch] = []

        self.notes.append(note)
        self.channels[note.pitch].append(len(self.notes) - 1)

    def remove_note(self, note: MidiNote):
        self.notes.remove(note)
        self.sort()

    def create_and_add_note(self, position, channel):
        new_note = MidiNote(channel, position, 0)

        if channel not in self.channels.keys():
            self.channels[channel] = []

        self.notes.append(new_note)
        self.channels[channel].append(len(self.notes) - 1)

    def get_note_count(self):
        return len(self.notes)

    def find_test_pattern(self, note_count):
        EPSILON = 1E-4
        MAX_ATTEMPTS = 256

        for _ in range(MAX_ATTEMPTS):
            notes = []
            start_min, start_max = float('inf'), float('-inf')
            
            for _ in range(note_count):
                note = random.randint(0, len(self.notes) - 1)
                if note in notes:
                    break

                start_min = min(start_min, self.notes[note].start)
                start_max = max(start_max, self.notes[note].start)

                notes.append(note)

            if len(notes) != note_count or (start_max - start_min) < EPSILON:
                continue
            
            return notes
        
        return None

    def get_closest_note(self, note: MidiNote, threshold, s, t):
        if note.pitch not in self.channels.keys():
            return None

        smallest_distance = None
        closest = None
        for n in self.channels[note.pitch]:
            adjusted_start = MidiSegment.transform(self.notes[n].start, s, t)
            diff = abs(note.start - adjusted_start)
            if (closest is None or diff < smallest_distance) and diff < threshold:
                closest = n
                smallest_distance = diff

        return closest

    def get_mapping(self, reference_segment, s, t, corr_threshold=0.1):
        mapping = {}
        for i in range(len(reference_segment.notes)):
            note = reference_segment.notes[i]
            closest = self.get_closest_note(note, corr_threshold * reference_segment.length, s, t)
            mapping[i] = closest
        
        return mapping

    def assess_solution(self, reference_segment, s, t, corr_threshold=0.1):
        mapping = self.get_mapping(reference_segment, s, t, corr_threshold=corr_threshold)
        missed_notes = 0
        matched_notes = 0
        total_error = 0.0

        for r, p in mapping.items():
            if p is None:
                missed_notes += 1
            else:
                p_t = MidiSegment.transform(self.notes[p].start, s, t)
                total_error += abs(p_t - reference_segment.notes[r].start)
                matched_notes += 1

        missed_note_rate = missed_notes / float(reference_segment.get_note_count())

        if matched_notes > 0:
            return total_error / float(matched_notes), missed_note_rate
        else:
            return float('inf'), missed_note_rate

    def newton_solve(self, mapping, reference_segment):
        r_set = []
        p_set = []

        for r, p in mapping.items():
            if p is None:
                continue

            r_set.append(reference_segment.notes[r].start)
            p_set.append(self.notes[p].start)

        #print("r={}, p={}".format(r_set, p_set))
        try:
            s, t = NewtonsMethod.solve(r_set, p_set)
        except np.linalg.LinAlgError:
            #print("ERROR")
            #print("r={}, p={}".format(r_set, p_set))
            return None, None
        #print("s={}, t={}".format(s, t))

        return s, t

    def fast_check(self, test_note_indices, reference_segment):
        best_solution = self.find_best_solution_helper(test_note_indices, {}, reference_segment, best_solution=[None, (None, None)])
        return best_solution[0]

    def find_best_solution(self, test_note_indices, reference_segment):
        best_solution = self.find_best_solution_helper(test_note_indices, {}, reference_segment, best_solution=[None, (None, None)])
        if best_solution[0] is None:
            return None, None, None

        s0, t0 = best_solution[1]

        mapping = self.get_mapping(reference_segment, s0, t0)
        s, t = self.newton_solve(mapping, reference_segment)
        if s is None or t is None:
            return None, None, None

        err, mnr = self.assess_solution(reference_segment, s, t, 0.1)

        #print("Refined {} -> {}".format(best_solution[0], err))

        return err, s, t

    def create_copy(self):
        return copy.deepcopy(self)

    def find_best_solution_helper(self, test_note_indices, mapping, reference_segment, depth=0, best_solution=[None, (None, None)]):
        max_depth = len(test_note_indices)

        if depth == max_depth:
            s, t = self.newton_solve(mapping, reference_segment)

            #print("{}, {}, mapping={}".format(s, t, mapping))

            if s is None or t is None:
                return

            err, missed_note_rate = self.assess_solution(reference_segment, s, t, 0.1)
            if best_solution[0] is None or err < best_solution[0]:
                if missed_note_rate < 0.25:
                    best_solution[0] = err
                    best_solution[1] = (s, t)

        else:
            note_index = test_note_indices[depth]
            note = reference_segment.notes[note_index]

            if note.pitch not in self.channels.keys():
                mapping[note_index] = None
            else:
                for played_note in self.channels[note.pitch]:
                    if not self.notes[played_note].mapped:
                        mapping[note_index] = played_note
                        self.notes[played_note].mapped = True
                        self.find_best_solution_helper(test_note_indices, mapping, reference_segment, depth + 1, best_solution)

                        mapping[note_index] = None
                        self.notes[played_note].mapped = False

        if depth == 0:
            return best_solution
