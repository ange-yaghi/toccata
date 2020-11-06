#include "../include/metrics_panel.h"

#include "../include/grid.h"

#include <sstream>

toccata::MetricsPanel::MetricsPanel() {
    m_decisionThread = nullptr;
}

toccata::MetricsPanel::~MetricsPanel() {
    /* void */
}

void toccata::MetricsPanel::Construct() {

}

void toccata::MetricsPanel::Render() {
    Grid grid(m_boundingBox, 5, 5, 5.0f);

    RenderText("Latency", grid.GetRange(0, 0, 0, 0), 15.0f, 5.0f);
    RenderText("Index", grid.GetRange(0, 0, 1, 1), 15.0f, 5.0f);
    RenderText("Current", grid.GetRange(1, 1, 2, 2), 15.0f, 5.0f);
    RenderText("Target", grid.GetRange(2, 2, 2, 2), 15.0f, 5.0f);
    RenderText("Threads", grid.GetRange(3, 3, 4, 4), 15.0f, 5.0f);

    const int peakIndex = m_decisionThread->ReadPeakIndex();
    const int peakTargetIndex = m_decisionThread->ReadPeakTargetIndex();
    const double peakLatency = m_decisionThread->ReadPeakLatency();

    std::stringstream ss;
    ss << peakIndex;
    RenderText(ss.str(), grid.GetRange(1, 1, 1, 1), 20.0f, 5.0f);

    ss = std::stringstream();
    ss.precision(2);
    ss << std::fixed << peakLatency * 1000.0 << " ms";
    RenderText(ss.str(), grid.GetRange(1, 1, 0, 0), 20.0f, 5.0f);

    ss = std::stringstream();
    ss << peakTargetIndex;
    RenderText(ss.str(), grid.GetRange(2, 2, 1, 1), 20.0f, 5.0f);

    // Calculate target latency
    const int delta = peakTargetIndex - peakIndex;
    const double targetLatency = (delta > 0)
        ? m_engine->GetFrameLength() / delta
        : 0;

    ss = std::stringstream();
    ss.precision(2);
    ss << std::fixed << targetLatency;
    RenderText(ss.str(), grid.GetRange(2, 2, 0, 0), 20.0f, 5.0f);
}

void toccata::MetricsPanel::Update() {
    
}
