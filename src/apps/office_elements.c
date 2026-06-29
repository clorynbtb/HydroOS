#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static uint32_t *g_app_fb = NULL;
static uint64_t g_app_width = 1024;
static uint64_t g_app_height = 768;
static uint64_t g_app_pitch = 4096;

static void app_draw_pixel(int x, int y, uint32_t color) {
    if (g_app_fb == NULL) {
        return;
    }
    if (x < 0 || x >= (int)g_app_width || y < 0 || y >= (int)g_app_height) {
        return;
    }
    size_t index = (size_t)y * (g_app_pitch / 4u) + (size_t)x;
    g_app_fb[index] = color;
}

static void app_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        app_draw_pixel(x1, y1, color);
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

static void app_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            app_draw_pixel(px, py, color);
        }
    }
}

static void app_draw_string(int x, int y, const char *str, uint32_t color) {
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
                    default: data = 0x00; break;
                }
                for (int col = 0; col < 8; col++) {
                    if ((data >> (7 - col)) & 1) {
                        app_draw_pixel(cursor_x + col, y + row, color);
                    }
                }
            }
            cursor_x += 8;
        }
        str++;
    }
}

static void app_draw_grid(int x, int y, int w, int h, uint32_t color) {
    for (int i = x; i <= x + w; i += 16) {
        app_draw_line(x, y, i, y + h, color);
    }
    for (int j = y; j <= y + h; j += 16) {
        app_draw_line(x, y, x + w, j, color);
    }
}

void show_iron_app(void) {
    app_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 90, y = 60, w = 560, h = 420;
    app_draw_rect_filled(x, y, w, h, 0xFFDDDDDD);
    for (int i = 0; i < 2; i++) {
        app_draw_line(x + i, y + i, x + w - 1 - i, y + i, 0xFF555555);
        app_draw_line(x + i, y + i, x + i, y + h - 1 - i, 0xFF555555);
        app_draw_line(x + w - 1 - i, y + i, x + w - 1 - i, y + h - 1 - i, 0xFF555555);
        app_draw_line(x + i, y + h - 1 - i, x + w - 1 - i, y + h - 1 - i, 0xFF555555);
    }

    app_draw_rect_filled(x, y, w, 24, 0xFF6B6B6B);
    app_draw_string(x + 16, y + 8, "ELEMENT 26: IRON", COLOR_WHITE);

    app_draw_grid(x + 18, y + 56, w - 36, h - 74, 0xFF444444);
    app_draw_string(x + 20, y + 36, "SHEET // TECHNICAL", 0xFF333333);
}

void show_oxygen_app(void) {
    app_draw_rect_filled(0, 0, 1024, 768, COLOR_DESKTOP);

    int x = 130, y = 80, w = 760, h = 520;
    app_draw_rect_filled(x, y, w, h, 0xFFF8F8F8);
    app_draw_string(140, 40, "OXYGEN // PRESENTATION MODE", 0xFF8D6E63);

    app_draw_rect_filled(x + 70, y + 90, 620, 150, COLOR_WHITE);
    app_draw_rect_filled(x + 70, y + 280, 620, 150, COLOR_WHITE);
    app_draw_rect_filled(x + 70, y + 470, 620, 150, COLOR_WHITE);

    app_draw_string(x + 110, y + 132, "SLIDE 01", COLOR_TEXT);
    app_draw_string(x + 110, y + 322, "SLIDE 02", COLOR_TEXT);
    app_draw_string(x + 110, y + 512, "SLIDE 03", COLOR_TEXT);
}

void show_carbon_app(void) {
    app_draw_rect_filled(0, 0, 1024, 768, 0xFFF3E6D0);

    int x = 100, y = 70, w = 620, h = 520;
    app_draw_rect_filled(x, y, w, h, 0xFFF7EEDC);
    app_draw_line(x, y, x + w - 1, y, 0xFF8D6E63);
    app_draw_line(x, y + h - 1, x + w - 1, y + h - 1, 0xFF8D6E63);
    app_draw_line(x, y, x, y + h - 1, 0xFF8D6E63);
    app_draw_line(x + w - 1, y, x + w - 1, y + h - 1, 0xFF8D6E63);

    app_draw_string(x + 24, y + 20, "carbon manuscript", 0xFF7C4D3A);
    app_draw_string(x + 24, y + 42, "the ink of memory and breath.", 0xFF8D6E63);

    app_draw_string(x + 24, y + 88, "The page is a field of pressure and softness.", 0xFF3E2723);
    app_draw_string(x + 24, y + 104, "The letters settle like ash on warm paper.", 0xFF3E2723);
    app_draw_string(x + 24, y + 120, "The voice of carbon is old, patient, and alive.", 0xFF3E2723);
    app_draw_string(x + 24, y + 136, "Its script is quiet, but it carries weight.", 0xFF3E2723);
}

void show_carbon_app(void);
void show_boron_app(void);
void show_titanium_app(void);
