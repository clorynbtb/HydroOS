#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static uint32_t *g_util_fb = NULL;
static uint64_t g_util_width = 1024;
static uint64_t g_util_height = 768;
static uint64_t g_util_pitch = 4096;

static void util_draw_pixel(int x, int y, uint32_t color) {
    if (g_util_fb == NULL) {
        return;
    }
    if (x < 0 || x >= (int)g_util_width || y < 0 || y >= (int)g_util_height) {
        return;
    }
    size_t index = (size_t)y * (g_util_pitch / 4u) + (size_t)x;
    g_util_fb[index] = color;
}

static void util_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        util_draw_pixel(x1, y1, color);
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

static void util_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            util_draw_pixel(px, py, color);
        }
    }
}

static void util_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }

    int cursor_x = x;
    while (*str) {
        unsigned char ch = (unsigned char)*str;
        if (ch >= 128) {
            ch = '?';
        }

        if (ch == '\n') {
            y += 8;
            cursor_x = x;
        } else {
            for (int row = 0; row < 8; row++) {
                uint8_t data = 0;
                switch (ch) {
                    case 'A': data = 0x1c; break;
                    case 'B': data = 0x7e; break;
                    case 'C': data = 0x3e; break;
                    case 'D': data = 0x7c; break;
                    case 'E': data = 0x7f; break;
                    case 'F': data = 0x7f; break;
                    case 'G': data = 0x3e; break;
                    case 'H': data = 0x63; break;
                    case 'I': data = 0x3e; break;
                    case 'L': data = 0x60; break;
                    case 'M': data = 0x63; break;
                    case 'N': data = 0x63; break;
                    case 'O': data = 0x3e; break;
                    case 'P': data = 0x7e; break;
                    case 'R': data = 0x7e; break;
                    case 'S': data = 0x3e; break;
                    case 'T': data = 0x7f; break;
                    case 'U': data = 0x63; break;
                    case 'W': data = 0x63; break;
                    case 'X': data = 0x63; break;
                    case 'Y': data = 0x63; break;
                    case 'Z': data = 0x7f; break;
                    case ' ': data = 0x00; break;
                    case ':': data = 0x18; break;
                    case '/': data = 0x06; break;
                    case '-': data = 0x7e; break;
                    case '.': data = 0x18; break;
                    case '0': data = 0x3e; break;
                    case '1': data = 0x0c; break;
                    case '2': data = 0x3e; break;
                    case '3': data = 0x7f; break;
                    case '4': data = 0x0c; break;
                    case '5': data = 0x7f; break;
                    case '6': data = 0x1c; break;
                    case '7': data = 0x7f; break;
                    case '8': data = 0x3e; break;
                    case '9': data = 0x3e; break;
                    default: data = 0x00; break;
                }
                for (int col = 0; col < 8; col++) {
                    if ((data >> (7 - col)) & 1) {
                        util_draw_pixel(cursor_x + col, y + row, color);
                    }
                }
            }
            cursor_x += 8;
        }
        str++;
    }
}

void show_nitrogen_browser_app(void) {
    util_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 70, y = 80, w = 884, h = 560;
    util_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    util_draw_line(x, y, x + w - 1, y, COLOR_BORDER);
    util_draw_line(x, y + h - 1, x + w - 1, y + h - 1, COLOR_BORDER);
    util_draw_line(x, y, x, y + h - 1, COLOR_BORDER);
    util_draw_line(x + w - 1, y, x + w - 1, y + h - 1, COLOR_BORDER);

    util_draw_rect_filled(120, 120, 784, 32, COLOR_WHITE);
    util_draw_line(120, 120, 904, 120, COLOR_BORDER);
    util_draw_line(120, 152, 904, 152, COLOR_BORDER);
    util_draw_line(120, 120, 120, 152, COLOR_BORDER);
    util_draw_line(904, 120, 904, 152, COLOR_BORDER);
    util_draw_string(132, 130, "nitrogen://web.network", COLOR_TEXT);

    draw_retro_button(100, 120, 28, 28, COLOR_HE_BG);
    draw_retro_button(136, 120, 28, 28, COLOR_LI_BG);
    util_draw_string(108, 130, "<", COLOR_TEXT);
    util_draw_string(144, 130, ">", COLOR_TEXT);

    util_draw_rect_filled(120, 182, 784, 340, COLOR_HE_BG);
    util_draw_string(142, 210, "NETWORK // FLUID STREAM", COLOR_TEXT);
    util_draw_string(142, 230, "signal: stable", COLOR_BEVEL_MID);
    util_draw_string(142, 260, "protocol: http/2", COLOR_BEVEL_MID);
    util_draw_string(142, 290, "latency: low", COLOR_BEVEL_MID);
}

void show_gold_secure_app(void) {
    util_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 150, y = 90, w = 700, h = 520;
    util_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    util_draw_line(x, y, x + w - 1, y, 0xFFD9B48F);
    util_draw_line(x + w - 1, y, x + w - 1, y + h - 1, 0xFFD9B48F);
    util_draw_line(x, y, x, y + h - 1, 0xFFD9B48F);
    util_draw_line(x, y + h - 1, x + w - 1, y + h - 1, 0xFFD9B48F);

    util_draw_rect_filled(x + 20, y + 20, w - 40, 140, COLOR_HE_BG);
    util_draw_string(x + 36, y + 42, "VPN STATUS", COLOR_TEXT);
    draw_retro_button(x + 36, y + 78, 68, 24, COLOR_LI_BG);
    util_draw_rect_filled(x + 102, y + 84, 12, 12, 0xFFB8C9A0);
    util_draw_string(x + 126, y + 82, "SECURE TUNNEL ACTIVE", COLOR_TEXT);

    util_draw_rect_filled(x + 20, y + 190, w - 40, 280, COLOR_HE_BG);
    util_draw_string(x + 36, y + 214, "MAIL INBOX", COLOR_TEXT);
    util_draw_line(x + 36, y + 244, x + w - 36, y + 244, COLOR_BORDER);
    util_draw_line(x + 36, y + 274, x + w - 36, y + 274, COLOR_BORDER);
    util_draw_line(x + 36, y + 304, x + w - 36, y + 304, COLOR_BORDER);
    util_draw_string(x + 36, y + 256, "security@vault", COLOR_TEXT);
    util_draw_string(x + 36, y + 286, "ops@relay", COLOR_TEXT);
    util_draw_string(x + 36, y + 316, "audit@archive", COLOR_TEXT);
}

void show_zinc_planner_app(void) {
    util_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 70, y = 70, w = 884, h = 560;
    util_draw_rect_filled(x, y, w, h, COLOR_WHITE);
    util_draw_line(x, y, x + w - 1, y, COLOR_BORDER);
    util_draw_line(x, y + h - 1, x + w - 1, y + h - 1, COLOR_BORDER);
    util_draw_line(x, y, x, y + h - 1, COLOR_BORDER);
    util_draw_line(x + w - 1, y, x + w - 1, y + h - 1, COLOR_BORDER);

    int col_w = 200;
    for (int i = 0; i < 3; i++) {
        int cx = x + 20 + i * col_w;
        util_draw_rect_filled(cx, y + 20, 180, 36, COLOR_LI_BG);
        const char *title = (i == 0) ? "BACKLOG" : (i == 1) ? "IN FLIGHT" : "ARCHIVED";
        util_draw_string(cx + 18, y + 32, title, COLOR_TEXT);

        for (int row = 0; row < 3; row++) {
            int card_y = y + 70 + row * 60;
            util_draw_rect_filled(cx + 8, card_y, 164, 36, COLOR_HE_BG);
            util_draw_string(cx + 16, card_y + 12, "TODO", COLOR_TEXT);
        }
    }
}

