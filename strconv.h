#define STRCONV_MAX_U32_LENGTH 10
#define STRCONV_MAX_F32_LENGTH 64

string8 strconv_u32_to_string8(string8 str, u32 n);
string8 strconv_f32_to_string8(string8 str, f32 n, i32 p);
