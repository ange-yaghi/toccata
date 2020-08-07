#include "../include/session.h"

toccata::Session::Session() {
    m_sessionID = -1;

    m_directory = "";
    m_fullDirectory = "";
}

toccata::Session::~Session() {
    /* void */
}

bool toccata::Session::IsDirectory(const char *dirName) {
    DWORD ftyp = GetFileAttributesA(dirName);

    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;		// Error getting file attributes/not accessible
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;		// The directory exists
    else return false;										// Director doesn't exist
}

int toccata::Session::GetYear(const tm *time) {
    return time->tm_year + 1900;
}

void toccata::Session::GetMonth(const tm *time, char *target) {
    static const char mon_name[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    strcpy_s(target, 1024, mon_name[time->tm_mon]);
}

int toccata::Session::GetDay(const tm *time) {
    return time->tm_mday;
}

void toccata::Session::FormatTime(char *targetString, time_t t) {
    static const char wday_name[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
}

void toccata::Session::CreateSessionDirectory() {
    time(&m_time);

    struct tm timeInfo;
    localtime_s(&timeInfo, &m_time);

    char month[256];

    int year = GetYear(&timeInfo);
    GetMonth(&timeInfo, month);
    int day = GetDay(&timeInfo);

    std::string path = "";
    path += m_directory;

    if (!IsDirectory(path.c_str())) {
        CreateDirectoryA(path.c_str(), NULL);
    }

    path += "/";

    char buffer[256];
    sprintf_s(buffer, "%d", year);

    path += buffer;

    if (!IsDirectory(path.c_str())) {
        CreateDirectoryA(path.c_str(), NULL);
    }

    path += "/";
    path += month;

    if (!IsDirectory(path.c_str())) {
        CreateDirectoryA(path.c_str(), NULL);
    }

    sprintf_s(buffer, "%s-%d", month, GetDay(&timeInfo));

    path += "/";
    path += buffer;

    if (!IsDirectory(path.c_str())) {
        CreateDirectoryA(path.c_str(), NULL);
    }

    m_sessionID = 1;

    while (true) {
        char sessionID[32];
        sprintf_s(sessionID, "Session %d", m_sessionID);

        std::string dir = path + "/";
        dir += sessionID;

        if (!IsDirectory(dir.c_str())) {
            CreateDirectoryA(dir.c_str(), NULL);

            path = dir;
            break;
        }
        else {
            m_sessionID++;
        }
    }

    path += "/";
    m_fullDirectory = path;
}

toccata::Exercise *toccata::Session::NewExercise(const char *name, MidiPianoSegment *segment) {
    Exercise *newExercise = m_exercises.New();
    newExercise->SetReferenceSegment(segment);
    newExercise->SetName(name);

    return newExercise;
}
