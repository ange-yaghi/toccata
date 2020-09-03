#include "../include/bar_display.h"

#include "../include/transform.h"

#include <sstream>

toccata::BarDisplay::BarDisplay() {
    m_channelCount = 3;
    m_engine = nullptr;
    m_inputSegment = nullptr;
    m_referenceSegment = nullptr;
    m_position = ysMath::Constants::Zero;
    m_size = ysMath::Constants::Zero;
    m_timeOffset = 0.0;
    m_timeRange = 0.0;
    m_textRenderer = nullptr;
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

    const float totalHeight = ysMath::GetY(m_size);
    const float totalWidth = ysMath::GetX(m_size);

    const float channelHeight = totalHeight / m_channelCount;

    const float cornerX = ysMath::GetX(m_position);
    const float cornerY = ysMath::GetY(m_position);

    for (MatchedBar &bar : m_bars) {
        const int channel = bar.Channel;
        const double s0 = Transform::inv_f(0.0, bar.Bar.s, bar.Bar.t);
        const double e0 = Transform::inv_f(bar.Bar.MatchedBar->GetSegment()->Length, bar.Bar.s, bar.Bar.t);

        if (s0 >= m_timeRange + m_timeOffset) continue;
        if (e0 <= m_timeOffset) continue;

        const double local_s = GetLocalX(s0);
        const double local_e = GetLocalX(e0);

        const float y = cornerY - (m_channelCount - channel) * channelHeight;
        const float x = cornerX + local_s;
        const double width = local_e - local_s;

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));
        DrawBox(local_s + cornerX, y, width, channelHeight);

        RenderBarInformation(&bar.Bar, x, y, x + channelHeight, y + channelHeight);
    }
}

void toccata::BarDisplay::AllocateChannels() {
    const int barCount = (int)m_bars.size();
    for (int i = 0; i < barCount; ++i) {
        MatchedBar &b0 = m_bars[i];

        const double s0 = Transform::inv_f(0.0, b0.Bar.s, b0.Bar.t);
        const double e0 = Transform::inv_f(b0.Bar.MatchedBar->GetSegment()->Length, b0.Bar.s, b0.Bar.t);

        std::set<int> conflicts;
        for (int j = 0; j < i; ++j) {
            const MatchedBar &b1 = m_bars[j];

            const double s1 = Transform::inv_f(0.0, b1.Bar.s, b1.Bar.t);
            const double e1 = Transform::inv_f(b1.Bar.MatchedBar->GetSegment()->Length, b1.Bar.s, b1.Bar.t);

            if (s0 > e1 || e0 < s1) continue;
            if (s1 > e0 || e1 < s0) continue;

            const int channel = m_bars[j].Channel;
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

double toccata::BarDisplay::GetLocalX(double timestamp) const {
    const float width = ysMath::GetX(m_size);
    const float local = ((timestamp - m_timeOffset) / m_timeRange) * width;

    if (local > width) return width;
    else if (local < 0.0) return 0.0;
    else return local;
}

void toccata::BarDisplay::RenderBarInformation(
    DecisionTree::MatchedBar *matchedBar, float x0, float y0, float x1, float y1) 
{
    const int tempo = std::round(Transform::f(120.0, matchedBar->s, 0.0));

    std::stringstream ss;
    ss << tempo << " BPM";

    m_textRenderer->RenderText(ss.str(), x0, y0, 20.0f);
}
