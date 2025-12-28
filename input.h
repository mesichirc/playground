typedef u32 input_event;
enum {
  input_event_none,
  input_event_key_pressed,
  input_event_key_released,
  input_event_mouse_btn_pressed,
  input_event_mouse_btn_released,
  input_event_mouse_scroll,
  input_event_mouse_position_changed,
};

typedef u32 input_key_code;
enum {
  input_key_code_null = 0,
  input_key_code_escape = '\033',
  input_key_code_backtick = '`',
  input_key_code0 = '0',
  input_key_code1 = '1',
  input_key_code2 = '2',
  input_key_code3 = '3',
  input_key_code4 = '4',
  input_key_code5 = '5',
  input_key_code6 = '6',
  input_key_code7 = '7',
  input_key_code8 = '8',
  input_key_code9 = '9',
  input_key_code_minus = '-',
	input_key_code_equals = '=',
	input_key_code_back_space = '\b',
	input_key_code_tab = '\t',
	input_key_code_space = ' ',
  input_key_code_a = 'a',
	input_key_code_b = 'b',
	input_key_code_c = 'c',
	input_key_code_d = 'd',
	input_key_code_e = 'e',
	input_key_code_f = 'f',
	input_key_code_g = 'g',
	input_key_code_h = 'h',
	input_key_code_i = 'i',
	input_key_code_j = 'j',
	input_key_code_k = 'k',
	input_key_code_l = 'l',
	input_key_code_m = 'm',
	input_key_code_n = 'n',
	input_key_code_o = 'o',
	input_key_code_p = 'p',
	input_key_code_q = 'q',
	input_key_code_r = 'r',
	input_key_code_s = 's',
	input_key_code_t = 't',
	input_key_code_u = 'u',
	input_key_code_v = 'v',
	input_key_code_w = 'w',
	input_key_code_x = 'x',
	input_key_code_y = 'y',
	input_key_code_z = 'z',
  input_key_code_period = '.',
	input_key_code_comma = ',',
	input_key_code_slash = '/',
	input_key_code_bracket = '[',
  input_key_code_close_bracket = ']',
  input_key_code_semicolon = ';',
	input_key_code_apostrophe = '\'',
	input_key_code_back_slash = '\\',
	input_key_code_return = '\n',
	input_key_code_enter = input_key_code_return,
	input_key_code_delete = '\177', /* 127 */
	input_key_code_f1,
	input_key_code_f2,
	input_key_code_f3,
	input_key_code_f4,
	input_key_code_f5,
	input_key_code_f6,
	input_key_code_f7,
	input_key_code_f8,
	input_key_code_f9,
	input_key_code_f10,
	input_key_code_f11,
	input_key_code_f12,
  input_key_code_f13,
  input_key_code_f14,
  input_key_code_f15,
  input_key_code_f16,
  input_key_code_f17,
  input_key_code_f18,
  input_key_code_f19,
  input_key_code_f20,
  input_key_code_f21,
  input_key_code_f22,
  input_key_code_f23,
  input_key_code_f24,
  input_key_code_f25,
	input_key_code_caps_lock,
	input_key_code_shift_l,
	input_key_code_control_l,
	input_key_code_alt_l,
	input_key_code_super_l,
	input_key_code_shift_r,
	input_key_code_control_r,
	input_key_code_alt_r,
	input_key_code_super_r,
	input_key_code_up,
	input_key_code_down,
	input_key_code_left,
	input_key_code_right,
	input_key_code_insert,
	input_key_code_menu,
	input_key_code_end,
	input_key_code_home,
	input_key_code_page_up,
	input_key_code_page_down,
	input_key_code_num_lock,
	input_key_code_kp_slash,
	input_key_code_kp_multiply,
	input_key_code_kp_plus,
	input_key_code_kp_minus,
	input_key_code_kp_equal,
	input_key_code_kp1,
	input_key_code_kp2,
	input_key_code_kp3,
	input_key_code_kp4,
	input_key_code_kp5,
	input_key_code_kp6,
	input_key_code_kp7,
	input_key_code_kp8,
	input_key_code_kp9,
	input_key_code_kp0,
	input_key_code_kp_period,
	input_key_code_kp_return,
	input_key_code_scroll_lock,
  input_key_code_print_screen,
  input_key_code_pause,
	input_key_code_world1,
  input_key_code_world2,
  input_key_code_last = 256
};


typedef u32 input_flag;
enum {
  input_flag_key_pressed = (1 << input_event_key_pressed),
  input_flag_key_released = (1 << input_event_key_released),
  input_mouse_scroll_flag = (1 << input_event_mouse_scroll),
  input_mouse_btn_pressed_flag = (1 << input_event_mouse_btn_pressed),
  input_mouse_btn_released_flag = (1 << input_event_mouse_btn_released),
  input_mouse_position_changed_flag  = (1 << input_event_mouse_position_changed)

};

typedef struct input_state input_state;
struct input_state {
  u8                  chars[512];
  input_key_code      key_codes[input_key_code_last];
  i32                 mouse_x;
  i32                 mouse_y;
  u16                 chars_length;
  input_flag          flag;
  u8                  mouse_buttons_state[64];
  f32                 delta_x;
  f32                 delta_y;
};

typedef struct input_handle input_handle;
struct input_handle {
  input_state *next;
  input_state *prev;
};
