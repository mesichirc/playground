typedef struct physics_aabb physics_aabb;
struct physics_aabb {
  graphics_vec2 position;
  graphics_vec2 half_size;
};

typedef struct physics_body physics_body;
struct physics_body {
  physics_aabb aabb;
  graphics_vec2 velocity;
  graphics_vec2 acceleration;
};

void physics_init(slice memory);
void physics_update(f32 delta_time);
i32 physics_body_create(graphics_vec2 pos, graphics_vec2 size);
physics_body *physics_body_get(u32 index);

typedef struct physics_state physics_state;
struct physics_state {
  physics_body *bodies;
  u32          length;
  u32          capacity;
};
