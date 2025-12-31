typedef struct memory_arena memory_arena;
struct memory_arena {
  u64 pos;
  u64 commit_pos;
  u64 reserve_size;
  u64 commit_size;
};

memory_arena* memory_arena_create(u64 reserve_size, u64 commit_size);
void memory_arena_destroy(memory_arena* arena);
void* memory_arena_push(memory_arena* arena, u64 size, b32 non_zero);
void memory_arena_pop(memory_arena* arena, u64 size);
void memory_arena_pop_to(memory_arena *arena, u64 pos);
void memory_arena_clear(memory_arena *arena);

#define KB(n) ((u64)(n) << 10)
#define MB(n) ((u64)(n) << 20)
#define GB(n) ((u64)(n) << 30)
#define ALIGN_UP_POW2(n, p) (((u64)(n) + ((u64)p - 1)) & (~((u64)p - 1)))
#define ARENA_BASE_POS (sizeof(memory_arena))
#define ARENA_ALIGN (sizeof(void*))
#define PUSH_STRUCT(arena, T) (T*)memory_arena_push((arena), sizeof(T), false)
#define PUSH_STRUCT_NZ(arena, T) (T*)memory_arena_push((arena), sizeof(T), true)

#define PUSH_ARRAY(arena, T, n) (T*)memory_arena_push((arena), sizeof(T) * n, false)
#define PUSH_ARRAY_NZ(arena, T) (T*)memory_arena_push((arena), sizeof(T) * n, true)
