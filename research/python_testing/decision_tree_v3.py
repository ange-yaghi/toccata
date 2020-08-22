from rpm_library import Library, Piece, Bar
from rpm_midi import MidiSegment

import math

class Decision_v3(object):
    def __init__(self):
        self.notes = set()

        self.previous_decision = None
        self.cached = False
        self.depth = 0

        self.matched_bar = None
        self.s = 1.0
        self.t = 0.0

        self.err = 0.0
        self.mapped_notes = 0

    def invalidate_cache(self):
        self.cached = False
        self.previous_decision = None
        self.depth = -1

    def get_end(self):
        return max(self.notes)

    def get_footprint(self):
        if len(self.notes) == 0: 
            return 0

        return max(self.notes) - min(self.notes)

    def __repr__(self):
        return "{{{} -> {} bar={}, piece={}, L={}, s={}, t={}, err={}, mapped={}/{}}}".format(
            min(self.notes), 
            max(self.notes), 
            self.matched_bar.id, 
            self.matched_bar.piece_id, 
            self.depth, 
            self.s, 
            self.t,
            self.err,
            self.mapped_notes,
            self.matched_bar.segment.get_note_count())

class DecisionTree_v3(object):
    def __init__(self):
        self.decisions = []

    def get_bounds(self, segment, reference_bar, s, t):
        mapping = segment.get_mapping(reference_bar.segment, s, t)
        notes = set()
        for p in mapping.values():
            if p is not None:
                notes.add(p)

        return notes, len(notes)

    def find_best_previous(self, decision: Decision_v3):
        best_depth = -1
        best = None
        for prev in self.decisions:
            if prev == decision:
                continue
            elif prev.get_end() < decision.get_end():
                if not prev.matched_bar.find_next(decision.matched_bar, 1) and not decision.matched_bar.find_next(prev.matched_bar, 1):
                    continue

                depth = self.get_depth(prev)

                if depth > best_depth:
                    best = prev
                    best_depth = depth

        return best

    def is_cached(self, decision: Decision_v3):
        if not decision.cached:
            return False
        else:
            if decision.previous_decision is not None:
                return self.is_cached(decision.previous_decision)
            else:
                return True

    def get_depth(self, decision: Decision_v3):
        if not self.is_cached(decision):
            best_prev = self.find_best_previous(decision)
            
            if best_prev is not None:
                decision.cached = True
                decision.previous_decision = best_prev
                decision.depth = 1 + self.get_depth(best_prev)
            else:
                decision.cached = True
                decision.previous_decision = None
                decision.depth = 1

        return decision.depth

    def same(self, decision0: Decision_v3, decision1: Decision_v3):
        if decision0.matched_bar != decision1.matched_bar:
            return False

        return True

    def half_overlapping(self, decision0: Decision_v3, decision1: Decision_v3):
        notes = min(decision0.mapped_notes, decision1.mapped_notes)
        half = int(math.ceil(0.5 * notes))

        shared_notes = 0
        for n0 in decision0.notes:
            for n1 in decision1.notes:
                if n0 == n1:
                    shared_notes += 1

        return shared_notes >= half

    def overlapping(self, decision0: Decision_v3, decision1: Decision_v3):
        for n0 in decision0.notes:
            for n1 in decision1.notes:
                if n0 == n1:
                    return True

        return False

    def better(self, decision0: Decision_v3, decision1: Decision_v3):
        if decision0.mapped_notes > decision1.mapped_notes:
            return True
        elif decision0.mapped_notes < decision1.mapped_notes:
            return False

        footprint0 = decision0.get_footprint()
        footprint1 = decision1.get_footprint()

        if footprint0 < footprint1:
            return True
        if footprint1 > footprint0:
            return False

        if decision0.err < decision1.err:
            return True

        return False

    def invalidate_cache(self, start_index):
        for decision in self.decisions:
            if decision.get_end() > start_index:
                decision.invalidate_cache()

    def add_decision(self, new_decision: Decision_v3):
        for decision in self.decisions:
            if self.same(decision, new_decision) and self.half_overlapping(decision, new_decision):
                if self.better(new_decision, decision):
                    self.invalidate_cache(min(decision.get_end(), new_decision.get_end()))
                    decision.invalidate_cache()

                    decision.s = new_decision.s
                    decision.t = new_decision.t
                    decision.err = new_decision.err
                    decision.mapped_notes = new_decision.mapped_notes
                    decision.notes = new_decision.notes

                return False

        self.invalidate_cache(new_decision.get_end())
        self.decisions.append(new_decision)

        return True

    def match(self, input_buffer: MidiSegment, start_clip, bar: Bar):
        test_pattern = bar.segment.find_test_pattern(4)
        if test_pattern is None:
            return None

        err = input_buffer.fast_check(test_pattern, bar.segment)

        if err is not None and err < 100:
            err, s, t = input_buffer.find_best_solution(test_pattern, bar.segment)

            notes, mapped_notes = self.get_bounds(input_buffer, bar, s, t)
            offset_notes = set()
            for note in notes:
                offset_notes.add(note + start_clip)

            new_decision = Decision_v3()
            new_decision.matched_bar = bar
            new_decision.s = s
            new_decision.t = t
            new_decision.notes = offset_notes
            new_decision.previous_decision = None
            new_decision.err = err
            new_decision.mapped_notes = mapped_notes
            return new_decision

        return None

    def seed_match(self, start_clip, input_buffer: MidiSegment, library: Library):
        for bar in library.bars:
            snipped_input = input_buffer.create_copy()
            end_index = min(len(snipped_input.notes), start_clip + int(1.25 * bar.segment.get_note_count()))
            snipped_input.notes = snipped_input.notes[start_clip:end_index]
            snipped_input.sort()

            decision = self.match(snipped_input, start_clip, bar)

            if decision is not None:
                self.add_decision(decision)
