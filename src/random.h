#pragma once
#include <stdint.h>
#include "real_number.h"

// XorShift64
class Rnd
{
private:
	uint64_t m_state;

public:
	static uint64_t MAX_RND_UINT64;

public:
	Rnd();
	Rnd(int64_t seed);

	uint32_t next_uint32();
	int32_t next_int32();
	uint64_t next_uint64();
	int64_t next_int64();
	Real next_norm();
};
