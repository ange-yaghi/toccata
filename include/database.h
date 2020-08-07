#ifndef TOCCATA_CORE_DATABASE_H
#define TOCCATA_CORE_DATABASE_H

#include "performance_report.h"

struct sqlite3;

namespace toccata {

	class Database {
	public:
		enum class DatabaseVersion {
			v_1_0,
		};

		static const DatabaseVersion CurrentVersion = DatabaseVersion::v_1_0;

		enum class TableType {
			Application,
			SongInformation,
			Performance,
		};

		struct Settings {
			int PerformanceID;
			DatabaseVersion FileVersion;
		};

		static const char *ReportColumns_v1_0;

	public:
		Database();
		~Database();

		void OpenDatabase();
		void InitializeDatabase();
		void PortDatabase(DatabaseVersion newVersion);
		void Test();

		int ReadSettingInt(const char *settingName);
		void ReadSettingString(const char *settingName, char *target);

		void AddPerformance(int performanceID, const char *songName);
		void FindPerformance(int performanceID, char *nameTarget);

		void WritePerformanceReport(PerformanceReport *performanceReport, const char *detailedFile, const char *midiFile);
		void ReadPerformanceReport(int performanceID, PerformanceReport *targetReport);

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

		Settings Settings;

	protected:
		DatabaseVersion m_version;
		sqlite3 *m_database;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_DATABASE_H */
