from rpm_library import Library, Piece, Bar
from rpm_midi import MidiSegment

class Decision(object):
    def __init__(self):
        self.start_index = 0
        self.end_index = 0
        self.previous_decision = None

        self.matched_bar = None
        self.s = 1.0
        self.t = 0.0

        self.err = 0.0
        self.mapped_notes = 0

    def get_depth(self):
        if self.previous_decision is None:
            return 1
        else:
            return 1 + self.previous_decision.get_depth()

    def __repr__(self):
        return "{{{} -> {} bar={}, piece={}, L={}, s={}, t={}, err={}, mapped={}/{}}}".format(
            self.start_index, 
            self.end_index, 
            self.matched_bar.id, 
            self.matched_bar.piece_id, 
            self.get_depth(), 
            self.s, 
            self.t,
            self.err,
            self.mapped_notes,
            self.matched_bar.segment.get_note_count())

class DecisionTree(object):
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

    def same(self, decision0: Decision, decision1: Decision):
        if decision0.matched_bar != decision1.matched_bar:
            return False

        if decision0.end_index <= decision1.end_index and decision0.start_index >= decision1.start_index:
            return True

        if decision1.end_index <= decision0.end_index and decision1.start_index >= decision0.start_index:
            return True

        return False

    def better(self, decision0: Decision, decision1: Decision):
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

    def add_decision(self, new_decision: Decision):
        for decision in self.decisions:
            if self.same(decision, new_decision):
                if decision.previous_decision != new_decision.previous_decision:
                    continue
                else:
                    better = self.better(new_decision, decision)

                    if better:
                        decision.s = new_decision.s
                        decision.t = new_decision.t
                        decision.err = new_decision.err
                        decision.mapped_notes = new_decision.mapped_notes
                        decision.start_index = new_decision.start_index
                        decision.end_index = new_decision.end_index

                    return False

        self.decisions.append(new_decision)
        return True

    def match(self, input_buffer: MidiSegment, start_clip, previous: Decision, bar: Bar, timestamp):
        test_pattern = bar.segment.find_test_pattern(4)
        if test_pattern is None:
            return None

        err = input_buffer.fast_check(test_pattern, bar.segment)

        if err is not None and err < 100:
            err, s, t = input_buffer.find_best_solution(test_pattern, bar.segment)
            adjusted_timestamp = MidiSegment.transform(timestamp, s, t)

            if adjusted_timestamp > bar.segment.length:
                #print("s={}, t={}, t_a={}".format(s, t, adjusted_timestamp))

                clip_start, clip_end, mapped_notes = self.get_bounds(input_buffer, bar, s, t)
                clip_start += start_clip
                clip_end += start_clip

                new_decision = Decision()
                new_decision.matched_bar = bar
                new_decision.s = s
                new_decision.t = t
                new_decision.start_index = clip_start
                new_decision.end_index = clip_end
                new_decision.previous_decision = previous
                new_decision.err = err
                new_decision.mapped_notes = mapped_notes
                return new_decision

        return None

    def seed_match(self, start_clip, input_buffer: MidiSegment, library: Library, timestamp):
        for bar in library.bars:
            snipped_input = input_buffer.create_copy()
            end_index = min(len(snipped_input.notes), start_clip + int(1.25 * bar.segment.get_note_count()))
            snipped_input.notes = snipped_input.notes[start_clip:end_index]
            snipped_input.sort()

            #print(start_clip, end_index)

            decision = self.match(snipped_input, start_clip, None, bar, timestamp)

            if decision is not None:
                self.add_decision(decision)

    def check_decisions(self, start_clip, input_buffer, timestamp):
        for decision in self.decisions:  
            snip_start = max(decision.end_index + 1, start_clip)
            for bar in decision.matched_bar.next:
                snipped_input = input_buffer.create_copy()
                end_index = min(len(snipped_input.notes), snip_start + int(1.25 * bar.segment.get_note_count()))
                snipped_input.notes = snipped_input.notes[snip_start:end_index]
                snipped_input.sort()

                next_decision = self.match(snipped_input, snip_start, decision, bar, timestamp)

                if next_decision is not None:
                    next_decision.previous = decision
                    self.add_decision(next_decision)

    def prune(self):
        pruned = set()
        for d0 in self.decisions:
            for d1 in self.decisions:
                if d0 == d1:
                    continue

                if self.same(d0, d1):
                    if d0.get_depth() > d1.get_depth():
                        pruned.add(d1)
                    elif d0.get_depth() < d1.get_depth():
                        pruned.add(d0)
                    elif self.better(d0, d1):
                        pruned.add(d1)
                    else:
                        pruned.add(d0)

        for decision in pruned:
            self.decisions.remove(decision)
