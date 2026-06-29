#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void social_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void social_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        social_draw_pixel(x1, y1, color);
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

static void social_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            social_draw_pixel(px, py, color);
        }
    }
}

static void social_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    social_draw_line(x, y, x + w - 1, y, color);
    social_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    social_draw_line(x, y, x, y + h - 1, color);
    social_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void social_draw_char(int x, int y, char ch, uint32_t color) {
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
                social_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void social_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }
    int cursor_x = x;
    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else {
            social_draw_char(cursor_x, y, *str, color);
            cursor_x += 8;
        }
        str++;
    }
}

static void social_draw_button(int x, int y, int w, int h, const char *label, uint32_t color) {
    draw_retro_button(x, y, w, h, color);
    social_draw_string(x + 8, y + 10, label, COLOR_TEXT);
}

void show_helium_video_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 100;
    int y = 100;
    int w = 824;
    int h = 500;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    social_draw_string(x + 24, y + 24, "HELIUM // CINEMA SCOPE", COLOR_TEXT);

    int player_x = x + 26;
    int player_y = y + 70;
    int player_w = 500;
    int player_h = 300;
    social_draw_rect_filled(player_x, player_y, player_w, player_h, 0xFF3E2723);
    social_draw_rect_outline(player_x, player_y, player_w, player_h, COLOR_BORDER);

    social_draw_rect_filled(player_x + 30, player_y + 70, 140, 80, COLOR_WHITE);
    social_draw_rect_outline(player_x + 30, player_y + 70, 140, 80, COLOR_BORDER);
    social_draw_string(player_x + 44, player_y + 92, "FRAME", COLOR_TEXT);

    social_draw_rect_filled(player_x + 112, player_y + 160, 120, 70, COLOR_LI_BG);
    social_draw_rect_outline(player_x + 112, player_y + 160, 120, 70, COLOR_BORDER);
    social_draw_string(player_x + 130, player_y + 184, "PLAY", COLOR_TEXT);

    social_draw_rect_filled(player_x + 40, player_y + 250, 420, 16, COLOR_BORDER);
    social_draw_rect_filled(player_x + 40, player_y + 250, 180, 16, COLOR_TEXT);
    social_draw_string(player_x + 50, player_y + 268, "00:12 / 01:05", COLOR_TEXT);

    int side_x = x + 550;
    int side_y = y + 70;
    int side_w = 240;
    int side_h = 360;
    social_draw_rect_filled(side_x, side_y, side_w, side_h, COLOR_HE_BG);
    social_draw_rect_outline(side_x, side_y, side_w, side_h, COLOR_BORDER);
    social_draw_string(side_x + 18, side_y + 18, "SUGGESTED", COLOR_TEXT);

    for (int i = 0; i < 4; i++) {
        int card_y = side_y + 48 + i * 72;
        social_draw_rect_filled(side_x + 18, card_y, side_w - 36, 48, COLOR_WHITE);
        social_draw_rect_outline(side_x + 18, card_y, side_w - 36, 48, COLOR_BORDER);
        social_draw_string(side_x + 28, card_y + 14, "CLIP", COLOR_TEXT);
        social_draw_string(side_x + 28, card_y + 30, "#", COLOR_BEVEL_MID);
    }
}

void show_lithium_chat_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 200;
    int y = 120;
    int w = 600;
    int h = 480;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    int side_w = 160;
    social_draw_rect_filled(x, y, side_w, h, COLOR_HE_BG);
    social_draw_string(x + 18, y + 24, "#general", COLOR_TEXT);
    social_draw_string(x + 18, y + 44, "#family", COLOR_TEXT);
    social_draw_string(x + 18, y + 64, "#dev", COLOR_TEXT);

    int content_x = x + side_w;
    int content_y = y + 16;
    social_draw_rect_filled(content_x, content_y, w - side_w - 16, h - 32, COLOR_WHITE);

    social_draw_rect_filled(content_x + 24, content_y + 44, 180, 30, COLOR_WHITE);
    social_draw_rect_outline(content_x + 24, content_y + 44, 180, 30, COLOR_BORDER);
    social_draw_string(content_x + 34, content_y + 56, "hello there", COLOR_TEXT);

    social_draw_rect_filled(content_x + 140, content_y + 112, 180, 30, COLOR_DESKTOP);
    social_draw_rect_outline(content_x + 140, content_y + 112, 180, 30, COLOR_BORDER);
    social_draw_string(content_x + 150, content_y + 124, "ready to sync", COLOR_TEXT);

    social_draw_rect_filled(content_x + 24, content_y + 180, 220, 30, COLOR_WHITE);
    social_draw_rect_outline(content_x + 24, content_y + 180, 220, 30, COLOR_BORDER);
    social_draw_string(content_x + 34, content_y + 192, "send the latest", COLOR_TEXT);
}

void show_potassium_classroom(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 120;
    int y = 90;
    int w = 750;
    int h = 520;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    social_draw_string(x + 24, y + 24, "POTASSIUM // CLASSROOM", COLOR_TEXT);

    social_draw_rect_filled(x + 40, y + 70, w - 80, 280, 0xFF3E2723);
    social_draw_rect_outline(x + 40, y + 70, w - 80, 280, COLOR_BORDER);
    social_draw_string(x + 58, y + 96, "int main() {", COLOR_WHITE);
    social_draw_string(x + 58, y + 116, "  return 0;", COLOR_WHITE);
    social_draw_string(x + 58, y + 136, "// note: keep it simple", COLOR_WHITE);
    social_draw_string(x + 58, y + 156, "x = sin(theta) + 1", COLOR_WHITE);

    int seat_y = y + 384;
    for (int i = 0; i < 8; i++) {
        int sx = x + 48 + i * 76;
        social_draw_rect_filled(sx, seat_y, 40, 40, COLOR_HE_BG);
        social_draw_rect_outline(sx, seat_y, 40, 40, COLOR_BORDER);
    }
}

void show_magnesium_family(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 180;
    int y = 140;
    int w = 640;
    int h = 440;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    social_draw_string(x + 24, y + 24, "MAGNESIUM // FAMILY TREE", COLOR_TEXT);

    int mid_x = x + 320;
    int root_y = y + 120;
    social_draw_line(mid_x, root_y, mid_x, root_y + 70, COLOR_TEXT);
    social_draw_line(mid_x, root_y + 70, mid_x - 90, root_y + 120, COLOR_TEXT);
    social_draw_line(mid_x, root_y + 70, mid_x + 90, root_y + 120, COLOR_TEXT);
    social_draw_button(mid_x - 92, root_y + 120, 90, 34, "MOTHER", COLOR_WHITE);
    social_draw_button(mid_x + 8, root_y + 120, 90, 34, "FATHER", COLOR_WHITE);
    social_draw_button(mid_x - 50, root_y + 180, 100, 34, "ME", COLOR_WHITE);
}

void show_iodine_map_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 150;
    int y = 100;
    int w = 700;
    int h = 500;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    for (int i = 0; i < 10; i++) {
        int gx = x + 30 + i * 60;
        social_draw_line(gx, y + 40, gx, y + h - 40, COLOR_BORDER);
    }
    for (int i = 0; i < 8; i++) {
        int gy = y + 40 + i * 60;
        social_draw_line(x + 30, gy, x + w - 30, gy, COLOR_BORDER);
    }

    social_draw_line(x + 120, y + 140, x + 220, y + 210, COLOR_BEVEL_DARK);
    social_draw_line(x + 220, y + 210, x + 320, y + 180, COLOR_BEVEL_DARK);
    social_draw_line(x + 320, y + 180, x + 420, y + 260, COLOR_BEVEL_DARK);
    social_draw_line(x + 420, y + 260, x + 520, y + 220, COLOR_BEVEL_DARK);
    social_draw_line(x + 520, y + 220, x + 580, y + 300, COLOR_BEVEL_DARK);

    int cx = x + 350;
    int cy = y + 250;
    social_draw_line(cx - 18, cy, cx + 18, cy, COLOR_TEXT);
    social_draw_line(cx, cy - 18, cx, cy + 18, COLOR_TEXT);
    social_draw_string(cx + 28, cy - 6, "YOU ARE HERE", COLOR_TEXT);
    social_draw_string(cx + 28, cy + 12, "(NODE_53)", COLOR_TEXT);
}

void show_silver_translate_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 220;
    int y = 180;
    int w = 580;
    int h = 360;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);

    int pane_w = 250;
    int pane_h = 240;
    int left_x = x + 28;
    int right_x = x + w - pane_w - 28;
    int pane_y = y + 60;

    social_draw_rect_filled(left_x, pane_y, pane_w, pane_h, COLOR_HE_BG);
    social_draw_rect_outline(left_x, pane_y, pane_w, pane_h, COLOR_BORDER);
    social_draw_string(left_x + 18, pane_y + 24, "original text", COLOR_TEXT);
    social_draw_string(left_x + 18, pane_y + 44, "the quiet room", COLOR_TEXT);

    social_draw_rect_filled(right_x, pane_y, pane_w, pane_h, COLOR_LI_BG);
    social_draw_rect_outline(right_x, pane_y, pane_w, pane_h, COLOR_BORDER);
    social_draw_string(right_x + 18, pane_y + 24, "translated text", COLOR_TEXT);
    social_draw_string(right_x + 18, pane_y + 44, "la salle silencieuse", COLOR_TEXT);

    social_draw_button(x + w / 2 - 32, pane_y + 90, 64, 32, "[ ⇄ ]", COLOR_WHITE);
}

void show_tungsten_journal_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 260;
    int y = 80;
    int w = 500;
    int h = 560;
    social_draw_rect_filled(x, y, w, h, 0xFFE0D4C3);
    social_draw_rect_outline(x, y, w, h, COLOR_BEVEL_DARK);
    social_draw_rect_outline(x + 8, y + 8, w - 16, h - 16, COLOR_BEVEL_DARK);

    social_draw_string(x + 24, y + 40, "JUNE 2026", COLOR_TEXT);
    social_draw_string(x + 24, y + 72, "the room held the hush of a clock.", COLOR_TEXT);
    social_draw_string(x + 24, y + 92, "i wrote by a pale lamp and listened", COLOR_TEXT);
    social_draw_string(x + 24, y + 112, "to the house breathe around me.", COLOR_TEXT);
    social_draw_string(x + 24, y + 132, "notes gathered like dust and memory.", COLOR_TEXT);
}

void show_lead_fonts_app(void) {
    social_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 300;
    int y = 150;
    int w = 450;
    int h = 420;
    social_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    social_draw_rect_outline(x, y, w, h, COLOR_BORDER);
    social_draw_string(x + 24, y + 24, "LEAD // FONT SPECIMEN", COLOR_TEXT);

    const char *sample = "HYDROOS PIXEL TYPE";
    int sizes[4] = {8, 12, 16, 24};
    for (int i = 0; i < 4; i++) {
        int py = y + 70 + i * 80;
        social_draw_string(x + 24, py, sample, COLOR_TEXT);
    }
}
