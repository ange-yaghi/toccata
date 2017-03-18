#include "toccata_exercise.h"

#include "toccata_core.h"

Toccata_Exercise::Toccata_Exercise()
{

	m_referenceSegment = NULL;

}

Toccata_Exercise::~Toccata_Exercise()
{
}

Toccata_PerformanceReport *Toccata_Exercise::NewPerformanceReport()
{ 
	
	Toccata_PerformanceReport *performanceReport = m_performanceReports.New();
	performanceReport->m_parent = this;
	performanceReport->SetPerformanceID(Toccata.NewPerformanceID());

	return performanceReport; 

}

