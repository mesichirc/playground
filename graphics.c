u32 
graphics_color_make(u8 r, u8 g, u8 b, u8 a) 
{
  return (((u32)(a) << 24) & (0xFF000000)) |
         (((u32)(b) << 16) & (0x00FF0000)) |
         (((u32)(g) << 8 ) & (0x0000FF00)) |
         (((u32)(r) << 0 ) & (0x000000FF));
}

u32 
graphics_color_unpack(u32 color, u8 buf[graphics_color_part_comps]) 
{
  buf[graphics_color_part_a] = (u8)((color >> 24) & 0xFF);
  buf[graphics_color_part_b] = (u8)((color >> 16) & 0xFF);
  buf[graphics_color_part_g] = (u8)((color >> 8) & 0xFF);
  buf[graphics_color_part_r] = (u8)((color >> 0) & 0xFF);
}

u8 
graphics_mix_comps(u16 c1, u16 c2, u16 a) 
{
  return c1 + (c2 - c1) * a / 255;
}

u32 
graphics_mix_colors(u32 c1, u32 c2)
{
  u32 result;
  u8 c2a = graphics_color_get_a(c2);
  return graphics_color_make(
    graphics_mix_comps(graphics_color_get_r(c1),  graphics_color_get_r(c2), c2a),
    graphics_mix_comps(graphics_color_get_g(c1),  graphics_color_get_g(c2), c2a),
    graphics_mix_comps(graphics_color_get_b(c1),  graphics_color_get_b(c2), c2a),
    c2a
  );
}

void 
graphics_clip_rect(graphics_rect *rect, graphics_rect clip)
{
  i32 min_x = clip.x;
  i32 max_x = clip.x + clip.w;
  i32 min_y = clip.y;
  i32 max_y = clip.y + clip.h;

  i32 origin_x0 = rect->x;
  rect->x = graphics_clip_numbers(rect->x, min_x, max_x);
  i32 origin_y0 = graphics_clip_numbers(rect->y, min_y, max_y);
  i32 origin_x1 = rect->x + rect->w;
  origin_x1 = graphics_clip_numbers(origin_x1, min_x, max_x);
  i32 origin_y1 = rect->y + rect->h;
  origin_y1 = graphics_clip_numbers(origin_y1, min_y, max_y);

  rect->x = origin_x0;
  rect->w = max(0, origin_x1 - origin_x0);
  rect->y = origin_y0;
  rect->h = max(0, origin_y1 - origin_y0);
}

static void
graphics_draw_rect_opaque(graphics_bitmap *canvas, graphics_rect *rect, u32 color)
{
 u32 stride = canvas->stride;
 u32 scale = canvas->scale;
 u32 y0 = rect->y * scale;
 u32 y1 = (rect->y + rect->h) * scale;
 u32 x0 = rect->x * scale;
 u32 x1 = (rect->x + rect->w) * scale;
 u32 *memory = (u32 *)canvas->memory;
 // TODO: figure out faster alternative for setting ints to array, maybe something like SIMD for cases where stride equals width
 for (u32 y = y0; y < y1; y += 1) {
   for (u32 x = x0; x < x1; x += 1) {
     u32 index = graphics_get_pixel(x, y, stride, scale);
     memory[index] = color;
   } 
 }
}

void 
graphics_draw_rect(graphics_bitmap *canvas, i32 x, i32 y, u32 w, u32 h, u32 color)
{
  graphics_rect rect;
  graphics_rect clip;
  u32 cwidth = canvas->width;
  u32 cheight = canvas->height;
  rect.x = (x < 0 ? cwidth + x : x);
  rect.y = (y < 0 ? cheight + y : y);
  rect.w = w;
  rect.h = h;

  clip.x = 0;
  clip.y = 0;
  clip.w = cwidth;
  clip.h = cheight;

  graphics_clip_rect(&rect, clip);
  if (rect.w == 0 || rect.h == 0) {
    return;
  }
  if (graphics_color_get_a(color) == 255) {
    /** It is faster, to draw opaque rect, then mixing colors */
    graphics_draw_rect_opaque(canvas, &rect, color);
    return;
  }

  u32 stride = canvas->stride;
  u32 scale = canvas->scale;
  u32 y0 = rect.y * scale;
  u32 y1 = (rect.y + rect.h) * scale;
  u32 x0 = rect.x * scale;
  u32 x1 = (rect.x + rect.w) * scale;
  u32 *memory = (u32 *)canvas->memory; 

  u32 c1;
  for (u32 y = y0; y < y1; y += 1) {
    for (u32 x = x0; x < x1; x += 1) {
      u32 pixel = graphics_get_pixel(x, y, stride, scale);
      c1 = memory[pixel];
      memory[pixel] = graphics_mix_colors(c1, color);
    }
  }
}

void 
graphics_clear_canvas(graphics_bitmap *canvas, u32 color)
{
  graphics_draw_rect(canvas, 0, 0, canvas->width, canvas->height, color);
}

void 
graphics_draw_debug_glyph(graphics_bitmap *canvas, i32 x, i32 y, u8 *glyph, u16 w, u16 h, u32 color, u32 size)
{
  int cx, cy, st, pos;
  cx = x;
  cy = y;
  pos = 0;
  u8 scale = (u8)max(size / LETTER_HEIGHT, 1); 
  for (int ly = 0; ly < h; ly += 1) {
    for (int lx = 0; lx < w; lx += 1) {
      u8 b = *(glyph + (pos / 8));
      if ((b >> (u8)(pos % 8)) & 1) {
        graphics_draw_rect(canvas, cx + lx * scale, cy + ly * scale, scale, scale, color);
      }
      pos += 1;
    }
  } 
}

graphics_point 
graphics_draw_debug_string(graphics_bitmap *canvas, i32 x, i32 y, string8 str, u32 color, u32 size)
{
  u32 scale = max(size / LETTER_HEIGHT, 1); 
  u32 step = LETTER_WIDTH * scale + scale;
  for (int i = 0; i < str.size; i += 1) {
    if (str.base[i] == '\n') {
      y += LETTER_HEIGHT;
      continue;
    }
    graphics_draw_debug_glyph(canvas, x, y, (u8 *)getglyph((int)str.base[i]), LETTER_WIDTH, LETTER_HEIGHT, color, size);
    x += step;
  }
  return (graphics_point){ x, y };
}
