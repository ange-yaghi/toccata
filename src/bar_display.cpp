#include "../include/bar_display.h"

#include "../include/transform.h"

#include <sstream>

toccata::BarDisplay::BarDisplay() {
    m_channelCount = 3;
}

toccata::BarDisplay::~BarDisplay() {
    /* void */
}

void toccata::BarDisplay::Initialize(dbasic::DeltaEngine *engine) {
    m_engine = engine;
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
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.Bar);
        const MusicSegment *segment = bar.Bar.MatchedBar->GetSegment();
        const int channel = bar.Channel;

        const double s0 = m_timeline->ReferenceToInputSpace(0.0, bar.Bar.T);
        const double e0 = m_timeline->ReferenceToInputSpace(segment->GetNormalizedLength(), bar.Bar.T);

        const float world_s = (float)m_timeline->InputSpaceToWorldX(s0);
        const float world_e = (float)m_timeline->InputSpaceToWorldX(e0);

        const float y = cornerY - (m_channelCount - channel) * channelHeight;
        const float x = world_s;
        const float width = world_e - world_s;

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));
        DrawBox(x, y, width, channelHeight);

        RenderBarInformation(info, x, y, x + channelHeight, y + channelHeight);
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

void toccata::BarDisplay::DrawBox(float x, float y, float w, float h) {
    m_engine->SetObjectTransform(ysMath::TranslationTransform(ysMath::LoadVector(x + w / 2, y + h / 2)));
    m_engine->DrawBox(w, h);
}

void toccata::BarDisplay::RenderBarInformation(
    const Analyzer::BarInformation &info, float x0, float y0, float x1, float y1)
{
    std::stringstream ss;
    ss << (int)std::round(info.Tempo) << " BPM";

    m_textRenderer->RenderText(ss.str(), x0, y0, 30.0f);

    ss = std::stringstream();
    ss << "B: " << info.Bar;

    m_textRenderer->RenderText(ss.str(), x0 + 175.0f, y0, 20.0f);

    const Timeline::MatchedBar &bar = m_timeline->GetBar(info.Bar);
    int missedNotes = 0;
    for (const Analyzer::NoteInformation &noteInfo : info.NoteInformation) {
        if (noteInfo.InputNote == -1) ++missedNotes;
    }

    ss = std::stringstream();
    ss << "ERR: " << info.AverageError;

    if (missedNotes > 0) {
        ss << "/" << missedNotes;
    }
    else {
        ss << "/-";
    }

    m_textRenderer->RenderText(ss.str(), x0 + 175.0f, y0 + 20.0f, 20.0f);
}
