#include "toccata_error_reporting.h"

#include <stdio.h>

Toccata_Logger::Toccata_Logger()
{

	m_level = TOCCATA_DEBUG;

	m_currentIncrement = 0;

}

Toccata_Logger::~Toccata_Logger()
{
}

void Toccata_Logger::_Log(LogLevel level, const char *format, va_list args)
{

	if (level < m_level)
	{

		return;

	}

	for (int i = 0; i < m_currentIncrement; i++)
	{

		printf(" ");

	}

	vprintf(format, args);

}

void Toccata_Logger::Log(LogLevel level, const char *format, ...)
{

	va_list arg;

	va_start(arg, format);
	_Log(level, format, arg);
	va_end(arg);

}

void Toccata_Logger::LogUser(const char *format, ...)
{

	va_list arg;

	va_start(arg, format);
	_Log(TOCCATA_USER, format, arg);
	va_end(arg);

}

int Toccata_Logger::Increment(int n)
{

	int prev = m_currentIncrement;

	m_currentIncrement += n;

	return prev;

}

int Toccata_Logger::Decrement(int n)
{

	int prev = m_currentIncrement;

	m_currentIncrement -= n;

	return prev;

}

void Toccata_Logger::SetIncrement(int n)
{

	m_currentIncrement = n;

}
