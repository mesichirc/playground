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

  return color;
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
  rect->x = max(0, rect->x);
  rect->y = max(0, rect->y);

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

void
graphics_draw_rect_opaque(graphics_bitmap *canvas, graphics_rect *rect, u32 color)
{
  u32 scale = canvas->scale;
  u32 stride = canvas->stride * scale;
  u32 y0 = max(rect->y * scale, 0);
  u32 y1 = (rect->y + rect->h) * scale;
  u32 x0 = max(rect->x * scale, 0);
  u32 x1 = (rect->x + rect->w) * scale;
  u32 *memory = (u32 *)canvas->memory;
  for (u32 x = x0; x < x1; x += 1) {
    u32 index = graphics_get_pixel(x, y0, stride, 1);
    memory[index] = color;
  }
  u32 size = (x1 - x0) * 4;
  u32 *from = memory + x0 + y0 * stride;
  u32 *dest = memory + x0 + y0 * stride; 
  for (u32 y = y0 + 1; y < y1; y += 1) {
    dest += stride;
    memcpy(dest, from, size);
  }
}

void 
graphics_draw_rect(graphics_bitmap *canvas, i32 x, i32 y, u32 w, u32 h, u32 color)
{
  graphics_rect rect;
  graphics_rect clip;
  u32 cwidth = canvas->width;
  u32 cheight = canvas->height;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  clip.x = 0;
  clip.y = 0;
  clip.w = cwidth;
  clip.h = cheight;

  if (x < 0) {
    rect.w += x;
  }
  if (y < 0) {
    rect.h += y;
  }

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

void 
graphics_draw_debug_glyph_rainbow(graphics_bitmap *canvas, i32 x, i32 y, u8 *glyph, u16 w, u16 h, u32 color[4], u32 size)
{
  int cx, cy, st, pos;
  cx = x;
  cy = y;
  pos = 0;
  u8 scale = (u8)max(size / LETTER_HEIGHT, 1); 
  graphics_vertex v0 = {0};
  graphics_vertex v1 = {0};
  graphics_vertex v2 = {0};
  graphics_vertex v3 = {0};

  v0.color = color[0];
  v1.color = color[1];
  v2.color = color[2];
  v3.color = color[3];

  for (int ly = 0; ly < h; ly += 1) {
    for (int lx = 0; lx < w; lx += 1) {
      u8 b = *(glyph + (pos / 8));
      if ((b >> (u8)(pos % 8)) & 1) {
        v0.x = cx + lx * scale;
        v0.y = cy + ly * scale;
        v1.x = cx + lx * scale + scale;
        v1.y = v0.y;
        v2.x = cx + lx * scale + scale;
        v2.y = cy + ly * scale + scale;
        v3.x = v0.x;
        v3.y = v2.y;
        graphics_draw_triangle(canvas, v0, v1, v2);
        graphics_draw_triangle(canvas, v2, v3, v0);
      }
      pos += 1;
    }
  }
}


graphics_point 
graphics_measure_debug_string(string8 str, u32 size)
{
  u32 scale = max(size / LETTER_HEIGHT, 1); 
  return (graphics_point) { scale * LETTER_WIDTH * str.size, scale  * LETTER_HEIGHT };
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
    if (str.base[i] == ' ') {
      x += step;
      continue;
    }
    graphics_draw_debug_glyph(canvas, x, y, (u8 *)getglyph((int)str.base[i]), LETTER_WIDTH, LETTER_HEIGHT, color, size);
    x += step;
  }
  return (graphics_point){ x, y };
}


graphics_point
graphics_draw_debug_string_rainbow(graphics_bitmap *canvas, i32 x, i32 y, string8 str, u32 color[4], u32 size)
{
  u32 scale = max(size / LETTER_HEIGHT, 1); 
  u32 step = LETTER_WIDTH * scale + scale;
  for (int i = 0; i < str.size; i += 1) {
    if (str.base[i] == '\n') {
      y += LETTER_HEIGHT;
      continue;
    }
    graphics_draw_debug_glyph_rainbow(canvas, x, y, (u8 *)getglyph((int)str.base[i]), LETTER_WIDTH, LETTER_HEIGHT, color, size);
    x += step;
  }
  return (graphics_point){ x, y + LETTER_WIDTH * scale };
}

f32
graphics_edge_function(f32 x0, f32 y0, f32 x1, f32 y1, f32 px, f32 py)
{
  return (px - x0) * (y1 - y0) - (py - y0) * (x1 - x0);
}

void
graphics_draw_triangle_with_texture(graphics_bitmap *canvas, graphics_texture_vertex v0, graphics_texture_vertex v1, graphics_texture_vertex v2, graphics_texture *texture)
{
  u32 stride = canvas->stride;
  u32 scale  = canvas->scale;
  u32 *memory = (u32 *)canvas->memory;
  i32 minx = min(v0.x, min(v1.x, v2.x));
  i32 miny = min(v0.y, min(v1.y, v2.y));
  i32 maxx = max(v0.x, max(v1.x, v2.x));
  i32 maxy = max(v0.y, max(v1.y, v2.y));
  f32 area =
    (v1.x - v0.x) * (v2.y - v0.y) -
    (v1.y - v0.y) * (v2.x - v0.x);
  graphics_texture_vertex tmp;
  if (area > 0) {
    tmp = v1;
    v1 = v2;
    v2 = tmp;
  }
  
  f32 px, py;
  u32 color;
  f32 al0 = graphics_edge_function(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
  f32 al1 = graphics_edge_function(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);
  u8 r0 = graphics_color_get_r(v0.color);
  u8 g0 = graphics_color_get_g(v0.color);
  u8 b0 = graphics_color_get_b(v0.color);
  u8 a0 = graphics_color_get_a(v0.color);

  u8 r1 = graphics_color_get_r(v1.color);
  u8 g1 = graphics_color_get_g(v1.color);
  u8 b1 = graphics_color_get_b(v1.color);
  u8 a1 = graphics_color_get_a(v1.color);

  u8 r2 = graphics_color_get_r(v2.color);
  u8 g2 = graphics_color_get_g(v2.color);
  u8 b2 = graphics_color_get_b(v2.color);
  u8 a2 = graphics_color_get_a(v2.color);

  f32 dx_v12 = v2.x - v1.x;
  f32 dy_v12 = v2.y - v1.y;
  f32 dx_v20 = v0.x - v2.x;
  f32 dy_v20 = v0.y - v2.y;
  f32 dx_v01 = v1.x - v0.x;
  f32 dy_v01 = v1.y - v0.y;

  f32 w0start = (f32)minx * dy_v12 - v1.x * dy_v12 - (f32)miny * dx_v12 + v1.y * dx_v12;
  f32 w1start = ((f32)minx - v2.x) * dy_v20 - ((f32)miny - v2.y) * dx_v20;
  f32 w2start = ((f32)minx - v0.x) * dy_v01 - ((f32)miny - v0.y) * dx_v01;
  f32 alpha_delta = a0 == 0 ? 0 : 1 / al0;
  f32 beta_delta = al1 == 0 ? 0 : 1 / al1;
  f32 alpha_delta_x = dy_v12 * alpha_delta;
  f32 alpha_delta_y = (-dx_v12) * alpha_delta;
  f32 alpha_start = w0start * alpha_delta;
  f32 beta_delta_x = dy_v20 * beta_delta;
  f32 beta_delta_y = (-dx_v20) * beta_delta;
  f32 beta_start = w1start * beta_delta;

  f32 gamma_start = (1.0f - alpha_start - beta_start);
  f32 gamma_delta_x = - alpha_delta_x - beta_delta_x;
  f32 gamma_delta_y = - alpha_delta_y - beta_delta_y;

  f32 r0start = alpha_start * (f32)r0; 
  f32 r1start = beta_start * (f32)r1; 
  f32 r2start = gamma_start * (f32)r2;

  f32 g0start = alpha_start * g0; 
  f32 g1start = beta_start * g1; 
  f32 g2start = gamma_start * g2;

  f32 b0start = alpha_start * b0; 
  f32 b1start = beta_start * b1; 
  f32 b2start = gamma_start * b2;

  f32 a0start = alpha_start * a0; 
  f32 a1start = beta_start * a1; 
  f32 a2start = gamma_start * a2;
  f32 rr0, rr1, rr2, gg0, gg1, gg2, bb0, bb1, bb2, aa0, aa1, aa2;
  f32 r0_delta_x = r0 * alpha_delta_x;
  f32 r0_delta_y = r0 * alpha_delta_y;
  f32 r1_delta_x = r1 * beta_delta_x;
  f32 r1_delta_y = r1 * beta_delta_y;
  f32 r2_delta_x = r2 * gamma_delta_x;
  f32 r2_delta_y = r2 * gamma_delta_y;

  f32 g0_delta_x = g0 * alpha_delta_x;
  f32 g0_delta_y = g0 * alpha_delta_y;
  f32 g1_delta_x = g1 * beta_delta_x;
  f32 g1_delta_y = g1 * beta_delta_y;
  f32 g2_delta_x = g2 * gamma_delta_x;
  f32 g2_delta_y = g2 * gamma_delta_y;


  f32 b0_delta_x = b0 * alpha_delta_x;
  f32 b0_delta_y = b0 * alpha_delta_y;
  f32 b1_delta_x = b1 * beta_delta_x;
  f32 b1_delta_y = b1 * beta_delta_y;
  f32 b2_delta_x = b2 * gamma_delta_x;
  f32 b2_delta_y = b2 * gamma_delta_y;

  f32 a0_delta_x = a0 * alpha_delta_x;
  f32 a0_delta_y = a0 * alpha_delta_y;
  f32 a1_delta_x = a1 * beta_delta_x;
  f32 a1_delta_y = a1 * beta_delta_y;
  f32 a2_delta_x = a2 * gamma_delta_x;
  f32 a2_delta_y = a2 * gamma_delta_y;

  f32 ustart = alpha_start * v0.u + beta_start * v1.u + gamma_start * v2.u;
  f32 tx_start = (ustart * (f32)texture->width);
  f32 tx_delta_x = ((alpha_delta_x * v0.u + beta_delta_x * v1.u + gamma_delta_x * v2.u) * (f32)texture->width);
  f32 tx_delta_y = ((alpha_delta_y * v0.u + beta_delta_y * v1.u + gamma_delta_y * v2.u) * (f32)texture->width);
  f32 tx;
  f32 vstart = alpha_start * v0.v + beta_start * v1.v + gamma_start * v2.v;
  f32 ty_start = (vstart * (f32)texture->height);
  f32 ty_delta_x = ((alpha_delta_x * v0.v + beta_delta_x * v1.v + gamma_delta_x * v2.v) * (f32)texture->height);
  f32 ty_delta_y = ((alpha_delta_y * v0.v + beta_delta_y * v1.v + gamma_delta_y * v2.v) * (f32)texture->height);
  f32 ty;

  for (i32 y = miny; y <= maxy; y += 1) {
    tx = tx_start;
    ty = ty_start;
    f32 w0 = w0start;
    f32 w1 = w1start;
    f32 w2 = w2start;
    f32 alpha = alpha_start;
    f32 beta = beta_start;
    f32 gamma = gamma_start; 
    rr0 = r0start;
    rr1 = r1start;
    rr2 = r2start;

    gg0 = g0start;
    gg1 = g1start;
    gg2 = g2start;

    bb0 = b0start;
    bb1 = b1start;
    bb2 = b2start;

    aa0 = a0start;
    aa1 = a1start;
    aa2 = a2start;

    for (i32 x = minx; x <= maxx; x += 1) {
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        u32 pixel = graphics_get_pixel(x, y, stride, scale);
        color = memory[pixel];
        u8 vert_alpha = (u8)(aa0 + aa1 + aa2);
        u32 color1 = graphics_color_make(
          rr0 + rr1 + rr2,
          gg0 + gg1 + gg2,
          bb0 + bb1 + bb2,
          vert_alpha
        );

        u32 texture_color = ((u32*)texture->memory)[graphics_get_pixel((u32)tx % texture->width, (u32)ty % texture->height, texture->width, 1)];
        color1 = graphics_mix_colors(color1, texture_color);
        color1 = graphics_color_set_a(color1, vert_alpha);
        memory[pixel] = graphics_mix_colors(color, color1);
      }
      w0 += dy_v12;
      w1 += dy_v20;
      w2 += dy_v01;
      alpha += alpha_delta_x;
      beta += beta_delta_x;
      gamma += gamma_delta_x;
      rr0 += r0_delta_x;
      rr1 += r1_delta_x;
      rr2 += r2_delta_x;
      gg0 += g0_delta_x;
      gg1 += g1_delta_x;
      gg2 += g2_delta_x;
      bb0 += b0_delta_x;
      bb1 += b1_delta_x;
      bb2 += b2_delta_x;
      aa0 += a0_delta_x;
      aa1 += a1_delta_x;
      aa2 += a2_delta_x;
      tx += tx_delta_x;
      ty += ty_delta_x;
    }
    w0start -= dx_v12;
    w1start -= dx_v20;
    w2start -= dx_v01;
    alpha_start += alpha_delta_y;
    beta_start += beta_delta_y;
    gamma_start += gamma_delta_y;
    r0start += r0_delta_y;
    r1start += r1_delta_y;
    r2start += r2_delta_y;
    g0start += g0_delta_y;
    g1start += g1_delta_y;
    g2start += g2_delta_y;
    b0start += b0_delta_y;
    b1start += b1_delta_y;
    b2start += b2_delta_y;
    a0start += a0_delta_y;
    a1start += a1_delta_y;
    a2start += a2_delta_y;
    tx_start += tx_delta_y;
    ty_start += ty_delta_y;
  }
}

void 
graphics_draw_triangle(graphics_bitmap *canvas, graphics_vertex v0, graphics_vertex v1, graphics_vertex v2)
{
  u32 stride = canvas->stride;
  u32 scale  = canvas->scale;
  u32 *memory = (u32 *)canvas->memory;
  i32 minx = min(v0.x, min(v1.x, v2.x));
  i32 miny = min(v0.y, min(v1.y, v2.y));
  i32 maxx = max(v0.x, max(v1.x, v2.x));
  i32 maxy = max(v0.y, max(v1.y, v2.y));
  f32 area =
    (v1.x - v0.x) * (v2.y - v0.y) -
    (v1.y - v0.y) * (v2.x - v0.x);
  graphics_vertex tmp;
  if (area > 0) {
    tmp = v1;
    v1 = v2;
    v2 = tmp;
  }
  
  f32 px, py;
  u32 color;
  f32 al0 = graphics_edge_function(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
  f32 al1 = graphics_edge_function(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);
  u8 r0 = graphics_color_get_r(v0.color);
  u8 g0 = graphics_color_get_g(v0.color);
  u8 b0 = graphics_color_get_b(v0.color);
  u8 a0 = graphics_color_get_a(v0.color);

  u8 r1 = graphics_color_get_r(v1.color);
  u8 g1 = graphics_color_get_g(v1.color);
  u8 b1 = graphics_color_get_b(v1.color);
  u8 a1 = graphics_color_get_a(v1.color);

  u8 r2 = graphics_color_get_r(v2.color);
  u8 g2 = graphics_color_get_g(v2.color);
  u8 b2 = graphics_color_get_b(v2.color);
  u8 a2 = graphics_color_get_a(v2.color);

  f32 dx_v12 = v2.x - v1.x;
  f32 dy_v12 = v2.y - v1.y;
  f32 dx_v20 = v0.x - v2.x;
  f32 dy_v20 = v0.y - v2.y;
  f32 dx_v01 = v1.x - v0.x;
  f32 dy_v01 = v1.y - v0.y;

  f32 w0start = (f32)minx * dy_v12 - v1.x * dy_v12 - (f32)miny * dx_v12 + v1.y * dx_v12;
  f32 w1start = ((f32)minx - v2.x) * dy_v20 - ((f32)miny - v2.y) * dx_v20;
  f32 w2start = ((f32)minx - v0.x) * dy_v01 - ((f32)miny - v0.y) * dx_v01;
  f32 alpha_delta = a0 == 0 ? 0 : 1 / al0;
  f32 beta_delta = al1 == 0 ? 0 : 1 / al1;
  f32 alpha_delta_x = dy_v12 * alpha_delta;
  f32 alpha_delta_y = (-dx_v12) * alpha_delta;
  f32 alpha_start = w0start * alpha_delta;
  f32 beta_delta_x = dy_v20 * beta_delta;
  f32 beta_delta_y = (-dx_v20) * beta_delta;
  f32 beta_start = w1start * beta_delta;

  f32 gamma_start = (1.0f - alpha_start - beta_start);
  f32 gamma_delta_x = - alpha_delta_x - beta_delta_x;
  f32 gamma_delta_y = - alpha_delta_y - beta_delta_y;

  f32 r0start = alpha_start * (f32)r0; 
  f32 r1start = beta_start * (f32)r1; 
  f32 r2start = gamma_start * (f32)r2;

  f32 g0start = alpha_start * g0; 
  f32 g1start = beta_start * g1; 
  f32 g2start = gamma_start * g2;

  f32 b0start = alpha_start * b0; 
  f32 b1start = beta_start * b1; 
  f32 b2start = gamma_start * b2;

  f32 a0start = alpha_start * a0; 
  f32 a1start = beta_start * a1; 
  f32 a2start = gamma_start * a2;
  f32 rr0, rr1, rr2, gg0, gg1, gg2, bb0, bb1, bb2, aa0, aa1, aa2;
  f32 r0_delta_x = r0 * alpha_delta_x;
  f32 r0_delta_y = r0 * alpha_delta_y;
  f32 r1_delta_x = r1 * beta_delta_x;
  f32 r1_delta_y = r1 * beta_delta_y;
  f32 r2_delta_x = r2 * gamma_delta_x;
  f32 r2_delta_y = r2 * gamma_delta_y;

  f32 g0_delta_x = g0 * alpha_delta_x;
  f32 g0_delta_y = g0 * alpha_delta_y;
  f32 g1_delta_x = g1 * beta_delta_x;
  f32 g1_delta_y = g1 * beta_delta_y;
  f32 g2_delta_x = g2 * gamma_delta_x;
  f32 g2_delta_y = g2 * gamma_delta_y;


  f32 b0_delta_x = b0 * alpha_delta_x;
  f32 b0_delta_y = b0 * alpha_delta_y;
  f32 b1_delta_x = b1 * beta_delta_x;
  f32 b1_delta_y = b1 * beta_delta_y;
  f32 b2_delta_x = b2 * gamma_delta_x;
  f32 b2_delta_y = b2 * gamma_delta_y;

  f32 a0_delta_x = a0 * alpha_delta_x;
  f32 a0_delta_y = a0 * alpha_delta_y;
  f32 a1_delta_x = a1 * beta_delta_x;
  f32 a1_delta_y = a1 * beta_delta_y;
  f32 a2_delta_x = a2 * gamma_delta_x;
  f32 a2_delta_y = a2 * gamma_delta_y;


  for (i32 y = miny; y <= maxy; y += 1) {
    f32 w0 = w0start;
    f32 w1 = w1start;
    f32 w2 = w2start;
    f32 alpha = alpha_start;
    f32 beta = beta_start;
    f32 gamma = gamma_start; 
    rr0 = r0start;
    rr1 = r1start;
    rr2 = r2start;

    gg0 = g0start;
    gg1 = g1start;
    gg2 = g2start;

    bb0 = b0start;
    bb1 = b1start;
    bb2 = b2start;

    aa0 = a0start;
    aa1 = a1start;
    aa2 = a2start;

    for (i32 x = minx; x <= maxx; x += 1) {
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        u32 pixel = graphics_get_pixel(x, y, stride, scale);
        color = memory[pixel];
        u32 color1 = graphics_color_make(
          rr0 + rr1 + rr2,
          gg0 + gg1 + gg2,
          bb0 + bb1 + bb2,
          aa0 + aa1 + aa2
        );
        memory[pixel] = graphics_mix_colors(color, color1);
      }
      w0 += dy_v12;
      w1 += dy_v20;
      w2 += dy_v01;
      alpha += alpha_delta_x;
      beta += beta_delta_x;
      gamma += gamma_delta_x;
      rr0 += r0_delta_x;
      rr1 += r1_delta_x;
      rr2 += r2_delta_x;
      gg0 += g0_delta_x;
      gg1 += g1_delta_x;
      gg2 += g2_delta_x;
      bb0 += b0_delta_x;
      bb1 += b1_delta_x;
      bb2 += b2_delta_x;
      aa0 += a0_delta_x;
      aa1 += a1_delta_x;
      aa2 += a2_delta_x;
    }
    w0start -= dx_v12;
    w1start -= dx_v20;
    w2start -= dx_v01;
    alpha_start += alpha_delta_y;
    beta_start += beta_delta_y;
    gamma_start += gamma_delta_y;
    r0start += r0_delta_y;
    r1start += r1_delta_y;
    r2start += r2_delta_y;
    g0start += g0_delta_y;
    g1start += g1_delta_y;
    g2start += g2_delta_y;
    b0start += b0_delta_y;
    b1start += b1_delta_y;
    b2start += b2_delta_y;
    a0start += a0_delta_y;
    a1start += a1_delta_y;
    a2start += a2_delta_y;
  }
}

void
graphics_print_rect(graphics_rect rect)
{
  printf("GRect{x=%d, y=%d, width=%d, height=%d}\n", rect.x, rect.y, rect.w, rect.h);
}

void 
graphics_draw_rect_image(
    graphics_bitmap *canvas, 
    i32 x, i32 y, u32 w, u32 h, 
    graphics_bitmap *bitmap,
    b32 uniform_scale,
    b32 right_to_left
)
{
  f32 aspect_ratio = (f32)bitmap->width / (f32)bitmap->height;
  u32 target_width;
  u32 target_height;


  if ((w != bitmap->width || h != bitmap->height) && uniform_scale) {
    f32 scaling_factor = max((f32)w / (f32)bitmap->width, (f32)h/(f32)bitmap->height);
    w = (u32)((f32)bitmap->width * scaling_factor);
    h = (u32)((f32)bitmap->height * scaling_factor); 
  }  

  graphics_rect rect;
  graphics_rect clip;
  u32 cwidth = canvas->width;
  u32 cheight = canvas->height;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  clip.x = 0;
  clip.y = 0;
  clip.w = cwidth;
  clip.h = cheight;

  if (x < 0) {
    rect.w += x;
  }
  if (y < 0) {
    rect.h += y;
  }

  graphics_clip_rect(&rect, clip);
  graphics_print_rect(rect);
  if (rect.w == 0 || rect.h == 0) {
    return;
  }
  u32 stride = canvas->stride;
  u32 scale = canvas->scale;
  i32 y0 = rect.y * scale;
  i32 y1 = (rect.y + rect.h) * scale;
  i32 x0 = rect.x * scale;
  i32 x1 = (rect.x + rect.w) * scale;
  u32 *memory = (u32 *)canvas->memory; 
  u32 *texture_mem = (u32 *)bitmap->memory;
  i32 orig_x = x;
  i32 orig_y = y;

  for (i32 y = y0; y < y1; y += 1) {
    for (i32 x = x0; x < x1; x += 1) {
      i32 x2 = x - x0;
      i32 texture_x = (x - orig_x) * bitmap->width / w / scale;
      i32 texture_y = (y - orig_y) * bitmap->height / h / scale;
      u32 pixel = graphics_get_pixel(x, y, stride, scale);
      u32 prev_color = memory[pixel];
      u32 texture_color = texture_mem[graphics_get_pixel(right_to_left ? bitmap->width - texture_x : texture_x, texture_y, bitmap->stride, 1)];
      memory[pixel] = graphics_mix_colors(prev_color, texture_color);
    }
  }
}
