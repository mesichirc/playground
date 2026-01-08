static u8 debug_string[256];
char *fox_image_src = "./assets/fox_sheet.png";
char *bg_image = "./assets/bg.png";
char *bg_forest_image = "./assets/bg_forest.png";
char *floor_image = "./assets/floor.png";


#define FOX_START_X 100

i32 rect_x = 0;
#define RECT_X_MIN -300
#define RECT_X_MAX 1280

static playground_state state;
#define TILES_ROWS 9
#define TILES_COLS 17
u32 tiles0[TILES_ROWS][TILES_COLS] = {
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,0},
  {1, 1, 1, 1,   1, 1, 1, 0,   1, 1, 1, 1,   1, 1, 1, 1,1},
};

u32 tiles1[TILES_ROWS][TILES_COLS] = {
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,0},
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
};
u32 tiles2[TILES_ROWS][TILES_COLS] = {
  {1, 1, 1, 1,   1, 1, 1, 0,   1, 1, 1, 1,   1, 1, 1, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
};

u32 tiles3[TILES_ROWS][TILES_COLS] = {
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   1, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {0, 0, 0, 1,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,1},
  {1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 1,   1, 1, 1, 0,1},
};


playground_world world;
playground_tile_map tilemaps[2][2];

void
playground_init(memory_arena *arena, u64 ctime, u32 refres_rate, u32 target_fps)
{
  state.arena = arena;
  state.current_time = ctime;
  state.player_x = 200;
  state.player_y = 200;
  state.player_tile_x = 0;
  state.player_tile_y = 0;
  state.player_tilemap_x = 0;
  state.player_tilemap_y = 0;
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

  world.count_x = TILES_COLS;
  world.count_y = TILES_ROWS;
  world.upper_left_x = -40;
  world.upper_left_y = 0;
  world.tile_width = 80;
  world.tile_height = 80;
  world.tilemaps = (playground_tile_map *)tilemaps;
  world.tilemap_count_x = 2;
  world.tilemap_count_y = 2;

  tilemaps[0][0].tiles = (u32*)tiles0;
  tilemaps[0][1].tiles = (u32*)tiles1;
  tilemaps[1][0].tiles = (u32*)tiles2;
  tilemaps[1][1].tiles = (u32*)tiles3;
  
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

playground_cannonical_position 
playground_get_cannonical_position(playground_world *world, playground_raw_position pos)
{
    playground_cannonical_position result;
    result.tilemap_x = pos.tilemap_x;
    result.tilemap_y = pos.tilemap_y;

    f32 x = pos.x - world->upper_left_x;
    f32 y = pos.y - world->upper_left_y;
  
    result.tile_x = floor_f32_to_i32(x / world->tile_width);
    result.tile_y = floor_f32_to_i32(y / world->tile_height);
    result.x = x - result.tile_x * world->tile_width;
    result.y = y - result.tile_y * world->tile_height;

    assert(result.x >= 0);
    assert(result.y >= 0);
    assert(result.x < world->tile_width);
    assert(result.y < world->tile_height);

    if (result.tile_x < 0) {
      result.tile_x = world->count_x + result.tile_x;
      result.tilemap_x -= 1;
    }

    if (result.tile_y < 0) {
      result.tile_y = world->count_y + result.tile_y;
      result.tilemap_y -= 1;
    }

    if (result.tile_x >= world->count_x) {
      result.tile_x = result.tile_x - world->count_x;
      result.tilemap_x += 1;
    }

    if (result.tile_y >= world->count_y) {
      result.tile_y = result.tile_y - world->count_y;
      result.tilemap_y += 1;
    }

    return result;
}

inline u32
playground_get_tilemap_value_unchecked(playground_world *world, playground_tile_map *tile_map, i32 x, i32 y) 
{
  u32 result = tile_map->tiles[y * world->count_x + x];

  return result;
}

static playground_tile_map*
playground_get_tilemap(playground_world *world, i32 tilemap_x, i32 tilemap_y)
{
  playground_tile_map* tilemap = nil;
  if ((tilemap_x >= 0) && (tilemap_x < world->tilemap_count_x) && 
      (tilemap_y >= 0) && (tilemap_y < world->tilemap_count_y)) {
    tilemap = &world->tilemaps[tilemap_y * world->tilemap_count_x + tilemap_x];
  }
  return tilemap;
}

static b32 
playground_is_world_point_empty(playground_world *world, playground_raw_position test_pos)
{
    b32 empty = false;

    playground_cannonical_position canpos = playground_get_cannonical_position(world, test_pos);
    playground_tile_map *tilemap = playground_get_tilemap(world, canpos.tilemap_x, canpos.tilemap_y);
    if (tilemap) {
      u32 value = tilemap->tiles[world->count_x * canpos.tile_y + canpos.tile_x];
      empty = (value == 0);
    }

    return empty;
}

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
  playground_tile_map *tilemap = &world.tilemaps[state.player_tilemap_y * world.tilemap_count_x + state.player_tilemap_x];
 
  u32 player_width = world.tile_width * .7f;
  u32 player_height = world.tile_height * .8f; 
  f32 delta_time = (f32)(timestamp - state.current_time);
  f32 delta_time_sec = delta_time / 1000.0f;
  state.current_time = timestamp;
  graphics_bitmap canvas;
  canvas.memory = buffer;
  canvas.width = width;
  canvas.height = height;
  canvas.bytes_per_pixel = 4;
  canvas.stride = width;
  canvas.scale = 1;

  graphics_clear_canvas(&canvas, 0xFF000000);  
  
  /** Controller */
  {
    f32 player_x_delta = 0.0f;
    f32 player_y_delta = 0.0f;

    if (input->down.ended_down) {
      player_y_delta += 1.0f;
    }
    if (input->right.ended_down) {
      player_x_delta += 1.0f;
    }

    if (input->left.ended_down) {
      player_x_delta -= 1.0f;
    }

    if (input->up.ended_down) {
      player_y_delta -= 1.0f;
    }


    if (player_y_delta) {
      u32 a = 0;
    }

    player_x_delta *= 128.0f;
    player_y_delta *= 128.0f;

    f32 new_player_x = state.player_x + player_x_delta * delta_time_sec;
    f32 new_player_y = state.player_y + player_y_delta * delta_time_sec;

    playground_raw_position player_pos = {
      .tilemap_x = state.player_tilemap_x,
      .tilemap_y = state.player_tilemap_y,
      .x = new_player_x,
      .y = new_player_y,
    };

    playground_raw_position left_pos = player_pos;
    playground_raw_position right_pos = player_pos;
    left_pos.x -= (f32)player_width * 0.5f;
    right_pos.x += (f32)player_width * 0.5f;

    if (playground_is_world_point_empty(&world, player_pos) &&
        playground_is_world_point_empty(&world, left_pos) && 
        playground_is_world_point_empty(&world, right_pos)) {
      playground_cannonical_position canpos = playground_get_cannonical_position(&world, player_pos);
      state.player_tilemap_x = canpos.tilemap_x;
      state.player_tilemap_y = canpos.tilemap_y;

      state.player_x = world.upper_left_x + world.tile_width * canpos.tile_x + canpos.x;
      state.player_y = world.upper_left_y + world.tile_height * canpos.tile_y + canpos.y;
    }
  }

  /** Draw tilemap */
  {
    u32 base_color = 0xFF000000;
    u32 tile_color = 0xFFFFFFFF;
    for (i32 row = 0; row < world.count_y; row+=1) {
      for (i32 col = 0; col < world.count_x; col+=1) {
        u32 tile = tilemap->tiles[row * world.count_x + col];
        u32 color = tile == 1 ? tile_color : base_color;
        i32 x = world.upper_left_x + world.tile_width * col;
        i32 y = world.upper_left_y + world.tile_height * row;
        graphics_draw_rect(&canvas, x, y, world.tile_width, world.tile_height, color);
      }
    }
  }

  /** Draw Player */
  {
    f32 player_left = state.player_x - (f32)player_width * 0.5f;
    f32 player_top = state.player_y - (f32)player_height;
    graphics_draw_rect(&canvas, player_left, player_top, player_width, player_height, playground_colors[playground_color_rose]);
  }

  draw_debug_text(&canvas, 8, 8, fps);
}
