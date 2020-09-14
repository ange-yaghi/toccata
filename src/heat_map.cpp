#include "../include/heat_map.h"

toccata::HeatMap::HeatMap() {
    /* void */
}

toccata::HeatMap::~HeatMap() {
    /* void */
}

void toccata::HeatMap::Clear() {
    m_samples.clear();
}

void toccata::HeatMap::AddSample(double s, const ysVector &v) {
    m_samples.push_back({ s, v });
    std::sort(m_samples.begin(), m_samples.end(),
        [](const SamplePoint &a, const SamplePoint &b) { return a.s < b.s; });
}

ysVector toccata::HeatMap::Sample(double s) const {
    const int sampleCount = (int)m_samples.size();
    for (int i = 0; i < sampleCount - 1; ++i) {
        const SamplePoint &a = m_samples[i];
        const SamplePoint &b = m_samples[i + 1];

        if (a.s > s || b.s < s) continue;

        const double s0 = s - a.s;
        const double l = b.s - a.s;

        const double s_norm = s0 / l;

        return ysMath::Add(
            ysMath::Mul(a.v, ysMath::LoadScalar(1 - (float)s_norm)),
            ysMath::Mul(b.v, ysMath::LoadScalar((float)s_norm)));
    }

    if (sampleCount == 0) return ysMath::Constants::Zero;
    else if (s < m_samples[0].s) return m_samples[0].v;
    else if (s > m_samples[sampleCount - 1].s) return m_samples[sampleCount - 1].v;
    else return m_samples[0].v;
}
