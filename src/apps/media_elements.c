#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void media_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void media_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        media_draw_pixel(x1, y1, color);
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

static void media_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            media_draw_pixel(px, py, color);
        }
    }
}

static void media_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    media_draw_line(x, y, x + w - 1, y, color);
    media_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    media_draw_line(x, y, x, y + h - 1, color);
    media_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void media_draw_dotted_rect(int x, int y, int w, int h, uint32_t color) {
    for (int px = x; px < x + w; px += 8) {
        media_draw_pixel(px, y, color);
        media_draw_pixel(px, y + h - 1, color);
    }
    for (int py = y; py < y + h; py += 8) {
        media_draw_pixel(x, py, color);
        media_draw_pixel(x + w - 1, py, color);
    }
}

static void media_draw_char(int x, int y, char ch, uint32_t color) {
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
        ['='] = {0x00,0x7f,0x00,0x7f,0x00,0x00,0x00,0x00},
    };

    const uint8_t *glyph = font[c];
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((glyph[row] >> (7 - col)) & 1) {
                media_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void media_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }
    int cursor_x = x;
    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else {
            media_draw_char(cursor_x, y, *str, color);
            cursor_x += 8;
        }
        str++;
    }
}

/* Shared implementations for phosphorus and fluorine now live in extended_elements.c. */

void show_titanium_clock_app(void) {
    media_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int panel_x = 180;
    int panel_y = 90;
    int panel_w = 680;
    int panel_h = 560;
    media_draw_rect_filled(panel_x, panel_y, panel_w, panel_h, COLOR_WHITE);
    media_draw_rect_outline(panel_x, panel_y, panel_w, panel_h, COLOR_BORDER);
    media_draw_string(panel_x + 24, panel_y + 24, "TITANIUM // MECHANICAL DIAL", COLOR_TEXT);

    int cx = 512;
    int cy = 384;
    int radius = 120;

    for (int angle = 0; angle < 360; angle += 15) {
        int x1 = cx + (int)((double)radius * 0.95 * (angle % 360 == 90 ? 0 : (angle % 360 == 270 ? 0 : 1)));
        int y1 = cy + (int)((double)radius * 0.95 * (angle % 360 == 180 ? 0 : (angle % 360 == 0 ? 0 : 1)));
        (void)x1; (void)y1;
    }

    for (int angle = 0; angle < 360; angle += 30) {
        int ex = cx;
        int ey = cy;
        switch (angle) {
            case 0: ex = cx + radius; ey = cy; break;
            case 30: ex = cx + 112; ey = cy - 56; break;
            case 60: ex = cx + 56; ey = cy - 112; break;
            case 90: ex = cx; ey = cy - radius; break;
            case 120: ex = cx - 56; ey = cy - 112; break;
            case 150: ex = cx - 112; ey = cy - 56; break;
            case 180: ex = cx - radius; ey = cy; break;
            case 210: ex = cx - 112; ey = cy + 56; break;
            case 240: ex = cx - 56; ey = cy + 112; break;
            case 270: ex = cx; ey = cy + radius; break;
            case 300: ex = cx + 56; ey = cy + 112; break;
            case 330: ex = cx + 112; ey = cy + 56; break;
            default: break;
        }

        int inner = radius - 12;
        int tx = cx + (ex - cx) * inner / radius;
        int ty = cy + (ey - cy) * inner / radius;
        media_draw_line(tx, ty, ex, ey, COLOR_TEXT);
    }

    for (int angle = 0; angle < 360; angle += 30) {
        int x1 = cx + (int)((double)radius * 0.95 * (angle % 360 == 90 ? 0 : (angle % 360 == 270 ? 0 : 1)));
        int y1 = cy + (int)((double)radius * 0.95 * (angle % 360 == 180 ? 0 : (angle % 360 == 0 ? 0 : 1)));
        (void)x1; (void)y1;
    }

    media_draw_rect_filled(cx - 3, cy - 3, 6, 6, COLOR_TEXT);
    media_draw_line(cx, cy, cx + 42, cy - 58, COLOR_TEXT);
    media_draw_line(cx, cy, cx + 88, cy + 12, COLOR_TEXT);
    media_draw_line(cx, cy, cx + 92, cy + 70, COLOR_TEXT);
    media_draw_string(panel_x + 280, panel_y + 500, "2026", COLOR_TEXT);
}

void show_boron_calc_app(void) {
    media_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 140;
    int y = 80;
    int w = 740;
    int h = 580;
    media_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    media_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    media_draw_string(x + 24, y + 24, "BORON // SCIENTIFIC DASHBOARD", COLOR_TEXT);

    media_draw_rect_filled(300, 140, 424, 60, COLOR_HE_BG);
    media_draw_rect_outline(300, 140, 424, 60, COLOR_BORDER);
    media_draw_string(320, 154, "SIN(LOG(X)) + MATRIX", COLOR_TEXT);
    media_draw_string(320, 172, "= 3.14159265", COLOR_TEXT);

    int key_w = 60;
    int key_h = 30;
    int start_x = 180;
    int start_y = 260;
    const char *labels[30] = {
        "SIN", "COS", "TAN", "LOG", "MAT", "DET",
        "EXP", "INT", "FFT", "INV", "RND", "SUM",
        "AVG", "MOD", "ABS", "MIN", "MAX", "PWR",
        "ZER", "ONE", "PI", "EUL", "LIM", "DEL",
        "X^2", "x^y", "->", "CLR", "RUN"
    };

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 6; col++) {
            int idx = row * 6 + col;
            int px = start_x + col * 70;
            int py = start_y + row * 40;
            media_draw_rect_filled(px, py, key_w, key_h, 0xFF9A958E);
            media_draw_rect_outline(px, py, key_w, key_h, COLOR_BEVEL_DARK);
            media_draw_string(px + 8, py + 9, labels[idx], COLOR_WHITE);
        }
    }
}
