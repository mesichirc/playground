u32 platform_pagesize(void);
void* platform_memory_reserve(u64 size);
b32 platform_memory_commit(void *ptr, u64 size);
b32 platform_memory_decommit(void *ptr, u64 size); 
b32 platform_memory_release(void *ptr, u64 size);

void* platform_read_entire_file(char *path, u64 *size);
