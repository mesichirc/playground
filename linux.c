#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>

#if (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
#include <emmintrin.h>
#elif defined(__aarch64__)
#include <arm_neon.h>
#endif

#include "core.h"
#include <math.h>
#include "memory.h"
#include "platform.h"
#include "graphics.h"
#include "strconv.h"
#include "input.h"
#include "animation.h"
#include "ppm.h"
#include "playground.h"
#include "debug_font.h"
#include "pcg_random.h"
#include "physics.h"

#define RGFW_IMPLEMENTATION
#include "./external/RGFW.h"
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "./external/stb_image.h"

#include "core.c"
#include "memory.c"
#include "strconv.c"
#include "graphics.c"
#include "playground.c"
#include "ppm.c"
#include "animation.c"
#include "pcg_random.c"
#include "physics.c"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define BYTES_PER_PIXEL 4

u8 *buffer = nil;
u32 windowWidth = WINDOW_WIDTH;
u32 windowHeight = WINDOW_HEIGHT;


i32
floor_f32_to_i32(f32 n)
{
  return (i32)floorf((f32)n);
}
u32 
platform_pagesize(void) {
  return (u32)sysconf(_SC_PAGESIZE);
}

void* 
platform_memory_reserve(u64 size)
{
  void *result = mmap(nil, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (result == MAP_FAILED) {
    return nil;
  }

  return result;
}

b32 
platform_memory_commit(void *ptr, u64 size)
{
  i32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return ret == 0;
}
b32 
platform_memory_decommit(void *ptr, u64 size)
{
  i32 ret = mprotect(ptr, size, PROT_NONE);
  if (ret != 0) return false;
  ret = madvise(ptr, size, MADV_DONTNEED);
  return ret == 0;
}

b32 
platform_memory_release(void *ptr, u64 size)
{
  i32 ret = munmap(ptr, size);

  return ret == 0;
}

void
platform_memcpy(void *dest, void *source, u64 size)
{
	memcpy(dest, source, size);
}

static u64
current_time_ms(void)
{
  struct timespec tp;
  clockid_t clk_id;

  clk_id = CLOCK_REALTIME;
  clock_gettime(clk_id, &tp);

  return (u64)((u64)tp.tv_sec * 1000000000 + (u64)tp.tv_nsec) / 1000000;
}

static u64
monotonic_time_ns(void)
{
  struct timespec tp;
  clockid_t clk_id;

  clk_id = CLOCK_BOOTTIME;
  clock_gettime(clk_id, &tp);

  return (u64)tp.tv_sec * 1000000000 + (u64)tp.tv_nsec;
}

static void
sleep_ns(u64 nanosecs)
{
  struct timespec ts;
  ts.tv_sec = nanosecs / 1000000000;
  ts.tv_nsec = nanosecs % 1000000000; 
  nanosleep(&ts, nil);
}

void* 
platform_read_entire_file(char *path, u64 *size)
{
  i32 fd = open(path, O_RDONLY);
  if (fd < 0) {
    return nil;
  }
  struct stat s;
  if (fstat(fd, &s) < 0) {
    return nil;
  }
  *size = s.st_size;

  return mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
}

void
platform_print(string8 str)
{
	printf("%.*s", (u32)str.size, (char *)str.base);
}

void
platform_memset(void *ptr, u8 placeholder, u64 size)
{
  memset(ptr, (int)placeholder, size);
}

void *
platform_load_image(void *mem, u64 size, i32 *w, i32 *h, i32 *channels, i32 desired_channels)
{
  return stbi_load_from_memory(mem, size, w, h, channels, desired_channels);
}

static void
process_rgfw_key(RGFW_event *event, u32 key, playground_key *prev, playground_key *next)
{
  if (event->type == RGFW_keyPressed && event->key.value == key) {
    next->ended_down = true;
  }
  if (event->type == RGFW_keyReleased && event->key.value == key) {
    next->ended_down = false;
  }
  if (next->ended_down != prev->ended_down) {
    next->half_transition_count = 1;
  } else {
    next->half_transition_count = 0;
  }
}

i32
main()
{
  i32 window_width = WINDOW_WIDTH;
  i32 window_height = WINDOW_HEIGHT;
  memory_arena *arena = memory_arena_create(MB(10), KB(100));
  assert(arena);
  memory_arena *playground_arena = memory_arena_create(MB(100), KB(100));
  assert(playground_arena);

  RGFW_window *win = RGFW_createWindow("THE playground", 0, 0, windowWidth, windowHeight, RGFW_windowCenter | RGFW_windowNoResize | RGFW_windowScaleToMonitor | RGFW_windowNoBorder);

  playground_input inputs[2] = {0};
  playground_input *prev_input = inputs;
  playground_input *next_input = inputs + 1;

  RGFW_window_getSize(win, &window_width, &window_height);
  u64 bsize = window_width * window_height * BYTES_PER_PIXEL;
  u8 *buffer = PUSH_ARRAY(arena, u8, bsize);
  RGFW_surface *surface = RGFW_createSurface(buffer, window_width, window_height, RGFW_formatRGBA8);
  RGFW_monitor mon = RGFW_window_getMonitor(win);
  u32 target_fps = 60;
  f32 ns_per_frame = 1000000000.0 / target_fps;
  f32 fps = 60.0;

  pcg_random_seed_u32(time(nil) ^ (u64)&printf, (u64)&window_width);
  playground_init(playground_arena, current_time_ms(), (f64)mon.mode.refreshRate, target_fps);


  while (RGFW_window_shouldClose(win) == RGFW_FALSE) {
    u64 startframe = monotonic_time_ns();
    RGFW_event event;

    while (RGFW_window_checkEvent(win, &event)) {
      if (event.type == RGFW_quit) break;
      i32 mouse_x, mouse_y;

      process_rgfw_key(&event, RGFW_w, &prev_input->up, &next_input->up); 
      process_rgfw_key(&event, RGFW_s, &prev_input->down, &next_input->down); 
      process_rgfw_key(&event, RGFW_a, &prev_input->left, &next_input->left); 
      process_rgfw_key(&event, RGFW_d, &prev_input->right, &next_input->right); 
      RGFW_window_getMouse(win, &mouse_x, &mouse_y);
      if (event.type == RGFW_windowResized) {
        RGFW_window_getSize(win, &window_width, &window_height);
        memory_arena_pop(arena, bsize);
        bsize = window_width * window_height * BYTES_PER_PIXEL;
        buffer = PUSH_ARRAY(arena, u8, bsize);
        RGFW_surface_free(surface);
        surface = RGFW_createSurface(buffer, window_width, window_height, RGFW_formatRGBA8);
      }
    }

    playground_update_and_render(buffer, window_width, window_height, next_input, current_time_ms(), fps);
    u64 elapsed_ns = monotonic_time_ns() - startframe;
   
    // assert(elapsed_ns < ns_per_frame);
    while (ns_per_frame > elapsed_ns) {
      sleep_ns(ns_per_frame - elapsed_ns);
      elapsed_ns = monotonic_time_ns() - startframe;
    }
    
    fps = (f32)((f64)1000000000.0 / (f64)elapsed_ns);
    RGFW_window_blitSurface(win, surface); 
    playground_input *temp;
    temp = next_input;
    next_input = prev_input;
    prev_input = next_input;
 }
  
  RGFW_window_close(win);
  return 0;
}
