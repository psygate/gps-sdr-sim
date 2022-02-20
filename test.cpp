#include <catch.hpp>
#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <bitset>
#include <random>
#include <benchmark/benchmark.h>
#include <immintrin.h>
#include <memory>

extern "C"
{
#include "gpssim.h"
}

#include "gpssim.hpp"

namespace bof = boost::filesystem;;

static char* copy_char_ptr(const char* original)
{
	const std::size_t strsize(strlen(original));
	const std::size_t allocsize(strsize + 1);
	char* strcopy = new char[allocsize];
	memset(strcopy, 0, allocsize);
	strncpy(strcopy, original, strsize);

	return strcopy;
}

static char** copy_argv(const int argc, const char** argv)
{
	char** nvargs = new char* [argc + 1];
	nvargs[argc] = NULL;

	for (int i = 0; i < argc; i++)
	{

		nvargs[i] = copy_char_ptr(argv[i]);
	}

	return nvargs;
}

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

TEST_CASE("Ensure argument parsing.", "[gps-sim-legacy]")
{
	const char* BRDC_TEST_FILE = "../brdc3540.14n";
	const char* TIME = "2014/12/20,06:00:00";

	SECTION("Empty args")
	{
		const char* cargv[] = { "gpssim", NULL };

		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(main_(argc, argv) == 1);
	}

	SECTION("Correct arguments.")
	{
		const char* cargv[] = { "gpssim", "-e", BRDC_TEST_FILE, "-l", "0.0,0.0,0.0", "-d", "1", NULL };
		
		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(main_(argc, argv) == 0);
	}

	SECTION("Test output file.")
	{
		const char* GEN_TEST_1_REF_IQ_FILE = "../GEN_TEST_1_ref.iq";
		const char* GEN_TEST_1_IQ_FILE = "../GEN_TEST_1.iq";

		const char* cargv[] = {
			"gpssim",
			"-e", BRDC_TEST_FILE,
			"-l", "0.0,0.0,0.0",
			"-d", "10",
			"-o", GEN_TEST_1_IQ_FILE,
			"-t", TIME,
			"-T", TIME,
			NULL
		};

		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(main_(argc, argv) == 0);
		REQUIRE(bof::is_regular_file(GEN_TEST_1_REF_IQ_FILE));
		REQUIRE(bof::is_regular_file(GEN_TEST_1_IQ_FILE));

		REQUIRE(compare_file_sizes(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
		REQUIRE(compare_files(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
	}
}

TEST_CASE("Ensure argument parsing new.", "[gps-sim]")
{
	const char* BRDC_TEST_FILE = "../brdc3540.14n";
	const char* TIME = "2014/12/20,06:00:00";

	SECTION("Empty args")
	{
		const char* cargv[] = { "gpssim", NULL };

		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(gpssim::main_(argc, argv) == 1);
	}

	SECTION("Correct arguments.")
	{
		const char* cargv[] = { "gpssim", "-e", BRDC_TEST_FILE, "-l", "0.0,0.0,0.0", "-d", "1", NULL };

		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(gpssim::main_(argc, argv) == 0);
	}

	SECTION("Test output file.")
	{
		const char* GEN_TEST_1_REF_IQ_FILE = "../GEN_TEST_1_ref.iq";
		const char* GEN_TEST_1_IQ_FILE = "../GEN_TEST_1.iq";

		const char* cargv[] = {
			"gpssim",
			"-e", BRDC_TEST_FILE,
			"-l", "0.0,0.0,0.0",
			"-d", "10",
			"-o", GEN_TEST_1_IQ_FILE,
			"-t", TIME,
			"-T", TIME,
			NULL
		};

		std::size_t argc = sizeof(cargv) / sizeof(*cargv) - 1;
		char** argv = copy_argv(argc, cargv);

		REQUIRE(gpssim::main_(argc, argv) == 0);
		REQUIRE(bof::is_regular_file(GEN_TEST_1_REF_IQ_FILE));
		REQUIRE(bof::is_regular_file(GEN_TEST_1_IQ_FILE));

		REQUIRE(compare_file_sizes(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
		REQUIRE(compare_files(GEN_TEST_1_REF_IQ_FILE, GEN_TEST_1_IQ_FILE));
	}
}