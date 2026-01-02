typedef struct graphics_rect graphics_rect;
struct graphics_rect {
  i32 x;
  i32 y;
  u32 w;
  u32 h;
};

typedef struct graphics_point graphics_point;
struct graphics_point {
  i32 x;
  i32 y;
};

typedef struct graphics_vertex graphics_vertex;
struct graphics_vertex {
  f32 x;
  f32 y;
  u32 color;
};

typedef struct graphics_texture_vertex graphics_texture_vertex;
struct graphics_texture_vertex {
  f32 x;
  f32 y;
  f32 u;
  f32 v;
  u32 color;
};

typedef struct graphics_bitmap graphics_bitmap;
struct graphics_bitmap {
  u8 *memory;
  u32 width;
  u32 height;
  /** Offset to next y position in memory, usefull if need to create bitmap, from parent bitmap, without copying memory */
  u32 stride;
  u8  bytes_per_pixel;
  u8  scale;
};

typedef struct graphics_texture graphics_texture;
struct graphics_texture {
  u8  *memory;
  u32 width;
  u32 height;
};

enum graphics_color_part {
  graphics_color_part_r = 0,
  graphics_color_part_g,
  graphics_color_part_b,
  graphics_color_part_a,
  graphics_color_part_comps,
};

// Functions

inline u32 graphics_color_make(u8 r, u8 g, u8 b, u8 a);
inline u32 graphics_color_unpack(u32 color, u8 buf[graphics_color_part_comps]);
inline u8 graphics_mix_comps(u16 c1, u16 c2, u16 a);
inline u32 graphics_mix_colors(u32 c1, u32 c2);
inline void graphics_clip_rect(graphics_rect *rect, graphics_rect clip);
inline void graphics_draw_rect(graphics_bitmap *canvas, i32 x, i32 y, u32 w, u32 h, u32 color);
inline void graphics_clear_canvas(graphics_bitmap *canvas, u32 color);
void graphics_draw_debug_glyph(graphics_bitmap *canvas, i32 x, i32 y, u8 *glyph, u16 w, u16 h, u32 color, u32 size);
graphics_point graphics_draw_debug_string(graphics_bitmap *canvas, i32 x, i32 y, string8 str, u32 color, u32 size);
void graphics_draw_triangle(graphics_bitmap *canvas, graphics_vertex v0, graphics_vertex v1, graphics_vertex v2);
void graphics_draw_triangle_with_texture(graphics_bitmap *canvas, graphics_texture_vertex v0, graphics_texture_vertex v1, graphics_texture_vertex v2, graphics_texture* texture);

graphics_point graphics_draw_debug_string_rainbow(graphics_bitmap *canvas, i32 x, i32 y, string8 str, u32 color[4], u32 size);
void graphics_draw_debug_glyph_rainbow(graphics_bitmap *canvas, i32 x, i32 y, u8 *glyph, u16 w, u16 h, u32 color[4], u32 size);
inline void graphics_draw_rect_image(graphics_bitmap *canvas, i32 x, i32 y, u32 w, u32 h, graphics_bitmap *bitmap, b32 uniform_scale, b32 right_to_left);
inline graphics_point graphics_measure_debug_string(string8 str, u32 size);

// Macros

#define graphics_color_get_r(c) (u8)(((c) >> 0) & 0xFF)
#define graphics_color_get_g(c) (u8)(((c) >> 8) & 0xFF)
#define graphics_color_get_b(c) (u8)(((c) >> 16) & 0xFF)
#define graphics_color_get_a(c) (u8)(((c) >> 24) & 0xFF)
#define graphics_clip_numbers(c, l, r) ((c) < (l) ? (l) : ((c) > (r) ? (r) : (c)))

#define graphics_color_set_r(c, r) ((c) | (((u32)(r) << 0) & (0x000000FF)))
#define graphics_color_set_g(c, g) ((c) | (((u32)(g) << 8) & (0x0000FF00)))
#define graphics_color_set_b(c, b) ((c) | (((u32)(b) << 16) & (0x00FF0000)))
#define graphics_color_set_a(c, a) ((c) | (((u32)(a) << 24) & (0xFF000000)))
#define graphics_get_pixel(x, y, stride, scale) ((x) + (y) * (stride) * (scale))

