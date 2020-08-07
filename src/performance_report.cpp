#include "../include/performance_report.h"

#include "../include/exercise.h"
#include "../include/session.h"

#include <sstream>
#include <iomanip>

toccata::PerformanceReport::PerformanceReport() {
    ResetStats(&m_leftHandStats);
    ResetStats(&m_rightHandStats);

    m_latencyCorrected = false;
    m_latencyCorrection = 0.0;

    m_metronomeOffset = 0;
    m_performanceID = 0;

    m_parent = NULL;
}

toccata::PerformanceReport::~PerformanceReport() {
    /* void */
}

void toccata::PerformanceReport::ResetStats(PerformanceStats *stats) {
    stats->MissedNotes = 0;
    stats->NonsenseNotes = 0;
    stats->WrongNotes = 0;
    stats->CorrectNotes = 0;

    stats->AverageTimingError = 0.0;
    stats->MaxTimingError = 0.0;
    stats->MinTimingError = 0.0;
}

void toccata::PerformanceReport::CorrectLatency() {
    CorrectLatency(&m_playedSegment.m_leftHand, &m_leftHandStats);
    CorrectLatency(&m_playedSegment.m_rightHand, &m_rightHandStats);

    m_latencyCorrected = true;
}

void toccata::PerformanceReport::CorrectLatency(MidiTrack *track, PerformanceStats *stats) {
    double accum = 0;
    double accum2 = 0;
    double deltaAccum = 0;
    int deltaSamples = 0;
    int samples = 0;

    int nNotes = track->GetNoteCount();

    stats->MaxTimingError = DBL_MIN;
    stats->MinTimingError = DBL_MAX;

    for (int i = 0; i < nNotes; i++) {
        MidiAnalysisNote *note = static_cast<MidiAnalysisNote *>(track->GetBuffer()[i]);

        if (note->m_errorType == MidiAnalysisNote::NoteErrorType::None) {
            note->m_timingError -= m_latencyCorrection;

            accum += abs(note->m_timingError);
            accum2 += note->m_timingError * note->m_timingError;
            samples++;

            if (abs(note->m_timingError) < stats->MinTimingError) stats->MinTimingError = abs(note->m_timingError);
            if (abs(note->m_timingError) > stats->MaxTimingError) stats->MaxTimingError = abs(note->m_timingError);

            if (i > 0) {
                MidiAnalysisNote *lastNote = static_cast<MidiAnalysisNote *>(track->GetBuffer()[i - 1]);

                if (lastNote->m_errorType == MidiAnalysisNote::NoteErrorType::None) {
                    deltaAccum += abs(note->m_timingError - lastNote->m_timingError);
                    deltaSamples++;
                }
            }
        }
    }

    if (samples > 0) {
        stats->AverageTimingError = accum / samples;
        stats->ErrorStandardDeviation = std::sqrt(accum2 / samples);
        stats->Choppiness = deltaAccum / deltaSamples;
    }
    else {
        stats->MaxTimingError = 0;
        stats->MinTimingError = 0;
        stats->AverageTimingError = 0.0;
        stats->ErrorStandardDeviation = 0.0;
        stats->Choppiness = 0.0;
    }
}

void toccata::PerformanceReport::SetTime() {
    time(&m_timePlayed);
}

const char *toccata::PerformanceReport::GetName() const {
    return m_parent->GetName();
}

double toccata::PerformanceReport::RelativeError(double absoluteError, int tempoBPM) {
    double period = 60.0 / tempoBPM;
    double period16 = period / 4;
    double period128 = period / 32;

    return absoluteError / (period128 * 1000);
}

void toccata::PerformanceReport::WriteToFile(std::ofstream &file) {
    int tempoBPM = m_playedSegment.GetTempoBPM();

    file << "Performance Report" << " [" << m_parent->GetName() << "]" << std::endl;

    file << std::endl;

    file << "- Performance Data ----------------------" << std::endl;
    file << "  Performance ID: \t" << m_performanceID << std::endl;
    file << "  Tempo: \t" << m_playedSegment.GetTempoBPM() << " bpm" << std::endl;
    file << "  Time Signature: \t" << m_playedSegment.GetTimeSignatureNumerator() << "/" << (1 << m_playedSegment.GetTimeSignatureDenominator()) << std::endl;
    file << "  Notes Played: \t" << m_playedSegment.m_leftHand.GetNoteCount() + m_playedSegment.m_rightHand.GetNoteCount() << std::endl;
    file << std::endl << std::endl;

    file << "- Left Hand Stats -----------------------" << std::endl;
    file << "  Correct notes: \t" << m_leftHandStats.CorrectNotes << std::endl;
    file << "  Wrong notes: \t" << m_leftHandStats.WrongNotes << std::endl;
    file << "  Missed notes: \t" << m_leftHandStats.MissedNotes << std::endl;
    file << "  Nonsense notes: \t" << m_leftHandStats.NonsenseNotes << std::endl;
    file << "  **" << std::endl;

    int leftHandTotalNotes = m_leftHandStats.CorrectNotes + m_leftHandStats.MissedNotes + m_leftHandStats.NonsenseNotes + m_leftHandStats.WrongNotes;

    file << "  Key accuracy: \t" << (m_leftHandStats.CorrectNotes / (double)leftHandTotalNotes) * 100 << "%" << std::endl;
    file << "  Timing accuracy: \t" << m_leftHandStats.AverageTimingError << " ms" << " [" << RelativeError(m_leftHandStats.AverageTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Max Error: \t" << m_leftHandStats.MaxTimingError << " ms" << " [" << RelativeError(m_leftHandStats.MaxTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Min Error: \t" << m_leftHandStats.MinTimingError << " ms" << " [" << RelativeError(m_leftHandStats.MinTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Std. Dev.: \t" << m_leftHandStats.ErrorStandardDeviation << " ms" << " [" << RelativeError(m_leftHandStats.ErrorStandardDeviation, tempoBPM) << " shds]" << std::endl;
    file << "  Choppiness: \t" << m_leftHandStats.Choppiness << " ms" << " [" << RelativeError(m_leftHandStats.Choppiness, tempoBPM) << " shds]" << std::endl;

    file << std::endl;

    // Right hand
    file << "- Right Hand Stats ----------------------" << std::endl;
    file << "  Correct notes: \t" << m_rightHandStats.CorrectNotes << std::endl;
    file << "  Wrong notes: \t" << m_rightHandStats.WrongNotes << std::endl;
    file << "  Missed notes: \t" << m_rightHandStats.MissedNotes << std::endl;
    file << "  Nonsense notes: \t" << m_rightHandStats.NonsenseNotes << std::endl;
    file << "  **" << std::endl;

    int rightHandTotalNotes = m_rightHandStats.CorrectNotes + m_rightHandStats.MissedNotes + m_rightHandStats.NonsenseNotes + m_rightHandStats.WrongNotes;

    file << "  Key accuracy: \t" << (m_rightHandStats.CorrectNotes / (double)rightHandTotalNotes) * 100 << "%" << std::endl;
    file << "  Timing accuracy: \t" << m_rightHandStats.AverageTimingError << " ms" << " [" << RelativeError(m_rightHandStats.AverageTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Max Error: \t" << m_rightHandStats.MaxTimingError << " ms" << " [" << RelativeError(m_rightHandStats.MaxTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Min Error: \t" << m_rightHandStats.MinTimingError << " ms" << " [" << RelativeError(m_rightHandStats.MinTimingError, tempoBPM) << " shds]" << std::endl;
    file << "  Std. Dev.: \t" << m_rightHandStats.ErrorStandardDeviation << " ms" << " [" << RelativeError(m_rightHandStats.ErrorStandardDeviation, tempoBPM) << " shds]" << std::endl;
    file << "  Choppiness: \t" << m_rightHandStats.Choppiness << " ms" << " [" << RelativeError(m_rightHandStats.Choppiness, tempoBPM) << " shds]" << std::endl;
}

void toccata::PerformanceReport::WriteNewMasterFile(std::fstream &file) {
    file << "Date,Tempo,";

    file << "Left Hand Correct Notes,Left Hand Wrong Notes,Left Hand Missed Notes,Left Hand Nonsense Notes,";
    file << "Left Hand Key Accuracy,Left Hand Timing Error,Left Hand Max Error,Left Hand Min Error,Left Hand Std. Dev.,Left Hand Choppiness,";
    file << "Left Hand Timing Accuracy (Rel.),Left Hand Max Error (Rel.),Left Hand Min Error (Rel.),Left Hand Std. Dev. (Rel.),Left Hand Choppiness (Rel.),";

    file << "Right Hand Correct Notes,Right Hand Wrong Notes,Right Hand Missed Notes,Right Hand Nonsense Notes,";
    file << "Right Hand Key Accuracy,Right Hand Timing Error,Right Hand Max Error,Right Hand Min Error,Right Hand Std. Dev.,Right Hand Choppiness,";
    file << "Right Hand Timing Accuracy (Rel.),Right Hand Max Error (Rel.),Right Hand Min Error (Rel.),Right Hand Std. Dev. (Rel.),Right Hand Choppiness (Rel.),";

    file << "Detailed File,";

    file << std::endl;
}

std::string toccata::PerformanceReport::GetDate() {
    struct tm timeInfo;
    localtime_s(&timeInfo, &m_timePlayed);

    int year = Session::GetYear(&timeInfo);
    int month = timeInfo.tm_mon + 1;
    int day = Session::GetDay(&timeInfo);
    int hour = (timeInfo.tm_hour);
    int minute = timeInfo.tm_min;

    std::stringstream ss;

    ss << std::setfill('0') << std::setw(2) << year << "-" << month << "-" << day << " " << hour << ":" << minute;

    return ss.str();
}

void toccata::PerformanceReport::WriteToMasterFile(std::fstream &file, const char *detailedFile) {
    int tempoBPM = m_playedSegment.GetTempoBPM();

    struct tm timeInfo;
    localtime_s(&timeInfo, &m_timePlayed);

    int year = Session::GetYear(&timeInfo);
    int month = timeInfo.tm_mon + 1;
    int day = Session::GetDay(&timeInfo);
    int hour = (timeInfo.tm_hour);
    int minute = timeInfo.tm_min;

    file << year << "-" << month << "-" << day << " " << hour << ":" << minute << ",";
    file << m_playedSegment.GetTempoBPM() << ",";

    file << m_leftHandStats.CorrectNotes << ",";
    file << m_leftHandStats.WrongNotes << ",";
    file << m_leftHandStats.MissedNotes << ",";
    file << m_leftHandStats.NonsenseNotes << ",";
    file << (m_leftHandStats.CorrectNotes / (double)(m_leftHandStats.CorrectNotes + m_leftHandStats.MissedNotes + m_leftHandStats.WrongNotes + m_leftHandStats.NonsenseNotes)) << ",";
    file << m_leftHandStats.AverageTimingError << ",";
    file << m_leftHandStats.MaxTimingError << ",";
    file << m_leftHandStats.MinTimingError << ",";
    file << m_leftHandStats.ErrorStandardDeviation << ",";
    file << m_leftHandStats.Choppiness << ",";
    file << RelativeError(m_leftHandStats.AverageTimingError, tempoBPM) << ",";
    file << RelativeError(m_leftHandStats.MaxTimingError, tempoBPM) << ",";
    file << RelativeError(m_leftHandStats.MinTimingError, tempoBPM) << ",";
    file << RelativeError(m_leftHandStats.ErrorStandardDeviation, tempoBPM) << ",";
    file << RelativeError(m_leftHandStats.Choppiness, tempoBPM) << ",";

    file << m_rightHandStats.CorrectNotes << ",";
    file << m_rightHandStats.WrongNotes << ",";
    file << m_rightHandStats.MissedNotes << ",";
    file << m_rightHandStats.NonsenseNotes << ",";
    file << (m_rightHandStats.CorrectNotes / (double)(m_rightHandStats.CorrectNotes + m_rightHandStats.MissedNotes + m_rightHandStats.WrongNotes + m_rightHandStats.NonsenseNotes)) << ",";
    file << m_rightHandStats.AverageTimingError << ",";
    file << m_rightHandStats.MaxTimingError << ",";
    file << m_rightHandStats.MinTimingError << ",";
    file << m_rightHandStats.ErrorStandardDeviation << ",";
    file << m_rightHandStats.Choppiness << ",";
    file << RelativeError(m_rightHandStats.AverageTimingError, tempoBPM) << ",";
    file << RelativeError(m_rightHandStats.MaxTimingError, tempoBPM) << ",";
    file << RelativeError(m_rightHandStats.MinTimingError, tempoBPM) << ",";
    file << RelativeError(m_rightHandStats.ErrorStandardDeviation, tempoBPM) << ",";
    file << RelativeError(m_rightHandStats.Choppiness, tempoBPM) << ",";

    file << detailedFile;
    file << std::endl;
}
