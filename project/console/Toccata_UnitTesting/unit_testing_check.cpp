#define BOOST_TEST_MODULE Toccata_UnitTestingCheck
#define BOOST_TEST_ALTERNATIVE_INIT_API
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

		std::string fname = "Logs/toccata_unit_test_log_" + ss.str() + ".log";

		m_testLog.open(fname);

		boost::unit_test::unit_test_log.set_stream(m_testLog);

		boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_warnings);

	}

	~Toccata_UnitTestConfiguration()
	{

		boost::unit_test::unit_test_log.set_stream(std::cout);

	}

	std::ofstream m_testLog;

};

BOOST_GLOBAL_FIXTURE(Toccata_UnitTestConfiguration);

BOOST_AUTO_TEST_SUITE(SanityCheckSuite);

BOOST_AUTO_TEST_CASE(SanityTest)
{

	BOOST_TEST(true);

}

BOOST_AUTO_TEST_SUITE_END()

// Boost entry point
//int main(int argc, char* argv[], char* envp[])
//{
//	return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
//}