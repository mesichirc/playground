memory_arena* 
memory_arena_create(u64 reserve_size, u64 commit_size)
{
 u32 pagesize = platform_pagesize();

 reserve_size = ALIGN_UP_POW2(reserve_size, pagesize);
 commit_size = ALIGN_UP_POW2(commit_size, pagesize);

 memory_arena *arena = platform_memory_reserve(reserve_size);
 if (!platform_memory_commit(arena, commit_size)) {
   return nil;
 }

 arena->reserve_size = reserve_size;
 arena->commit_size = commit_size;
 arena->pos = ARENA_BASE_POS;
 arena->commit_pos = commit_size;

 return arena;
}

void
memory_arena_destroy(memory_arena *arena)
{
  platform_memory_release(arena, arena->reserve_size);
}

void* 
memory_arena_push(memory_arena* arena, u64 size, b32 non_zero)
{
  u64 pos_aligned = ALIGN_UP_POW2(arena->pos, ARENA_ALIGN);
  u64 new_pos = pos_aligned + size;

  if (new_pos > arena->reserve_size) { return nil; }

  if (new_pos > arena->commit_pos) {
    u64 new_commit_pos = new_pos;
    new_commit_pos += arena->commit_size - 1;
    new_commit_pos -= new_commit_pos % arena->commit_size;
    new_commit_pos = min(new_commit_pos, arena->reserve_size);
    u8* mem = (u8*)arena + arena->commit_pos;
    u64 commit_size = new_commit_pos - arena->commit_pos;

    if (!platform_memory_commit(mem, commit_size)) {
      return nil;
    }

    arena->commit_pos = new_commit_pos;
  }

  arena->pos = new_pos;
  u8* out = (u8*)arena + pos_aligned;
  if (!non_zero) {
    memset(out, 0, size);
  }

  return out;
}

void 
memory_arena_pop(memory_arena* arena, u64 size)
{
  size = min(size, arena->pos - ARENA_BASE_POS);
  arena->pos -= size;
}

void 
memory_arena_pop_to(memory_arena *arena, u64 pos)
{
  u64 size = pos < arena->pos ? arena->pos - pos : 0;
  memory_arena_pop(arena, size);
}

void 
memory_arena_clear(memory_arena *arena)
{
  memory_arena_pop_to(arena, ARENA_BASE_POS);
}
