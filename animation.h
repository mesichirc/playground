#define ANIMATION_SPRITE_MAX_FRAMES 16

typedef struct animation_sprite_frame animation_sprite_frame;
struct animation_sprite_frame {
  f32 duration;
  u8 col;
  u8 row;
};

typedef struct animation_duration_value animation_duration_value;
struct animation_duration_value {
  f32 value;
  f32 time;
  f32 duration;
  b32 is_running;
};

#define animation_sprite_looping 0x1
#define animation_sprite_flipped 0x2
#define animation_sprite_active  0x4

typedef struct animation_sprite_definition animation_sprite_definition;
struct animation_sprite_definition {
  graphics_bitmap *bitmap;
  f32             current_frame_time;
  u32             frame_size;
  u8              frame_count;
  u8              current_frame_index;
  u8              state;
  f32             durations[ANIMATION_SPRITE_MAX_FRAMES];
  u8              columns[ANIMATION_SPRITE_MAX_FRAMES];
  u8              rows[ANIMATION_SPRITE_MAX_FRAMES];
};

inline b32  animation_sprite_is_looping(animation_sprite_definition *animation);
inline b32  animation_sprite_is_flipped(animation_sprite_definition *animation);
inline b32  animation_sprite_is_active(animation_sprite_definition *animation);
inline void animation_sprite_set_looping(animation_sprite_definition *animation, b32 looping);
inline void animation_sprite_set_flipped(animation_sprite_definition *animation, b32 flipped);
inline void animation_sprite_set_active(animation_sprite_definition *animation, b32 active);
void animation_sprite_get_bitmap(animation_sprite_definition *animation, graphics_bitmap *out);
void animation_sprite_update(animation_sprite_definition *animation, f32 delta_t);
void animation_sprite_start(animation_sprite_definition *animation);
void animation_sprite_stop(animation_sprite_definition *animation);

inline f32 animation_interpolate_f32(f32 from, f32 to, f32 v);
inline u32 animation_interpolate_u32(u32 from, u32 to, f32 v);
inline graphics_point animation_interpolate_point(graphics_point from, graphics_point to, f32 v);

inline f32 animation_duration_value_update(
    animation_duration_value *value,
    f32 delta_time
);
inline void animation_duration_value_reset(animation_duration_value *value);
inline void animation_duration_start(animation_duration_value *value);
inline void animation_duration_stop(animation_duration_value *value);
