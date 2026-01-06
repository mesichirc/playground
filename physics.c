static physics_state internal_physics_state;

void 
physics_init(slice memory)
{
  internal_physics_state.bodies = (physics_body*)memory.base;
  internal_physics_state.length = 0;
  internal_physics_state.capacity = memory.capacity / sizeof(physics_body);
}

void 
physics_update(f32 delta_time)
{
  physics_body *body;
  for (u32 i = 0; i < internal_physics_state.length; i+=1) {
    body = internal_physics_state.bodies + i;
    body->velocity.x += body->acceleration.x * delta_time;
    body->velocity.y += body->acceleration.y * delta_time;
    body->aabb.position.x += body->velocity.x * delta_time;
    body->aabb.position.y += body->velocity.y * delta_time;
  }
}

i32
physics_body_create(graphics_vec2 pos, graphics_vec2 size)
{
  physics_body body = {
    .aabb = {
      .position = { pos.x, pos.y },
      .half_size = { size.x * 0.5f, size.y * 0.5f },
    },
    .velocity = { 0, 0 },
  };

  if (internal_physics_state.capacity > internal_physics_state.length) {
    internal_physics_state.bodies[internal_physics_state.length] = body;
    return internal_physics_state.length++;
  }
  return -1;
}

physics_body 
*physics_body_get(u32 index)
{
  return internal_physics_state.bodies + index;
}

