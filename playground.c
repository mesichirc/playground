void 
playground_init(u64 ctime, u32 refres_rate, u32 target_fps)
{
  if (IS_BE()) {
    return;
  }
  for (u32 i = 0; i < playground_color_size; i += 1) {
    playground_colors[i] = swap_bytes_u32(playground_colors[i]);
  }
}

static u8 debug_string[256];

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
  graphics_bitmap canvas;
  canvas.memory = buffer;
  canvas.width = width;
  canvas.height = height;
  canvas.bytes_per_pixel = 4;
  canvas.stride = width;
  canvas.scale = 1;

  graphics_clear_canvas(&canvas, playground_colors[playground_color_base]);  
  /** Debug text */
  {
    graphics_point point = graphics_draw_debug_string(&canvas, 8, 8, STR8_LIT("fps:"), playground_colors[playground_color_pine], 32);
    string8 fps_string = (string8) { debug_string, sizeof(debug_string) };
    fps_string = strconv_f32_to_string8(fps_string, fps, 6);
    graphics_draw_debug_string(&canvas, point.x, point.y, fps_string, playground_colors[playground_color_pine], 32);
  }


}
