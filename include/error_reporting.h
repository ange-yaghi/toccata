#ifndef TOCCATA_CORE_ERROR_REPORTING_H
#define TOCCATA_CORE_ERROR_REPORTING_H

#include <stdarg.h>

namespace toccata {

	class Logger {
	public:
		enum class LogLevel {
			Debug,
			User,
			Urgent,
		};

	public:
		Logger();
		~Logger();

		void Log(LogLevel level, const char *format, ...);
		void LogUser(const char *format, ...);

		int Increment(int n = 1);
		int Decrement(int n = 1);
		void SetIncrement(int n);

	protected:
		void _Log(LogLevel level, const char *format, va_list args);

		LogLevel m_level;
		int m_currentIncrement;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_ERROR_REPORTING_H */
