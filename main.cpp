#include <iostream>
#include <string>
#include "gpssim.h"

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <benchmark/benchmark.h>

#include <boost/program_options.hpp>

#define BITCOUNT

namespace po = boost::program_options;

int parse_cmd_args(int argc, char** argv)
{
	// Declare the supported options.

	const char* GPSNAV_OPT_LONG = "gpsnav";
	const char* GPSNAV_OPT = "gpsnav,e";
	const char* LOC_OPT_LONG = "location";
	const char* LOC_OPT = "location,l";

	//"  -u <user_motion> User motion file (dynamic mode)\n"
	//	"  -g <nmea_gga>    NMEA GGA stream (dynamic mode)\n"
	//	"  -c <location>    ECEF X,Y,Z in meters (static mode) e.g. 3967283.154,1022538.181,4872414.484\n"
	//	"  -l <location>    Lat,Lon,Hgt (static mode) e.g. 35.681298,139.766247,10.0\n"
	//	"  -t <date,time>   Scenario start time YYYY/MM/DD,hh:mm:ss\n"
	//	"  -T <date,time>   Overwrite TOC and TOE to scenario start time\n"
	//	"  -d <duration>    Duration [sec] (dynamic mode max: %.0f, static mode max: %d)\n"
	//	"  -o <output>      I/Q sampling data file (default: gpssim.bin)\n"
	//	"  -s <frequency>   Sampling frequency [Hz] (default: 2600000)\n"
	//	"  -b <iq_bits>     I/Q data format [1/8/16] (default: 16)\n"
	//	"  -i               Disable ionospheric delay for spacecraft scenario\n"
	//	"  -v               Show details about simulated channels\n",

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		(GPSNAV_OPT, po::value<std::string>()->required(), "Set RINEX navigation file for GPS ephemerides.")
		(LOC_OPT, po::value<std::string>()->required(), "Location to spoof (latitude,longitude,elevation).")
		;  

	try
	{
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}

		if (vm.count(GPSNAV_OPT_LONG)) {
			std::cout
				<< "GPS Nav set to "
				<< vm[GPSNAV_OPT_LONG].as<std::string>()
				<< ".\n";
		}
		else {
			std::cout << "Rinex nav file missing. --" << GPSNAV_OPT << "\n";
		}

		if (vm.count(LOC_OPT_LONG)) {
			std::cout
				<< "Location set to "
				<< vm[LOC_OPT_LONG].as<std::string>()
				<< ".\n";
		}
		else {
			std::cout << "Location missing. --" << LOC_OPT << "\n";
		}
	}
	catch (const po::error& ex)
	{
		std::cerr << ex.what() << '\n';
	}
}

int run_tests(int argc, char** argv)
{
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();

	return Catch::Session().run(argc, argv);
}


int main(int argc, char** argv)
{
	int err = run_tests(argc, argv);
	//parse_cmd_args(argc, argv);
	
	//char t;
	//std::cin.read(&t, 1);

	return err;
}
