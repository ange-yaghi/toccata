#include "../include/bar_display.h"

#include "../include/transform.h"

#include <sstream>

toccata::BarDisplay::BarDisplay() {
    m_channelCount = 3;
    m_minimumChannelCount = 3;
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

    const float totalHeight = m_height;
    const float totalWidth = m_timeline->GetWidth();

    const float channelHeight = totalHeight / m_channelCount;

    const float cornerX = m_timeline->GetPositionX();
    const float cornerY = m_positionY;

    const int n = m_timeline->GetBarCount();

    for (int i = 0; i < n; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);

        const int channel = bar.Channel;
        const double s0 = 
            Transform::inv_f(0.0, bar.Bar.s, bar.Bar.t);
        const double e0 = 
            Transform::inv_f(bar.Bar.MatchedBar->GetSegment()->Length, bar.Bar.s, bar.Bar.t);

        if (e0 < m_timeline->GetTimeOffset()) continue;
        if (s0 > m_timeline->GetTimeOffset() + m_timeline->GetTimeRange()) continue;

        const double world_s = m_timeline->GetWorldX(s0);
        const double world_e = m_timeline->GetWorldX(e0);

        const float y = cornerY - (m_channelCount - channel) * channelHeight;
        const float x = world_s;
        const double width = world_e - world_s;

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));
        DrawBox(x, y, width, channelHeight);

        RenderBarInformation(&bar.Bar, x, y, x + channelHeight, y + channelHeight);
    }
}

void toccata::BarDisplay::AllocateChannels() {
    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        Timeline::MatchedBar &b0 = m_timeline->GetBar(i);

        const double s0 = Transform::inv_f(0.0, b0.Bar.s, b0.Bar.t);
        const double e0 = Transform::inv_f(b0.Bar.MatchedBar->GetSegment()->Length, b0.Bar.s, b0.Bar.t);

        std::set<int> conflicts;
        for (int j = 0; j < i; ++j) {
            const Timeline::MatchedBar &b1 = m_timeline->GetBar(j);

            const double s1 = Transform::inv_f(0.0, b1.Bar.s, b1.Bar.t);
            const double e1 = Transform::inv_f(b1.Bar.MatchedBar->GetSegment()->Length, b1.Bar.s, b1.Bar.t);

            if (s0 > e1 || e0 < s1) continue;
            if (s1 > e0 || e1 < s0) continue;

            const int channel = b1.Channel;
            conflicts.insert(channel);
        }

        for (int c = 0; c < m_channelCount; ++c) {
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
    const DecisionTree::MatchedBar *matchedBar, float x0, float y0, float x1, float y1) 
{
    const int tempo = std::round(Transform::f(120.0, matchedBar->s, 0.0));

    std::stringstream ss;
    ss << tempo << " BPM";

    m_textRenderer->RenderText(ss.str(), x0, y0, 20.0f);
}
