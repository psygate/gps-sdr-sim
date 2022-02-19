#include <catch.hpp>
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>

extern "C"
{
#include "gpssim.h"
}

namespace bof = boost::filesystem;;

static bool compare_file_sizes(const std::string& p1, const std::string& p2)
{
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		throw std::runtime_error("Files are not accessible.");
	}

	return f1.tellg() == f2.tellg();
}

static bool compare_files(const std::string& p1, const std::string& p2)
{
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		throw std::runtime_error("Files are not accessible.");
	}

	if (!compare_file_sizes(p1, p2))
	{
		return false;
	}

	//seek back to beginning and use std::equal to compare contents
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
		std::istreambuf_iterator<char>(),
		std::istreambuf_iterator<char>(f2.rdbuf()));
}


TEST_CASE("Ensure argument parsing.", "[gps-sim-legacy]") {
	const char* BRDC_TEST_FILE = "../brdc3540.14n";
	const char* TIME = "2014/12/20,06:00:00";

	SECTION("Empty args")
	{
		const char* argv[] = { "gpssim", NULL };
		std::size_t argc = sizeof(argv) / sizeof(*argv) - 1;

		REQUIRE(main_(argc, argv) == 1);
	}

	SECTION("Correct arguments.")
	{
		const char* argv[] = { "gpssim", "-e", BRDC_TEST_FILE, "-l", "0.0,0.0,0.0", "-d", "1", NULL };
		std::size_t argc = sizeof(argv) / sizeof(*argv) - 1;

		REQUIRE(main_(argc, argv) == 0);
	}
	/*
	ptions:
  -e <gps_nav>     RINEX navigation file for GPS ephemerides (required)
  -u <user_motion> User motion file (dynamic mode)
  -g <nmea_gga>    NMEA GGA stream (dynamic mode)
  -c <location>    ECEF X,Y,Z in meters (static mode) e.g. 3967283.154,1022538.181,4872414.484
  -l <location>    Lat,Lon,Hgt (static mode) e.g. 35.681298,139.766247,10.0
  -t <date,time>   Scenario start time YYYY/MM/DD,hh:mm:ss
  -T <date,time>   Overwrite TOC and TOE to scenario start time
  -d <duration>    Duration [sec] (dynamic mode max: 300, static mode max: 86400)
  -o <output>      I/Q sampling data file (default: gpssim.bin)
  -s <frequency>   Sampling frequency [Hz] (default: 2600000)
  -b <iq_bits>     I/Q data format [1/8/16] (default: 16)
  -i               Disable ionospheric delay for spacecraft scenario
  -v               Show details about simulated channels
	*/

	SECTION("Test output file.")
	{
		const char* GEN_TEST_1_REF_IQ_FILE = "../GEN_TEST_1_ref.iq";
		const char* GEN_TEST_1_IQ_FILE = "../GEN_TEST_1.iq";

		const char* argv[] = { 
			"gpssim", 
			"-e", BRDC_TEST_FILE,
			"-l", "0.0,0.0,0.0", 
			"-d", "10",
			"-o", GEN_TEST_1_IQ_FILE,
			"-t", TIME,
			"-T", TIME,
			NULL
		};

		std::size_t argc = sizeof(argv) / sizeof(*argv) - 1;

		REQUIRE(main_(argc, argv) == 0);
		REQUIRE(bof::is_regular_file(GEN_TEST_1_REF_IQ_FILE));
		REQUIRE(bof::is_regular_file(GEN_TEST_1_IQ_FILE));

		REQUIRE(compare_file_sizes(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
		REQUIRE(compare_files(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
	}
}
