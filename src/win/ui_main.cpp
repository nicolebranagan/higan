void MainWindow::set_frameskip(uint8 fs) {
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_OFF, MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_1,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_2,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_3,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_4,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_5,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_6,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_7,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_8,   MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_9,   MF_UNCHECKED);

  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_FRAMESKIP_OFF + fs, MF_CHECKED);
  clock->set_frameskip(fs);
  w_main->frameskip = fs;
}

void MainWindow::to_fullscreen() {
  if(bsnes->debugger_enabled() == true) {
    bsnes->debugger_disable();
  }
  SetWindowLong(hwnd, GWL_STYLE,   WS_POPUP);
  SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, width, height, 0);
  dd_renderer->to_fullscreen(width, height);
  hide_menu();
}

void MainWindow::to_windowed() {
  SetWindowLong(hwnd, GWL_STYLE,   WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
  SetWindowLong(hwnd, GWL_EXSTYLE, 0);
//always call in case the video mode change is simply changing
//the backbuffer from a VRAM surface to a DRAM surface.
  dd_renderer->to_windowed();
  if(dd_renderer->fullscreen == true) {
    show_menu();
  }
}

void MainWindow::adjust_video_mode(bool fullscreen_mode) {
  fullscreen = fullscreen_mode;

  if(fullscreen == true) {
    to_fullscreen();
  } else {
    to_windowed();
  }

  resize(width, height);
  if(fullscreen == false)center();

  if(bsnes->debugger_enabled() == true) {
    to_bottom();
    to_right();
  }
}

void MainWindow::set_video_mode(uint8 mode) {
  hide();
  CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_256x224w,  MF_UNCHECKED);
  CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_512x448w,  MF_UNCHECKED);
  CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_960x720w,  MF_UNCHECKED);
  CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_640x480f,  MF_UNCHECKED);
  CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_1024x768f, MF_UNCHECKED);
  switch(mode) {
  case VIDEOMODE_256x224w:
    CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_256x224w, MF_CHECKED);
    width  =  256;
    height =  223;
    adjust_video_mode(false);
    break;
  case VIDEOMODE_512x448w:
    CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_512x448w, MF_CHECKED);
    width  =  512;
    height =  446;
    adjust_video_mode(false);
    break;
  case VIDEOMODE_960x720w:
    CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_960x720w, MF_CHECKED);
    width  =  960;
    height =  720;
    adjust_video_mode(false);
    break;
  case VIDEOMODE_640x480f:
    CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_640x480f, MF_CHECKED);
    width  =  640;
    height =  480;
    adjust_video_mode(true);
    break;
  case VIDEOMODE_1024x768f:
    CheckMenuItem(hmenu, MENU_SETTINGS_VIDEOMODE_1024x768f, MF_CHECKED);
    width  = 1024;
    height =  768;
    adjust_video_mode(true);
    break;
  }

  cfg.video.mode = mode;
  show();
}

void MainWindow::menu_load() {
OPENFILENAME ofn;
char t[4096];
  strcpy(t, "");
  memset(&ofn, 0, sizeof(ofn));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner   = hwnd;
  ofn.lpstrFilter = "SNES ROM Images (*.smc;*.swc;*.fig;*.ufo;*.gd3;*.078)\0"
                      "*.smc;*.swc;*.fig;*.ufo;*.gd3;*.078\0"
                    "All Files (*.*)\0"
                      "*.*\0";
  ofn.lpstrFile   = t;
  ofn.nMaxFile    = MAX_PATH;
  ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST;
  ofn.lpstrDefExt = "smc";

  if(!GetOpenFileName(&ofn))return;

  rom_image->unload();
  dprintf("");
  rom_image->select(t);
  rom_image->load();

  snes->power();
  bsnes->debugger_update();
}

void MainWindow::menu_unload() {
  rom_image->unload();
}

long __stdcall wndproc_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch(msg) {
  case WM_KEYDOWN:
    if(wparam == VK_ESCAPE) {
      if(GetMenu(w_main->hwnd) == NULL) {
        w_main->show_menu();
      } else {
        w_main->hide_menu();
      }
      w_main->center();
    }
    break;
  case WM_COMMAND:
    switch(LOWORD(wparam)) {
    case MENU_FILE_LOAD:
      w_main->menu_load();
      break;
    case MENU_FILE_UNLOAD:
      w_main->menu_unload();
      break;
    case MENU_FILE_RESET:
      if(rom_image->loaded() == false)break;
      dprintf("* Soft Reset");
      snes->reset();
      bsnes->debugger_update();
      break;
    case MENU_FILE_POWER:
      if(rom_image->loaded() == false)break;
      dprintf("* Power (Hard Reset)");
      snes->power();
      bsnes->debugger_update();
      break;
    case MENU_FILE_EXIT:
      PostQuitMessage(0);
      break;
    case MENU_SETTINGS_FRAMESKIP_OFF:
    case MENU_SETTINGS_FRAMESKIP_1:
    case MENU_SETTINGS_FRAMESKIP_2:
    case MENU_SETTINGS_FRAMESKIP_3:
    case MENU_SETTINGS_FRAMESKIP_4:
    case MENU_SETTINGS_FRAMESKIP_5:
    case MENU_SETTINGS_FRAMESKIP_6:
    case MENU_SETTINGS_FRAMESKIP_7:
    case MENU_SETTINGS_FRAMESKIP_8:
    case MENU_SETTINGS_FRAMESKIP_9:
      w_main->set_frameskip(LOWORD(wparam) - MENU_SETTINGS_FRAMESKIP_OFF);
      break;
    case MENU_SETTINGS_VIDEOMODE_256x224w:
      w_main->set_video_mode(VIDEOMODE_256x224w);
      break;
    case MENU_SETTINGS_VIDEOMODE_512x448w:
      w_main->set_video_mode(VIDEOMODE_512x448w);
      break;
    case MENU_SETTINGS_VIDEOMODE_960x720w:
      w_main->set_video_mode(VIDEOMODE_960x720w);
      break;
    case MENU_SETTINGS_VIDEOMODE_640x480f:
      w_main->set_video_mode(VIDEOMODE_640x480f);
      break;
    case MENU_SETTINGS_VIDEOMODE_1024x768f:
      w_main->set_video_mode(VIDEOMODE_1024x768f);
      break;
    case MENU_SETTINGS_USEVRAM:
      cfg.video.use_vram ^= 1;
      w_main->set_video_mode(cfg.video.mode);
      CheckMenuItem(w_main->hmenu, MENU_SETTINGS_USEVRAM, (cfg.video.use_vram)?MF_CHECKED:MF_UNCHECKED);
      break;
    case MENU_SETTINGS_VBLANK:
      cfg.video.vblank ^= 1;
      CheckMenuItem(w_main->hmenu, MENU_SETTINGS_VBLANK, (cfg.video.vblank)?MF_CHECKED:MF_UNCHECKED);
      break;
    case MENU_SETTINGS_COLORCURVE:
      cfg.video.color_curve ^= 1;
      dd_renderer->update_color_lookup_table();
      CheckMenuItem(w_main->hmenu, MENU_SETTINGS_COLORCURVE, (cfg.video.color_curve)?MF_CHECKED:MF_UNCHECKED);
      break;
    case MENU_SETTINGS_SHOWFPS:
      cfg.gui.show_fps ^= 1;
      SetWindowText(w_main->hwnd, BSNES_TITLE);
      CheckMenuItem(w_main->hmenu, MENU_SETTINGS_SHOWFPS, (cfg.gui.show_fps)?MF_CHECKED:MF_UNCHECKED);
      break;
    case MENU_SETTINGS_DEBUGGER:
      if(bsnes->debugger_enabled() == true) {
        bsnes->debugger_disable();
      } else {
        bsnes->debugger_enable();
        w_main->set_video_mode(VIDEOMODE_256x224w);
      }
      break;
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_PAINT:
    dd_renderer->redraw();
    break;
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

void CreateWindowMain() {
WNDCLASS wc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
  wc.hCursor       = LoadCursor(0, IDC_ARROW);
  wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
  wc.hInstance     = GetModuleHandle(0);
  wc.lpfnWndProc   = wndproc_main;
  wc.lpszClassName = "bsnes";
  wc.lpszMenuName  = 0;
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  RegisterClass(&wc);

  w_main->hwnd = CreateWindowEx(0, "bsnes", "bsnes v" BSNES_VERSION " ~byuu",
    WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    0, 0, 512, 446,
    0, 0, GetModuleHandle(0), 0);

HMENU hsubmenu, hbranchmenu;
  w_main->hmenu = CreateMenu();

  hsubmenu = CreatePopupMenu();
  AppendMenu(hsubmenu, MF_STRING, MENU_FILE_LOAD, "&Load ROM");
  AppendMenu(hsubmenu, MF_STRING, MENU_FILE_UNLOAD, "&Unload ROM");
  AppendMenu(hsubmenu, MF_SEPARATOR, 0, "");
  AppendMenu(hsubmenu, MF_STRING, MENU_FILE_RESET, "&Reset");
  AppendMenu(hsubmenu, MF_STRING, MENU_FILE_POWER, "&Power (Hard Reset)");
  AppendMenu(hsubmenu, MF_SEPARATOR, 0, "");
  AppendMenu(hsubmenu, MF_STRING, MENU_FILE_EXIT, "E&xit");
  AppendMenu(w_main->hmenu, MF_STRING | MF_POPUP, (unsigned int)hsubmenu, "&File");

  hsubmenu = CreatePopupMenu();

  hbranchmenu = CreatePopupMenu();
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_OFF, "Off");
  AppendMenu(hbranchmenu, MF_SEPARATOR, 0, "");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_1, "1");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_2, "2");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_3, "3");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_4, "4");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_5, "5");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_6, "6");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_7, "7");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_8, "8");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_FRAMESKIP_9, "9");
  AppendMenu(hsubmenu, MF_STRING | MF_POPUP, (unsigned int)hbranchmenu, "&Frameskip");

  hbranchmenu = CreatePopupMenu();
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_VIDEOMODE_256x224w,   "256x224 Windowed [16:15]");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_VIDEOMODE_512x448w,   "512x448 Windowed [16:15]");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_VIDEOMODE_960x720w,   "960x720 Windowed [4:3]");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_VIDEOMODE_640x480f,   "640x480 Fullscreen [16:15]");
  AppendMenu(hbranchmenu, MF_STRING, MENU_SETTINGS_VIDEOMODE_1024x768f,  "1024x768 Fullscreen [4:3]");
  AppendMenu(hsubmenu, MF_STRING | MF_POPUP, (unsigned int)hbranchmenu, "&Video Mode");

  AppendMenu(hsubmenu, MF_STRING, MENU_SETTINGS_USEVRAM,    "Use &Video Memory Surface");
  AppendMenu(hsubmenu, MF_STRING, MENU_SETTINGS_VBLANK,     "&Wait for Vertical Retrace");
  AppendMenu(hsubmenu, MF_STRING, MENU_SETTINGS_COLORCURVE, "Use &Color Curve");
  AppendMenu(hsubmenu, MF_STRING, MENU_SETTINGS_SHOWFPS,    "&Show FPS");

  AppendMenu(hsubmenu, MF_SEPARATOR, 0, "");
  AppendMenu(hsubmenu, MF_STRING, MENU_SETTINGS_DEBUGGER, "&Debugger");
  AppendMenu(w_main->hmenu, MF_STRING | MF_POPUP, (unsigned int)hsubmenu, "&Settings");

  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_USEVRAM,    (cfg.video.use_vram)?MF_CHECKED:MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_VBLANK,     (cfg.video.vblank)?MF_CHECKED:MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_COLORCURVE, (cfg.video.color_curve)?MF_CHECKED:MF_UNCHECKED);
  CheckMenuItem(w_main->hmenu, MENU_SETTINGS_SHOWFPS,    (cfg.gui.show_fps)?MF_CHECKED:MF_UNCHECKED);
}

MainWindow::MainWindow() {
}