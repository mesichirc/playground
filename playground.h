enum playground_color_tokens {
  playground_color_text = 0,
  playground_color_base,
  playground_color_love,
  playground_color_gold,
  playground_color_rose,
  playground_color_pine,
  playground_color_size
};

u32 playground_colors[playground_color_size] = {
  [playground_color_text] = 0xe0def4ff,
  [playground_color_base] = 0xfaf4edff,
  [playground_color_love] = 0xb4637aff,
  [playground_color_gold] = 0xea9d34ff,
  [playground_color_rose] = 0xd7827eff,
  [playground_color_pine] = 0x286983ff,
};

typedef struct playground_key playground_key;
struct playground_key {
  int half_transition_count;
  b32 ended_down;
};

typedef struct playground_input playground_input;
struct playground_input {
  union {
    playground_key buttons[4];
    struct {
      playground_key up;
      playground_key down;
      playground_key left;
      playground_key right;
    };
  };
};

typedef struct playground_tile_map playground_tile_map;
struct playground_tile_map {
  u32 *tiles;
};

typedef struct playground_world playground_world;
struct playground_world {
  i32 count_x;
  i32 count_y;

  f32 upper_left_x;
  f32 upper_left_y;
  f32 tile_width;
  f32 tile_height;

  i32 tilemap_count_x;
  i32 tilemap_count_y;
  playground_tile_map *tilemaps;
};


typedef struct playground_state playground_state;
struct playground_state {
  memory_arena* arena;
  u64 current_time;
  f32 player_x;
  f32 player_y;
  i32 player_tile_x;
  i32 player_tile_y;
  i32 player_tilemap_x;
  i32 player_tilemap_y;
};

typedef struct playground_cannonical_position playground_cannonical_position;
struct playground_cannonical_position {
  i32 tilemap_x;
  i32 tilemap_y;

  i32 tile_x;
  i32 tile_y;

  f32 x;
  f32 y;
};

typedef struct playground_raw_position playground_raw_position;
struct playground_raw_position {
  i32 tilemap_x;
  i32 tilemap_y;

  f32 x;
  f32 y;
};

inline playground_cannonical_position playground_get_cannonical_position(playground_world *world, playground_raw_position pos);

void playground_init(
    memory_arena *arena,
    u64 ctime,
    u32 refres_rate,
    u32 target_fps
);
void playground_update_and_render(
    u8 *buffer,
    i32 width,
    i32 height,
    playground_input *input,
    u64 timestamp,
    f32 fps
);

