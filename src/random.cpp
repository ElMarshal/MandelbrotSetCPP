#include "random.h"
#include <time.h>

uint64_t Rnd::MAX_RND_UINT64 = UINT64_MAX;

Rnd::Rnd()
{
	m_state = time(NULL);
}

Rnd::Rnd(int64_t seed) :
	m_state(seed)
{
}

uint32_t Rnd::next_uint32()
{
	uint64_t x = next_uint64();
	return *(uint32_t*)(&x);
}

int32_t Rnd::next_int32()
{
	uint64_t x = next_uint64();
	return *(int32_t*)(&x);
}

uint64_t Rnd::next_uint64()
{
	uint64_t x = m_state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	m_state = x;
	return x;
}

int64_t Rnd::next_int64()
{
	uint64_t x = next_uint64();
	return *(int64_t*)(&x);
}

Real Rnd::next_norm()
{
	return (Real)next_uint64() / (Real)MAX_RND_UINT64;
}
