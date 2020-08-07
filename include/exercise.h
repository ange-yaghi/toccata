#ifndef TOCCATA_CORE_EXERCISE_H
#define TOCCATA_CORE_EXERCISE_H

#include "delta.h"

#include "midi.h"
#include "performance_report.h"

namespace toccata {

	class Exercise : public ysObject {
	public:
		Exercise();
		~Exercise();

		void SetReferenceSegment(MidiPianoSegment *segment) { m_referenceSegment = segment; }

		PerformanceReport *NewPerformanceReport();
		PerformanceReport *GetPerformanceReport(int i) { return m_performanceReports.Get(i); }
		int GetPerformanceCount() const { return m_performanceReports.GetNumObjects(); }

		void SetName(const std::string &name) { m_name = name; }
		const char *GetName() const { return m_name.c_str(); }

	protected:
		std::string m_name;

		ysDynamicArray<PerformanceReport, 4> m_performanceReports;

		MidiPianoSegment *m_referenceSegment;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_EXERCISE_H */
