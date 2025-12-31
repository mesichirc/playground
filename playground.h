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
  [playground_color_text] = 0x575279ff,
  [playground_color_base] = 0xfaf4edff,
  [playground_color_love] = 0xb4637aff,
  [playground_color_gold] = 0xea9d34ff,
  [playground_color_rose] = 0xd7827eff,
  [playground_color_pine] = 0x286983ff,
};

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
    input_handle *ihandle,
    u64 timestamp,
    f32 fps
);

