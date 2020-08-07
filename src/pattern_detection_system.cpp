#include "../include/pattern_detection_system.h"
#include "../include/exercise.h"

#include "../include/core.h"

#include <string.h>

toccata::PatternDetectionSystem::PatternDetectionSystem() {
    m_core = nullptr;
    m_lockedDetector = nullptr;
}

toccata::PatternDetectionSystem::~PatternDetectionSystem() {
    /* void */
}

void toccata::PatternDetectionSystem::NewSegment(MidiPianoSegment *referenceSegment, Exercise *associatedExercise) {
    for (int i = 0; i < DetectorsPerSegment; i++) {
        PatternDetector *newDetector = m_patternDetectors.New();
        newDetector->SetCore(m_core);
        newDetector->SetReferenceSegment(referenceSegment);
        newDetector->SetExercise(associatedExercise);
        newDetector->Reset();
    }
}

void toccata::PatternDetectionSystem::ProcessNote(MidiNote *note) {
    if (m_lockedDetector != nullptr) {
        m_lockedDetector->ProcessNote(note);
    }
    else {
        int nDetectors = m_patternDetectors.GetNumObjects();

        for (int i = 0; i < nDetectors; i++) {
            m_patternDetectors.Get(i)->ProcessNote(note);
        }
    }
}

void toccata::PatternDetectionSystem::Synchronize(MidiTrack *playedTrack, int currentDeltaTime) {
    if (m_lockedDetector != nullptr) {
        m_lockedDetector->Synchronize(playedTrack, currentDeltaTime);
    }
    else {
        int nDetectors = m_patternDetectors.GetNumObjects();

        for (int i = 0; i < nDetectors; i++) {
            m_patternDetectors.Get(i)->Synchronize(playedTrack, currentDeltaTime);
        }
    }
}

void toccata::PatternDetectionSystem::Update() {
    if (m_lockedDetector != nullptr) {
        m_lockedDetector->CheckLock();

        if (!m_lockedDetector->IsLocked()) {
            // Detector lost lock on the song
            m_lockedDetector = nullptr;
        }
        else {
            if (m_lockedDetector->IsComplete()) {
                // Segment has completed

                // Create a new performance report
                PerformanceReport *newPerformanceReport = m_lockedDetector->GetExercise()->NewPerformanceReport();
                m_lockedDetector->WritePerformanceReport(newPerformanceReport, m_core->GetRecorder()->GetCurrentTarget());
                newPerformanceReport->SetTime();

                /* Write to file */

                std::string path = m_core->GetDirectory();

                path += newPerformanceReport->GetName();

                if (!Session::IsDirectory(path.c_str())) {
                    CreateDirectoryA(path.c_str(), NULL);
                }

                std::string mainPath = path;

                char buffer[256];
                sprintf_s(buffer, "%s %d.txt", newPerformanceReport->GetName(), newPerformanceReport->GetPerformanceID());

                path += "/";
                path += buffer;

                std::ofstream newFile;
                newFile.open(path);

                std::string detailedReport = path;

                newPerformanceReport->WriteToFile(newFile);

                newFile.close();

                // Write to master file

                path = m_core->GetTopDirectory();
                path += "/";
                path += "Master";

                if (!Session::IsDirectory(path.c_str())) {
                    CreateDirectoryA(path.c_str(), NULL);
                }

                path += "/";
                path += newPerformanceReport->GetName();
                path += "_Master.csv";

                std::fstream masterFile;

                masterFile.open(path, std::ios::in);

                if (!masterFile.is_open()) {
                    masterFile.open(path, std::ios::out);
                    newPerformanceReport->WriteNewMasterFile(masterFile);
                }
                else {
                    masterFile.close();
                    masterFile.open(path, std::ios::out | std::ios::app);
                }

                newPerformanceReport->WriteToMasterFile(masterFile, detailedReport.c_str());

                if (!masterFile.is_open()) {
                    printf("Could not open master file.\n");
                }

                masterFile.close();

                // Write the midi file
                path = mainPath;
                sprintf_s(buffer, "%s %d.mid", newPerformanceReport->GetName(), newPerformanceReport->GetPerformanceID());

                path += "/";
                path += buffer;

                MidiFile file;
                file.Write(path.c_str(), &newPerformanceReport->m_playedSegment);

                toccata::Core::Get()->GetDatabase()->WritePerformanceReport(newPerformanceReport, path.c_str(), detailedReport.c_str());

                // TEMP
                PerformanceReport test;
                toccata::Core::Get()->GetDatabase()->ReadPerformanceReport(newPerformanceReport->GetPerformanceID(), &test);

                // Reset the detector
                m_lockedDetector->Reset();
            }
        }
    }
    else {
        int nDetectors = m_patternDetectors.GetNumObjects();

        for (int i = 0; i < nDetectors; i++) {
            PatternDetector *detector = m_patternDetectors.Get(i);
            detector->CheckLock();

            if (detector->IsLocked()) {
                m_lockedDetector = detector;
                ResetAllExceptLocked();
            }
        }
    }
}

void toccata::PatternDetectionSystem::ResetAllExceptLocked() {
    int nDetectors = m_patternDetectors.GetNumObjects();

    for (int i = 0; i < nDetectors; i++) {
        PatternDetector *detector = m_patternDetectors.Get(i);

        if (detector != m_lockedDetector) {
            detector->Reset();
        }
    }
}
