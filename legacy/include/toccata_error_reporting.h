#ifndef TOCCATA_ERROR_REPORTING_H
#define TOCCATA_ERROR_REPORTING_H

#include <stdarg.h>

enum LogLevel
{

	TOCCATA_DEBUG,
	TOCCATA_USER,
	TOCCATA_URGENT,

};

class Toccata_Logger
{

public:

	Toccata_Logger();
	~Toccata_Logger();

	void Log(LogLevel level, const char *format, ...);
	void LogUser(const char *format, ...);

	int Increment(int n=1);
	int Decrement(int n=1);
	void SetIncrement(int n);

protected:

	void _Log(LogLevel level, const char *format, va_list args);

	LogLevel m_level;
	int m_currentIncrement;

};


#endif