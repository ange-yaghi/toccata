#ifndef TOCCATA_CORE_SESSION_H
#define TOCCATA_CORE_SESSION_H

#include "exercise.h"

#include "delta.h"

#include <time.h>

namespace toccata {

    class Session : public ysObject {
    public:
        Session();
        ~Session();

        static bool IsDirectory(const char *dirName);
        static void FormatTime(char *targetString, time_t t);

        static int GetYear(const tm *time);
        static void GetMonth(const tm *time, char *target);
        static int GetDay(const tm *time);

        void CreateSessionDirectory();

        void SetDirectory(const char *dir) { m_directory = dir; }

        const std::string &GetTopLevelDirectory() const { return m_directory; }
        const std::string &GetFullDirectory() const { return m_fullDirectory; }

        Exercise *NewExercise(const char *name, MidiPianoSegment *segment);

    protected:
        std::string m_directory;
        std::string m_fullDirectory;

        ysDynamicArray<Exercise, 4> m_exercises;

        time_t m_time;
        int m_sessionID;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SESSION_H */
