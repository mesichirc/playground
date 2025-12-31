b32
animation_sprite_is_looping(animation_sprite_definition *animation)
{
  return animation->state & animation_sprite_looping;
}

b32
animation_sprite_is_flipped(animation_sprite_definition *animation)
{
  return animation->state & animation_sprite_flipped;
}

b32 
animation_sprite_is_active(animation_sprite_definition *animation)
{
  return (animation->state & animation_sprite_active) != 0;
}

void 
animation_sprite_set_looping(animation_sprite_definition *animation, b32 looping)
{
  if (!looping) {
    animation->state = (animation->state & ~(animation_sprite_looping));
  } else {
    animation->state = (animation->state | (animation_sprite_looping));
  }
}

void 
animation_sprite_set_flipped(animation_sprite_definition *animation, b32 flipped)
{
  if (!flipped) {
    animation->state = animation->state & ~(animation_sprite_flipped);
  } else {
    animation->state = animation->state | (animation_sprite_flipped);
  }
}

void
animation_sprite_set_active(animation_sprite_definition *animation, b32 active)
{
  if (!active) {
    animation->state = (animation->state & ~(animation_sprite_active));
  } else {
    animation->state = animation->state | (animation_sprite_active);
  }
}

void 
animation_sprite_get_bitmap(animation_sprite_definition *animation, graphics_bitmap *out)
{
  u32 index = (u32)animation->current_frame_index;
  u32* memory = (u32*)animation->bitmap->memory;
  u32 row = (u32)animation->rows[index];
  u32 column = (u32)animation->columns[index];
  memory += animation->frame_size * column + row * animation->frame_size * animation->bitmap->stride;
  out->memory = (u8*)memory;
  out->width = animation->frame_size;
  out->height = animation->frame_size;
  out->stride = animation->bitmap->stride;
  out->scale = animation->bitmap->scale;
}

void 
animation_sprite_update(animation_sprite_definition *animation, f32 delta_t)
{
  b32 is_active = animation_sprite_is_active(animation);
  b32 is_looped = animation_sprite_is_looping(animation);
  if (!is_active) {
    return;
  }
  u32 index = animation->current_frame_index;
  animation->current_frame_time += delta_t;
  if (animation->current_frame_time <= animation->durations[index]) {  
    return;
  }

  animation->current_frame_time = 0;

  if (animation->frame_count == animation->current_frame_index + 1) {
    if (!is_looped) {
      animation_sprite_stop(animation);
    } else {
      animation->current_frame_index += 1;
      animation->current_frame_index %= animation->frame_count;
    }
  } else {
    animation->current_frame_index += 1;
  }
}

void 
animation_sprite_start(animation_sprite_definition *animation)
{
  animation->current_frame_time = 0;
  animation->current_frame_index = 0;
  animation_sprite_set_active(animation, true);
}

void 
animation_sprite_stop(animation_sprite_definition *animation)
{
  animation_sprite_set_active(animation, false);
}

f32
animation_interpolate_f32(f32 from, f32 to, f32 v)
{
  return from + (to - from) * v;
}

u32
animation_interpolate_u32(u32 from, u32 to, f32 v)
{
  return (u32)animation_interpolate_f32((f32)from, (f32)to, v);
}

graphics_point
animation_interpolate_point(graphics_point from, graphics_point to, f32 v)
{
  return (graphics_point) {
    (i32)animation_interpolate_u32(from.x, to.x, v),
    (i32)animation_interpolate_u32(from.y, to.y, v)
  };
}

f32
animation_duration_value_update(
    animation_duration_value *value,
    f32 delta_time
)
{
  if (!value->is_running) {
    return value->value;
  }
  value->time += delta_time;
  value->value = value->time / value->duration;
  value->value = max(min(value->value, 1.0), 0.0);
  if (value->value == 1.0) {
    value->is_running = false;
  }
  return value->value;
}

void 
animation_duration_value_reset(animation_duration_value *value)
{
  value->time = 0.0f;
  value->value = 0.0f;
}

void 
animation_duration_start(animation_duration_value *value)
{
  value->is_running = true;
}

void animation_duration_stop(animation_duration_value *value)
{
  value->is_running = false;
}
