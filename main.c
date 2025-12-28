#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "core.h"
#include "strconv.h"
#include "input.h"
#include "graphics.h"
#define RGFW_IMPLEMENTATION
#include "./external/RGFW.h"
#include "playground.h"
#include "debug_font.h"

#include "core.c"
#include "strconv.c"
#include "graphics.c"
#include "playground.c"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BYTES_PER_PIXEL 4

u8 *buffer = nil;
u32 windowWidth = WINDOW_WIDTH;
u32 windowHeight = WINDOW_HEIGHT;


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

i32
main()
{
  i32 window_width = WINDOW_WIDTH;
  i32 window_height = WINDOW_HEIGHT;

  RGFW_window *win = RGFW_createWindow("THE playground", 0, 0, windowWidth, windowHeight, RGFW_windowCenter | RGFW_windowNoResize);
  input_state inputs[2];
  memset(inputs, 0, sizeof(inputs));
  u32 prev_input = 0;
  u32 input = 1;

  input_handle ihandle; 
  RGFW_window_getSize(win, &window_width, &window_height);
  u8 *buffer = (u8*)malloc(window_width * window_height * BYTES_PER_PIXEL);
  RGFW_surface *surface = RGFW_createSurface(buffer, window_width, window_height, RGFW_formatRGBA8);
  RGFW_monitor mon = RGFW_window_getMonitor(win);
  u32 target_fps = 60.0;
  f32 ns_per_frame = 1000000000.0 / target_fps;
  f32 fps = 60.0;

  playground_init(current_time_ms(), (f64)mon.mode.refreshRate, target_fps);

  while (RGFW_window_shouldClose(win) == RGFW_FALSE) {
    u64 startframe = monotonic_time_ns();
    RGFW_event event;
    ihandle.next = inputs + input;
    ihandle.prev = inputs + prev_input;
    prev_input = input;
    input = (input + 1) & 1;

    while (RGFW_window_checkEvent(win, &event)) {
      if (event.type == RGFW_quit) break;
      i32 mouse_x, mouse_y;
      RGFW_window_getMouse(win, &mouse_x, &mouse_y);
      if (event.type == RGFW_windowResized) {
        RGFW_window_getSize(win, &window_width, &window_height);
        free(buffer);
        buffer = (u8*)malloc(window_width * window_height * BYTES_PER_PIXEL);
        RGFW_surface_free(surface);
        surface = RGFW_createSurface(buffer, window_width, window_height, RGFW_formatRGBA8);
      }
    }

    playground_update_and_render(buffer, window_width, window_height, &ihandle, current_time_ms(), fps);
    u64 elapsed_ns = monotonic_time_ns() - startframe;
    assert(elapsed_ns < ns_per_frame);
    while (ns_per_frame > elapsed_ns) {
      sleep_ns(ns_per_frame - elapsed_ns);
      elapsed_ns = monotonic_time_ns() - startframe;
    }
    fps = (f32)((f64)1000000000.0 / (f64)elapsed_ns);
    RGFW_window_blitSurface(win, surface); 
  }
  
  RGFW_window_close(win);
  return 0;
}
