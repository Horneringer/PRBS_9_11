#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

class PRBS
{

public:
	PRBS();

	uint8_t reverse_bit_within_byte(uint8_t val);

	array<uint8_t, 256> get_reverse_order_table();

	vector<uint8_t> gen_lfsr_table(vector<uint8_t>& taps, bool inversion, bool no_long, bool bit7_first);

	int check_prbs_errors(const vector<uint8_t>& table, int offset, const vector<uint8_t>& data,
						  vector<uint8_t>& previous, bool quiet = false);
};
