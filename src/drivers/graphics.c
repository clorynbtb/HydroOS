#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"

static uint32_t *g_fb_addr = NULL;
static uint64_t g_fb_width = 1024;
static uint64_t g_fb_height = 768;
static uint64_t g_fb_pitch = 4096;

static void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
static void blend_pixel(int x, int y, uint32_t src_color);
static void draw_circle_filled(int cx, int cy, int radius, uint32_t color);
static int inside_rounded_corner(int x, int y, int rect_x, int rect_y, int radius);
static void draw_rounded_rect_filled(int x, int y, int w, int h, int radius, uint32_t color);
static void draw_rounded_rect_filled_alpha(int x, int y, int w, int h, int radius, uint32_t color);

static const uint8_t font_8x8[128][8] = {
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    [33] = {0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
    [35] = {0x36, 0x36, 0x7f, 0x36, 0x7f, 0x36, 0x36, 0x00},
    [36] = {0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00},
    [37] = {0x00, 0x66, 0x66, 0x10, 0x20, 0x66, 0x66, 0x00},
    [40] = {0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00},
    [41] = {0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00},
    [45] = {0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00},
    [46] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
    [48] = {0x3e, 0x63, 0x67, 0x6f, 0x7b, 0x63, 0x3e, 0x00},
    [49] = {0x0c, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00},
    [50] = {0x3e, 0x63, 0x06, 0x1c, 0x30, 0x63, 0x7f, 0x00},
    [51] = {0x7f, 0x06, 0x0c, 0x1c, 0x06, 0x63, 0x3e, 0x00},
    [52] = {0x0c, 0x1c, 0x3c, 0x6c, 0x7f, 0x0c, 0x0c, 0x00},
    [53] = {0x7f, 0x60, 0x7e, 0x03, 0x03, 0x63, 0x3e, 0x00},
    [54] = {0x1c, 0x30, 0x60, 0x7e, 0x63, 0x63, 0x3e, 0x00},
    [55] = {0x7f, 0x03, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00},
    [56] = {0x3e, 0x63, 0x63, 0x3e, 0x63, 0x63, 0x3e, 0x00},
    [57] = {0x3e, 0x63, 0x63, 0x7f, 0x03, 0x06, 0x3c, 0x00},
    [65] = {0x1c, 0x36, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00},
    [66] = {0x7e, 0x63, 0x63, 0x7e, 0x63, 0x63, 0x7e, 0x00},
    [67] = {0x3e, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3e, 0x00},
    [68] = {0x7c, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7c, 0x00},
    [69] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7f, 0x00},
    [70] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00},
    [71] = {0x3e, 0x63, 0x60, 0x6f, 0x63, 0x63, 0x3e, 0x00},
    [72] = {0x63, 0x63, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00},
    [73] = {0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00},
    [76] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7f, 0x00},
    [77] = {0x63, 0x77, 0x7f, 0x6b, 0x63, 0x63, 0x63, 0x00},
    [78] = {0x63, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 0x00},
    [79] = {0x3e, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00},
    [80] = {0x7e, 0x63, 0x63, 0x7e, 0x60, 0x60, 0x60, 0x00},
    [83] = {0x3e, 0x63, 0x60, 0x3e, 0x03, 0x63, 0x3e, 0x00},
    [85] = {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00},
    [87] = {0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x63, 0x00},
    [88] = {0x63, 0x63, 0x34, 0x1c, 0x34, 0x63, 0x63, 0x00},
    [97] = {0x00, 0x00, 0x3c, 0x03, 0x3d, 0x63, 0x3d, 0x00},
    [98] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x7c, 0x00},
    [99] = {0x00, 0x00, 0x3e, 0x60, 0x60, 0x63, 0x3e, 0x00},
    [100] = {0x03, 0x03, 0x3f, 0x63, 0x63, 0x63, 0x3f, 0x00},
    [101] = {0x00, 0x00, 0x3e, 0x63, 0x7f, 0x60, 0x3e, 0x00},
    [104] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00},
    [105] = {0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00},
    [110] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00},
    [111] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x63, 0x3e, 0x00},
    [114] = {0x00, 0x00, 0x5e, 0x30, 0x30, 0x30, 0x30, 0x00},
    [116] = {0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x0c, 0x00},
    [117] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x3d, 0x00},
    [118] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00},
    [121] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x3f, 0x03, 0x3e},
    [122] = {0x00, 0x00, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x00},
};

void graphics_init(uint32_t *fb_addr, uint64_t width, uint64_t height, uint64_t pitch) {
    g_fb_addr = fb_addr;
    g_fb_width = width;
    g_fb_height = height;
    g_fb_pitch = pitch;
}

void draw_pixel(int x, int y, uint32_t color) {
    if (g_fb_addr == NULL) {
        return;
    }
    if (x < 0 || x >= (int)g_fb_width || y < 0 || y >= (int)g_fb_height) {
        return;
    }

    size_t index = (size_t)y * (g_fb_pitch / 4u) + (size_t)x;
    g_fb_addr[index] = color;
}

static void blend_pixel(int x, int y, uint32_t src_color) {
    if (g_fb_addr == NULL) {
        return;
    }
    if (x < 0 || x >= (int)g_fb_width || y < 0 || y >= (int)g_fb_height) {
        return;
    }

    uint8_t src_a = (src_color >> 24) & 0xFF;
    if (src_a == 0) {
        return;
    }

    size_t index = (size_t)y * (g_fb_pitch / 4u) + (size_t)x;
    uint32_t dst_color = g_fb_addr[index];

    uint8_t dst_r = (dst_color >> 16) & 0xFF;
    uint8_t dst_g = (dst_color >> 8) & 0xFF;
    uint8_t dst_b = dst_color & 0xFF;

    uint8_t src_r = (src_color >> 16) & 0xFF;
    uint8_t src_g = (src_color >> 8) & 0xFF;
    uint8_t src_b = src_color & 0xFF;

    uint32_t inv_a = 255u - src_a;
    uint8_t out_r = (uint8_t)((src_r * src_a + dst_r * inv_a) / 255u);
    uint8_t out_g = (uint8_t)((src_g * src_a + dst_g * inv_a) / 255u);
    uint8_t out_b = (uint8_t)((src_b * src_a + dst_b * inv_a) / 255u);

    g_fb_addr[index] = (0xFFu << 24) | ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | out_b;
}

void draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            draw_pixel(px, py, color);
        }
    }
}

static int inside_rounded_corner(int x, int y, int rect_x, int rect_y, int radius) {
    int dx = rect_x - x;
    int dy = rect_y - y;
    return dx * dx + dy * dy <= radius * radius;
}

static void draw_rounded_rect_filled(int x, int y, int w, int h, int radius, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    if (radius <= 0) {
        draw_rect_filled(x, y, w, h, color);
        return;
    }

    int r = radius;
    int r_sq = r * r;
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    for (int py = y; py <= y2; py++) {
        for (int px = x; px <= x2; px++) {
            int dx = 0;
            int dy = 0;
            if (px < x + r) {
                dx = x + r - px;
            } else if (px > x2 - r) {
                dx = px - (x2 - r);
            }
            if (py < y + r) {
                dy = y + r - py;
            } else if (py > y2 - r) {
                dy = py - (y2 - r);
            }
            if (dx > 0 && dy > 0) {
                if (dx * dx + dy * dy > r_sq) {
                    continue;
                }
            }
            draw_pixel(px, py, color);
        }
    }
}

static void draw_rounded_rect_filled_alpha(int x, int y, int w, int h, int radius, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    if (radius <= 0) {
        for (int py = y; py < y + h; py++) {
            for (int px = x; px < x + w; px++) {
                blend_pixel(px, py, color);
            }
        }
        return;
    }

    int r = radius;
    int r_sq = r * r;
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    for (int py = y; py <= y2; py++) {
        for (int px = x; px <= x2; px++) {
            int dx = 0;
            int dy = 0;
            if (px < x + r) {
                dx = x + r - px;
            } else if (px > x2 - r) {
                dx = px - (x2 - r);
            }
            if (py < y + r) {
                dy = y + r - py;
            } else if (py > y2 - r) {
                dy = py - (y2 - r);
            }
            if (dx > 0 && dy > 0) {
                if (dx * dx + dy * dy > r_sq) {
                    continue;
                }
            }
            blend_pixel(px, py, color);
        }
    }
}

static void draw_circle_filled(int cx, int cy, int radius, uint32_t color) {
    if (radius <= 0) {
        return;
    }

    int r_sq = radius * radius;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= r_sq) {
                draw_pixel(cx + dx, cy + dy, color);
            }
        }
    }
}

void draw_window_shadow(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) {
        return;
    }

    const uint32_t base_color = 0x8D6E63u;
    const uint32_t alpha_base = 0x22u;
    const int spread = 8;
    const int x0 = x - spread;
    const int y0 = y - spread;
    const int x1 = x + w + spread - 1;
    const int y1 = y + h + spread - 1;

    for (int py = y0; py <= y1; py++) {
        for (int px = x0; px <= x1; px++) {
            if (px >= x && px < x + w && py >= y && py < y + h) {
                continue;
            }

            int dx = 0;
            if (px < x) {
                dx = x - px;
            } else if (px >= x + w) {
                dx = px - (x + w - 1);
            }

            int dy = 0;
            if (py < y) {
                dy = y - py;
            } else if (py >= y + h) {
                dy = py - (y + h - 1);
            }

            int dist_sq = dx * dx + dy * dy;
            if (dist_sq >= spread * spread) {
                continue;
            }

            uint32_t alpha = (alpha_base * (spread * spread - dist_sq) + (spread * spread - 1)) / (spread * spread);
            uint32_t shadow = (alpha << 24) | base_color;
            blend_pixel(px, py, shadow);
        }
    }
}

void draw_modern_window(int x, int y, int w, int h, const char *title) {
    if (w <= 0 || h <= 0 || title == NULL) {
        return;
    }

    draw_window_shadow(x, y, w, h);
    draw_rounded_rect_filled(x, y, w, h, 12, COLOR_WHITE);

    const int title_height = 32;
    const int title_y = y + 10;
    const int dot_radius = 4;
    const int dot_spacing = 14;
    const uint32_t red_muted = 0xFFCE8D88u;
    const uint32_t yellow_muted = 0xFFE3C57Au;
    const uint32_t green_muted = 0xFF94B48Bu;

    draw_circle_filled(x + 20, y + title_height / 2, dot_radius, red_muted);
    draw_circle_filled(x + 20 + dot_spacing, y + title_height / 2, dot_radius, yellow_muted);
    draw_circle_filled(x + 20 + 2 * dot_spacing, y + title_height / 2, dot_radius, green_muted);

    draw_mac_string(x + 52, title_y, title, COLOR_TEXT);
}

void draw_warm_dock(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) {
        return;
    }

    const uint32_t glass_color = 0xB2FFFBF5u;
    draw_rounded_rect_filled_alpha(x, y, w, h, 16, glass_color);

    const int icon_size = 46;
    const int icon_radius = 12;
    const int icon_gap = 18;
    const int count = 5;
    const int start_x = x + 20;
    const int icon_y = y + (h - icon_size) / 2;

    for (int i = 0; i < count; i++) {
        int icon_x = start_x + i * (icon_size + icon_gap);
        uint32_t icon_color = (i == 2) ? 0xFFFFFFFFu : 0xFFF5EFE6u;
        draw_rounded_rect_filled(icon_x, icon_y, icon_size, icon_size, icon_radius, icon_color);

        int inner_x = icon_x + 12;
        int inner_y = icon_y + 12;
        draw_rect_filled(inner_x, inner_y, 22, 8, 0xFFD7C5B0u);
        draw_rect_filled(inner_x, inner_y + 18, 22, 8, 0xFFD7C5B0u);
    }
}

void draw_retro_button(int x, int y, int w, int h, uint32_t bg_color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    draw_rounded_rect_filled(x, y, w, h, 10, bg_color);
}

void draw_mac_window(int x, int y, int w, int h, const char *title) {
    draw_modern_window(x, y, w, h, title);
}

void draw_mac_string(int x, int y, const char *str, uint32_t color) {
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
            y += 10;
            cursor_x = x;
        } else {
            for (int row = 0; row < 8; row++) {
                uint8_t data = font_8x8[ch][row];
                for (int col = 0; col < 8; col++) {
                    if ((data >> (7 - col)) & 1) {
                        draw_pixel(cursor_x + col, y + row, color);
                    }
                }
            }
            cursor_x += 8;
        }
        str++;
    }
}

static void draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x1, y1, color);
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
