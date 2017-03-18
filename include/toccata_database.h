#ifndef TOCCATA_DATABASE_H
#define TOCCATA_DATABASE_H

#include <sqlite3.h>

#include "toccata_performance_report.h"

class Toccata_Database
{

public:

	enum DATABASE_VERSION
	{

		VERSION_1_0,

	};

	static const DATABASE_VERSION CurrentVersion = VERSION_1_0;

	enum TABLE_TYPE
	{

		TABLE_APPLICATION,
		TABLE_SONG_INFORMATION,
		TABLE_PERFORMANCE,

	};

	struct Toccata_Settings
	{

		int PerformanceID;
		DATABASE_VERSION FileVersion;

	};

	static const char *ReportColumns_v1_0;

public:

	Toccata_Database();
	~Toccata_Database();

	void OpenDatabase();
	void InitializeDatabase();
	void PortDatabase(DATABASE_VERSION newVersion);
	void Test();

	int ReadSettingInt(const char *settingName);
	void ReadSettingString(const char *settingName, char *target);

	void AddPerformance(int performanceID, const char *songName);
	void FindPerformance(int performanceID, char *nameTarget);

	void WritePerformanceReport(Toccata_PerformanceReport *performanceReport, const char *detailedFile, const char *midiFile);
	void ReadPerformanceReport(int performanceID, Toccata_PerformanceReport *targetReport);

	int NewPerformanceID();

	/* General purpose SQLite Wrappers */
	
	// Check whether a table exists
	bool DoesTableExist(const char *name);

	// Rename a table
	void RenameTable(const char *name, const char *newName);
		
	// Create a new table
	void CreateTable(const char *name, const char *columns);

	// Insert a new row in a table
	void Insert(const char *table, const char *data);

	// Update an entry in the table using a simple lookup scheme
	void SimpleUpdate(const char *table, const char *idColumn, const char *id, const char *values);

	// Create a table name
	void CreateTableName(char *string);

	Toccata_Settings Settings;

protected:

	DATABASE_VERSION m_version;
	sqlite3 *m_database;

};

#endif