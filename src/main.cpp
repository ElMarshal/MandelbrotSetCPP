#include <stdio.h>
#include <stdint.h>
#include "stb_image_write.h"
#include "real_number.h"
#include "vector2.h"
#include "complex_number.h"

struct ColorRGBA8
{
	int8_t r, g, b, a;
};

struct ColorRGBA32
{
	int32_t r, g, b, a;
};

// COLOR PALLETE SOURCE: https://stackoverflow.com/a/16505538/9218594
const ColorRGBA8 COLOR_PALLETE[16] = {
	{  66,  30,  15, 255 },
	{  25,   7,  26, 255 },
	{   9,   1,  47, 255 },
	{   4,   4,  73, 255 },
	{   0,   7, 100, 255 },
	{  12,  44, 138, 255 },
	{  24,  82, 177, 255 },
	{  57, 125, 209, 255 },
	{ 134, 181, 229, 255 },
	{ 211, 236, 248, 255 },
	{ 241, 233, 191, 255 },
	{ 248, 201,  95, 255 },
	{ 255, 170,   0, 255 },
	{ 204, 128,   0, 255 },
	{ 153,  87,   0, 255 },
	{ 106,  52,   3, 255 }
};

void print_progress(int progress)
{
	char progress_bar[51];

	for (size_t i = 0; i < 50; ++i)
	{
		progress_bar[i] = (i <= progress/2) ? '=' : ' ';
	}
	progress_bar[50] = '\0';

	printf("[%s] %d%% \r", progress_bar, progress);
}

int main()
{
	const size_t BUFFER_WIDTH = 1920;
	const size_t BUFFER_HEIGHT = 1080;
	const Real ASPECT_RATIO = (Real)BUFFER_WIDTH / (Real)BUFFER_HEIGHT;
	const char* OUTPUT_FILE_NAME = "output/image.png";
	const Vector2 CENTER_POS = { -0.7453, 0.1127 };
	const Vector2 VIEW_SIZE = { 6.5e-4*ASPECT_RATIO, 6.5e-4};
	const size_t MAX_ITERATIONS = 250;
	const Real MAX_LENGTH = 2.0;

	ColorRGBA8* color_buffer = new ColorRGBA8[BUFFER_WIDTH*BUFFER_HEIGHT];

	// clear the buffer
	for (size_t y = 0; y < BUFFER_HEIGHT; ++y)
	{
		for (size_t x = 0; x < BUFFER_WIDTH; ++x)
		{
			color_buffer[y*BUFFER_WIDTH + x] = ColorRGBA8{ (int8_t)0, (int8_t)0, (int8_t)0, (int8_t)255 };
		}
	}

	// render to the buffer
	for (size_t y = 0; y < BUFFER_HEIGHT; ++y)
	{
		for (size_t x = 0; x < BUFFER_WIDTH; ++x)
		{
			const Vector2 normalized_pos = {((Real)x / (Real)BUFFER_WIDTH * 2.0 - 1), -((Real)y / (Real)BUFFER_HEIGHT * 2.0 - 1.0) }; // x: [-1, 1], y: [-1, 1]
			const Vector2 pos = { CENTER_POS.x + normalized_pos.x*VIEW_SIZE.x/2.0, CENTER_POS.y + normalized_pos.y*VIEW_SIZE.y/2.0 };

			Complex z = { 0.0, 0.0 };
			size_t iterations = 0;
			while (z.length() <= MAX_LENGTH && iterations < MAX_ITERATIONS)
			{
				z = z.square() + Complex{ pos.x, pos.y };
				iterations += 1;
			}

			color_buffer[y*BUFFER_WIDTH + x] = COLOR_PALLETE[iterations%16];
		}
		print_progress(y*100/(BUFFER_HEIGHT-1));
	}
	printf("\n");

	// save the buffer to an image file
	stbi_write_png(OUTPUT_FILE_NAME, BUFFER_WIDTH, BUFFER_HEIGHT, 4, (const void*)color_buffer, 0);
	printf("Saved the buffer into %s\n", OUTPUT_FILE_NAME);

	return 0;
}
