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

/* 2. BẢNG MÀU VINTAGE MINIMALIST CHUẨN GU THẨM MỸ (ARGB) */
#define COLOR_DESKTOP  0xFFF5EFE6  /* Nền kem ấm toàn màn hình */
#define COLOR_TEXT     0xFF3E2723  /* Chữ nâu Espresso đậm */
#define COLOR_BORDER   0xFFD7CCC8  /* Đường viền nâu tro mảnh 1px */
#define COLOR_WHITE    0xFFFFFFFF  /* Trắng ngà cho Menu và Dock */
#define COLOR_HE_BG    0xFFF9F6F0  /* Trắng sữa nhạt cho khối [HE] */
#define COLOR_LI_BG    0xFFEFEBE9  /* Nữ Latte nhạt cho khối [LI] */
#define COLOR_DB_BG    0xFFE0D4C3  /* Nâu gỗ sáng cho khối [DB] */

/* Các hằng số tương thích ngược để giữ đồng bộ hệ thống */
#define COLOR_BACKGROUND         COLOR_DESKTOP
#define COLOR_HE_TEXT            COLOR_TEXT
#define COLOR_LI_TEXT            COLOR_TEXT
#define COLOR_DB_TEXT            COLOR_TEXT
#define COLOR_MENU_BAR           COLOR_WHITE

#define NOTION_COLOR_BG          COLOR_DESKTOP
#define NOTION_COLOR_CANVAS      COLOR_DESKTOP
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
    /* Tô toàn màn hình bằng COLOR_WHITE */
    for (int y = 0; y < (int)g_fb_height; y++) {
        for (int x = 0; x < (int)g_fb_width; x++) {
            draw_pixel(x, y, COLOR_WHITE);
        }
    }

    /* Logo chữ "H" nét mảnh ở chính giữa (X=512, Y=350), rộng 30px, cao 50px */
    /* Trụ dọc trái: (497, 325) -> (497, 375) */
    draw_line(497, 325, 497, 375, COLOR_TEXT);
    /* Trụ dọc phải: (527, 325) -> (527, 375) */
    draw_line(527, 325, 527, 375, COLOR_TEXT);
    /* Thanh ngang giữa: (497, 350) -> (527, 350) */
    draw_line(497, 350, 527, 350, COLOR_TEXT);

    /* Khung Loading bar rỗng: X=412, Y=420, Rộng=200, Cao=4, viền COLOR_BORDER */
    draw_line(412, 420, 612, 420, COLOR_BORDER);
    draw_line(412, 423, 612, 423, COLOR_BORDER);
    draw_line(412, 420, 412, 423, COLOR_BORDER);
    draw_line(612, 420, 612, 423, COLOR_BORDER);

    /* Vòng lặp delay vẽ đầy thanh Loading bar bằng COLOR_TEXT */
    for (int pct = 0; pct <= 100; pct += 4) {
        int fill_w = (200 * pct) / 100;
        if (fill_w > 0) {
            draw_rect_filled(412, 421, fill_w, 2, COLOR_TEXT);
        }
        delay(35);
    }
    delay(300);
}

/* Giao diện Desktop Vintage Minimalist của HydroOS */
void hydro_notion_desktop(void) {
    /* Đổ toàn bộ nền màn hình màu kem ấm COLOR_DESKTOP */
    draw_rect_filled(0, 0, g_fb_width, g_fb_height, COLOR_DESKTOP);

    /* 3. CHI TIẾT THANH TOP MENU BAR (Sát cạnh trên) */
    /* Gốc (0,0), Rộng 1024, Cao 32, tô COLOR_WHITE */
    draw_rect_filled(0, 0, 1024, 32, COLOR_WHITE);
    /* Viền cạnh dưới tại Y=31, từ X=0 đến 1024, màu COLOR_BORDER */
    draw_line(0, 31, 1024, 31, COLOR_BORDER);
    /* Nội dung chữ "HYDROOS  |  V.0.1" tại (20, 11), màu COLOR_TEXT */
    draw_string(20, 11, "HYDROOS  |  V.0.1", COLOR_TEXT);

    /* 4. CHI TIẾT THANH DOCK PHẲNG (Nằm ngang, chính giữa sát cạnh dưới) */
    /* Tọa độ: X=362, Y=680, Rộng=300, Cao=56. Tô COLOR_WHITE. */
    draw_rect_filled(362, 680, 300, 56, COLOR_WHITE);
    /* Viền khung 1px bao quanh bằng COLOR_BORDER */
    draw_rect_outline(362, 680, 300, 56, COLOR_BORDER, COLOR_WHITE);
    /* Hiệu ứng Vintage Elevation (Hard Shadow 2D):
       Đường thẳng COLOR_BORDER ở cạnh dưới (Y=737) và cạnh phải (X=663) dịch ra 1px */
    /* Cạnh dưới shadow: Y=737, từ X=362 đến X=662 (cạnh phải của dock là 362+300=662, shadow ở 663 nên đoạn ngang đến 663) */
    draw_line(362, 737, 663, 737, COLOR_BORDER);
    /* Cạnh phải shadow: X=663, từ Y=680 đến Y=736 (cạnh dưới của dock là 680+56=736, shadow ở 737 nên đoạn dọc đến 737) */
    draw_line(663, 680, 663, 737, COLOR_BORDER);

    /* 5. CHI TIẾT 3 Ô KHỐI ỨNG DỤNG TRÊN THANH DOCK */
    /* Ô [HE] (Helium): (374, 688), Rộng=76, Cao=40, tô COLOR_HE_BG, viền COLOR_BORDER */
    draw_rect_outline(374, 688, 76, 40, COLOR_BORDER, COLOR_HE_BG);
    /* Chữ "HE" căn giữa ô: font 8x8, 2 ký tự = 16px. X = 374 + (76-16)/2 = 404. Y = 688 + (40-8)/2 = 704. */
    draw_string(404, 704, "HE", COLOR_TEXT);

    /* Ô [LI] (Lithium): (462, 688), Rộng=76, Cao=40, tô COLOR_LI_BG, viền COLOR_BORDER */
    draw_rect_outline(462, 688, 76, 40, COLOR_BORDER, COLOR_LI_BG);
    /* Chữ "LI" căn giữa ô: X = 462 + (76-16)/2 = 492. Y = 704. */
    draw_string(492, 704, "LI", COLOR_TEXT);

    /* Ô [DB] (Dubnium/Docker): (550, 688), Rộng=76, Cao=40, tô COLOR_DB_BG, viền COLOR_BORDER */
    draw_rect_outline(550, 688, 76, 40, COLOR_BORDER, COLOR_DB_BG);
    /* Chữ "DB" căn giữa ô: 2 ký tự = 16px. X = 550 + (76-16)/2 = 580. Y = 704. */
    draw_string(580, 704, "DB", COLOR_TEXT);
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
