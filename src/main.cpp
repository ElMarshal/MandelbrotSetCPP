#include <stdio.h>
#include <stdint.h>
#include "stb_image_write.h"

struct ColorRGBA8
{
	int8_t r, g, b, a;
};

void print_progress(int progress)
{
	char progress_bar[51];

	for (size_t i = 0; i < 50; i++)
	{
		progress_bar[i] = (i <= progress/2) ? '=' : ' ';
	}
	progress_bar[50] = '\0';

	printf("[%s] %d%% \r", progress_bar, progress);
}

int main()
{
	const size_t WIDTH = 600;
	const size_t HEIGHT = 400;
	const char* output_file_name = "output/image.png";

	ColorRGBA8* color_buffer = new ColorRGBA8[WIDTH*HEIGHT];

	// clear the buffer
	for (size_t y = 0; y < HEIGHT; ++y)
	{
		for (size_t x = 0; x < WIDTH; ++x)
		{
			color_buffer[y*WIDTH + x] = ColorRGBA8{ (int8_t)0, (int8_t)0, (int8_t)0, (int8_t)255 };
		}
	}

	// render to the buffer
	for (size_t y = 0; y < HEIGHT; ++y)
	{
		for (size_t x = 0; x < WIDTH; ++x)
		{
			color_buffer[y*WIDTH + x] = ColorRGBA8{ (int8_t)(y*255/HEIGHT), (int8_t)(x*255/WIDTH), (int8_t)10, (int8_t)255 };
		}
		print_progress(y*100/(HEIGHT-1));
	}
	printf("\n");

	// save the buffer to an image file
	stbi_write_png(output_file_name, WIDTH, HEIGHT, 4, (const void*)color_buffer, 0);
	printf("Saved the buffer into %s\n", output_file_name);

	return 0;
}
