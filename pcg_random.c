__thread static pcg_random_state32 pcg_global = PCG32_INITIALIZER;

void 
pcg_random_seed_u32(u64 seed, u64 initseq)
{
  pcg_random_seed_rng_u32(&pcg_global, seed, initseq);
}

void
pcg_random_seed_rng_u32(pcg_random_state32 *rng, u64 initstate, u64 initseq)
{
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_random_rng_u32(rng);
  rng->state += initstate;
  pcg_random_rng_u32(rng);
}

u32 
pcg_random_u32(void)
{
  return pcg_random_rng_u32(&pcg_global);
}

u32 
pcg_random_rng_u32(pcg_random_state32 *rng)
{
  u64 oldstate = rng->state;
  rng->state = oldstate * 6364136223846793005ULL + rng->inc;
  u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  u32 rot = (u32)((u64)oldstate >> (u64)59u);
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

u32 
pcg_random_bounded_u32(u32 bound)
{
  return pcg_random_bounded_rng_u32(&pcg_global, bound);
}

u32 
pcg_random_bounded_rng_u32(pcg_random_state32 *rng, u32 bound)
{
  u32 threshold = -bound % bound;
  for (;;) {
    u32 r = pcg_random_rng_u32(rng);
    if (r >= threshold)
      return r % bound;
  }
}
