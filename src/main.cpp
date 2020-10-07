#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "stb_image_write.h"
#include "real_number.h"
#include "vector2.h"
#include "complex_number.h"
#include "random.h"

struct ColorRGBA8
{
	uint8_t r, g, b, a;
};

struct ColorRGBA32
{
	uint32_t r, g, b, a;
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

size_t divide_roundup(size_t numinator, size_t denominator)
{
	if (numinator%denominator == 0)
	{
		return numinator/denominator;
	}
	return numinator/denominator + 1;
}

size_t clamp(size_t value, size_t min, size_t max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}

size_t min(size_t a, size_t b)
{
	if (a < b)
	{
		return a;
	}
	return b;
}

void print_progress(int progress)
{
	char progress_bar[51];

	for (size_t i = 0; i < 50; ++i)
	{
		progress_bar[i] = (i <= progress/2) ? '=' : ' ';
	}
	progress_bar[50] = '\0';

	printf("\r[%s] %d%%   ", progress_bar, progress);
}

struct RenderParameters
{
	size_t buffer_width;
	size_t buffer_height;
	size_t sample_count;
	Vector2 center_pos;
	Vector2 view_size;
	size_t max_iterations;
	Real max_length;
};

struct ThreadParameters
{
	size_t x;
	size_t y;
	size_t width;
	size_t height;
	ColorRGBA8* color_buffer;

	size_t thread_id;
	std::mutex& finished_queue_mutex;
	std::queue<size_t>& finished_queue;
};

void thread_worker(const RenderParameters* parms, ThreadParameters* tparms)
{
	Rnd rnd;

	for (size_t y = 0; y < tparms->height; ++y)
	{
		for (size_t x = 0; x < tparms->width; ++x)
		{
			// calculate the position on the actual buffer
			const size_t xx = tparms->x + x;
			const size_t yy = tparms->y + y;

			ColorRGBA32 accumelator = ColorRGBA32{ 0, 0, 0, 0 };
			for (size_t i = 0; i < parms->sample_count; ++i)
			{
				const Vector2 normalized_pos = {
					(((Real)xx+rnd.next_norm()-0.5f) / (Real)parms->buffer_width * 2.0f - 1.0f),  // x: [-1, 1]
					-(((Real)yy+rnd.next_norm()-0.5f) / (Real)parms->buffer_height * 2.0f - 1.0f) }; // y: [-1, 1]
				const Vector2 pos = { parms->center_pos.x + normalized_pos.x*parms->view_size.x/2.0f, parms->center_pos.y + normalized_pos.y*parms->view_size.y/2.0f };

				Complex z = { 0.0, 0.0 };
				size_t iterations = 0;
				while (z.length() <= parms->max_length && iterations < parms->max_iterations)
				{
					z = z.square() + Complex{ pos.x, pos.y };
					iterations += 1;
				}

				ColorRGBA8 sample_color = COLOR_PALLETE[iterations%16];
				accumelator.r += sample_color.r;
				accumelator.g += sample_color.g;
				accumelator.b += sample_color.b;
				accumelator.a += sample_color.a;
			}

			accumelator.r /= parms->sample_count;
			accumelator.g /= parms->sample_count;
			accumelator.b /= parms->sample_count;
			accumelator.a /= parms->sample_count;
			tparms->color_buffer[(y+tparms->y)*parms->buffer_width + (x+tparms->x)] = ColorRGBA8{ (uint8_t)accumelator.r, (uint8_t)accumelator.g, (uint8_t)accumelator.b, (uint8_t)accumelator.a };
		}
	}

	tparms->finished_queue_mutex.lock();
	tparms->finished_queue.push(tparms->thread_id);
	tparms->finished_queue_mutex.unlock();
}

int main()
{
	const size_t BUFFER_WIDTH = 1366; //1920;
	const size_t BUFFER_HEIGHT = 768; // 1080;
	const Real ASPECT_RATIO = (Real)BUFFER_WIDTH / (Real)BUFFER_HEIGHT;
	const size_t SAMPLE_COUNT = 16;
	const char* OUTPUT_FILE_NAME = "output/image.png";
	const Vector2 CENTER_POS = { -0.7453, 0.1127 };
	const Vector2 VIEW_SIZE = { 6.5e-4*ASPECT_RATIO, 6.5e-4};
	const size_t MAX_ITERATIONS = 250;
	const Real MAX_LENGTH = 2.0;
	const size_t THREAD_WIDTH = 128;
	const size_t THREAD_HEIGHT = 128;
	const size_t THREAD_COUNT = 4;

	ColorRGBA8* color_buffer = new ColorRGBA8[BUFFER_WIDTH*BUFFER_HEIGHT];

	const RenderParameters render_params = RenderParameters{
		BUFFER_WIDTH,
		BUFFER_HEIGHT,
		SAMPLE_COUNT,
		CENTER_POS,
		VIEW_SIZE,
		MAX_ITERATIONS,
		MAX_LENGTH
	};

	// clear the buffer
	for (size_t y = 0; y < BUFFER_HEIGHT; ++y)
	{
		for (size_t x = 0; x < BUFFER_WIDTH; ++x)
		{
			color_buffer[y*BUFFER_WIDTH + x] = ColorRGBA8{ 0, 0, 0, 255 };
		}
	}

	// thread messaging
	std::mutex finished_queue_mutex;
	std::queue<size_t> finished_queue;

	std::vector<ThreadParameters> thread_parameters;
	for (size_t y = 0; y < divide_roundup(BUFFER_HEIGHT, THREAD_HEIGHT); ++y)
	{
		for (size_t x = 0; x < divide_roundup(BUFFER_WIDTH, THREAD_WIDTH); ++x)
		{
			const size_t max_width = BUFFER_WIDTH - x*THREAD_WIDTH;
			const size_t max_height = BUFFER_HEIGHT - y*THREAD_HEIGHT;
			thread_parameters.push_back(ThreadParameters{
					x*THREAD_WIDTH,
					y*THREAD_HEIGHT,
					clamp(THREAD_WIDTH, 0, max_width-1),
					clamp(THREAD_HEIGHT, 0, max_height-1),
					color_buffer,
					0,
					std::ref(finished_queue_mutex),
					std::ref(finished_queue)
				});
		}
	}
	
	// start rendering threads
	printf("Rendering:\n");
	std::vector<std::thread> threads;
	size_t started_threads = 0;
	size_t finished_threads = 0;
	for (size_t i = 0; i < min(THREAD_COUNT, thread_parameters.size()); ++i)
	{
		thread_parameters[started_threads].thread_id = started_threads;
		threads.push_back(std::thread(thread_worker, &render_params, &thread_parameters[started_threads]));
		started_threads += 1;
	}
	while (finished_threads < thread_parameters.size())
	{
		finished_queue_mutex.lock();
		while (finished_queue.empty())
		{
			finished_queue_mutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			finished_queue_mutex.lock();
		}
		const size_t finished_id = finished_queue.front();
		finished_queue.pop();
		finished_queue_mutex.unlock();

		// start a new thread
		if (started_threads < thread_parameters.size())
		{
			ThreadParameters* next_tprams = &thread_parameters[started_threads];
			next_tprams->thread_id = finished_id;
			threads.push_back(std::thread(thread_worker, &render_params, next_tprams));
			started_threads += 1;
		}

		finished_threads += 1;
		print_progress(finished_threads*100/thread_parameters.size());
	}
	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}
	printf("\n");

	// save the buffer to an image file
	if (!stbi_write_png(OUTPUT_FILE_NAME, BUFFER_WIDTH, BUFFER_HEIGHT, 4, (const void*)color_buffer, 0))
	{
		printf("Failed to save the buffer to %s\n", OUTPUT_FILE_NAME);
		return EXIT_FAILURE;

	}
	printf("Finished rendering\n");
	printf("Saved the image to \"%s\" file\n", OUTPUT_FILE_NAME);

	// cleanup
	delete[] color_buffer;

	return EXIT_SUCCESS;
}
