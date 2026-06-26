/**
 * @file linker.ld
 * @brief Script liên kết (linker script) cho HydroOS Kernel
 *
 * Chỉ định bố cục bộ nhớ cho kernel x86_64 64-bit.
 * Entry point: _start (trong kernel.c)
 * Địa chỉ tải: 0x100000 (1MB) - địa chỉ chuẩn cho kernel freestanding.
 */

ENTRY(_start)

SECTIONS
{
    /* Địa chỉ bắt đầu tải kernel vào RAM */
    . = 0x100000;

    .text : {
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
    }

    .bss : {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    /* Ký hiệu kết thúc kernel (dùng cho phân bổ heap/stack sau này) */
    kernel_end = .;
}
