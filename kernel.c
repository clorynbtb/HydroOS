/**
 * @file kernel.c
 * @brief HydroOS Kernel Core (Hydrogen Core - H) - Vintage Editorial & Notion Minimalist Theme Edition
 * 
 * Architecture: 64-bit Freestanding Kernel (x86_64-none-elf)
 * Bootloader: Limine Protocol v5.x
 * Design System: Vintage Minimalist (Warm Manuscript Cream, Espresso Brown, Ash-Brown & Ivory Panels)
 */

#include <stdint.h>
#include <stddef.h>

/* Specifying framebuffer structure according to the Limine specification */
struct limine_framebuffer {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
};

struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    struct {
        uint64_t count;
        struct limine_framebuffer **framebuffers;
    } *response;
};

#define LIMINE_FRAMEBUFFER_REQUEST { \
    0xc7b1dd30df4c8b88, 0x0a82e7ee10ab4ee6, \
    0x97a3efd2039120af, 0xf6be6cfb1ef96d66  \
    }

static volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

/* 1. BẢNG MÀU VINTAGE MINIMALIST CHUẨN GU THẨM MỸ (ARGB) */
#define COLOR_BG                 0xFFF5EFE6  /* Nền kem ấm hoài cổ, bản thảo giấy cũ */
#define COLOR_TEXT               0xFF3E2723  /* Chữ nâu Espresso đậm tinh tế */
#define COLOR_BORDER             0xFFD7CCC8  /* Viền nâu tro mảnh dẻ 1px */
#define COLOR_PANEL              0xFFFFFFFF  /* Trắng ngà nổi bật cho Menu & Dock */

#define COLOR_HE_BG              0xFFF5F5F5  /* Trắng sữa tĩnh lặng cho [HE] */
#define COLOR_LI_BG              0xFFEFEBE9  /* Nâu Latte rất nhạt hoài niệm cho [LI] */
#define COLOR_DB_BG              0xFFE0D4C3  /* Nâu gỗ sáng/Cát cho [DB] */

/* Các hằng số tương thích ngược để giữ đồng bộ hệ thống */
#define COLOR_BACKGROUND         COLOR_BG
#define COLOR_HE_TEXT            COLOR_TEXT
#define COLOR_LI_TEXT            COLOR_TEXT
#define COLOR_DB_TEXT            COLOR_TEXT
#define COLOR_MENU_BAR           COLOR_PANEL

#define NOTION_COLOR_BG          COLOR_BG
#define NOTION_COLOR_CANVAS      COLOR_BG
#define NOTION_COLOR_TEXT        COLOR_TEXT
#define NOTION_COLOR_BORDER      COLOR_BORDER
#define NOTION_COLOR_TEXT_MUTED  0xFF8D6E63  /* Nâu trung tính mờ dịu mắt */
#define NOTION_COLOR_EMERALD     COLOR_TEXT
#define NOTION_COLOR_AMBER       COLOR_TEXT
#define COLOR_TEXT_MAIN          COLOR_TEXT

/* Biến con trỏ framebuffer toàn cục nhận diện từ Limine */
static uint32_t *g_fb_address = NULL;
static uint64_t g_fb_width = 1024;
static uint64_t g_fb_height = 768;
static uint64_t g_fb_pitch = 4096;

/* Khai báo nguyên mẫu các hàm đồ họa cơ bản */
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect_filled(int x, int y, int w, int h, uint32_t color);
void draw_rect_outline(int x, int y, int w, int h, uint32_t border_color, uint32_t bg_color);
void draw_char(int x, int y, char c, uint32_t color);
void draw_string(int x, int y, const char *str, uint32_t color);
void delay(uint64_t count);

/* 3. BỘ FONT CHỮ PIXEL-ART TỐI GIẢN SẮC NÉT (Monospace 8x8) */
static const uint8_t font_8x8[128][8] = {
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* ' ' */
    [33] = {0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00}, /* '!' */
    [34] = {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '"' */
    [35] = {0x36, 0x36, 0x7f, 0x36, 0x7f, 0x36, 0x36, 0x00}, /* '#' */
    [36] = {0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00}, /* '$' */
    [37] = {0x00, 0x66, 0x66, 0x10, 0x20, 0x66, 0x66, 0x00}, /* '%' */
    [38] = {0x1c, 0x36, 0x1c, 0x3b, 0x6e, 0x3e, 0x00, 0x00}, /* '&' */
    [39] = {0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '\'' */
    [40] = {0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00}, /* '(' */
    [41] = {0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00}, /* ')' */
    [42] = {0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00}, /* '*' */
    [43] = {0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00}, /* '+' */
    [44] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30}, /* ',' */
    [45] = {0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00}, /* '-' */
    [46] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, /* '.' */
    [47] = {0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00}, /* '/' */
    [48] = {0x3e, 0x63, 0x67, 0x6f, 0x7b, 0x63, 0x3e, 0x00}, /* '0' */
    [49] = {0x0c, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00}, /* '1' */
    [50] = {0x3e, 0x63, 0x06, 0x1c, 0x30, 0x63, 0x7f, 0x00}, /* '2' */
    [51] = {0x7f, 0x06, 0x0c, 0x1c, 0x06, 0x63, 0x3e, 0x00}, /* '3' */
    [52] = {0x0c, 0x1c, 0x3c, 0x6c, 0x7f, 0x0c, 0x0c, 0x00}, /* '4' */
    [53] = {0x7f, 0x60, 0x7e, 0x03, 0x03, 0x63, 0x3e, 0x00}, /* '5' */
    [54] = {0x1c, 0x30, 0x60, 0x7e, 0x63, 0x63, 0x3e, 0x00}, /* '6' */
    [55] = {0x7f, 0x03, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00}, /* '7' */
    [56] = {0x3e, 0x63, 0x63, 0x3e, 0x63, 0x63, 0x3e, 0x00}, /* '8' */
    [57] = {0x3e, 0x63, 0x63, 0x7f, 0x03, 0x06, 0x3c, 0x00}, /* '9' */
    [58] = {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, /* ':' */
    [59] = {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, 0x00}, /* ';' */
    [60] = {0x06, 0x0c, 0x18, 0x30, 0x18, 0x0c, 0x06, 0x00}, /* '<' */
    [61] = {0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00}, /* '=' */
    [62] = {0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00}, /* '>' */
    [63] = {0x3e, 0x63, 0x06, 0x0c, 0x18, 0x00, 0x18, 0x00}, /* '?' */
    [64] = {0x3e, 0x63, 0x6f, 0x6f, 0x6e, 0x60, 0x3e, 0x00}, /* '@' */
    [65] = {0x1c, 0x36, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00}, /* 'A' */
    [66] = {0x7e, 0x63, 0x63, 0x7e, 0x63, 0x63, 0x7e, 0x00}, /* 'B' */
    [67] = {0x3e, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3e, 0x00}, /* 'C' */
    [68] = {0x7c, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7c, 0x00}, /* 'D' */
    [69] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7f, 0x00}, /* 'E' */
    [70] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00}, /* 'F' */
    [71] = {0x3e, 0x63, 0x60, 0x6f, 0x63, 0x63, 0x3e, 0x00}, /* 'G' */
    [72] = {0x63, 0x63, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00}, /* 'H' */
    [73] = {0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00}, /* 'I' */
    [74] = {0x07, 0x03, 0x03, 0x03, 0x03, 0x63, 0x3e, 0x00}, /* 'J' */
    [75] = {0x63, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0x63, 0x00}, /* 'K' */
    [76] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7f, 0x00}, /* 'L' */
    [77] = {0x63, 0x77, 0x7f, 0x6b, 0x63, 0x63, 0x63, 0x00}, /* 'M' */
    [78] = {0x63, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 0x00}, /* 'N' */
    [79] = {0x3e, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'O' */
    [80] = {0x7e, 0x63, 0x63, 0x7e, 0x60, 0x60, 0x60, 0x00}, /* 'P' */
    [81] = {0x3e, 0x63, 0x63, 0x63, 0x6b, 0x66, 0x3d, 0x00}, /* 'Q' */
    [82] = {0x7e, 0x63, 0x63, 0x7e, 0x6c, 0x66, 0x63, 0x00}, /* 'R' */
    [83] = {0x3e, 0x63, 0x60, 0x3e, 0x03, 0x63, 0x3e, 0x00}, /* 'S' */
    [84] = {0x7f, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00}, /* 'T' */
    [85] = {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'U' */
    [86] = {0x63, 0x63, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00}, /* 'V' */
    [87] = {0x63, 0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x00}, /* 'W' */
    [88] = {0x63, 0x63, 0x34, 0x1c, 0x34, 0x63, 0x63, 0x00}, /* 'X' */
    [89] = {0x63, 0x63, 0x63, 0x3e, 0x0c, 0x0c, 0x0c, 0x00}, /* 'Y' */
    [90] = {0x7f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x7f, 0x00}, /* 'Z' */
    [91] = {0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00}, /* '[' */
    [92] = {0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00}, /* '\\' */
    [93] = {0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00}, /* ']' */
    [94] = {0x08, 0x1c, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, /* '^' */
    [95] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00}, /* '_' */
    [96] = {0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '`' */
    [97] = {0x00, 0x00, 0x3c, 0x03, 0x3d, 0x63, 0x3d, 0x00}, /* 'a' */
    [98] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x7c, 0x00}, /* 'b' */
    [99] = {0x00, 0x00, 0x3e, 0x60, 0x60, 0x63, 0x3e, 0x00}, /* 'c' */
    [100] = {0x03, 0x03, 0x3f, 0x63, 0x63, 0x63, 0x3f, 0x00}, /* 'd' */
    [101] = {0x00, 0x00, 0x3e, 0x63, 0x7f, 0x60, 0x3e, 0x00}, /* 'e' */
    [102] = {0x1c, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x30, 0x00}, /* 'f' */
    [103] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x3e, 0x03, 0x3e}, /* 'g' */
    [104] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00}, /* 'h' */
    [105] = {0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00}, /* 'i' */
    [106] = {0x03, 0x00, 0x07, 0x03, 0x03, 0x03, 0x03, 0x1e}, /* 'j' */
    [107] = {0x60, 0x60, 0x63, 0x66, 0x7c, 0x66, 0x63, 0x00}, /* 'k' */
    [108] = {0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00}, /* 'l' */
    [109] = {0x00, 0x00, 0x7e, 0x6b, 0x6b, 0x6b, 0x6b, 0x00}, /* 'm' */
    [110] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00}, /* 'n' */
    [111] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'o' */
    [112] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x7c, 0x60, 0x60}, /* 'p' */
    [113] = {0x00, 0x00, 0x3f, 0x63, 0x63, 0x3f, 0x03, 0x03}, /* 'q' */
    [114] = {0x00, 0x00, 0x5e, 0x30, 0x30, 0x30, 0x30, 0x00}, /* 'r' */
    [115] = {0x00, 0x00, 0x3e, 0x60, 0x3c, 0x03, 0x3e, 0x00}, /* 's' */
    [116] = {0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x0c, 0x00}, /* 't' */
    [117] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x3d, 0x00}, /* 'u' */
    [118] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00}, /* 'v' */
    [119] = {0x00, 0x00, 0x63, 0x63, 0x6b, 0x7f, 0x34, 0x00}, /* 'w' */
    [120] = {0x00, 0x00, 0x63, 0x34, 0x1c, 0x34, 0x63, 0x00}, /* 'x' */
    [121] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x3f, 0x03, 0x3e}, /* 'y' */
    [122] = {0x00, 0x00, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x00}, /* 'z' */
    [123] = {0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00}, /* '{' */
    [124] = {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, /* '|' */
    [125] = {0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00}, /* '}' */
    [126] = {0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '~' */
};

/* Hàm trì hoãn phần cứng thực tế */
void delay(uint64_t count) {
    for (volatile uint64_t i = 0; i < count * 35000; i++) {
        __asm__("nop");
    }
}

/* 2. HÀM ĐỒ HỌA GỐC (Xử lý vùng nhớ framebuffer đồ họa thuần C) */

/* Tô màu pixel tại tọa độ màn hình */
void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)g_fb_width || y < 0 || y >= (int)g_fb_height) {
        return;
    }
    if (g_fb_address == NULL) return;
    
    size_t index = y * (g_fb_pitch / 4) + x;
    g_fb_address[index] = color;
}

/* Vẽ đường thẳng mảnh 1px bằng thuật toán Bresenham */
void draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
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

/* Vẽ khối hình chữ nhật đặc màu sắc bất kỳ */
void draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            draw_pixel(px, py, color);
        }
    }
}

/* Tương thích ngược */
void draw_rect(int x, int y, int w, int h, uint32_t color) {
    draw_rect_filled(x, y, w, h, color);
}

/* Vẽ khối hình chữ nhật có đường viền mảnh 1px */
void draw_rect_outline(int x, int y, int w, int h, uint32_t border_color, uint32_t bg_color) {
    draw_rect_filled(x, y, w, h, bg_color);
    draw_line(x, y, x + w - 1, y, border_color);                 /* Cạnh trên */
    draw_line(x, y + h - 1, x + w - 1, y + h - 1, border_color); /* Cạnh dưới */
    draw_line(x, y, x, y + h - 1, border_color);                 /* Cạnh trái */
    draw_line(x + w - 1, y, x + w - 1, y + h - 1, border_color); /* Cạnh phải */
}

/* Vẽ một ký tự đơn từ bảng font Pixel-Art */
void draw_char(int x, int y, char c, uint32_t color) {
    uint8_t glyph_idx = (uint8_t)c;
    if (glyph_idx >= 128) glyph_idx = '?';
    
    for (int row = 0; row < 8; row++) {
        uint8_t data = font_8x8[glyph_idx][row];
        for (int col = 0; col < 8; col++) {
            if ((data >> (7 - col)) & 1) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

/* Vẽ chuỗi ký tự bằng bộ font Pixel-Art */
void draw_string(int x, int y, const char *str, uint32_t color) {
    while (*str) {
        draw_char(x, y, *str, color);
        x += 8; /* Monospace gap tinh tế */
        str++;
    }
}

/* Hàm tương thích ngược */
void draw_vector_char(int x, int y, char c, uint32_t color) {
    draw_char(x, y, c, color);
}
void draw_vector_string(int x, int y, const char *str, uint32_t color) {
    draw_string(x, y, str, color);
}

/* 4. LOGIC HIỂN THỊ UI (Hàm C điều khiển) */

/* Màn hình khởi động hoài cổ tối giản (Splash Screen) */
void hydro_splash_screen(void) {
    /* Đổ nền màu giấy cũ hoài cổ COLOR_BG */
    for (int y = 0; y < (int)g_fb_height; y++) {
        for (int x = 0; x < (int)g_fb_width; x++) {
            draw_pixel(x, y, COLOR_BG);
        }
    }

    int cx = (int)g_fb_width / 2;
    int cy = (int)g_fb_height / 2;

    /* Vẽ chữ "HYDROOS" Monospace lớn sắc nét ở giữa màn hình */
    int text_x = cx - 28;
    int text_y = cy - 4;
    draw_string(text_x, text_y, "HYDROOS", COLOR_TEXT);

    /* Vẽ đường Loading bar chạy dài ở dưới */
    int bar_w = 200;
    int bar_x = cx - bar_w / 2;
    int bar_y = cy + 25;

    /* Trục nền Loading mảnh 1px màu nâu tro */
    draw_line(bar_x, bar_y, bar_x + bar_w, bar_y, COLOR_BORDER);

    /* Vòng lặp delay chạy dài Loading bar bằng màu Espresso sẫm */
    for (int pct = 0; pct <= 100; pct += 4) {
        int fill_w = (bar_w * pct) / 100;
        draw_line(bar_x, bar_y, bar_x + fill_w, bar_y, COLOR_TEXT);
        
        /* Hiển thị số phần trăm mờ nhã nhặn màu nâu nhạt ở dưới */
        int pct_x = cx - 12;
        int pct_y = bar_y + 12;
        char pct_str[5] = "  0%";
        pct_str[0] = '0' + (pct / 100);
        pct_str[1] = '0' + ((pct % 100) / 10);
        pct_str[2] = '0' + (pct % 10);
        if (pct_str[0] == '0') {
            pct_str[0] = ' ';
            if (pct_str[1] == '0') {
                pct_str[1] = ' ';
            }
        }
        draw_string(pct_x, pct_y, pct_str, NOTION_COLOR_TEXT_MUTED);
        
        delay(35); /* Độ trễ chân thực tạo cảm giác hoài cổ */
    }
    delay(300); /* Chờ một nhịp trước khi nạp vào Desktop */
}

/* Giao diện Desktop Vintage Minimalist của HydroOS */
void hydro_notion_desktop(void) {
    /* Đổ toàn bộ nền màn hình màu giấy cũ kem ấm COLOR_BG */
    draw_rect_filled(0, 0, g_fb_width, g_fb_height, COLOR_BG);

    /* 1. Thanh Top Menu màu trắng ngà COLOR_PANEL, sát cạnh trên */
    draw_rect_filled(0, 0, g_fb_width, 28, COLOR_PANEL);
    /* Đường viền dưới mảnh 1px màu nâu tro COLOR_BORDER */
    draw_line(0, 27, g_fb_width, 27, COLOR_BORDER);

    /* Ghi chữ bên góc trái Menu */
    draw_string(16, 10, "HYDROOS  |  V.0.1", COLOR_TEXT);
    draw_string(180, 10, "File", NOTION_COLOR_TEXT_MUTED);
    draw_string(230, 10, "Docker", COLOR_TEXT);
    draw_string(290, 10, "Status", NOTION_COLOR_TEXT_MUTED);

    /* Các chỉ số hệ thống mộc mạc bên góc phải */
    draw_string(g_fb_width - 250, 10, "[CLANG x86_64]", NOTION_COLOR_TEXT_MUTED);
    draw_string(g_fb_width - 120, 10, "15% CPU", COLOR_TEXT);

    /* 2. Cửa sổ Dubnium Docker Manager mẫu ngay giữa Desktop */
    int win_w = 640;
    int win_h = 360;
    int win_x = (g_fb_width - win_w) / 2;
    int win_y = (g_fb_height - win_h) / 2 - 20;

    /* Cửa sổ phẳng màu trắng ngà, viền nâu tro 1px */
    draw_rect_outline(win_x, win_y, win_w, win_h, COLOR_BORDER, COLOR_PANEL);
    
    /* Thanh tiêu đề cửa sổ phẳng, viền ngăn cách dưới */
    draw_rect_filled(win_x + 1, win_y + 1, win_w - 2, 30, COLOR_BG);
    draw_line(win_x, win_y + 31, win_x + win_w - 1, win_y + 31, COLOR_BORDER);
    draw_string(win_x + 16, win_y + 12, "Dubnium Docker Manager v1.0.4 - [db.c]", COLOR_TEXT);

    /* Thống kê container dạng bảng biểu tối giản trong cửa sổ */
    int content_y = win_y + 50;
    draw_string(win_x + 20, content_y, "CONTAINER ID   IMAGE           STATUS        PORT", NOTION_COLOR_TEXT_MUTED);
    draw_line(win_x + 20, content_y + 12, win_x + win_w - 20, content_y + 12, COLOR_BORDER);

    draw_string(win_x + 20, content_y + 24, "db-7f31a       nginx-stable    Active        :80", COLOR_TEXT);
    draw_string(win_x + 20, content_y + 40, "he-92k1l       alpine-linux    Active        :none", COLOR_TEXT);
    draw_string(win_x + 20, content_y + 56, "li-00x9z       postgres-db     Paused        :5432", NOTION_COLOR_TEXT_MUTED);

    /* Hộp lệnh log terminal giả lập bên dưới cửa sổ (Nền Latte nhẹ) */
    int term_y = win_y + 160;
    draw_rect_outline(win_x + 20, term_y, win_w - 40, 160, COLOR_BORDER, COLOR_LI_BG);

    draw_string(win_x + 32, term_y + 16, "[kernel.c] Boot sequence completed.", NOTION_COLOR_TEXT_MUTED);
    draw_string(win_x + 32, term_y + 32, "[oxygen.c] Editorial theme loaded: Warm Cream & Espresso", NOTION_COLOR_TEXT_MUTED);
    draw_string(win_x + 32, term_y + 48, "[db.c] Connected to virtual Docker socket layer", COLOR_TEXT);
    draw_string(win_x + 32, term_y + 64, "hydro@vintage-os:~$ docker ps", COLOR_TEXT);

    /* 3. Thanh DOCK phẳng, vuông vắn tinh tế, màu trắng ngà COLOR_PANEL ở cạnh dưới */
    int dock_w = 280;
    int dock_h = 80;
    int dock_x = (g_fb_width - dock_w) / 2;
    int dock_y = g_fb_height - dock_h - 20;

    /* Khung Dock phẳng viền nâu tro */
    draw_rect_outline(dock_x, dock_y, dock_w, dock_h, COLOR_BORDER, COLOR_PANEL);

    /* 3 Ô vuông dạng thẻ (Card đứng) xếp ngang inside Dock: [HE], [LI], [DB] */
    int card_w = 54;
    int card_h = 54;
    int gap = 16;
    int start_card_x = dock_x + (dock_w - (3 * card_w + 2 * gap)) / 2;
    int card_y = dock_y + (dock_h - card_h) / 2;

    /* Ô 1: [HE] - Khối phẳng màu trắng sữa COLOR_HE_BG viền mảnh */
    int card1_x = start_card_x;
    draw_rect_outline(card1_x, card_y, card_w, card_h, COLOR_BORDER, COLOR_HE_BG);
    draw_string(card1_x + 11, card_y + 23, "[HE]", COLOR_TEXT);

    /* Ô 2: [LI] - Khối phẳng màu nâu Latte nhạt COLOR_LI_BG viền mảnh */
    int card2_x = card1_x + card_w + gap;
    draw_rect_outline(card2_x, card_y, card_w, card_h, COLOR_BORDER, COLOR_LI_BG);
    draw_string(card2_x + 11, card_y + 23, "[LI]", COLOR_TEXT);

    /* Ô 3: [DB] - Khối phẳng màu nâu gỗ sáng/cát COLOR_DB_BG viền mảnh */
    int card3_x = card2_x + card_w + gap;
    draw_rect_outline(card3_x, card_y, card_w, card_h, COLOR_BORDER, COLOR_DB_BG);
    draw_string(card3_x + 11, card_y + 23, "[DB]", COLOR_TEXT);

    /* Chỉ báo hoạt động chấm nâu gỗ mộc mạc bên dưới ô DB */
    draw_rect_filled(card3_x + card_w / 2 - 3, dock_y + dock_h - 7, 6, 2, COLOR_TEXT);
}

/* 5. Hàm kernel_main nhận địa chỉ framebuffer trực tiếp từ Limine hoặc đối số */
void kernel_main(uint32_t *framebuffer_addr) {
    if (framebuffer_addr != NULL) {
        g_fb_address = framebuffer_addr;
    }
    
    /* Chạy hoạt cảnh khởi động */
    hydro_splash_screen();

    /* Khởi động giao diện Desktop Vintage hoài cổ tối giản */
    hydro_notion_desktop();

    /* Giữ CPU nhàn rỗi trong vòng lặp vô hạn */
    for (;;) {
        __asm__("hlt");
    }
}

/* Điểm vào bootloader truyền thống */
void _start(void) {
    if (fb_request.response != NULL && fb_request.response->count > 0) {
        struct limine_framebuffer *fb = fb_request.response->framebuffers[0];
        g_fb_address = (uint32_t *)fb->address;
        g_fb_width = fb->width;
        g_fb_height = fb->height;
        g_fb_pitch = fb->pitch;
    }
    kernel_main(g_fb_address);
}
