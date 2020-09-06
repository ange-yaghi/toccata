#include "../include/analyzer.h"

#include "../include/note_mapper.h"

toccata::Analyzer::Analyzer() {
    m_timeline = nullptr;
    m_bars = nullptr;
    m_barCount = 0;
}

toccata::Analyzer::~Analyzer() {
    /* void */
}

void toccata::Analyzer::Analyze() {
    if (m_bars != nullptr) delete[] m_bars;

    m_barCount = 0;
    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);
        if (BarInRange(bar)) ++m_barCount;
    }

    m_bars = new BarInformation[m_barCount];
    for (int i = 0, j = 0; i < barCount; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);

        if (BarInRange(bar)) {
            ProcessBar(bar, j++, i);
        }
    }
}

bool toccata::Analyzer::BarInRange(const Timeline::MatchedBar &bar) const {
    const MusicSegment *segment = bar.Bar.MatchedBar->GetSegment();
    const MusicSegment *input = m_timeline->GetInputSegment();
    const double length = segment->GetNormalizedLength();

    const double start = m_timeline->ReferenceToInputSpace(0.0, bar.Bar.T);
    const double end = m_timeline->ReferenceToInputSpace(length, bar.Bar.T);

    if (m_timeline->InRangeInputSpace(start, end)) return true;

    const timestamp noteEnd = input->NoteContainer.GetPoints()[bar.Bar.End].GetEnd();
    const timestamp noteStart = input->NoteContainer.GetPoints()[bar.Bar.Start].Timestamp;

    return m_timeline->InRange(noteStart, noteEnd);
}

void toccata::Analyzer::ProcessBar(const Timeline::MatchedBar &bar, int index, int masterIndex) {
    BarInformation &info = m_bars[index];
    
    MusicSegment *reference = bar.Bar.MatchedBar->GetSegment();
    MusicSegment *input = m_timeline->GetInputSegment();

    const int referenceCount = reference->NoteContainer.GetCount();
    const int inputCount = input->NoteContainer.GetCount();

    NoteMapper::InjectiveMappingRequest request;
    NoteMapper::AllocateMemorySpace(&request.Memory, referenceCount, inputCount);

    request.CorrelationThreshold = 0.1;
    request.Start = bar.Bar.Start;
    request.End = bar.Bar.End;
    request.ReferenceSegment = reference;
    request.Segment = input;
    request.T = bar.Bar.T;
    request.Target = new int[referenceCount];

    int *mapping = NoteMapper::GetInjectiveMapping(&request);
    NoteMapper::FreeMemorySpace(&request.Memory);

    int mappedNotes = 0;
    double cumulativeError = 0.0;
    for (int i = 0; i < referenceCount; ++i) {
        info.NoteInformation.push_back(NoteInformation());
        NoteInformation &noteInfo = info.NoteInformation.back();

        const int mappedTo = mapping[i];

        noteInfo.InputNote = mappedTo;
        noteInfo.ReferenceNote = i;

        if (mappedTo != -1) {
            const double r = reference->Normalize(reference->NoteContainer.GetPoints()[i].Timestamp);
            const double p = bar.Bar.T.f(
                input->Normalize(bar.Bar.T.Local(input->NoteContainer.GetPoints()[mappedTo].Timestamp)));

            noteInfo.Error = std::abs(r - p);

            ++mappedNotes;
            cumulativeError += noteInfo.Error;
        }
        else {
            noteInfo.Error = 0.0;
        }
    }

    info.Bar = masterIndex;
    info.Tempo = CalculateTempo(bar);
    info.AverageError = cumulativeError / mappedNotes;

    delete[] mapping;
}

double toccata::Analyzer::CalculateTempo(const Timeline::MatchedBar &bar) const {
    const double refPulseRate = bar.Bar.MatchedBar->GetSegment()->PulseRate;
    const double inputPulseRate = m_timeline->GetInputSegment()->PulseRate;

    const double ratio = inputPulseRate / refPulseRate;

    const double scaledPulseRate = bar.Bar.T.Scale(refPulseRate);
    return scaledPulseRate * ratio * 60;
}
