#ifndef TOCCATA_SESSION_H
#define TOCCATA_SESSION_H

#include <yds_base.h>
#include <time.h>

#include "toccata_exercise.h"

class Toccata_Session : public ysObject
{

public:

	Toccata_Session();
	~Toccata_Session();

	static bool IsDirectory(const char *dirName);
	static void FormatTime(char *targetString, time_t t);

	static int GetYear(const tm *time);
	static void GetMonth(const tm *time, char *target);
	static int GetDay(const tm *time);

	void CreateSessionDirectory();

	void SetDirectory(const char *dir) { m_directory = dir; }

	const std::string &GetTopLevelDirectory() const { return m_directory; }
	const std::string &GetFullDirectory() const { return m_fullDirectory; }

	Toccata_Exercise *NewExercise(const char *name, MidiPianoSegment *segment);

protected:

	std::string m_directory;
	std::string m_fullDirectory;

	ysDynamicArray<Toccata_Exercise, 4> m_exercises;

	time_t m_time;
	int m_sessionID;

};

#endif