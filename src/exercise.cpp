#include "../include/exercise.h"

#include "../include/core.h"

toccata::Exercise::Exercise() {
    m_referenceSegment = nullptr;
}

toccata::Exercise::~Exercise() {
    /* void */
}

toccata::PerformanceReport *toccata::Exercise::NewPerformanceReport() {
    PerformanceReport *performanceReport = m_performanceReports.New();
    performanceReport->m_parent = this;
    performanceReport->SetPerformanceID(toccata::Core::Get()->NewPerformanceID());

    return performanceReport;
}
