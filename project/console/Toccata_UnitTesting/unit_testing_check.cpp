#define BOOST_TEST_MODULE Toccata_UnitTestingCheck
#include <boost/test/included/unit_test.hpp>

#include <toccata_session.h>
#include <fstream>

struct Toccata_UnitTestConfiguration
{
	Toccata_UnitTestConfiguration()
	{

		time_t currentTime;

		time(&currentTime);
		struct tm *timeInfo = localtime(&currentTime);

		int year = Toccata_Session::GetYear(timeInfo);
		int month = timeInfo->tm_mon + 1;
		int day = Toccata_Session::GetDay(timeInfo);
		int hour = (timeInfo->tm_hour);
		int minute = timeInfo->tm_min;
		int second = timeInfo->tm_sec;

		std::stringstream ss;

		ss << std::setfill('0') << std::setw(2) << year << "-" << month << "-" << day << " " << hour << "_" << minute << "_" << second;

		std::string fname = "toccata_unit_test_log_" + ss.str() + ".log";

		m_testLog.open(fname);

		boost::unit_test::unit_test_log.set_stream(m_testLog);

	}

	~Toccata_UnitTestConfiguration()
	{

		boost::unit_test::unit_test_log.set_stream(std::cout);

	}

	std::ofstream m_testLog;

};

BOOST_AUTO_TEST_SUITE(SanityCheckSuite);

BOOST_GLOBAL_FIXTURE(Toccata_UnitTestConfiguration);

BOOST_AUTO_TEST_CASE(test_case1)
{

	BOOST_TEST(false);

}

BOOST_AUTO_TEST_SUITE_END()