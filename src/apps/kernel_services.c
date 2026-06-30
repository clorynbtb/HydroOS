#include <stdint.h>
#include <stddef.h>

#include "../include/colors.h"
#include "../include/graphics.h"
#include "../include/elements.h"

static void kernel_draw_pixel(int x, int y, uint32_t color) {
    draw_pixel(x, y, color);
}

static void kernel_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 >= x1) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 >= y1) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        kernel_draw_pixel(x1, y1, color);
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

static void kernel_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            kernel_draw_pixel(px, py, color);
        }
    }
}

static void kernel_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    kernel_draw_line(x, y, x + w - 1, y, color);
    kernel_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    kernel_draw_line(x, y, x, y + h - 1, color);
    kernel_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

static void kernel_draw_string(int x, int y, const char *str, uint32_t color) {
    if (str == NULL) {
        return;
    }

    int cursor_x = x;
    char buffer[2] = {'\0', '\0'};

    while (*str) {
        if (*str == '\n') {
            y += 10;
            cursor_x = x;
        } else if (*str == '_') {
            for (int px = cursor_x + 1; px < cursor_x + 7; px++) {
                kernel_draw_pixel(px, y + 6, color);
            }
            cursor_x += 8;
        } else {
            buffer[0] = *str;
            draw_mac_string(cursor_x, y, buffer, color);
            cursor_x += 8;
        }
        str++;
    }
}

static void kernel_draw_window(int x, int y, int w, int h, uint32_t bg_color) {
    kernel_draw_rect_filled(x, y, w, h, bg_color);
    kernel_draw_rect_outline(x, y, w, h, COLOR_BORDER);
}

void show_rutherfordium_scheduler(void) {
    const int x = 150;
    const int y = 120;
    const int w = 650;
    const int h = 400;

    kernel_draw_window(x, y, w, h, COLOR_WHITE);
    kernel_draw_string(x + 18, y + 18, "ELEMENT 104: RUTHERFORDIUM SCHEDULER", COLOR_TEXT);

    const int first_bar_y = y + 90;
    const int bar_height = 28;
    const int bar_x_start = 250;
    const int bar_x_end = 550;
    const int gantt_bar_width = bar_x_end - bar_x_start;

    kernel_draw_string(x + 20, first_bar_y - 6, "CORE_CLK", COLOR_TEXT);
    kernel_draw_rect_filled(bar_x_start, first_bar_y, gantt_bar_width, bar_height, COLOR_TEXT);

    const int second_bar_y = first_bar_y + 70;
    kernel_draw_string(x + 20, second_bar_y - 6, "DOCKER_D", COLOR_TEXT);
    for (int segment = 0; segment < 5; segment++) {
        const int segment_x = bar_x_start + segment * 52;
        kernel_draw_rect_filled(segment_x, second_bar_y, 36, bar_height, COLOR_TEXT);
    }

    const int third_bar_y = second_bar_y + 70;
    kernel_draw_string(x + 20, third_bar_y - 6, "UI_RENDER", COLOR_TEXT);
    kernel_draw_rect_filled(bar_x_end - 80, third_bar_y, 70, bar_height, COLOR_TEXT);
}

void show_polonium_log_app(void) {
    const int x = 200;
    const int y = 90;
    const int w = 500;
    const int h = 500;

    kernel_draw_window(x, y, w, h, COLOR_DB_BG);
    kernel_draw_string(x + 20, y + 18, "POLONIUM LOG DAEMON", COLOR_TEXT);

    const int content_x = x + 20;
    int row_y = y + 70;
    const int entry_height = 30;
    const int entry_width = w - 40;

    kernel_draw_rect_filled(content_x, row_y, entry_width, entry_height, COLOR_HE_BG);
    kernel_draw_string(content_x + 8, row_y + 8,
                       "00:01:23 [INFO] INITIALIZING OXYGEN GRAPHICS ENGINE... SUCCESS",
                       COLOR_TEXT);

    row_y += entry_height + 18;
    kernel_draw_rect_filled(content_x, row_y, entry_width, entry_height, COLOR_HE_BG);
    kernel_draw_string(content_x + 8, row_y + 8,
                       "00:01:25 [WARN] SILICON REGISTER 0x0F IS UNALIGNED",
                       COLOR_TEXT);

    row_y += entry_height + 18;
    kernel_draw_rect_filled(content_x, row_y, entry_width, entry_height, COLOR_HE_BG);
    kernel_draw_string(content_x + 8, row_y + 8,
                       "00:01:26 [INFO] DUBNIUM LAUNCHED CONTAINER: SODIUM_CORE",
                       COLOR_TEXT);
}

void show_technetium_debugger(void) {
    const int x = 120;
    const int y = 100;
    const int w = 740;
    const int h = 460;

    kernel_draw_window(x, y, w, h, COLOR_WHITE);
    kernel_draw_string(x + 20, y + 18, "ELEMENT 43: TECHNETIUM DEBUGGER", COLOR_TEXT);

    const int left_x = x + 20;
    const int left_width = 500;
    const int separator_x = left_x + left_width;
    const int right_x = separator_x + 20;

    kernel_draw_line(separator_x, y + 12, separator_x, y + h - 12, COLOR_BORDER);

    const char *hex_rows[] = {
        "0000: 7F 45 4C 46 01 01 01 00 00 00 00 00 00 00 00 00",
        "0010: 02 63 01 00 00 00 00 00 00 00 00 00 00 00 00 00",
        "0020: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
        "0030: 68 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
        "0040: 48 65 6C 6C 6F 20 48 59 44 52 4F 53 00 00 00 00",
        "0050: 50 41 53 53 00 00 00 00 00 00 00 00 00 00 00 00"
    };

    const char *ascii_rows[] = {
        ".ELF.... .... ....",
        ".c........ ......",
        "................",
        "h...............",
        "Hello HYDROS....",
        "PASS............"
    };

    int row_y = y + 60;
    for (int i = 0; i < 6; i++) {
        kernel_draw_string(left_x, row_y, hex_rows[i], COLOR_TEXT);
        kernel_draw_string(right_x, row_y, ascii_rows[i], COLOR_TEXT);
        row_y += 18;
    }
}

void show_gallium_io_app(void) {
    const int x = 220;
    const int y = 140;
    const int w = 550;
    const int h = 380;

    kernel_draw_window(x, y, w, h, COLOR_HE_BG);
    kernel_draw_string(x + 20, y + 18, "ELEMENT 31: GALLIUM I/O BUS", COLOR_TEXT);

    const int wave_x = x + 30;
    const int keyboard_y = y + 90;
    const int mouse_y = y + 190;
    const int pulse_height = 14;

    kernel_draw_string(wave_x, keyboard_y - 18, "KEYBOARD IRQ1", COLOR_TEXT);
    kernel_draw_rect_filled(wave_x, keyboard_y + pulse_height, 20, 2, COLOR_BORDER);
    kernel_draw_rect_filled(wave_x, keyboard_y, 36, pulse_height, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 36, keyboard_y + pulse_height, 16, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 52, keyboard_y + pulse_height, 16, 2, COLOR_BORDER);
    kernel_draw_rect_filled(wave_x + 52, keyboard_y, 24, pulse_height, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 76, keyboard_y + pulse_height, 24, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 100, keyboard_y + pulse_height, 20, 2, COLOR_BORDER);
    kernel_draw_rect_filled(wave_x + 100, keyboard_y, 32, pulse_height, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 132, keyboard_y + pulse_height, 28, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 132, keyboard_y, 16, pulse_height, COLOR_TEXT);

    kernel_draw_string(wave_x, mouse_y - 18, "MOUSE IRQ12", COLOR_TEXT);
    kernel_draw_rect_filled(wave_x, mouse_y + (pulse_height / 2), 220, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 220, mouse_y + (pulse_height / 2) + 2, 60, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 280, mouse_y + (pulse_height / 2), 120, 2, COLOR_TEXT);
    kernel_draw_rect_filled(wave_x + 400, mouse_y + (pulse_height / 2), 60, 2, COLOR_TEXT);
}
