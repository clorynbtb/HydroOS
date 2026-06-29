#include <stdint.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void core_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void core_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        core_draw_pixel(x1, y1, color);
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

static void core_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            core_draw_pixel(px, py, color);
        }
    }
}

static void core_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    core_draw_line(x, y, x + w - 1, y, color);
    core_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    core_draw_line(x, y, x, y + h - 1, color);
    core_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void core_draw_char(int x, int y, char ch, uint32_t color) {
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
        ['!'] = {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00},
        ['['] = {0x1e,0x18,0x18,0x18,0x18,0x18,0x1e,0x00},
        [']'] = {0x1e,0x06,0x06,0x06,0x06,0x06,0x1e,0x00},
        [':'] = {0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00},
        ['.'] = {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},
        ['-'] = {0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00},
        ['_'] = {0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x00},
        ['/'] = {0x03,0x06,0x0c,0x18,0x30,0x60,0x7f,0x00},
        ['@'] = {0x3e,0x63,0x6f,0x6f,0x6e,0x60,0x3e,0x00},
    };

    const uint8_t *glyph = font[c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((glyph[row] >> (7 - col)) & 1) {
                core_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void core_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }
    int cursor_x = x;
    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else {
            core_draw_char(cursor_x, y, *str, color);
            cursor_x += 8;
        }
        str++;
    }
}

static void core_draw_warning_triangle(int x, int y, int size, uint32_t color) {
    int top_x = x + size / 2;
    int top_y = y;
    int left_x = x;
    int left_y = y + size;
    int right_x = x + size;
    int right_y = y + size;
    core_draw_line(top_x, top_y, left_x, left_y, color);
    core_draw_line(top_x, top_y, right_x, right_y, color);
    core_draw_line(left_x, left_y, right_x, right_y, color);
}

void show_neon_terminal_app(void) {
    int x = 150;
    int y = 120;
    int w = 600;
    int h = 440;

    core_draw_rect_filled(x, y, w, h, 0xFF271714u);
    core_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    core_draw_string(x + 24, y + 24, "NEON TERMINAL v0.1 - HYDROOS CORE", 0xFFFFF8E7u);
    core_draw_string(x + 24, y + 48, "root@dubnium:/# _", 0xFFFFF8E7u);

    core_draw_rect_filled(x + 24, y + 76, 550, 150, 0xFF1F1612u);
    core_draw_rect_outline(x + 24, y + 76, 550, 150, COLOR_BORDER);
    core_draw_string(x + 36, y + 92, "[boot] hydro kernel loaded", 0xFFFFF8E7u);
    core_draw_string(x + 36, y + 112, "[sys] neon interface active", 0xFFFFF8E7u);
    core_draw_string(x + 36, y + 132, "[cmd] awaiting input...", 0xFFFFF8E7u);
}

void show_argon_crash_app(void) {
    int x = 212;
    int y = 184;
    int w = 600;
    int h = 400;

    core_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    core_draw_rect_outline(x, y, w, h, 0xFF3E2723u);
    core_draw_rect_outline(x + 1, y + 1, w - 2, h - 2, 0xFF3E2723u);
    core_draw_rect_outline(x + 2, y + 2, w - 4, h - 4, 0xFF3E2723u);

    core_draw_warning_triangle(x + 260, y + 56, 80, 0xFF3E2723u);
    core_draw_string(x + 292, y + 114, "[!]", 0xFF3E2723u);

    core_draw_string(x + 72, y + 220, "FATAL ERROR: ELEMENT OVERFLOW.", COLOR_TEXT);
    core_draw_string(x + 72, y + 238, "CORE DUMPED AT NODE_18.", COLOR_TEXT);
    core_draw_string(x + 72, y + 256, "PRESS ANY KEY TO REBOOT.", COLOR_TEXT);
}

void show_xenon_driver_app(void) {
    int x = 180;
    int y = 100;
    int w = 660;
    int h = 480;

    core_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    core_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    int center_x = x + 300;
    int center_y = y + 220;
    core_draw_rect_filled(center_x - 48, center_y - 24, 96, 48, COLOR_HE_BG);
    core_draw_rect_outline(center_x - 48, center_y - 24, 96, 48, COLOR_TEXT);
    core_draw_string(center_x - 28, center_y - 10, "CPU [Si]", COLOR_TEXT);

    int left_x = x + 90;
    int left_y = y + 140;
    core_draw_rect_filled(left_x - 40, left_y - 18, 96, 36, COLOR_LI_BG);
    core_draw_rect_outline(left_x - 40, left_y - 18, 96, 36, COLOR_TEXT);
    core_draw_string(left_x - 18, left_y - 6, "DISPLAY [O]", COLOR_TEXT);
    core_draw_string(left_x + 60, left_y - 6, "[OK]", 0xFF3E2723u);

    int right_x = x + 500;
    int right_y = y + 140;
    core_draw_rect_filled(right_x - 44, right_y - 18, 96, 36, COLOR_LI_BG);
    core_draw_rect_outline(right_x - 44, right_y - 18, 96, 36, COLOR_TEXT);
    core_draw_string(right_x - 20, right_y - 6, "KEYBOARD [H]", COLOR_TEXT);
    core_draw_string(right_x + 56, right_y - 6, "[OK]", 0xFF3E2723u);

    int bottom_x = x + 300;
    int bottom_y = y + 340;
    core_draw_rect_filled(bottom_x - 42, bottom_y - 18, 96, 36, COLOR_LI_BG);
    core_draw_rect_outline(bottom_x - 42, bottom_y - 18, 96, 36, COLOR_TEXT);
    core_draw_string(bottom_x - 18, bottom_y - 6, "STORAGE [C]", COLOR_TEXT);
    core_draw_string(bottom_x + 58, bottom_y - 6, "[OK]", 0xFF3E2723u);

    core_draw_line(center_x - 36, center_y, left_x + 12, left_y, COLOR_BORDER);
    core_draw_line(center_x + 36, center_y, right_x - 12, right_y, COLOR_BORDER);
    core_draw_line(center_x, center_y + 24, bottom_x, bottom_y, COLOR_BORDER);
}

void show_mercury_ram_app(void) {
    int x = 312;
    int y = 150;
    int w = 400;
    int h = 450;

    core_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    core_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    core_draw_string(x + 24, y + 24, "ELEMENT 80: MERCURY (RAM PURGE)", COLOR_TEXT);

    int tube_x = x + 170;
    int tube_y = y + 92;
    int tube_w = 60;
    int tube_h = 300;
    core_draw_rect_outline(tube_x, tube_y, tube_w, tube_h, 0xFF3E2723u);
    core_draw_rect_filled(tube_x + 6, tube_y + 6, tube_w - 12, 210, 0xFFEFEBE9u);
    core_draw_rect_outline(tube_x + 6, tube_y + 6, tube_w - 12, 210, 0xFF8D6E63u);

    core_draw_rect_filled(tube_x + 12, tube_y + 222, 36, 56, 0xFFBCAAA4u);
    core_draw_rect_outline(tube_x + 12, tube_y + 222, 36, 56, 0xFF3E2723u);

    draw_retro_button(x + 88, y + 368, 224, 44, COLOR_LI_BG);
    core_draw_string(x + 112, y + 386, "PURGE MEMORY", COLOR_TEXT);
}
