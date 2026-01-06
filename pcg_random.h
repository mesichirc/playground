
typedef struct pcg_random_state32 pcg_random_state32;
struct pcg_random_state32 {
  u64 state; // RNG state
  u64 inc;   // Controls which RNG seq is selected. Must be odd
};

#define PCG32_INITIALIZER { (u64)0x853c49e6748fea9bULL, (u64)0xda3e39cb94b95bdbULL }

void pcg_random_seed_u32(u64 seed, u64 initseq);
void pcg_random_seed_rng_u32(pcg_random_state32 *rng, u64 initstate, u64 initseq);

u32 pcg_random_u32(void);
u32 pcg_random_rng_u32(pcg_random_state32 *rng);

u32 pcg_random_bounded_u32(u32 bound);
u32 pcg_random_bounded_rng_u32(pcg_random_state32 *rng, u32 bound);
