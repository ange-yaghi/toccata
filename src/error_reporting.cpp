#include "../include/error_reporting.h"

#include <stdio.h>

toccata::Logger::Logger() {
    m_level = LogLevel::Debug;

    m_currentIncrement = 0;
}

toccata::Logger::~Logger() {
}

void toccata::Logger::_Log(LogLevel level, const char *format, va_list args) {
    if (level < m_level) return;

    for (int i = 0; i < m_currentIncrement; i++) {
        printf(" ");
    }

    vprintf(format, args);
}

void toccata::Logger::Log(LogLevel level, const char *format, ...) {
    va_list arg;

    va_start(arg, format);
    _Log(level, format, arg);
    va_end(arg);
}

void toccata::Logger::LogUser(const char *format, ...) {
    va_list arg;

    va_start(arg, format);
    _Log(LogLevel::User, format, arg);
    va_end(arg);
}

int toccata::Logger::Increment(int n) {
    int prev = m_currentIncrement;

    m_currentIncrement += n;

    return prev;
}

int toccata::Logger::Decrement(int n) {
    int prev = m_currentIncrement;

    m_currentIncrement -= n;

    return prev;
}

void toccata::Logger::SetIncrement(int n) {
    m_currentIncrement = n;
}
