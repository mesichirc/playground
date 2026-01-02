static u8 debug_string[256];
char *fox_image_src = "./assets/fox_sheet.png";
char *bg_image = "./assets/bg.png";
char *bg_forest_image = "./assets/bg_forest.png";
char *floor_image = "./assets/floor.png";

typedef struct playground_state playground_state;
struct playground_state {
  memory_arena* arena;
  animation_sprite_definition fox_animation;
  u64 current_time;
  graphics_bitmap fox_sprites;
  graphics_bitmap bg_bitmap_cached;
  graphics_bitmap bg_bitmap;
  graphics_bitmap bg_forest;
  graphics_bitmap floor;
  animation_duration_value walk_animation_value;
  u32 from_x;
  u32 to_x;
  f32 time_to_walk_transition;
  b32 walk_done;
};

#define FOX_START_X 100

i32 rect_x = 0;
#define RECT_X_MIN -300
#define RECT_X_MAX 1280

static playground_state state;

void
fox_animation_setup(animation_sprite_definition *animation, u32 row, u32 frame_count, f32 duration) 
{
  animation_sprite_stop(animation);
  animation->frame_count = frame_count;
  for (int i = 0; i < frame_count; i += 1) {
    animation->durations[i] = duration;
    animation->columns[i] = (u8)i;
    animation->rows[i] = row;
  }
  animation->frame_size = 32;
  animation->bitmap = &state.fox_sprites;
  animation_sprite_set_looping(animation, true);
}

void 
playground_init(memory_arena *arena, u64 ctime, u32 refres_rate, u32 target_fps)
{
  state.arena = arena;
  state.current_time = ctime;
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
    state.fox_sprites.memory = stbi_load_from_memory(image, size, &w, &h, &channels, 4);
    state.fox_sprites.width = (u32)w;
    state.fox_sprites.height = (u32)h;
    state.fox_sprites.scale = 1;
    state.fox_sprites.stride = w;
    state.fox_sprites.bytes_per_pixel = 4;

    fox_animation_setup(&state.fox_animation, 1, 14, 120);
    state.fox_animation.durations[0]  = 1.2 * state.fox_animation.durations[0];
    state.fox_animation.durations[13] = 1.2 * state.fox_animation.durations[13];
    state.fox_animation.durations[12] = 1.2 * state.fox_animation.durations[12];
    animation_sprite_start(&state.fox_animation);
    state.from_x = FOX_START_X;
  }

  /** Bg */
  {
    u64 size;
    void* image = platform_read_entire_file(bg_image, &size);
    int channels;
    int w, h;
    state.bg_bitmap.memory = stbi_load_from_memory(image, size, &w, &h, &channels, 4);
    state.bg_bitmap.width = (u32)w;
    state.bg_bitmap.height = (u32)h;
    state.bg_bitmap.scale = 1;
    state.bg_bitmap.stride = w;
  }
  {
    u64 size;
    void* image = platform_read_entire_file(bg_forest_image, &size);
    int channels;
    int w, h;
    state.bg_forest.memory = stbi_load_from_memory(image, size, &w, &h, &channels, 4);
    state.bg_forest.width = (u32)w;
    state.bg_forest.height = (u32)h;
    state.bg_forest.scale = 1;
    state.bg_forest.stride = w;
  }

  {
    u64 size;
    void* image = platform_read_entire_file(floor_image, &size);
    int channels;
    int w, h;
    state.floor.memory = stbi_load_from_memory(image, size, &w, &h, &channels, 4);
    state.floor.width = (u32)w;
    state.floor.height = (u32)h;
    state.floor.scale = 1;
    state.floor.stride = w;
  }
  
  state.bg_bitmap_cached.memory = nil;
  state.bg_bitmap_cached.width = 0;
  state.bg_bitmap_cached.height = 0;
  state.bg_bitmap_cached.stride = 0;
  state.bg_bitmap_cached.scale = 1;

  state.walk_animation_value.duration = 10000;
  animation_duration_value_reset(&state.walk_animation_value);
  state.to_x = 1000;
  animation_duration_stop(&state.walk_animation_value);
}

b32 is_moving = false;
typedef enum moving_directions moving_directions;
enum moving_directions { 
  right = 0,
  left = 1,
};
moving_directions moving_direction = right;
u8 last_key_pressed = 0;


void 
playground_update_and_render(
    u8 *buffer,
    i32 width,
    i32 height,
    input_handle *ihandle,
    u64 timestamp,
    f32 fps
)
{
  
  f32 delta_time = (f32)(timestamp - state.current_time);
  state.current_time = timestamp;
  graphics_bitmap canvas;
  canvas.memory = buffer;
  canvas.width = width;
  canvas.height = height;
  canvas.bytes_per_pixel = 4;
  canvas.stride = width;
  canvas.scale = 1;

  graphics_clear_canvas(&canvas, playground_colors[playground_color_base]);  
  /** Background */
  {
    if (state.bg_bitmap_cached.width != width || state.bg_bitmap_cached.height != height) {
      memory_arena_clear(state.arena);
      state.bg_bitmap_cached.width = width;
      state.bg_bitmap_cached.stride = width;
      state.bg_bitmap_cached.height = height;
      state.bg_bitmap_cached.memory = PUSH_ARRAY(state.arena, u8, width * height * 4);
      graphics_draw_rect_image(
          &state.bg_bitmap_cached,
          0,
          0,
          width,
          height, 
          &state.bg_bitmap,
          true,
          false
      ); 
      graphics_draw_rect_image(&state.bg_bitmap_cached, 0, 0, canvas.width, canvas.height, &state.bg_forest, true, false);
      graphics_draw_rect_image(&state.bg_bitmap_cached, 0, 0, width, height, &state.floor, true, false);
    }
    memcpy(canvas.memory, state.bg_bitmap_cached.memory, canvas.width * canvas.height * 4);
    u32 y = canvas.height - state.bg_forest.height;
  }
  input_state *input = ihandle->next;
  /** Debug text */
  {
    u32 color =  playground_colors[playground_color_text];
    graphics_point point = graphics_draw_debug_string(&canvas, 8, 8, STR8_LIT("fps:"), color, 32);
    string8 fps_string = (string8) { debug_string, sizeof(debug_string) };
    fps_string = strconv_f32_to_string8(fps_string, fps, 6);
    point = graphics_draw_debug_string(&canvas, point.x, point.y, fps_string, color, 32);
    point = graphics_draw_debug_string(&canvas, point.x, point.y, STR8_LIT("  Key pressed:"), color, 32);
    if (input->flag & input_flag_key_pressed) {
      for (u32 i = 0; i < input_key_code_last; i += 1) {
        if (input->key_codes[i]) {
          last_key_pressed = i;
          break;
        }
      }
    }
    if (input->flag & input_flag_key_released) {
      last_key_pressed = 0;
    }
    if (last_key_pressed) {
      u8 key_string[1] = { last_key_pressed };
      string8 str = (string8) {key_string, 1};
      graphics_draw_debug_string(&canvas, point.x, point.y, str, color, 32);
    }
  }

  state.time_to_walk_transition += delta_time;
  u32 h_floor = 8;
  u32 h_floor_scaled = h_floor * canvas.height / state.bg_bitmap.height;
  u32 y = canvas.height - 128 - h_floor_scaled;
  graphics_bitmap fox_image;
  {
    if ((input->flag & input_flag_key_pressed) && !is_moving) {
      if (input->key_codes[input_key_code_a] || input->key_codes[input_key_code_d]) {
        fox_animation_setup(&state.fox_animation, 2, 8, 40);
        animation_sprite_start(&state.fox_animation);
        moving_direction = input->key_codes[input_key_code_a] ? left : right;
        is_moving = true;
      }
    }
    if ((input->flag & input_flag_key_released) && is_moving) {
      if (input->key_codes[input_key_code_a] || input->key_codes[input_key_code_d]) {
        fox_animation_setup(&state.fox_animation, 1, 14, 120);
        animation_sprite_start(&state.fox_animation);
        is_moving = false;
      }
    }
    u32 prev_frame_index = state.fox_animation.current_frame_index;
    animation_sprite_update(&state.fox_animation, delta_time);
    i32 velocity = canvas.width / 200;
    if (is_moving) {
      if (moving_direction == right) {
        rect_x += velocity;
      } else {
        rect_x -= velocity;
      }
    }
    animation_sprite_get_bitmap(&state.fox_animation, &fox_image);
    rect_x = max(min((i32)canvas.width, rect_x), RECT_X_MIN);
    graphics_draw_rect_image(&canvas, rect_x, y, 128, 128, &fox_image, true, moving_direction); 
  }
}
