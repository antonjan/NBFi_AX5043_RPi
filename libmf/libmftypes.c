#include "libmftypes.h"
#include <stdlib.h>

uint8_t checksignedlimit16(int16_t x, int16_t lim)
{
	if (x < 0) {
		x += lim;
		return x >= 0;
	}
	x -= lim;
	return x <= 0;
}

uint8_t checksignedlimit32(int32_t x, int32_t lim)
{
	if (x < 0) {
		x += lim;
		return x >= 0;
	}
	x -= lim;
	return x <= 0;
}

void fmemcpy(void *d, const void *s, uint16_t n)
{
	char *pd = (char *)d;
	const char *ps = (const char *)s;
	for (; n; --n)
		*pd++ = *ps++;
}

void fmemset(void *p, char c, uint16_t n)
{
	char *pp = (char *)p;
	for (; n; --n)
		*pp++ = c;
}

uint8_t gray_decode8(uint8_t x)
{
	x ^= (x >> 1) & 0x40;
	x ^= (x >> 1) & 0x20;
	x ^= (x >> 1) & 0x10;
	x ^= (x >> 1) & 0x08;
	x ^= (x >> 1) & 0x04;
	x ^= (x >> 1) & 0x02;
	x ^= (x >> 1) & 0x01;
	return x;
}

uint8_t gray_encode8(uint8_t x)
{
	x ^= x >> 1;
	return x;
}

uint8_t hweight8(uint8_t x)
{
	x = (x & 0x55) + ((x >> 1) & 0x55);
	x = (x & 0x33) + ((x >> 2) & 0x33);
	x = (x & 0x0F) + ((x >> 4) & 0x0F);
	return x;
}

uint8_t hweight16(uint16_t x)
{
	return hweight8(x) + hweight8(x >> 8);
}

uint8_t hweight32(uint32_t x)
{
	return hweight8(x) + hweight8(x >> 8) + hweight8(x >> 16) + hweight8(x >> 24);
}

uint8_t rev8(uint8_t x)
{
	x = ((x >> 4) & 0x0F) | ((x << 4) & 0xF0);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xCC);
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xAA);
	return x;
}

int32_t signextend24(int32_t x)
{
	x &= 0xFFFFFF;
	x |= -(x & 0x800000);
	return x;
}

int32_t signextend20(int32_t x)
{
	x &= 0xFFFFF;
	x |= -(x & 0x80000);
	return x;
}

int32_t signextend16(int16_t x)
{
	return x;
}

int32_t signextend12(int16_t x)
{
	x &= 0xFFF;
	x |= -(x & 0x800);
	return x;
}

int32_t signedlimit32(int32_t x, int32_t lim)
{
	if (x < 0) {
		int32_t xx = x + lim;
		if (xx >= 0)
			return x;
		return -lim;
	}
	{
		int32_t xx = x - lim;
		if (xx <= 0)
			return x;
		return lim;
	}
}

int16_t signedlimit16(int16_t x, int16_t lim)
{
	if (x < 0) {
		int16_t xx = x + lim;
		if (xx >= 0)
			return x;
		return -lim;
	}
	{
		int16_t xx = x - lim;
		if (xx <= 0)
			return x;
		return lim;
	}
}
