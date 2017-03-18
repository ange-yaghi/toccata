#include "toccata_database.h"
#include <sstream>
#include <string>

#include <stdio.h>
#include <string.h>

const char *Toccata_Database::ReportColumns_v1_0 =
	"PerformanceID UNIQUE,"
	"Date,"
	"Tempo,"

	"LH_Correct_Notes,"
	"LH_Wrong_Notes,"
	"LH_Missed_Notes,"
	"LH_Nonsense_Notes,"

	"LH_Key_Accuracy,"
	"LH_Timing_Error,"
	"LH_Max_Error,"
	"LH_Min_Error,"
	"LH_Std_Dev,"
	"LH_Choppiness,"

	"RH_Correct_Notes,"
	"RH_Wrong_Notes,"
	"RH_Missed_Notes,"
	"RH_Nonsense_Notes,"

	"RH_Key_Accuracy,"
	"RH_Timing_Error,"
	"RH_Max_Error,"
	"RH_Min_Error,"
	"RH_Std_Dev,"
	"RH_Choppiness,"

	"Detailed_File,"
	"Midi_File";

#include "toccata_core.h"


Toccata_Database::Toccata_Database()
{

	m_database = NULL;

}

Toccata_Database::~Toccata_Database()
{
}

void Toccata_Database::OpenDatabase()
{

	int result = sqlite3_open("toccata_database.tdb", &m_database);

	if (result != SQLITE_OK)
	{

		// TODO: better error reporting
		printf("Could not open database.\n");

	}

	InitializeDatabase();

}

bool Toccata_Database::DoesTableExist(const char *tableName)
{

	sqlite3_stmt *statement;
	char buffer[256];
	int result;
	bool found = true;

	sprintf(buffer, "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", tableName);

	result = sqlite3_prepare_v2(m_database, buffer, -1, &statement, NULL);
	
	result = sqlite3_step(statement);

	if (result != SQLITE_ROW)
	{

		found = false;

	}

	// Free the statement
	sqlite3_finalize(statement);

	return found;

}

void Toccata_Database::RenameTable(const char *name, const char *newName)
{

	sqlite3_stmt *statement;
	char buffer[256];
	int result;
	bool found = true;

	sprintf(buffer, "ALTER TABLE %s RENAME TO %s", name, newName);

	result = sqlite3_exec(m_database, buffer, NULL, NULL, NULL);

	if (result != SQLITE_OK &&
		result != SQLITE_ROW &&
		result != SQLITE_DONE)
	{

		// TODO: error could not create table

	}

}

int Toccata_Database::ReadSettingInt(const char *settingName)
{

	int result;
	char buffer[1024];
	sqlite3_stmt *statement;

	sprintf(buffer, "SELECT * FROM %s WHERE Parameter = '%s'", "toccata_application_main", settingName);

	result = sqlite3_prepare(m_database, buffer, -1, &statement, NULL);
	result = sqlite3_step(statement);

	int value = sqlite3_column_int(statement, 1);
	result = sqlite3_step(statement);

	sqlite3_finalize(statement);

	return value;

}

void Toccata_Database::ReadSettingString(const char *settingName, char *target)
{

	int result;
	char buffer[1024];
	sqlite3_stmt *statement;

	sprintf(buffer, "SELECT * FROM %s WHERE Parameter = '%s'", "toccata_application_main", settingName);

	result = sqlite3_prepare(m_database, buffer, -1, &statement, NULL);
	result = sqlite3_step(statement);

	const unsigned char *value = sqlite3_column_text(statement, 1);
	result = sqlite3_step(statement);

	strcpy(target, (const char *)value);

	sqlite3_finalize(statement);

}

void Toccata_Database::FindPerformance(int performanceID, char *targetName)
{

	int result;
	char buffer[1024];
	sqlite3_stmt *statement;

	sprintf(buffer, "SELECT * FROM %s WHERE PerformanceID = %d", "toccata_application_directory", performanceID);

	result = sqlite3_prepare(m_database, buffer, -1, &statement, NULL);
	result = sqlite3_step(statement);

	const unsigned char *value = sqlite3_column_text(statement, 1);
	strcpy(targetName, (const char *)value);

	result = sqlite3_step(statement);

	sqlite3_finalize(statement);

}

void Toccata_Database::ReadPerformanceReport(int performanceID, Toccata_PerformanceReport *report)
{

	char tableName[1024];
	FindPerformance(performanceID, tableName);

	// TODO: check if performance was actually found

	// Execute the query

	int result;
	char buffer[1024];
	sqlite3_stmt *statement;

	sprintf(buffer, "SELECT * FROM %s WHERE PerformanceID = %d", tableName, performanceID);

	result = sqlite3_prepare(m_database, buffer, -1, &statement, NULL);
	result = sqlite3_step(statement);

	report->SetPerformanceID(sqlite3_column_int(statement, 0));
	// TODO: Read date
	report->m_playedSegment.SetTempoBPM(sqlite3_column_int(statement, 2));
	report->m_referenceSegment.SetTempoBPM(sqlite3_column_int(statement, 2));

	report->m_leftHandStats.CorrectNotes = sqlite3_column_int(statement, 3);
	report->m_leftHandStats.WrongNotes = sqlite3_column_int(statement, 4);
	report->m_leftHandStats.MissedNotes = sqlite3_column_int(statement, 5);
	report->m_leftHandStats.NonsenseNotes = sqlite3_column_int(statement, 6);

	// Column 7: key accuracy (derived)
	report->m_leftHandStats.AverageTimingError = sqlite3_column_double(statement, 8);
	report->m_leftHandStats.MaxTimingError = sqlite3_column_double(statement, 9);
	report->m_leftHandStats.MinTimingError = sqlite3_column_double(statement, 10);
	report->m_leftHandStats.ErrorStandardDeviation = sqlite3_column_double(statement, 11);
	report->m_leftHandStats.Choppiness = sqlite3_column_double(statement, 12);

	report->m_rightHandStats.CorrectNotes = sqlite3_column_int(statement, 13);
	report->m_rightHandStats.WrongNotes = sqlite3_column_int(statement, 14);
	report->m_rightHandStats.MissedNotes = sqlite3_column_int(statement, 15);
	report->m_rightHandStats.NonsenseNotes = sqlite3_column_int(statement, 16);

	// Column 17: key accuracy (derived)
	report->m_rightHandStats.AverageTimingError = sqlite3_column_double(statement, 18);
	report->m_rightHandStats.MaxTimingError = sqlite3_column_double(statement, 19);
	report->m_rightHandStats.MinTimingError = sqlite3_column_double(statement, 20);
	report->m_rightHandStats.ErrorStandardDeviation = sqlite3_column_double(statement, 21);
	report->m_rightHandStats.Choppiness = sqlite3_column_double(statement, 22);

	result = sqlite3_step(statement);

	sqlite3_finalize(statement);


}

void Toccata_Database::AddPerformance(int performanceID, const char *songName)
{

	char buffer[256];
	sprintf(buffer, "%d, '%s'", performanceID, songName);

	Insert("toccata_application_directory", buffer);

}

void Toccata_Database::CreateTable(const char *tableName, const char *columns)
{

	char buffer[1024];

	sprintf(buffer, "CREATE TABLE %s (%s);", tableName, columns);

	int result = sqlite3_exec(m_database, buffer, NULL, NULL, NULL);

	if (result != SQLITE_OK &&
		result != SQLITE_ROW &&
		result != SQLITE_DONE)
	{
	
		// TODO: error could not create table

	}

}

void Toccata_Database::Insert(const char *tableName, const char *data)
{

	char buffer[1024];

	sprintf(buffer, "INSERT INTO %s VALUES (%s);", tableName, data);

	int result = sqlite3_exec(m_database, buffer, NULL, NULL, NULL);

	if (result != SQLITE_OK &&
		result != SQLITE_ROW &&
		result != SQLITE_DONE)
	{

		// TODO: error could not insert into table

		const char *err = sqlite3_errmsg(m_database);
		int a = 0;

	}

}

void Toccata_Database::SimpleUpdate(const char *table, const char *idColumn, const char *id, const char *values)
{

	char buffer[1024];

	sprintf(buffer, "UPDATE %s SET %s WHERE %s=%s;", table, values, idColumn, id);

	int result = sqlite3_exec(m_database, buffer, NULL, NULL, NULL);

	if (result != SQLITE_OK &&
		result != SQLITE_ROW &&
		result != SQLITE_DONE)
	{

		// TODO: error could not update table
		Toccata.GetLogger()->Log(TOCCATA_DEBUG, "SQL error while updating value.");

	}

}

void Toccata_Database::InitializeDatabase()
{

	bool hasMainTable = DoesTableExist("toccata_application_main");

	if (!hasMainTable)
	{

		CreateTable("toccata_application_main", "Parameter UNIQUE, Value");

		Insert("toccata_application_main", "'Performance ID', 0");

		char buffer[256];
		sprintf(buffer, "'Version', %d", CurrentVersion);
		Insert("toccata_application_main", buffer);

		// Initialize the settings
		Settings.PerformanceID = 0;
		Settings.FileVersion = CurrentVersion;

	}

	else
	{

		// Read the settings from the database
		Settings.FileVersion = (DATABASE_VERSION)ReadSettingInt("Version");

		if (Settings.FileVersion != CurrentVersion)
		{

			PortDatabase(CurrentVersion);

		}

		else
		{

			Settings.PerformanceID = ReadSettingInt("Performance ID");

		}

	}

	bool hasDirectoryTable = DoesTableExist("toccata_application_directory");

	if (!hasDirectoryTable)
	{

		CreateTable("toccata_application_directory", "PerformanceID UNIQUE, Song");

	}

}

void Toccata_Database::PortDatabase(DATABASE_VERSION newVersion)
{

	// To make life easier, porting only works in steps of 1
	while ((int)m_version != (int)newVersion - 1)
	{

		PortDatabase((DATABASE_VERSION)((int)m_version + 1));

	}

	if (newVersion == VERSION_1_0 /* Replace with VERSION_1_1 */ && m_version == VERSION_1_0)
	{

		// Do stuff

	}

}

void Toccata_Database::WritePerformanceReport(Toccata_PerformanceReport *performanceReport, const char *detailedFile, const char *midiFile)
{

	/* Columns
	"Performance ID,"
	"Date,"
	"Tempo,"

	"LH Correct Notes,"
	"LH Wrong Notes,"
	"LH Missed Notes,"
	"LH Nonsense Notes,"

	"LH Key Accuracy,"
	"LH Timing Error,"
	"LH Max Error,"
	"LH Min Error,"
	"LH Std Dev,"
	"LH Choppiness,"

	"RH Correct Notes,"
	"RH Wrong Notes,"
	"RH Missed Notes,"
	"RH Nonsense Notes,"

	"RH Key Accuracy,"
	"RH Timing Error,"
	"RH Max Error,"
	"RH Min Error,"
	"RH Std Dev,"
	"RH Choppiness"
	*/

	char buffer[1024];

	const char *songName = performanceReport->GetName();

	sprintf(buffer, "toccata_report_%s", songName);
	CreateTableName(buffer);

	bool tableExists = DoesTableExist(buffer);

	if (tableExists)
	{

	}

	else
	{

		CreateTable(buffer, ReportColumns_v1_0);

	}

	AddPerformance(performanceReport->GetPerformanceID(), buffer);

	std::stringstream ss;

	ss << performanceReport->GetPerformanceID() << ",";
	ss << "'" << performanceReport->GetDate() << "',";
	ss << performanceReport->m_playedSegment.GetTempoBPM() << ",";

	ss << performanceReport->m_leftHandStats.CorrectNotes << ",";
	ss << performanceReport->m_leftHandStats.WrongNotes << ",";
	ss << performanceReport->m_leftHandStats.MissedNotes << ",";
	ss << performanceReport->m_leftHandStats.NonsenseNotes << ",";

	ss << performanceReport->m_leftHandStats.CorrectNotes / 
		(double)(performanceReport->m_leftHandStats.CorrectNotes + performanceReport->m_leftHandStats.MissedNotes +
			performanceReport->m_leftHandStats.WrongNotes + performanceReport->m_leftHandStats.NonsenseNotes) << ",";
	ss << performanceReport->m_leftHandStats.AverageTimingError << ",";
	ss << performanceReport->m_leftHandStats.MaxTimingError << ",";
	ss << performanceReport->m_leftHandStats.MinTimingError << ",";
	ss << performanceReport->m_leftHandStats.ErrorStandardDeviation << ",";
	ss << performanceReport->m_leftHandStats.Choppiness << ",";

	ss << performanceReport->m_rightHandStats.CorrectNotes << ",";
	ss << performanceReport->m_rightHandStats.WrongNotes << ",";
	ss << performanceReport->m_rightHandStats.MissedNotes << ",";
	ss << performanceReport->m_rightHandStats.NonsenseNotes << ",";

	ss << performanceReport->m_rightHandStats.CorrectNotes /
		(double)(performanceReport->m_rightHandStats.CorrectNotes + performanceReport->m_rightHandStats.MissedNotes + 
			performanceReport->m_rightHandStats.WrongNotes + performanceReport->m_rightHandStats.NonsenseNotes) << ",";
	ss << performanceReport->m_rightHandStats.AverageTimingError << ",";
	ss << performanceReport->m_rightHandStats.MaxTimingError << ",";
	ss << performanceReport->m_rightHandStats.MinTimingError << ",";
	ss << performanceReport->m_rightHandStats.ErrorStandardDeviation << ",";
	ss << performanceReport->m_rightHandStats.Choppiness << ",";

	ss << "'" << detailedFile << "',";
	ss << "'" << midiFile << "'";

	Insert(buffer, ss.str().c_str());

}

int Toccata_Database::NewPerformanceID()
{

	int newPerformanceID = Settings.PerformanceID++;

	char buffer[256];
	sprintf(buffer, "Value = %d", Settings.PerformanceID);

	SimpleUpdate("toccata_application_main", "Parameter", "'Performance ID'", buffer);

	return newPerformanceID;

}

void Toccata_Database::Test()
{

	sqlite3_stmt *statement;
	int result;

	int err = SQLITE_ERROR;

	result = sqlite3_prepare_v2(m_database, "CREATE TABLE test_table(a, b, c, d UNIQUE);", -1, &statement, NULL);
	result = sqlite3_step(statement);

	result = sqlite3_prepare(m_database, "INSERT INTO test_table VALUES (1.0, 'Yes', 'Wow1', 0);", -1, &statement, NULL);
	result = sqlite3_step(statement);

	result = sqlite3_prepare(m_database, "INSERT INTO test_table VALUES (1.0, 'Yes', 'Wow2', 1);", -1, &statement, NULL);
	result = sqlite3_step(statement);

	result = sqlite3_prepare(m_database, "SELECT * FROM test_table WHERE b = 'Yes';", -1, &statement, NULL);
	result = sqlite3_step(statement);

	const unsigned char *test = sqlite3_column_text(statement, 2);
	result = sqlite3_step(statement);

	test = sqlite3_column_text(statement, 2);

	int a = 0;

}

void Toccata_Database::CreateTableName(char *name)
{

	int i = 0;
	while (true)
	{

		if (name[i] == '\0') return;

		if (name[i] == ' ')
		{

			name[i] = '_';

		}

		i++;

	}
}