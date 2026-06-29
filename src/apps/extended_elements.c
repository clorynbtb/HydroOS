#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void ext_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void ext_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        ext_draw_pixel(x1, y1, color);
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

static void ext_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            ext_draw_pixel(px, py, color);
        }
    }
}

static void ext_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    ext_draw_line(x, y, x + w - 1, y, color);
    ext_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    ext_draw_line(x, y, x, y + h - 1, color);
    ext_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void ext_draw_dotted_rect(int x, int y, int w, int h, uint32_t color) {
    for (int px = x; px < x + w; px += 8) {
        ext_draw_pixel(px, y, color);
        ext_draw_pixel(px, y + h - 1, color);
    }
    for (int py = y; py < y + h; py += 8) {
        ext_draw_pixel(x, py, color);
        ext_draw_pixel(x + w - 1, py, color);
    }
}

static void ext_draw_char(int x, int y, char ch, uint32_t color) {
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
                ext_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void ext_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }
    int cursor_x = x;
    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else {
            ext_draw_char(cursor_x, y, *str, color);
            cursor_x += 8;
        }
        str++;
    }
}

static void ext_draw_button(int x, int y, int w, int h, const char *label, uint32_t bg) {
    draw_retro_button(x, y, w, h, bg);
    ext_draw_string(x + 8, y + 10, label, COLOR_TEXT);
}

void show_calcium_store_app(void) {
    ext_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 120;
    int y = 100;
    int w = 784;
    int h = 520;
    ext_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    ext_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    ext_draw_string(x + 24, y + 24, "CALCIUM // ELEMENT STORE", COLOR_TEXT);

    ext_draw_rect_filled(x + 16, y + 54, w - 32, 40, COLOR_HE_BG);
    ext_draw_rect_outline(x + 16, y + 54, w - 32, 40, COLOR_BORDER);
    ext_draw_button(x + 28, y + 62, 140, 24, "AVAILABLE", COLOR_LI_BG);
    ext_draw_button(x + 188, y + 62, 100, 24, "STABLE", COLOR_LI_BG);
    ext_draw_button(x + 310, y + 62, 140, 24, "RADIOACTIVE", COLOR_LI_BG);

    int card_x = x + 24;
    int card_y = y + 118;
    int card_w = 170;
    int card_h = 92;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int px = card_x + col * 190;
            int py = card_y + row * 120;
            ext_draw_rect_filled(px, py, card_w, card_h, COLOR_HE_BG);
            ext_draw_rect_outline(px, py, card_w, card_h, COLOR_BORDER);
            ext_draw_string(px + 14, py + 16, "Z=20", COLOR_TEXT);
            ext_draw_string(px + 14, py + 34, "CA", COLOR_TEXT);
            ext_draw_button(px + 72, py + 60, 80, 20, "INTEGRATE", COLOR_LI_BG);
        }
    }
}

void show_phosphorus_meeting_app(void) {
    ext_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 200;
    int y = 120;
    int w = 620;
    int h = 450;
    ext_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    ext_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    ext_draw_string(x + 22, y + 20, "PHOSPHORUS // VIDEO CONFERENCE", COLOR_TEXT);

    int frame_w = 260;
    int frame_h = 300;
    int left_x = x + 34;
    int right_x = x + 326;
    int frame_y = y + 92;

    ext_draw_dotted_rect(left_x, frame_y, frame_w, frame_h, COLOR_BORDER);
    ext_draw_dotted_rect(right_x, frame_y, frame_w, frame_h, COLOR_BORDER);
    ext_draw_string(left_x + 24, frame_y + 20, "LOCAL CAMERA", COLOR_TEXT);
    ext_draw_string(right_x + 18, frame_y + 20, "REMOTE CAMERA", COLOR_TEXT);

    ext_draw_line(left_x + 88, frame_y + 120, left_x + 88, frame_y + 200, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 140, left_x + 54, frame_y + 170, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 140, left_x + 122, frame_y + 170, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 200, left_x + 64, frame_y + 248, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 200, left_x + 112, frame_y + 248, COLOR_TEXT);
    ext_draw_line(left_x + 68, frame_y + 92, left_x + 88, frame_y + 78, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 78, left_x + 108, frame_y + 92, COLOR_TEXT);
    ext_draw_line(left_x + 88, frame_y + 78, left_x + 88, frame_y + 102, COLOR_TEXT);
    ext_draw_string(left_x + 60, frame_y + 270, "YOU [MUTED]", COLOR_TEXT);

    ext_draw_line(right_x + 172, frame_y + 120, right_x + 172, frame_y + 200, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 140, right_x + 140, frame_y + 170, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 140, right_x + 204, frame_y + 170, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 200, right_x + 146, frame_y + 248, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 200, right_x + 198, frame_y + 248, COLOR_TEXT);
    ext_draw_line(right_x + 150, frame_y + 92, right_x + 172, frame_y + 78, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 78, right_x + 194, frame_y + 92, COLOR_TEXT);
    ext_draw_line(right_x + 172, frame_y + 78, right_x + 172, frame_y + 102, COLOR_TEXT);
    for (int i = 0; i < 6; i++) {
        int bar_h = 10 + i * 7;
        int bar_x = right_x + 76 + i * 14;
        ext_draw_rect_filled(bar_x, frame_y + frame_h - 54 - bar_h, 8, bar_h, COLOR_TEXT);
    }
    ext_draw_string(right_x + 56, frame_y + 270, "VOL: 78%", COLOR_TEXT);
}

void show_fluorine_form_app(void) {
    ext_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 250;
    int y = 150;
    int w = 500;
    int h = 400;
    ext_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    ext_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    ext_draw_string(x + 24, y + 24, "FLUORINE // FORM", COLOR_TEXT);

    int field_x = x + 48;
    int field_w = 400;
    int field_h = 32;
    const char *titles[3] = {"1. ENTER USERNAME:", "2. ACCESS CODE:", "3. NOTES:"};
    const char *samples[3] = {"vanta.01", "Q-4802", "clear signal"};

    for (int i = 0; i < 3; i++) {
        int row_y = y + 96 + i * 96;
        ext_draw_string(x + 54, row_y, titles[i], COLOR_BEVEL_DARK);

        int field_y = row_y + 20;
        ext_draw_rect_filled(field_x, field_y, field_w, field_h, COLOR_LI_BG);
        ext_draw_line(field_x, field_y, field_x + field_w - 1, field_y, COLOR_BEVEL_DARK);
        ext_draw_line(field_x, field_y, field_x, field_y + field_h - 1, COLOR_BEVEL_DARK);
        ext_draw_line(field_x + field_w - 1, field_y, field_x + field_w - 1, field_y + field_h - 1, COLOR_WHITE);
        ext_draw_line(field_x, field_y + field_h - 1, field_x + field_w - 1, field_y + field_h - 1, COLOR_WHITE);
        ext_draw_string(field_x + 10, field_y + 10, samples[i], COLOR_TEXT);
    }
}

void show_silicon_book_app(void) {
    ext_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 150;
    int y = 90;
    int w = 720;
    int h = 540;
    ext_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    ext_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    int sidebar_w = 200;
    ext_draw_rect_filled(x, y, sidebar_w, h, COLOR_HE_BG);
    ext_draw_string(x + 16, y + 28, "SILICON // NOTES", COLOR_TEXT);
    ext_draw_string(x + 24, y + 58, "▸ Chapter 1: Core Docs", COLOR_TEXT);
    ext_draw_string(x + 36, y + 76, "  ▪ Planner / To-Do", COLOR_TEXT);
    ext_draw_string(x + 24, y + 96, "▸ Chapter 2: Schematics", COLOR_TEXT);

    int content_x = x + sidebar_w;
    int content_y = y + 16;
    int content_w = w - sidebar_w - 32;
    int content_h = h - 32;
    ext_draw_rect_filled(content_x, content_y, content_w, content_h, COLOR_WHITE);
    ext_draw_string(content_x + 24, content_y + 24, "Knowledge is stored in layers.", COLOR_TEXT);
    ext_draw_string(content_x + 24, content_y + 42, "Each note carries structure, intent,", COLOR_TEXT);
    ext_draw_string(content_x + 24, content_y + 60, "and the quiet persistence of time.", COLOR_TEXT);
    ext_draw_string(content_x + 24, content_y + 88, "The archive remains open and ready.", COLOR_TEXT);
    ext_draw_string(content_x + 24, content_y + 118, "Planner / Notes / Drafts are arranged", COLOR_TEXT);
    ext_draw_string(content_x + 24, content_y + 136, "in a calm, navigable continuum.", COLOR_TEXT);
}
