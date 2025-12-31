u32 
swap_bytes_u32(u32 n)
{
  return (u32)((0x000000FF & n) << 24) |
         (u32)((0xFF000000 & n) >> 24) |
         (u32)((0x00FF0000 & n) >>  8) |
         (u32)((0x0000FF00 & n) <<  8);
}

u16
swap_bytes_u16(u16 n)
{
  return (n << 8) | ((n & 0xFF00) >> 8);
}

u32 
gcd_u32(u32 x, u32 y)
{
  while (y) {
    x %= y;
    swap_ints(x, y);
  }
  return x;
}
