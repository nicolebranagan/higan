//bppu_render.cpp
int  _screen_width;
bool _interlace;
int  _interlace_field;

inline void render_line_mode0();
inline void render_line_mode1();
inline void render_line_mode2();
inline void render_line_mode3();
inline void render_line_mode4();
inline void render_line_mode5();
inline void render_line_mode6();
inline void render_line_mode7();

//bppu_render_cache.cpp
enum { COLORDEPTH_4 = 0, COLORDEPTH_16 = 1, COLORDEPTH_256 = 2 };
enum { TILE_2BIT = 0, TILE_4BIT = 1, TILE_8BIT = 2 };

enum { PC_BG1 = 0x80, PC_BG2 = 0x81, PC_BG3 = 0x82, PC_BG4 = 0x83, PC_OAM = 0x84, PC_BACK = 0x00 };
struct _pixel {
//palette # index for main/subscreen pixels
//0 = transparent / use palette color # 0
uint8 src_main, src_sub;
//indicates source of palette # for main/subscreen (BG1-4, OAM, or back)
uint8 bg_main,  bg_sub;
//priority level of src_n. to set src_n,
//the priority of the pixel must be >pri_n
uint8 pri_main, pri_sub;
}pixel_cache[512];

uint8 *bg_tiledata[3];
uint8 *bg_tiledata_state[3];

void render_bg_tile(uint8 color_depth, uint16 tile_num);
inline void clear_pixel_cache();
void init_tiledata_cache();
void clear_tiledata_cache();

//bppu_render_windows.cpp
uint8 main_windowtable[5][512],   sub_windowtable[5][512],
      main_colorwindowtable[512], sub_colorwindowtable[512];

void build_window_table(uint8 bg, uint8 *wtbl, bool mainscreen);
void build_window_tables(uint8 bg);
void build_color_window_table(uint8 *wtbl, uint8 mask);
void build_color_window_tables();

//bppu_render_bg.cpp
void render_line_bg(uint8 bg, uint8 color_depth, uint8 pri0_pos, uint8 pri1_pos);

//bppu_render_oam.cpp
struct {
  byte num;
  byte width, height;
  word x, y;
  word character;
  byte v_flip, h_flip;
  byte palette;
  byte priority;
}current_sprite;

enum { OAM_PRI_NONE = 4 };
uint8 oam_line_pal[512], oam_line_pri[512];

void set_sprite_attributes(uint8 sprite_num);
void render_oam_sprite();
void render_line_oam(uint8 pri0_pos, uint8 pri1_pos, uint8 pri2_pos, uint8 pri3_pos);

//bppu_render_mode7.cpp
void render_line_mode7(uint8 bg1_pri, uint8 bg2b_pri, uint8 bg2a_pri);

//bppu_render_addsub.cpp
inline uint16 addsub_pixels(int cdest_index, int cdest_bg, int csrc_index, int csrc_bg);
inline uint16 addsub_pixel(int cdest_index, int cdest_bg);

//bppu_render_line.cpp
enum { BLENDTYPE_BACK = 0, BLENDTYPE_MAIN = 1, BLENDTYPE_SUB = 2, BLENDTYPE_COMBINE = 3 };

inline uint16 get_palette(int index);
inline uint16 get_pixel(int x);
inline void render_line_output();