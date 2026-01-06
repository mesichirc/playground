static u8 debug_string[256];
char *fox_image_src = "./assets/fox_sheet.png";
char *bg_image = "./assets/bg.png";
char *bg_forest_image = "./assets/bg_forest.png";
char *floor_image = "./assets/floor.png";

typedef struct playground_state playground_state;
struct playground_state {
  memory_arena* arena;
  u64 current_time;
  i32 player_x;
  i32 player_y;
};

#define FOX_START_X 100

i32 rect_x = 0;
#define RECT_X_MIN -300
#define RECT_X_MAX 1280

static playground_state state;

void
playground_init(memory_arena *arena, u64 ctime, u32 refres_rate, u32 target_fps)
{
  state.arena = arena;
  state.current_time = ctime;
  state.player_x = 100;
  state.player_y = 100;
  if (!IS_BE()) {

    for (u32 i = 0; i < playground_color_size; i += 1) {
      playground_colors[i] = swap_bytes_u32(playground_colors[i]);
    }
  } 
  
  /** Fox */
  { 
    u64 size;
    void* image = platform_read_entire_file(fox_image_src, &size);
    int channels;
    int w, h;
  }
  
}

b32 is_moving = false;
typedef enum moving_directions moving_directions;
enum moving_directions { 
  right = 0,
  left = 1,
  up = 2,
  down = 3,
};
moving_directions moving_direction = right;
u8 last_key_pressed = 0;

void
draw_debug_text(graphics_bitmap *canvas, i32 x, i32 y, f32 fps)
{
    u32 color = 0xFF000000;
    graphics_point point = graphics_draw_debug_string(canvas, x, y, STR8_LIT("fps:"), color, 32);
    string8 fps_string = (string8) { debug_string, sizeof(debug_string) };
    fps_string = strconv_f32_to_string8(fps_string, fps, 6);
    point = graphics_draw_debug_string(canvas, point.x, point.y, fps_string, color, 32);
}

#define TILES_ROWS 9
#define TILES_COLS 17
u32 tile_map[TILES_ROWS][TILES_COLS] = {
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 1,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1}
};

void 
playground_update_and_render(
    u8 *buffer,
    i32 width,
    i32 height,
    playground_input *input,
    u64 timestamp,
    f32 fps
)
{
 
  u32 tile_size = 80;
  u32 player_width = tile_size;
  u32 player_height = tile_size; 
  f32 delta_time = (f32)(timestamp - state.current_time);
  state.current_time = timestamp;
  graphics_bitmap canvas;
  canvas.memory = buffer;
  canvas.width = width;
  canvas.height = height;
  canvas.bytes_per_pixel = 4;
  canvas.stride = width;
  canvas.scale = 1;

  graphics_clear_canvas(&canvas, 0xFF000000);  
  
  i32 upper_left_x = -40;
  u32 upper_left_y = 0;

  /** Controller */
  {
    f32 player_x_delta = 0.0f;
    f32 player_y_delta = 0.0f;

    if (input->down.ended_down) {
      player_y_delta += 5.0f;
    }
    if (input->right.ended_down) {
      player_x_delta += 5.0f;
    }

    if (input->left.ended_down) {
      player_x_delta -= 5.0f;
    }

    if (input->up.ended_down) {
      player_y_delta -= 5.0f;
    }

    state.player_x += player_x_delta;
    state.player_y += player_y_delta;
  }

  /** Draw tilemap */
  {
    u32 base_color = 0xFF000000;
    u32 tile_color = 0xFFFFFFFF;
    for (i32 row = 0; row < TILES_ROWS; row+=1) {
      for (i32 col = 0; col < TILES_COLS; col+=1) {
        u32 tile = tile_map[row][col];
        u32 color = tile == 1 ? tile_color : base_color;
        i32 x = upper_left_x + tile_size * col;
        i32 y = upper_left_y + tile_size * row;
        graphics_draw_rect(&canvas, x, y, tile_size, tile_size, color);
      }
    }
  }

  /** Draw Player */
  {
    f32 player_left = state.player_x - player_width * 0.5f;
    f32 player_top = state.player_y - player_height;
    graphics_draw_rect(&canvas, player_left, player_top, player_width, player_height, playground_colors[playground_color_rose]);
  }

  draw_debug_text(&canvas, 8, 8, fps);
}
