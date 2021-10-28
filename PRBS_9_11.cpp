#include "PRBS_9_11.h"

PRBS::PRBS()
{
}

uint8_t PRBS::reverse_bit_within_byte(uint8_t val)
{

	uint8_t rev_val = 0;

	for (auto i = 7; i >= 0; i--)
	{

		rev_val |= (val & 1) << i;

		val = val >> 1;
	}

	return rev_val;
}

array<uint8_t, 256> PRBS::get_reverse_order_table()
{

	array<uint8_t, 256> result;

	result.fill({0});

	for (auto i = 0; i <= 255; ++i)
	{

		result[i] = reverse_bit_within_byte((uint8_t)i);
	}

	return result;
}

vector<uint8_t> PRBS::gen_lfsr_table(vector<uint8_t>& taps, bool inversion, bool no_long, bool bit7_first)
{
	int inv = 0;
	int excepted = 0;
	int srg = 0;

	const auto REVERSE_ORDER = get_reverse_order_table();

	vector<uint8_t> table(1 << 16, 0);

	auto mask = (1 << *max_element(taps.begin(), taps.end())) - 1;

	if (!bit7_first)
	{

		mask = ((REVERSE_ORDER[mask >> 8]) << 8) | REVERSE_ORDER[mask & 0xFF];
	}

	if (inversion)
	{

		inv = 1;

		excepted = 0xFFFF & mask;
	}

	else
	{

		inv = 0;

		excepted = 0;
	}

	for (auto adr = 0; adr <= table.size() - 1; adr++)
	{

		if (bit7_first)
		{

			srg = adr;
		}

		else
		{

			srg = ((REVERSE_ORDER[adr >> 8]) << 8) | REVERSE_ORDER[adr & 0xFF];
		}

		for (auto i = 0; i <= 7; i++)
		{

			auto srg_in = inv;

			for (auto tap : taps)
			{

				srg_in ^= (srg >> (tap - 1)) & 0x1;
			}

			srg = (srg << 1) | srg_in;
		}
		table[adr] = (srg & 0xFF);

		if (no_long)
		{

			if ((adr & mask) == excepted)
			{

				table[adr] = table[adr] ^ 0x1;
			}
		}

		if (!bit7_first)
		{

			table[adr] = REVERSE_ORDER[table[adr]];
		}
	}

	return table;
}

int PRBS::check_prbs_errors(const vector<uint8_t>& table, int offset, const vector<uint8_t>& data,
							vector<uint8_t>& previous, bool quiet)
{
	int errors = 0;

	for (auto d : data)
	{

		auto adr = (previous[1] << 8) | previous[0];
		uint8_t check = table[adr];

		if (d != check)
		{
			errors += 1;

			if (!quiet)
			{

				// printf("Error %d offset\n", offset);
			}
		}
		previous[1] = previous[0];
		previous[0] = d;
		offset += 1;
	}

	return errors;
}

// vector<columns> checks = {
//
//	{"checked", "errors", "prbs_inversion", "invert_data", "no_long", "bit7_first", "input_data"},
//	{1,			0,			false,				false,		false,		true,		{0xF8, 0x00, 0x00}},
//	{1,			0,			true,				false,		false,		true,		{0x07, 0xFF, 0xFF}},
//	{1,			1,			false,				false,		true,		true,		{0xF8, 0x00, 0x00}},
//	{1,			1,			true,				false,		true,		true,		{0x07, 0xFF, 0xFF}},
//	{1,			1,			false,				false,		false,		true,		{0xFC, 0x00, 0x00}},
//	{1,			1,			true,				false,		false,		true,		{0x03, 0xFF, 0xFF}},
//
//	{1,			0,			false,				false,		false,		false,		{0x1F, 0x00, 0x00}},
//	{1,			0,			true,				false,		false,		false,		{0xE0, 0xFF, 0xFF}},
//	{1,			1,			false,				false,		true,		false,		{0x1F, 0x00, 0x00}},
//	{1,			1,			true,				false,		true,		false,		{0xE0, 0xFF, 0xFF}},
//	{1,			1,			false,				false,		false,		false,		{0x3F, 0x00, 0x00}},
//	{1,			1,			true,				false,		false,		false,		{0xC0, 0xFF, 0xFF}},
//
//	{8,			0,			false,				false,		false,		true,		NULL},
//	{8,			0,			true,				false,		false,		true,		NULL},
//	{8,			8,			false,				true,		false,		true,		NULL},
//	{8,			8,			true,				true,		false,		true,		NULL},
//	{8,			8,			false,				false,		true,		true,		NULL},
//	{8,			8,			true,				false,		true,		true,		NULL},
//	{8,			8,			false,				true,		true,		true,		NULL},
//	{8,			8,			true,				true,		true,		true,		NULL},
//
//	{8,			0,			false,				false,		false,		false,		NULL},
//	{8,			0,			true,				false,		false,		false,		NULL},
//	{8,			8,			false,				true,		false,		false,		NULL},
//	{8,			8,			true,				true,		false,		false,		NULL},
//	{8,			8,			false,				false,		true,		false,		NULL},
//	{8,			8,			true,				false,		true,		false,		NULL},
//	{8,			8,			false,				true,		true,		false,		NULL},
//	{8,			8,			true,				true,		true,		false,		NULL},
//	{2045,		0,			true,				false,		false,		true,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		0,			true,				false,		true,		true,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		0,			true,				false,		false,		false,		test_location + "PRBS_9_11_X8.bin"},
//	{2045,		0,			true,				false,		true,		false,		test_location + "PRBS_9_11_X8.bin"},
//	{2045,		0,			false,				true,		false,		true,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		0,			false,				true,		true,		true,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		0,			false,				true,		false,		false,		test_location + "PRBS_9_11_X8.bin"},
//	{2045,		0,			false,				true,		true,		false,		test_location + "PRBS_9_11_X8.bin"},
//	{2045,		2038,		true,				false,		false,		false,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		2038,		true,				false,		true,		false,		test_location + "PRBS_9_11_X8.itm"},
//	{2045,		2038,		true,				false,		false,		true,		test_location + "PRBS_9_11_X8.bin"},
//	{2045,		2038,		true,				false,		true,		true,		test_location + "PRBS_9_11_X8.bin"}
//
//};