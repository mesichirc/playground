b32 
is_ppm_whitespace(u8 c)
{
  return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}
b32 
is_ppm_number(u8 c)
{
  return (c >= '0' && c <= '9');
}

b32 
ppm_load_bitmap(graphics_bitmap *bitmap, memory_arena* arena, void *filecontent, u32 filesize)
{
  u32 i = 0;
  u32 start;
  u32 w, h, m;
  u8* in = filecontent;
  // Read magic header
  if (in[i++] != 'P' || (in[i] != '6' && in[i] != '3')) {
    return false; 
  }
  i += 1;
  // Read all whitespaces
  while (is_ppm_whitespace(in[i])) {
    i += 1;
  }

  // Read all comments
  while (in[i] == '#') {
    while (in[i] != '\n') {
      i += 1;
    }
  }
 
  // Read width
  {
    start = i;
    if (!is_ppm_number(in[start])) {
      return false;
    }
    while (is_ppm_number(in[i])) {
      i += 1;
    }
    while (start < i) {
      w = w * 10 + (u32)(in[start] - '0');
      start += 1;
    }
  }
  // Read all whitespaces
  while (is_ppm_whitespace(in[i])) {
    i += 1;
  }

  // Read all comments
  while (in[i] == '#') {
    while (in[i] != '\n') {
      i += 1;
    }
  }

  // Read height
  {
    start = i;
    if (!is_ppm_number(in[start])) {
      return false;
    }
    while (is_ppm_number(in[i])) {
      i += 1;
    } 
    while (start < i) {
      h = h * 10 + (u32)(in[start] - '0');
      start += 1;
    }
  }

  // Read all whitespaces
  while (is_ppm_whitespace(in[i])) {
    i += 1;
  }

  // Read all comments
  while (in[i] == '#') {
    while (in[i] != '\n') { i += 1; }
  }

  // Read maxval
  {
    start = i;
    if (!is_ppm_number(in[start])) {
      return false;
    }
    while (is_ppm_number(in[i])) {
      i += 1;
    }    
    while (start < i) {
      m = m * 10 + (u32)(in[start] - '0');
      start += 1;
    }
  }

  // Read all whitespaces
  while (is_ppm_whitespace(in[i])) { i += 1; }

  // Read all comments
  while (in[i] == '#') {
    while (in[i] != '\n') { i+= 1; }
  }

  u8* out = PUSH_ARRAY(arena, u8, w * h * 4);
  bitmap->memory = out;
  bitmap->width = w;
  bitmap->height = h;
  bitmap->bytes_per_pixel = 4;
  bitmap->stride = w;

  if (m > 255) {
    // 16 bytes per channel not supported for now
    return false;
  }

  // Read all whitespaces
  while (is_ppm_whitespace(in[i]) && i < filesize) { i += 1; }
  // Read all comments
  while (in[i] == '#') {
    while (in[i] != '\n' && i < filesize) { 
      i+= 1;
    }
  }

  for (u32 y = 0; y < h; y += 1) {
    for (u32 x = 0; x < w; x += 1) {
      *out++ = (u8)(in[i++] - '0');
      if (((out - bitmap->memory) + 1) % 3 == 0) {
        *out++ = 255;
      }
      // Read all whitespaces
      while (i < filesize && is_ppm_whitespace(in[i])) { i += 1; }
      // Read all comments
      while (i < filesize && in[i] == '#') {
        while (i < filesize && in[i] != '\n') { i+= 1; }
      }
    }
  }

  return true;
}

