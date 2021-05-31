#include "../include/bar_display.h"

#include "../include/transform.h"
#include "../include/grid.h"

#include <sstream>

#undef min
#undef max

toccata::BarDisplay::BarDisplay() {
    m_channelCount = 3;
}

toccata::BarDisplay::~BarDisplay() {
    /* void */
}

void toccata::BarDisplay::Process() {
    /* void */
}

void toccata::BarDisplay::Render() {
    AllocateChannels();

    if (m_channelCount == 0) return;

    const float totalHeight = m_height;
    const float totalWidth = m_timeline->GetWidth();

    const float channelHeight = totalHeight / m_channelCount;

    const float cornerX = m_timeline->GetPositionX();
    const float cornerY = m_positionY;

    const int n = m_analyzer->GetBarCount();

    for (int i = 0; i < n; ++i) {
        const Analyzer::BarInformation &info = m_analyzer->GetBar(i);
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.MasterIndex);
        const MusicSegment *segment = bar.Bar.MatchedBar->GetSegment();
        const int channel = bar.Channel;

        const double s0 = m_timeline->ReferenceToInputSpace(0.0, bar.Bar.T);
        const double e0 = m_timeline->ReferenceToInputSpace(segment->GetNormalizedLength(), bar.Bar.T);

        const float world_s = (float)m_timeline->InputSpaceToWorldX(s0);
        const float world_e = (float)m_timeline->InputSpaceToWorldX(e0);

        const float y = cornerY - (m_channelCount - channel) * channelHeight;
        const float x = world_s;
        const float width = world_e - world_s;

        const ysVector color = m_settings->BarDisplay_BackgroundColor;
        DrawBox(BoundingBox(width, channelHeight)
            .AlignBottom(y)
            .AlignLeft(x), color);

        RenderBarInformation(info, 
            BoundingBox(width, channelHeight)
                .AlignLeft(x)
                .AlignBottom(y));
    }
}

void toccata::BarDisplay::AllocateChannels() {
    m_channelCount = m_settings->BarDisplay_MinimumChannelCount;

    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        Timeline::MatchedBar &b0 = m_timeline->GetBar(i);

        const double s0 = m_timeline->ReferenceToInputSpace(0.0, b0.Bar.T);
        const double e0 = m_timeline->ReferenceToInputSpace(
            b0.Bar.MatchedBar->GetSegment()->GetNormalizedLength(), b0.Bar.T);

        if (!m_timeline->InRangeInputSpace(s0, e0)) continue;

        std::set<int> conflicts;
        for (int j = 0; j < i; ++j) {
            const Timeline::MatchedBar &b1 = m_timeline->GetBar(j);

            const double s1 = m_timeline->ReferenceToInputSpace(0.0, b1.Bar.T);
            const double e1 = m_timeline->ReferenceToInputSpace(
                b1.Bar.MatchedBar->GetSegment()->GetNormalizedLength(), b1.Bar.T);

            if (!m_timeline->InRangeInputSpace(s1, e1)) continue;

            if (s0 > e1 || e0 < s1) continue;
            if (s1 > e0 || e1 < s0) continue;

            const int channel = b1.Channel;
            conflicts.insert(channel);
        }

        for (int c = 0; c <= m_channelCount; ++c) {
            if (c == m_channelCount) ++m_channelCount;

            if (conflicts.count(c) == 0) {
                b0.Channel = c;
                break;
            }
        }
    }
}

void toccata::BarDisplay::RenderBarInformation(
    const Analyzer::BarInformation &info,
    const BoundingBox &box)
{
    Grid grid;
    grid.SetBoundingBox(box);
    grid.SetVerticalDivisions(2);
    grid.SetHorizontalDivisions(2);
    grid.SetVerticalMargin(2.0f);
    grid.SetHorizontalMargin(5.0f);
    
    // BPM
    BoundingBox bpmBox;
    grid.GetCell(0, 1, bpmBox);

    std::stringstream ss;
    ss << (int)std::round(info.Tempo) << " BPM";
    const std::string bpmText = ss.str();

    // Bar number
    BoundingBox barBox;
    grid.GetCell(1, 1, barBox);

    ss = std::stringstream();
    ss << "SEQ " << info.Index + 1;
    const std::string barText = ss.str();

    // Error
    BoundingBox errBox;
    grid.GetCell(0, 0, errBox);

    ss = std::stringstream();
    ss.precision(2);
    ss << std::fixed << info.AverageError * 1000 << " MIL";
    const std::string errText = ss.str();

    // Missed notes
    const Timeline::MatchedBar &bar = m_timeline->GetBar(info.MasterIndex);
    int missedNotes = 0;
    for (const Analyzer::NoteInformation &noteInfo : info.NoteInformation) {
        if (noteInfo.InputNote == -1) ++missedNotes;
    }

    BoundingBox missedNotesBox;
    grid.GetCell(1, 0, missedNotesBox);

    ss = std::stringstream();
    if (missedNotes > 0) {
        ss << "-" << missedNotes;
    }
    else {
        ss << "CLEAN";
    }

    const std::string missedNotesText = ss.str();

    // Draw text
    const float maxTextSize = std::min(
        std::min(CalculateFontSize(bpmText, 30.0f, 10.0f, bpmBox), CalculateFontSize(errText, 30.0f, 10.0f, errBox) / 0.75f),
        std::min(CalculateFontSize(missedNotesText, 30.0f, 10.0f, barBox) / 0.75f, CalculateFontSize(barText, 30.0f, 10.0f, missedNotesBox) / 0.75f)
    );

    RenderText(bpmText, { bpmBox.Left(), bpmBox.Bottom() }, maxTextSize);
    RenderText(errText, { errBox.Left(), errBox.Bottom() }, maxTextSize * 0.75f);
    RenderText(missedNotesText, { missedNotesBox.Left(), missedNotesBox.Bottom() }, maxTextSize * 0.75f);
    RenderText(barText, { barBox.Left(), barBox.Bottom() }, maxTextSize * 0.75f);
}
