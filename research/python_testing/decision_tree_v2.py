from rpm_library import Library, Piece, Bar
from rpm_midi import MidiSegment

class Decision_v2(object):
    def __init__(self):
        self.start_index = 0
        self.end_index = 0

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

    def is_inside(self, decision):
        if self.end_index <= decision.end_index and self.start_index >= decision.start_index:
            return True
        else:
            return False

    def __repr__(self):
        return "{{{} -> {} bar={}, piece={}, L={}, s={}, t={}, err={}, mapped={}/{}}}".format(
            self.start_index, 
            self.end_index, 
            self.matched_bar.id, 
            self.matched_bar.piece_id, 
            self.depth, 
            self.s, 
            self.t,
            self.err,
            self.mapped_notes,
            self.matched_bar.segment.get_note_count())

class DecisionTree_v2(object):
    def __init__(self):
        self.decisions = []

    def get_bounds(self, segment, reference_bar, s, t):
        mapping = segment.get_mapping(reference_bar.segment, s, t)
        last_mapped = None
        first_mapped = None
        mapped_notes = 0
        for r, p in mapping.items():
            if p is not None:
                if last_mapped is None or p > last_mapped:
                    last_mapped = p
                
                if first_mapped is None or p < first_mapped:
                    first_mapped = p

                mapped_notes += 1

        return first_mapped, last_mapped, mapped_notes

    def find_best_previous(self, decision: Decision_v2):
        best_depth = -1
        best = None
        for prev in self.decisions:
            if prev.end_index < decision.start_index:
                if not prev.matched_bar.find_next(decision.matched_bar, 1):
                    continue

                depth = self.get_depth(prev)

                if depth > best_depth:
                    best = prev
                    best_depth = depth

        return best

    def is_cached(self, decision: Decision_v2):
        if not decision.cached:
            return False
        else:
            if decision.previous_decision is not None:
                return self.is_cached(decision.previous_decision)
            else:
                return True

    def get_depth(self, decision: Decision_v2):
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

    def same(self, decision0: Decision_v2, decision1: Decision_v2):
        if decision0.matched_bar != decision1.matched_bar:
            return False

        return True

    def better(self, decision0: Decision_v2, decision1: Decision_v2):
        if decision0.mapped_notes > decision1.mapped_notes:
            return True
        elif decision0.mapped_notes < decision1.mapped_notes:
            return False

        footprint0 = decision0.end_index - decision0.start_index
        footprint1 = decision1.end_index - decision1.start_index

        if footprint0 < footprint1:
            return True
        if footprint1 > footprint0:
            return False

        if decision0.err < decision1.err:
            return True

        return False

    def invalidate_cache(self, start_index):
        for decision in self.decisions:
            if decision.start_index > start_index:
                decision.invalidate_cache()

    def add_decision(self, new_decision: Decision_v2):
        for decision in self.decisions:
            if self.same(decision, new_decision) and (decision.is_inside(new_decision) or new_decision.is_inside(decision)):
                if self.better(new_decision, decision):
                    self.invalidate_cache(min(decision.end_index, new_decision.end_index))
                    decision.invalidate_cache()

                    decision.s = new_decision.s
                    decision.t = new_decision.t
                    decision.err = new_decision.err
                    decision.mapped_notes = new_decision.mapped_notes
                    decision.start_index = new_decision.start_index
                    decision.end_index = new_decision.end_index  

                return False

        self.invalidate_cache(new_decision.end_index)
        self.decisions.append(new_decision)

        return True

    def match(self, input_buffer: MidiSegment, start_clip, bar: Bar):
        test_pattern = bar.segment.find_test_pattern(4)
        if test_pattern is None:
            return None

        err = input_buffer.fast_check(test_pattern, bar.segment)

        if err is not None and err < 100:
            err, s, t = input_buffer.find_best_solution(test_pattern, bar.segment)

            clip_start, clip_end, mapped_notes = self.get_bounds(input_buffer, bar, s, t)
            clip_start += start_clip
            clip_end += start_clip

            new_decision = Decision_v2()
            new_decision.matched_bar = bar
            new_decision.s = s
            new_decision.t = t
            new_decision.start_index = clip_start
            new_decision.end_index = clip_end
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
