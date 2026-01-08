u32 platform_pagesize(void);
void* platform_memory_reserve(u64 size);
b32 platform_memory_commit(void *ptr, u64 size);
b32 platform_memory_decommit(void *ptr, u64 size); 
b32 platform_memory_release(void *ptr, u64 size);

void* platform_read_entire_file(char *path, u64 *size);

void platform_print(string8 str);
inline void platform_memcpy(void *dest, void *source, u64 size);
inline void platform_memset(void *ptr, u8 placeholder, u64 size);

void *platform_load_image(void *mem, u64 size, i32 *w, i32 *h, i32 *channels, i32 desired_channels);

inline i32 floor_f32_to_i32(f32 n);
