#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void sys_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void sys_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        sys_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

static void sys_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            sys_draw_pixel(px, py, color);
        }
    }
}

static void sys_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    sys_draw_line(x, y, x + w - 1, y, color);
    sys_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    sys_draw_line(x, y, x, y + h - 1, color);
    sys_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void sys_draw_char(int x, int y, char ch, uint32_t color) {
    unsigned char c = (unsigned char)ch;
    if (c >= 'a' && c <= 'z') {
        c = (unsigned char)(c - 'a' + 'A');
    }
    if (c >= 128) {
        c = 0;
    }

    static const uint8_t font[128][8] = {
        [32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        ['A'] = {0x1c,0x36,0x63,0x7f,0x63,0x63,0x63,0x00},
        ['B'] = {0x7e,0x63,0x63,0x7e,0x63,0x63,0x7e,0x00},
        ['C'] = {0x3e,0x63,0x60,0x60,0x60,0x63,0x3e,0x00},
        ['D'] = {0x7c,0x66,0x63,0x63,0x63,0x66,0x7c,0x00},
        ['E'] = {0x7f,0x60,0x60,0x7c,0x60,0x60,0x7f,0x00},
        ['F'] = {0x7f,0x60,0x60,0x7c,0x60,0x60,0x60,0x00},
        ['G'] = {0x3e,0x63,0x60,0x6f,0x63,0x63,0x3e,0x00},
        ['H'] = {0x63,0x63,0x63,0x7f,0x63,0x63,0x63,0x00},
        ['I'] = {0x3e,0x0c,0x0c,0x0c,0x0c,0x0c,0x3e,0x00},
        ['L'] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7f,0x00},
        ['M'] = {0x63,0x77,0x7f,0x6b,0x63,0x63,0x63,0x00},
        ['N'] = {0x63,0x63,0x73,0x7b,0x6f,0x67,0x63,0x00},
        ['O'] = {0x3e,0x63,0x63,0x63,0x63,0x63,0x3e,0x00},
        ['P'] = {0x7e,0x63,0x63,0x7e,0x60,0x60,0x60,0x00},
        ['R'] = {0x7e,0x63,0x63,0x7e,0x6c,0x66,0x63,0x00},
        ['S'] = {0x3e,0x63,0x60,0x3e,0x03,0x63,0x3e,0x00},
        ['T'] = {0x7f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x00},
        ['U'] = {0x63,0x63,0x63,0x63,0x63,0x63,0x3e,0x00},
        ['V'] = {0x63,0x63,0x63,0x63,0x63,0x34,0x1c,0x00},
        ['W'] = {0x63,0x63,0x6b,0x7f,0x77,0x63,0x63,0x00},
        ['X'] = {0x63,0x63,0x34,0x1c,0x34,0x63,0x63,0x00},
        ['Y'] = {0x63,0x63,0x63,0x3e,0x0c,0x0c,0x0c,0x00},
        ['Z'] = {0x7f,0x03,0x06,0x0c,0x18,0x30,0x7f,0x00},
        ['0'] = {0x3e,0x63,0x67,0x6f,0x7b,0x63,0x3e,0x00},
        ['1'] = {0x0c,0x1e,0x0c,0x0c,0x0c,0x0c,0x3e,0x00},
        ['2'] = {0x3e,0x63,0x06,0x1c,0x30,0x63,0x7f,0x00},
        ['3'] = {0x7f,0x06,0x0c,0x1c,0x06,0x63,0x3e,0x00},
        ['4'] = {0x0c,0x1c,0x3c,0x6c,0x7f,0x0c,0x0c,0x00},
        ['5'] = {0x7f,0x60,0x7e,0x03,0x03,0x63,0x3e,0x00},
        ['6'] = {0x1c,0x30,0x60,0x7e,0x63,0x63,0x3e,0x00},
        ['7'] = {0x7f,0x03,0x06,0x0c,0x18,0x18,0x18,0x00},
        ['8'] = {0x3e,0x63,0x63,0x3e,0x63,0x63,0x3e,0x00},
        ['9'] = {0x3e,0x63,0x63,0x7f,0x03,0x06,0x3c,0x00},
        [':'] = {0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00},
        ['.'] = {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},
        ['-'] = {0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00},
        ['['] = {0x1e,0x18,0x18,0x18,0x18,0x18,0x1e,0x00},
        [']'] = {0x1e,0x06,0x06,0x06,0x06,0x06,0x1e,0x00},
        ['?'] = {0x3e,0x63,0x03,0x1c,0x00,0x0c,0x0c,0x00},
        ['+'] = {0x00,0x0c,0x0c,0x7f,0x0c,0x0c,0x00,0x00},
        ['/'] = {0x03,0x06,0x0c,0x18,0x30,0x60,0x7f,0x00},
    };

    const uint8_t *glyph = font[c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((glyph[row] >> (7 - col)) & 1) {
                sys_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void sys_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }
    int cursor_x = x;
    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else {
            sys_draw_char(cursor_x, y, *str, color);
            cursor_x += 8;
        }
        str++;
    }
}

static void sys_draw_slider(int x, int y, const char *label, int knob_pos) {
    sys_draw_string(x, y, label, COLOR_TEXT);
    sys_draw_line(x + 90, y + 6, x + 340, y + 6, COLOR_BORDER);
    int knob_x = x + 90 + knob_pos;
    draw_retro_button(knob_x, y - 2, 12, 12, COLOR_LI_BG);
}

void show_silicon_settings_app(void) {
    const int x = 180;
    const int y = 80;
    const int w = 660;
    const int h = 540;
    const int title_height = 32;
    const int section1_y = y + 120;
    const int section2_y = y + 240;
    const int section3_y = y + 380;
    const uint32_t title_bg = 0xFFEFEBE9u;
    const uint32_t checkbox_fill = 0xFFFFFBF5u;
    const uint32_t brown_charcoal = 0xFF5D4037u;
    const uint32_t cream_warm = 0xFFF5EFE6u;
    const uint32_t latte_brown = 0xFFEFEBE9u;

    sys_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    sys_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    sys_draw_rect_filled(x, y, w, title_height, title_bg);
    sys_draw_rect_outline(x, y, w, title_height, COLOR_BORDER);
    for (int stripe = 0; stripe < 4; stripe++) {
        int stripe_y = y + 6 + stripe * 6;
        sys_draw_line(x + 22, stripe_y, x + w - 22, stripe_y, COLOR_BORDER);
    }

    sys_draw_rect_filled(x + 12, y + 10, 12, 12, COLOR_WHITE);
    sys_draw_rect_outline(x + 12, y + 10, 12, 12, brown_charcoal);
    sys_draw_line(x + 14, y + 12, x + 22, y + 20, brown_charcoal);
    sys_draw_line(x + 22, y + 12, x + 14, y + 20, brown_charcoal);

    draw_mac_string(x + 34, y + 10, "⚙️ ELEMENT 14: SILICON (ADVANCED SYSTEM CONTROL)", COLOR_TEXT);

    sys_draw_line(x + 18, section1_y, x + w - 18, section1_y, COLOR_BORDER);
    sys_draw_line(x + 18, section2_y, x + w - 18, section2_y, COLOR_BORDER);
    sys_draw_line(x + 18, section3_y, x + w - 18, section3_y, COLOR_BORDER);

    sys_draw_string(x + 20, y + 55, "[01] VISUAL PREFERENCES", COLOR_TEXT);
    sys_draw_rect_filled(220, y + 80, 14, 14, checkbox_fill);
    sys_draw_rect_outline(220, y + 80, 14, 14, brown_charcoal);
    sys_draw_line(222, y + 82, 232, y + 92, COLOR_TEXT);
    sys_draw_line(232, y + 82, 222, y + 92, COLOR_TEXT);
    sys_draw_string(240, y + 80, "Title Bar Textures", COLOR_TEXT);

    const int palette_x = 220;
    const int palette_y = y + 120;
    const int swatch = 24;
    const int gap = 12;

    sys_draw_rect_filled(palette_x, palette_y, swatch, swatch, cream_warm);
    sys_draw_rect_outline(palette_x, palette_y, swatch, swatch, COLOR_BORDER);
    sys_draw_rect_filled(palette_x + swatch + gap, palette_y, swatch, swatch, 0xFFFFFBF5u);
    sys_draw_rect_outline(palette_x + swatch + gap, palette_y, swatch, swatch, COLOR_BORDER);
    sys_draw_rect_filled(palette_x + 2 * (swatch + gap), palette_y, swatch, swatch, latte_brown);
    sys_draw_rect_outline(palette_x + 2 * (swatch + gap), palette_y, swatch, swatch, COLOR_BORDER);
    sys_draw_string(palette_x, palette_y + 34, "Wallpaper Tint", COLOR_TEXT);

    sys_draw_string(x + 20, y + 175, "[02] SYSTEM CONSTRAINTS", COLOR_TEXT);
    draw_retro_button(220, y + 200, 50, 20, brown_charcoal);
    sys_draw_string(228, y + 204, "ON", COLOR_WHITE);
    sys_draw_string(280, y + 204, "Mercury Memory Purge", COLOR_TEXT);

    const int fps1_x = 220;
    const int fps2_x = fps1_x + 132;
    const int fps_y = y + 330;
    const int fps_w = 100;
    const int fps_h = 28;

    sys_draw_rect_filled(fps1_x, fps_y, fps_w, fps_h, latte_brown);
    sys_draw_rect_outline(fps1_x, fps_y, fps_w, fps_h, COLOR_BORDER);
    sys_draw_string(fps1_x + 10, fps_y + 8, "[ 30 FPS ]", COLOR_TEXT);

    draw_retro_button(fps2_x, fps_y, fps_w, fps_h, COLOR_WHITE);
    sys_draw_string(fps2_x + 10, fps_y + 8, "[ 60 FPS ]", COLOR_TEXT);

    sys_draw_string(x + 20, y + 315, "[03] LOCALE & FONTS", COLOR_TEXT);
    const int dropdown_x = 220;
    const int dropdown_y = y + 420;
    const int dropdown_w = 200;
    const int dropdown_h = 28;

    sys_draw_rect_filled(dropdown_x, dropdown_y, dropdown_w, dropdown_h, brown_charcoal);
    sys_draw_rect_outline(dropdown_x, dropdown_y, dropdown_w, dropdown_h, COLOR_BORDER);
    sys_draw_string(dropdown_x + 8, dropdown_y + 8, "NOTION SANS (MINIMAL)", COLOR_TEXT);
    sys_draw_line(dropdown_x + dropdown_w - 18, dropdown_y + 12, dropdown_x + dropdown_w - 14, dropdown_y + 16, COLOR_TEXT);
    sys_draw_line(dropdown_x + dropdown_w - 14, dropdown_y + 16, dropdown_x + dropdown_w - 10, dropdown_y + 12, COLOR_TEXT);

    sys_draw_string(x + 20, y + 470, "⚠️ GEOLOCATION PRIVACY ACTIVE:", COLOR_TEXT);
    sys_draw_string(x + 20, y + 482, "ALL LOCAL DATA IS STRICTLY PROTECTED ON-DEVICE", COLOR_TEXT);
    sys_draw_string(x + 20, y + 494, "UNDER NORDIC CLEAN COMPLIANCE.", COLOR_TEXT);
}

void show_moscovium_task_app(void) {
    int x = 250;
    int y = 100;
    int w = 500;
    int h = 450;
    sys_draw_rect_filled(x, y, w, h, COLOR_HE_BG);
    sys_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    sys_draw_string(x + 20, y + 24, "ELEMENT 115: MOSCOVIUM (TASK MANAGER)", COLOR_TEXT);

    int chart_y = y + 70;
    int bar_w = 24;
    int bar_h = 44;
    int gap = 12;
    int start_x = x + 40;
    for (int i = 0; i < 6; i++) {
        int px = start_x + i * (bar_w + gap);
        int fill_h = (i < 4) ? bar_h : 18;
        sys_draw_rect_filled(px, chart_y + (bar_h - fill_h), bar_w, fill_h, 0xFF3E2723);
        sys_draw_rect_outline(px, chart_y + (bar_h - fill_h), bar_w, fill_h, COLOR_BORDER);
    }

    int chart_y2 = y + 140;
    for (int i = 0; i < 6; i++) {
        int px = start_x + i * (bar_w + gap);
        int fill_h = (i < 3) ? bar_h : 16;
        sys_draw_rect_filled(px, chart_y2 + (bar_h - fill_h), bar_w, fill_h, 0xFF3E2723);
        sys_draw_rect_outline(px, chart_y2 + (bar_h - fill_h), bar_w, fill_h, COLOR_BORDER);
    }

    sys_draw_string(x + 40, y + 210, "CPU USAGE: 60%", COLOR_TEXT);
    sys_draw_string(x + 40, y + 230, "RAM USAGE: 45%", COLOR_TEXT);

    sys_draw_rect_filled(x + 40, y + 270, w - 80, 120, COLOR_WHITE);
    sys_draw_rect_outline(x + 40, y + 270, w - 80, 120, COLOR_BORDER);
    sys_draw_string(x + 56, y + 292, "KERNEL.SYS | RUNNING", COLOR_TEXT);
    sys_draw_string(x + 56, y + 312, "DUBNIUM.APP | IDLE", COLOR_TEXT);
    sys_draw_string(x + 56, y + 332, "CARBON.APP | SLEEP", COLOR_TEXT);
}

void show_iron_explorer_app(void) {
    int x = 120;
    int y = 90;
    int w = 720;
    int h = 500;
    sys_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    sys_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    sys_draw_string(x + 20, y + 24, "ELEMENT 26: IRON (FILE EXPLORER)", COLOR_TEXT);

    int split_x = x + 320;
    sys_draw_line(split_x, y + 48, split_x, y + h - 20, COLOR_BORDER);

    sys_draw_rect_filled(x + 16, y + 54, 280, h - 74, COLOR_HE_BG);
    sys_draw_rect_outline(x + 16, y + 54, 280, h - 74, COLOR_BORDER);
    sys_draw_string(x + 32, y + 80, "/root", COLOR_TEXT);
    sys_draw_string(x + 32, y + 100, "/system", COLOR_TEXT);
    sys_draw_string(x + 32, y + 120, "/user_data", COLOR_TEXT);

    sys_draw_rect_filled(x + 336, y + 54, 360, h - 74, COLOR_WHITE);
    sys_draw_rect_outline(x + 336, y + 54, 360, h - 74, COLOR_BORDER);
    draw_retro_button(x + 352, y + 80, 200, 30, COLOR_HE_BG);
    sys_draw_string(x + 364, y + 92, "kernel.sys (42KB)", COLOR_TEXT);
    draw_retro_button(x + 352, y + 128, 200, 30, COLOR_HE_BG);
    sys_draw_string(x + 364, y + 140, "desktop.ui (12KB)", COLOR_TEXT);
    draw_retro_button(x + 352, y + 176, 200, 30, COLOR_HE_BG);
    sys_draw_string(x + 364, y + 188, "docker.cfg (2KB)", COLOR_TEXT);
}
