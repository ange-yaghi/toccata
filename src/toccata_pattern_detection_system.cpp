#include "toccata_pattern_detection_system.h"
#include "toccata_exercise.h"

#include "toccata_core.h"

#include <string.h>

Toccata_PatternDetectionSystem::Toccata_PatternDetectionSystem()
{

	m_core = NULL;
	m_lockedDetector = NULL;

}

Toccata_PatternDetectionSystem::~Toccata_PatternDetectionSystem()
{
}

void Toccata_PatternDetectionSystem::NewSegment(MidiPianoSegment *referenceSegment, Toccata_Exercise *associatedExercise)
{

	for (int i = 0; i < DETECTORS_PER_SEGMENT; i++)
	{

		Toccata_PatternDetector *newDetector = m_patternDetectors.New();
		newDetector->SetCore(m_core);
		newDetector->SetReferenceSegment(referenceSegment);
		newDetector->SetExercise(associatedExercise);
		newDetector->Reset();

	}

}

void Toccata_PatternDetectionSystem::ProcessNote(MidiNote *note)
{

	if (m_lockedDetector != NULL)
	{

		m_lockedDetector->ProcessNote(note);

	}

	else
	{

		int nDetectors = m_patternDetectors.GetNumObjects();

		for (int i = 0; i < nDetectors; i++)
		{

			m_patternDetectors.Get(i)->ProcessNote(note);

		}

	}

}

void Toccata_PatternDetectionSystem::Synchronize(MidiTrack *playedTrack, int currentDeltaTime)
{

	if (m_lockedDetector != NULL)
	{

		m_lockedDetector->Synchronize(playedTrack, currentDeltaTime);

	}

	else
	{

		int nDetectors = m_patternDetectors.GetNumObjects();

		for (int i = 0; i < nDetectors; i++)
		{

			m_patternDetectors.Get(i)->Synchronize(playedTrack, currentDeltaTime);

		}

	}

}

void Toccata_PatternDetectionSystem::Update()
{

	if (m_lockedDetector != NULL)
	{

		m_lockedDetector->CheckLock();

		if (!m_lockedDetector->IsLocked())
		{

			// Detector lost lock on the song
			m_lockedDetector = NULL;

		}

		else
		{

			if (m_lockedDetector->IsComplete())
			{

				/* Segment has completed */

				// Create a new performance report
				Toccata_PerformanceReport *newPerformanceReport = m_lockedDetector->GetExercise()->NewPerformanceReport();
				m_lockedDetector->WritePerformanceReport(newPerformanceReport, m_core->GetRecorder()->GetCurrentTarget());
				newPerformanceReport->SetTime();

				/* Write to file */

				std::string path = m_core->GetDirectory();

				path += newPerformanceReport->GetName();

				if (!Toccata_Session::IsDirectory(path.c_str()))
				{

					CreateDirectoryA(path.c_str(), NULL);

				}

				std::string mainPath = path;

				char buffer[256];
				sprintf(buffer, "%s %d.txt", newPerformanceReport->GetName(), newPerformanceReport->GetPerformanceID());

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

				if (!Toccata_Session::IsDirectory(path.c_str()))
				{

					CreateDirectoryA(path.c_str(), NULL);

				}

				path += "/";
				path += newPerformanceReport->GetName();
				path += "_Master.csv";

				std::fstream masterFile;

				masterFile.open(path, std::ios::in);

				if (!masterFile.is_open())
				{

					masterFile.open(path, std::ios::out);
					newPerformanceReport->WriteNewMasterFile(masterFile);

				}

				else
				{

					masterFile.close();
					masterFile.open(path, std::ios::out | std::ios::app);

				}

				newPerformanceReport->WriteToMasterFile(masterFile, detailedReport.c_str());

				if (!masterFile.is_open())
				{

					printf("Could not open master file.\n");

				}

				masterFile.close();

				/*
				std::ofstream outputFile("real_time_vs_midi_stamp.txt");
				
				int nNotes = newPerformanceReport->m_playedSegment.m_rightHand.GetNoteCount();
				MidiAnalysisNote **notes = (MidiAnalysisNote **)newPerformanceReport->m_playedSegment.m_rightHand.GetBuffer();

				for (int i = 0; i < nNotes; i++)
				{

					outputFile << notes[i]->m_realTimeError << "\t" << notes[i]->m_timingError << "\n";

				}

				outputFile.close();
				*/

				// Write the midi file
				path = mainPath;	
				sprintf(buffer, "%s %d.mid", newPerformanceReport->GetName(), newPerformanceReport->GetPerformanceID());

				path += "/";
				path += buffer;

				MidiFile file;
				file.Write(path.c_str(), &newPerformanceReport->m_playedSegment);

				Toccata.GetDatabase()->WritePerformanceReport(newPerformanceReport, path.c_str(), detailedReport.c_str());

				// TEMP
				Toccata_PerformanceReport test;
				Toccata.GetDatabase()->ReadPerformanceReport(newPerformanceReport->GetPerformanceID(), &test);
	
				// Reset the detector
				m_lockedDetector->Reset();

			}

		}

	}

	else
	{

		int nDetectors = m_patternDetectors.GetNumObjects();

		for (int i = 0; i < nDetectors; i++)
		{

			Toccata_PatternDetector *detector = m_patternDetectors.Get(i);

			detector->CheckLock();

			if (detector->IsLocked())
			{

				m_lockedDetector = detector;
				ResetAllExceptLocked();

			}

		}

	}

}

void Toccata_PatternDetectionSystem::ResetAllExceptLocked()
{

	int nDetectors = m_patternDetectors.GetNumObjects();

	for (int i = 0; i < nDetectors; i++)
	{

		Toccata_PatternDetector *detector = m_patternDetectors.Get(i);

		if (detector != m_lockedDetector)
		{

			detector->Reset();

		}

	}

}