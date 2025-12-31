inline b32 is_ppm_whitespace(u8 c);
inline b32 is_ppm_number(u8 c);

b32 ppm_load_bitmap(
    graphics_bitmap* bitmap,
    memory_arena* arena, 
    void *filecontent, u32 filesize
);

