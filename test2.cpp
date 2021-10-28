#include "PRBS_9_11.h"
#include <catch2.hpp>

TEST_CASE("Gen lfsr table")
{

	PRBS instance2;

	vector<uint8_t> table;

	ifstream in;

	in.open("9-11-bin-noInv.dat", ios::binary);

	uint8_t data;

	in.unsetf(ios::skipws);
	while (in >> data)
		table.push_back(data);

	vector<uint8_t> taps = {9, 11};

	bool inversion = false;
	bool no_long = true;
	bool bit7_first = false;

	REQUIRE(table.size() == 65536);
	REQUIRE(instance2.gen_lfsr_table(taps, inversion, no_long, bit7_first) == table);
}

TEST_CASE("Check_prbs_errors 1st line")
{

	PRBS instance3;

	vector<uint8_t> taps = {9, 11};

	bool inversion = false;
	bool no_long = false;
	bool bit7_first = true;
	bool quiet = false;

	vector<uint8_t> input_data = {0xF8, 0x00, 0x00};

	const vector<uint8_t> data_slice = {input_data.begin() + 2, input_data.end()};

	vector<uint8_t> previous;

	previous.push_back(input_data[1]);
	previous.push_back(input_data[0]);

	const vector<uint8_t> table1 = instance3.gen_lfsr_table(taps, inversion, no_long, bit7_first);

	REQUIRE(instance3.check_prbs_errors(table1, 2, data_slice, previous, quiet) == 0);
}

TEST_CASE("Check 1st and 2nd group table")
{

	PRBS instance4;

	vector<uint8_t> taps = {9, 11};

	bool quiet = false;

	struct columns
	{

		int checked;
		int errors;
		bool prbs_inversion;
		bool invert_data;
		bool no_long;
		bool bit7_first;
		vector<uint8_t> input_data;
	};

	vector<columns> checks = {

		{1, 0, false, false, false, true, {0xF8, 0x00, 0x00}},
		{1, 0, true, false, false, true, {0x07, 0xFF, 0xFF}},
		{1, 1, false, false, true, true, {0xF8, 0x00, 0x00}},
		{1, 1, true, false, true, true, {0x07, 0xFF, 0xFF}},
		{1, 1, false, false, false, true, {0xFC, 0x00, 0x00}},
		{1, 1, true, false, false, true, {0x03, 0xFF, 0xFF}},

		{1, 0, false, false, false, false, {0x1F, 0x00, 0x00}},
		{1, 0, true, false, false, false, {0xE0, 0xFF, 0xFF}},
		{1, 1, false, false, true, false, {0x1F, 0x00, 0x00}},
		{1, 1, true, false, true, false, {0xE0, 0xFF, 0xFF}},
		{1, 1, false, false, false, false, {0x3F, 0x00, 0x00}},
		{1, 1, true, false, false, false, {0xC0, 0xFF, 0xFF}},

		{8, 0, false, false, false, true, {}},
		{8, 0, true, false, false, true, {}},
		{8, 8, false, true, false, true, {}},
		{8, 8, true, true, false, true, {}},
		{8, 8, false, false, true, true, {}},
		{8, 8, true, false, true, true, {}},
		{8, 8, false, true, true, true, {}},
		{8, 8, true, true, true, true, {}},

		{8, 0, false, false, false, false, {}},
		{8, 0, true, false, false, false, {}},
		{8, 8, false, true, false, false, {}},
		{8, 8, true, true, false, false, {}},
		{8, 8, false, false, true, false, {}},
		{8, 8, true, false, true, false, {}},
		{8, 8, false, true, true, false, {}},
		{8, 8, true, true, true, false, {}},

	};

	for (auto item : checks)
	{

		vector<uint8_t> input_data_slice;
		vector<uint8_t> previous;

		if (item.input_data.empty())
		{
			if (item.prbs_inversion)
			{

				item.input_data.insert(item.input_data.begin(), 10, 0xFF);
			}

			else
			{

				item.input_data.insert(item.input_data.begin(), 10, 0x00);
			}
		}

		if (item.invert_data)
		{

			for (auto i = 0; i <= item.input_data.size() - 1; i++)
			{

				item.input_data[i] ^= 0xFF;
			}
		}

		auto table = instance4.gen_lfsr_table(taps, item.prbs_inversion, item.no_long, item.bit7_first);
		input_data_slice = {item.input_data.begin() + 2, item.input_data.end()};
		previous.push_back(item.input_data[1]);
		previous.push_back(item.input_data[0]);
		int result = instance4.check_prbs_errors(table, 2, input_data_slice, previous, quiet);

		CHECK(result == item.errors);
	}
}

TEST_CASE("Check 3rd group table")
{

	PRBS instance6;

	vector<uint8_t> taps = {9, 11};

	vector<uint8_t> input_data_slice;

	bool quiet = false;

	struct columns
	{

		int checked;
		int errors;
		bool prbs_inversion;
		bool invert_data;
		bool no_long;
		bool bit7_first;
		string input_data;
	};

	vector<columns> checks = {

		//{2045,		0,			true,				false,		false,		true,		"PRBS_9_11_X8.itm"},
		//{2045,		0,			true,				false,		true,		true,		"PRBS_9_11_X8.itm"},
		{2045, 0, true, false, false, false, "PRBS_9_11_X8.bin"},
		{2045, 0, true, false, true, false, "PRBS_9_11_X8.bin"},
		//{2045,		0,			false,				true,		false,		true,		"PRBS_9_11_X8.itm"},
		//{2045,		0,			false,				true,		true,		true,		"PRBS_9_11_X8.itm"},
		{2045, 0, false, true, false, false, "PRBS_9_11_X8.bin"},
		{2045, 0, false, true, true, false, "PRBS_9_11_X8.bin"},
		//{2045,		2038,		true,				false,		false,		false,		"PRBS_9_11_X8.itm"},
		//{2045,		2038,		true,				false,		true,		false,		"PRBS_9_11_X8.itm"},
		{2045, 2038, true, false, false, true, "PRBS_9_11_X8.bin"},
		{2045, 2038, true, false, true, true, "PRBS_9_11_X8.bin"},

	};

	for (auto item : checks)
	{

		vector<uint8_t> file_vec;
		ifstream in;
		vector<uint8_t> previous;

		in.open(item.input_data, ios::binary);

		uint8_t data;

		in.unsetf(ios::skipws);

		while (in >> data)
		{

			file_vec.push_back(data);
		}

		INFO("File " << item.input_data << " is not exists!");
		REQUIRE(!file_vec.empty() == true);

		if (item.invert_data)
		{

			for (auto i = 0; i <= file_vec.size() - 1; i++)
			{

				file_vec[i] ^= 0xFF;
			}
		}

		auto table = instance6.gen_lfsr_table(taps, item.prbs_inversion, item.no_long, item.bit7_first);
		input_data_slice = {file_vec.begin() + 2, file_vec.end()};
		previous.push_back(file_vec[1]);
		previous.push_back(file_vec[0]);

		int result = instance6.check_prbs_errors(table, 2, input_data_slice, previous, quiet);

		REQUIRE(result == item.errors);
	}
}

TEST_CASE("Check 8 tables")
{

	PRBS instance7;

	vector<uint8_t> taps = {9, 11};

	vector<uint8_t> input_data_slice;

	struct columns
	{

		bool prbs_inversion;
		bool no_long;
		bool bit7_first;
		string input_data;
	};

	vector<columns> checks = {

		{false, false, false, "9-11_False_False_False.dat"}, {false, false, true, "9-11_False_False_True.dat"},
		{false, true, false, "9-11_False_True_False.dat"},	 {false, true, true, "9-11_False_True_True.dat"},
		{true, false, false, "9-11_True_False_False.dat"},	 {true, false, true, "9-11_True_False_True.dat"},
		{true, true, false, "9-11_True_True_False.dat"},	 {true, true, true, "9-11_True_True_True.dat"},
	};

	for (auto item : checks)
	{

		vector<uint8_t> file_vec;
		ifstream in;
		vector<uint8_t> previous;
		in.open(item.input_data, ios::binary);

		uint8_t data;

		in.unsetf(ios::skipws);

		while (in >> data)
		{

			file_vec.push_back(data);
		}

		INFO("File " << item.input_data << " does not exist!");
		REQUIRE(file_vec.size() == 65536);

		auto table = instance7.gen_lfsr_table(taps, item.prbs_inversion, item.no_long, item.bit7_first);

		REQUIRE(table == file_vec);
	}
}
