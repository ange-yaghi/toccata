#ifndef TOCCATA_EXERCISE_H
#define TOCCATA_EXERCISE_H

#include <yds_base.h>

#include "toccata_midi.h"
#include "toccata_performance_report.h"

class Toccata_Exercise : public ysObject
{

public:

	Toccata_Exercise();
	~Toccata_Exercise();

	void SetReferenceSegment(MidiPianoSegment *segment) { m_referenceSegment = segment; }

	Toccata_PerformanceReport *NewPerformanceReport();
	Toccata_PerformanceReport *GetPerformanceReport(int i) { return m_performanceReports.Get(i); }
	int GetPerformanceCount() const { return m_performanceReports.GetNumObjects(); }

	void SetName(const std::string &name) { m_name = name; }
	const char *GetName() const { return m_name.c_str();  }

protected:

	std::string m_name;

	ysDynamicArray<Toccata_PerformanceReport, 4> m_performanceReports;

	MidiPianoSegment *m_referenceSegment;

};

#endif