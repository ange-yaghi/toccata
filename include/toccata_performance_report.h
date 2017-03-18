#ifndef TOCCATA_PERFORMANCE_REPORT_H
#define TOCCATA_PERFORMANCE_REPORT_H

#include <yds_base.h>
#include <time.h>

#include "toccata_midi.h"

class Toccata_PerformanceReport : public ysObject
{

	friend class Toccata_PatternDetector;
	friend class Toccata_Exercise;

public:

	struct PerformanceStats
	{

		int WrongNotes;
		int MissedNotes;
		int NonsenseNotes;
		int CorrectNotes;

		double AverageTimingError;
		double MaxTimingError;
		double MinTimingError;

		double ErrorStandardDeviation;
		double Choppiness;

	};

public:

	Toccata_PerformanceReport();
	~Toccata_PerformanceReport();

	MidiPianoSegment m_playedSegment;
	MidiPianoSegment m_referenceSegment;

	PerformanceStats m_leftHandStats;
	PerformanceStats m_rightHandStats;

	double m_latencyCorrection;

	void ResetStats(PerformanceStats *stats);
	void CorrectLatency();

	void SetMetronomeOffset(uint64_t offset) { m_metronomeOffset = offset; }

	void WriteToFile(std::ofstream &file);
	std::string GetDate();

	void WriteNewMasterFile(std::fstream &file);
	void WriteToMasterFile(std::fstream &file, const char *detailedRecord);

	// Sets the time played to right now
	void SetTime();

	const char *GetName() const;
	void SetPerformanceID(int id) { m_performanceID = id; }
	int GetPerformanceID() { return m_performanceID; }

	static double RelativeError(double absoluteError, int tempoBPM);

protected:

	Toccata_Exercise *m_parent;

	void CorrectLatency(MidiTrack *track, PerformanceStats *stats);

	time_t m_timePlayed;
	uint64_t m_metronomeOffset;
	bool m_latencyCorrected;
	int m_performanceID;

};

#endif
