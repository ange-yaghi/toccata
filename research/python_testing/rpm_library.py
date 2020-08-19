from rpm_midi import MidiSegment


class Bar(object):
    def __init__(self, id, piece_id, segment: MidiSegment, next=[]):
        self.segment = segment
        self.next = next
        self.piece_id = piece_id
        self.id = id

    def add_next(self, next):
        self.next.append(next)


class Piece(object):
    def __init__(self, id):
        self.bars = []
        self.id = id

    def add_bar(self, bar):
        bar.piece = self
        self.bars.append(bar)


class Library(object):
    def __init__(self):
        self.bars = []
        self.pieces = []

    def new_piece(self):
        piece = Piece(len(self.pieces))
        self.pieces.append(piece)

        return piece

    def get_bar_count(self):
        return len(self.bars)

    def new_bar(self, piece, segment, next=[]):
        bar = Bar(id=len(self.bars), piece_id=piece, segment=segment, next=next)
        bar.piece_id = piece.id
        self.bars.append(bar)

        return bar
