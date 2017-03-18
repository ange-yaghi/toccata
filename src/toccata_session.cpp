#include "toccata_session.h"

Toccata_Session::Toccata_Session()
{

	m_sessionID = -1;

	m_directory = "";
	m_fullDirectory = "";

}

Toccata_Session::~Toccata_Session()
{



}

bool Toccata_Session::IsDirectory(const char *dirName)
{

	DWORD ftyp = GetFileAttributesA(dirName);

	if (ftyp == INVALID_FILE_ATTRIBUTES) return false;		// Error getting file attributes/not accessible
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;		// The directory exists
	else return false;										// Director doesn't exist

}

int Toccata_Session::GetYear(const tm *time)
{

	return time->tm_year + 1900;

}

void Toccata_Session::GetMonth(const tm *time, char *target)
{

	static const char mon_name[][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	strcpy(target, mon_name[time->tm_mon]);

}

int Toccata_Session::GetDay(const tm *time)
{

	return time->tm_mday;

}

void Toccata_Session::FormatTime(char *targetString, time_t t)
{

	static const char wday_name[][4] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};


	

}

void Toccata_Session::CreateSessionDirectory()
{

	time(&m_time);

	struct tm *timeInfo;
	timeInfo = localtime(&m_time);

	char month[256];

	int year = GetYear(timeInfo);
	GetMonth(timeInfo, month);
	int day = GetDay(timeInfo);

	std::string path = "";
	path += m_directory;
	
	if (!IsDirectory(path.c_str()))
	{

		CreateDirectoryA(path.c_str(), NULL);

	}

	path += "/";

	char buffer[256];
	sprintf(buffer, "%d", year);

	path += buffer;

	if (!IsDirectory(path.c_str()))
	{

		CreateDirectoryA(path.c_str(), NULL);

	}

	path += "/";
	path += month;

	if (!IsDirectory(path.c_str()))
	{

		CreateDirectoryA(path.c_str(), NULL);

	}

	sprintf(buffer, "%s-%d", month, GetDay(timeInfo));

	path += "/";
	path += buffer;

	if (!IsDirectory(path.c_str()))
	{

		CreateDirectoryA(path.c_str(), NULL);

	}

	m_sessionID = 1;

	while (true)
	{

		char sessionID[32];
		sprintf(sessionID, "Session %d", m_sessionID);

		std::string dir = path + "/";
		dir += sessionID;

		if (!IsDirectory(dir.c_str()))
		{

			CreateDirectoryA(dir.c_str(), NULL);

			path = dir;
			break;

		}

		else
		{

			m_sessionID++;

		}

	}

	path += "/";
	m_fullDirectory = path;

}

Toccata_Exercise *Toccata_Session::NewExercise(const char *name, MidiPianoSegment *segment)
{

	Toccata_Exercise *newExercise = m_exercises.New();
	newExercise->SetReferenceSegment(segment);
	newExercise->SetName(name);

	return newExercise;

}