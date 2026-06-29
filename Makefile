# =============================================================================
# HydroOS - Makefile (He thong bien dich sieu nhe bang Clang + LLVM)
# =============================================================================
# Muc dich: Tu dong hoa bien dich kernel.c thanh file .iso khoi dong duoc
# Cong cu: Clang + LLD + xorriso + Limine Bootloader
# =============================================================================

# --- Cac bien thiet lap cong cu bien dich ---
# clang: Trinh bien dich chinh, target x86_64 bare-metal
# ld.lld: Trinh lien ket (linker) tu LLVM, thay the GNU ld
# llvm-objcopy: Sao chep va chuyen doi dinh dang file object
CC      := clang
LD      := ld.lld
OBJCOPY := llvm-objcopy

# --- Cac co bien dich C ---
# -target x86_64-none-elf    : Bien dich cho target x86_64 bare-metal (khong co OS)
# -ffreestanding             : Khong lien ket voi thu vien chuan C (libc)
# -O2                        : Toi uu hoa muc do 2 (can bang toc do va kich thuoc)
# -Wall -Wextra              : Bat tat ca canh bao de code sach
# -mcmodel=large             : Mo hinh bo nho large (kernel co the o bat ky dau)
# -fno-stack-protector       : Tat stack canary (kernel khong co libc ho tro)
# -fno-pic                   : Khong tao Position-Independent Code (kernel can dia chi co dinh)
# -fno-pie                   : Khong tao Position-Independent Executable
# -fno-omit-frame-pointer    : Giu frame pointer de debug de dang
# -fno-builtin               : Khong dung cac ham builtin cua compiler
# -nostdinc                  : Khong include thu muc header chuan
CFLAGS  := -target x86_64-none-elf \
           -ffreestanding \
           -O2 \
           -Wall \
           -Wextra \
           -mcmodel=large \
           -fno-stack-protector \
           -fno-pic \
           -fno-pie \
           -fno-omit-frame-pointer \
           -fno-builtin \
           -nostdinc \
           -I. \
           -isystem /usr/lib/llvm-18/lib/clang/18/include

# --- Cac co lien ket (linker flags) ---
# -T linker.ld               : Su dung script lien ket tuy chinh (dinh dia chi 0x100000)
# -nostdlib                  : Khong lien ket voi thu vien chuan C
# -static                    : Lien ket tinh (khong co shared library)
# -z max-page-size=0x1000    : Kich thuoc trang toi da 4KB (cho phan bo bo nho)
LDFLAGS := -T linker.ld \
           -nostdlib \
           -static \
           -z max-page-size=0x1000

# --- Cac file nguon va dich ---
SRC      := kernel.c db.c src/drivers/graphics.c src/apps/office_elements.c src/apps/utility_elements.c src/apps/media_elements.c src/apps/extended_elements.c src/apps/social_elements.c src/apps/system_elements.c
OBJ      := kernel.o db.o graphics.o office_elements.o utility_elements.o media_elements.o extended_elements.o social_elements.o system_elements.o
KERNEL   := kernel.elf
ISO      := hydroos.iso

# --- Cac thu muc lam viec ---
BUILD_DIR := build
ISO_DIR   := $(BUILD_DIR)/iso
BOOT_DIR  := $(ISO_DIR)/boot
LIMINE_BOOT_DIR := $(BOOT_DIR)/limine

# --- Phien ban Limine de download (on dinh, ho tro stivale2) ---
LIMINE_VERSION := 7.11.0
LIMINE_TAR     := limine-$(LIMINE_VERSION).tar.gz
LIMINE_DIR     := limine-$(LIMINE_VERSION)
LIMINE_URL     := https://github.com/limine-bootloader/limine/releases/download/v$(LIMINE_VERSION)/limine-$(LIMINE_VERSION).tar.gz

# --- Cac file thuc thi can thiet tu Limine ---
LIMINE_BIOS_SYS     := $(LIMINE_DIR)/limine-bios.sys
LIMINE_BIOS_CD_BIN  := $(LIMINE_DIR)/limine-bios-cd.bin
LIMINE_UEFI_CD_BIN  := $(LIMINE_DIR)/limine-uefi-cd.bin
LIMINE_TOOL         := $(LIMINE_DIR)/limine

# =============================================================================
# TARGET MAC DINH: Bien dich kernel
# =============================================================================
.PHONY: all
all: $(KERNEL)

# --- Bien dich file .c thanh file object .o ---
# Buoc 1: Clang bien dich kernel.c -> kernel.o (freestanding, khong libc)
kernel.o: kernel.c
	@echo "[*] Dang bien dich kernel.c -> kernel.o..."
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o
	@echo "[OK] Da bien dich xong kernel.o"

db.o: db.c
	@echo "[*] Dang bien dich db.c -> db.o..."
	$(CC) $(CFLAGS) -c db.c -o db.o
	@echo "[OK] Da bien dich xong db.o"

graphics.o: src/drivers/graphics.c
	@echo "[*] Dang bien dich src/drivers/graphics.c -> graphics.o..."
	$(CC) $(CFLAGS) -c src/drivers/graphics.c -o graphics.o
	@echo "[OK] Da bien dich xong graphics.o"

office_elements.o: src/apps/office_elements.c
	@echo "[*] Dang bien dich src/apps/office_elements.c -> office_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/office_elements.c -o office_elements.o
	@echo "[OK] Da bien dich xong office_elements.o"

utility_elements.o: src/apps/utility_elements.c
	@echo "[*] Dang bien dich src/apps/utility_elements.c -> utility_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/utility_elements.c -o utility_elements.o
	@echo "[OK] Da bien dich xong utility_elements.o"

media_elements.o: src/apps/media_elements.c
	@echo "[*] Dang bien dich src/apps/media_elements.c -> media_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/media_elements.c -o media_elements.o
	@echo "[OK] Da bien dich xong media_elements.o"

extended_elements.o: src/apps/extended_elements.c
	@echo "[*] Dang bien dich src/apps/extended_elements.c -> extended_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/extended_elements.c -o extended_elements.o
	@echo "[OK] Da bien dich xong extended_elements.o"

social_elements.o: src/apps/social_elements.c
	@echo "[*] Dang bien dich src/apps/social_elements.c -> social_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/social_elements.c -o social_elements.o
	@echo "[OK] Da bien dich xong social_elements.o"

system_elements.o: src/apps/system_elements.c
	@echo "[*] Dang bien dich src/apps/system_elements.c -> system_elements.o..."
	$(CC) $(CFLAGS) -c src/apps/system_elements.c -o system_elements.o
	@echo "[OK] Da bien dich xong system_elements.o"

# --- Lien ket file object thanh kernel ELF ---
# Buoc 2: ld.lld lien ket kernel.o -> kernel.elf (dung linker script)
$(KERNEL): kernel.o db.o graphics.o office_elements.o utility_elements.o media_elements.o extended_elements.o social_elements.o system_elements.o linker.ld
	@echo "[*] Dang lien ket kernel.o db.o graphics.o office_elements.o utility_elements.o media_elements.o extended_elements.o social_elements.o system_elements.o -> $(KERNEL)..."
	$(LD) $(LDFLAGS) -o $@ kernel.o db.o graphics.o office_elements.o utility_elements.o media_elements.o extended_elements.o social_elements.o system_elements.o
	@echo "[OK] Kernel ELF da san sang: $@"
	@echo "    -> Kich thuoc: $$(ls -lh $@ | awk '{print $$5}')"

# =============================================================================
# TARGET: Tao file ISO khoi dong duoc
# =============================================================================
.PHONY: iso
iso: $(KERNEL)
	@echo ""
	@echo "============================================================================="
	@echo "  [HYDROOS] Dang tao file ISO khoi dong..."
	@echo "============================================================================="
	@echo ""

	@echo "[*] Buoc 1: Tao cau truc thu muc ISO..."
	@rm -rf $(ISO_DIR)
	@mkdir -p $(LIMINE_BOOT_DIR)
	@echo "    -> Da tao: $(ISO_DIR)"

	@echo "[*] Buoc 2: Sao chep kernel va cau hinh bootloader..."
	@cp $(KERNEL) $(BOOT_DIR)/
	@cp limine.cfg $(LIMINE_BOOT_DIR)/
	@echo "    -> Da copy: $(KERNEL) -> $(BOOT_DIR)/"
	@echo "    -> Da copy: limine.cfg -> $(LIMINE_BOOT_DIR)/"

	@echo "[*] Buoc 3: Tai va giai nen Limine bootloader..."
	@if [ ! -f "$(LIMINE_TAR)" ]; then \
		echo "    -> Dang tai Limine v$(LIMINE_VERSION) tu GitHub..."; \
		curl -L --progress-bar -o $(LIMINE_TAR) $(LIMINE_URL) 2>/dev/null || \
		wget --progress=bar:force -O $(LIMINE_TAR) $(LIMINE_URL) 2>/dev/null || \
		(echo "[LOI] Khong the tai Limine! Hay kiem tra ket noi mang."; exit 1); \
	fi
	@if [ ! -d "$(LIMINE_DIR)" ]; then \
		echo "    -> Dang giai nen $(LIMINE_TAR)..."; \
		tar -xzf $(LIMINE_TAR); \
	fi
	@echo "    -> Limine da san sang: $(LIMINE_DIR)"

	@echo "[*] Buoc 4: Sao chep cac file thuc thi Limine vao ISO..."
	@cp $(LIMINE_BIOS_SYS) $(LIMINE_BOOT_DIR)/
	@cp $(LIMINE_BIOS_CD_BIN) $(LIMINE_BOOT_DIR)/
	@cp $(LIMINE_UEFI_CD_BIN) $(LIMINE_BOOT_DIR)/
	@echo "    -> Da copy: limine-bios.sys"
	@echo "    -> Da copy: limine-bios-cd.bin"
	@echo "    -> Da copy: limine-uefi-cd.bin"

	@echo "[*] Buoc 5: Dang tao file ISO bang xorriso..."
	@xorriso -as mkisofs \
		-b boot/limine/limine-bios-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		$(ISO_DIR) \
		-o $(ISO) \
		>/dev/null 2>&1
	@echo "    -> Da tao: $(ISO)"

	@echo "[*] Buoc 6: Cai dat Limine BIOS bootloader vao ISO..."
	@chmod +x $(LIMINE_TOOL)
	@$(LIMINE_TOOL) bios-install $(ISO) >/dev/null 2>&1 || \
		(echo "[CANH BAO] Khong the cai dat Limine BIOS, co the van boot duoc UEFI"; true)

	@echo ""
	@echo "============================================================================="
	@echo "  [OK] FILE ISO DA SAN SANG!"
	@echo "============================================================================="
	@echo "    -> Duong dan: $(ISO)"
	@echo "    -> Kich thuoc: $$(ls -lh $(ISO) | awk '{print $$5}')"
	@echo "    -> Chay 'make run' de khoi dong QEMU va test giao dien!"
	@echo "============================================================================="
	@echo ""

# =============================================================================
# TARGET: Chay QEMU de test giao dien
# =============================================================================
.PHONY: run
run: $(ISO)
	@echo ""
	@echo "============================================================================="
	@echo "  [HYDROOS] Dang khoi dong QEMU..."
	@echo "============================================================================="
	@echo ""
	qemu-system-x86_64 \
		-cdrom $(ISO) \
		-m 512M \
		-vga std \
		-display sdl \
		-serial stdio \
		-no-reboot \
		-no-shutdown
	@echo ""
	@echo "[OK] QEMU da ket thuc."
	@echo ""

# --- Chay QEMU khong co GUI (cho server khong co display) ---
.PHONY: run-nographic
run-nographic: $(ISO)
	@echo ""
	@echo "[*] Dang khoi dong QEMU (che do khong GUI)..."
	qemu-system-x86_64 \
		-cdrom $(ISO) \
		-m 512M \
		-vga std \
		-nographic \
		-serial stdio \
		-no-reboot \
		-no-shutdown
	@echo "[OK] QEMU da ket thuc."

# =============================================================================
# TARGET: Don dep
# =============================================================================
.PHONY: clean
clean:
	@echo "[*] Dang xoa cac file bien dich tam..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(OBJ) $(KERNEL) $(ISO)
	@echo "[OK] Da don dep xong."

# --- Xoa hoan toan bao gom ca Limine da tai ---
.PHONY: distclean
distclean: clean
	@echo "[*] Dang xoa toan bo (bao gom ca Limine)..."
	@rm -rf $(LIMINE_DIR) $(LIMINE_TAR)
	@echo "[OK] Da xoa sach toan bo du an."

# =============================================================================
# TARGET: Hien thi tro giup
# =============================================================================
.PHONY: help
help:
	@echo ""
	@echo "============================================================================="
	@echo "  HydroOS - Build System (Clang + LLVM + Limine)"
	@echo "============================================================================="
	@echo ""
	@echo "  CAC TARGET CHINH:"
	@echo ""
	@echo "    make             - Bien dich kernel (mac dinh)"
	@echo "    make iso         - Tao file ISO khoi dong duoc (hydroos.iso)"
	@echo "    make run         - Chay QEMU voi GUI de test giao dien"
	@echo "    make run-nographic - Chay QEMU khong GUI (chi console)"
	@echo "    make clean       - Xoa cac file bien dich tam"
	@echo "    make distclean   - Xoa toan bo (bao gom ca Limine da tai)"
	@echo "    make help        - Hien thi tro giup nay"
	@echo ""
	@echo "  CAC FILE DU AN:"
	@echo ""
	@echo "    kernel.c         - Ma nguon kernel chinh (giao dien Vintage Minimalist)"
	@echo "    linker.ld        - Script lien ket (dinh dia chi 0x100000)"
	@echo "    limine.cfg       - Cau hinh bootloader Limine"
	@echo "    Makefile         - File tu dong hoa bien dich (file nay)"
	@echo ""
	@echo "  YEU CAU HE THONG:"
	@echo ""
	@echo "    - clang          (trinh bien dich LLVM)"
	@echo "    - ld.lld         (trinh lien ket LLVM)"
	@echo "    - xorriso        (tao file ISO)"
	@echo "    - qemu-system-x86_64 (giai lap de test)"
	@echo "    - curl hoac wget (tai Limine tu GitHub)"
	@echo ""
	@echo "  CACH SU DUNG:"
	@echo ""
	@echo "    1. make          # Bien dich kernel"
	@echo "    2. make iso      # Tao file ISO"
	@echo "    3. make run      # Chay QEMU de test"
	@echo ""
	@echo "  THONG TIN BO SUNT:"
	@echo ""
	@echo "    - Do phan giai: 1024x768x32 bit"
	@echo "    - Boot protocol: stivale2 (Limine)"
	@echo "    - Entry point:  _start (dia chi 0x100000)"
	@echo "    - Architecture: x86_64 bare-metal"
	@echo ""
	@echo "============================================================================="
	@echo ""
