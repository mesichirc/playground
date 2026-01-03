static void
strconv_u32_to_buf(u8* buf, u64 *size, u32 n)
{
  if (*size == 0) {
    return;
  }
  i32 offset = STRCONV_MAX_U32_LENGTH - 1;
  u8 maxbuf[STRCONV_MAX_U32_LENGTH] = {0};
  u32 rest = n;

  if (n == 0) {
    buf[0] = '0';
    *size = 1;
    return;
  }

  while (rest > 0) {
    maxbuf[offset] = '0' + (u8)(rest % 10);
    rest /= 10;
    offset -= 1;
  }

  *size = min((STRCONV_MAX_U32_LENGTH - 1 - offset), *size);
  platform_memcpy(buf, maxbuf + offset + 1, *size);
}

string8 
strconv_u32_to_string8(string8 str, u32 n)
{
  strconv_u32_to_buf(str.base, &str.size, n);
  return str;
}

string8
strconv_f32_to_string8(string8 str, f32 n, i32 p)
{
  u8 maxbuf[STRCONV_MAX_F32_LENGTH] = {0};
  u64 size = min(STRCONV_MAX_F32_LENGTH, str.size);
  u64 offset = 0;
  if (n < 0) {
    maxbuf[0] = '-';
    offset += 1;
    n = -n;
  }
  u32 ipart = (u32)n;
  u64 ipart_size = size - offset;
  strconv_u32_to_buf(maxbuf + offset, &ipart_size, ipart);
  offset += ipart_size;
  
  maxbuf[offset] = '.';
  offset += 1;

  f32 fpart = n - (f32)ipart;
  while (p && fpart > 0.0) {
    fpart *= 10;
    ipart = (u32)fpart;
    maxbuf[offset] = '0' + (u8)ipart;
    offset += 1;

    fpart = fpart - (f32)ipart;
    p -= 1;
  }
  str.size = offset;
  platform_memcpy(str.base, maxbuf, str.size);
  return str;
}
