
# 🐧 Compiling Linux Kernel for Raspberry Pi (64-bit) – Step-by-Step Guide

---

## ✅ Prerequisites

Ensure your Linux development system has the following:

- Ubuntu/Debian system (recommended)
- Internet access
- Basic Git and terminal knowledge

Install required packages:

```bash
sudo apt update
sudo apt install -y git bc bison flex libssl-dev make libncurses-dev \
    crossbuild-essential-arm64 gcc-aarch64-linux-gnu \
    libelf-dev wget
```

> For 32-bit kernel support, use `gcc-arm-linux-gnueabihf` instead of `gcc-aarch64-linux-gnu`.

---

## 📥 Step 1: Clone Kernel Source

Clone the official Raspberry Pi Linux kernel source:

```bash
git clone --depth=1 https://github.com/raspberrypi/linux.git -b rpi-6.6.y
cd linux
```

> Replace `rpi-6.6.y` with another branch like `rpi-5.15.y` if needed.

---

## 🛠 Step 2: Set Environment Variables

Tell the build system to use the correct architecture and compiler:

```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```

---

## ⚙️ Step 3: Kernel Configuration

Choose the default configuration for your Raspberry Pi model:

- **Pi 4 (64-bit)**:
  ```bash
  make bcm2711_defconfig
  ```
- **Pi 3 (64-bit)**:
  ```bash
  make bcm2710_defconfig
  ```

To modify kernel options (optional):

```bash
make menuconfig
```

---

## 🏗 Step 4: Compile the Kernel

Start the kernel compilation:

```bash
make -j$(nproc)
```

This produces:
- Kernel: `arch/arm64/boot/Image`
- Device Tree Blobs (DTBs): `arch/arm64/boot/dts/broadcom/*.dtb`
- Overlays: `arch/arm64/boot/dts/overlays/*.dtbo`

---

## 📦 Step 5: Build and Install Kernel Modules (Optional)

If you want to include loadable kernel modules:

```bash
make modules
make INSTALL_MOD_PATH=../rpi-kernel-modules modules_install
```

Modules will be placed in `../rpi-kernel-modules/lib/modules/`.

---

## 💾 Step 6: Prepare SD Card or USB Boot Device

1. Insert Raspberry Pi SD card into your PC.
2. Mount both partitions:
   - `/media/$USER/boot` — boot partition (FAT32)
   - `/media/$USER/rootfs` — root filesystem (ext4)

Copy kernel and DTBs:

```bash
# Copy the kernel image
cp arch/arm64/boot/Image /media/$USER/boot/kernel8.img

# Copy device trees
cp arch/arm64/boot/dts/broadcom/*.dtb /media/$USER/boot/
cp -r arch/arm64/boot/dts/overlays /media/$USER/boot/
```

Check or edit `/media/$USER/boot/config.txt`:

```ini
arm_64bit=1
kernel=kernel8.img
```

If you built modules:

```bash
sudo cp -r ../rpi-kernel-modules/lib/modules /media/$USER/rootfs/lib/
```

---

## 🔁 Step 7: Boot Raspberry Pi

1. Eject the SD card safely.
2. Insert it into your Raspberry Pi.
3. Power on the device.

---

## 🧪 Step 8: Verify the Kernel

SSH into or use a monitor with the Pi:

```bash
uname -a
```

You should see your compiled kernel version.

---

## 📘 Notes

- You can recompile only the kernel (faster) without repeating modules:
  ```bash
  make -j$(nproc) Image dtbs
  ```

- Use `make localmodconfig` to tailor the kernel to your current system.

- Use `make savedefconfig` to export a trimmed-down config.

- For 32-bit Raspberry Pi systems, use:
  ```bash
  export ARCH=arm
  export CROSS_COMPILE=arm-linux-gnueabihf-
  make bcmrpi_defconfig  # or similar
  ```

---

## 📂 Output Summary

| Output File                        | Destination on SD card             |
|-----------------------------------|------------------------------------|
| `arch/arm64/boot/Image`           | `/boot/kernel8.img`                |
| `*.dtb` files                     | `/boot/`                           |
| `overlays/*.dtbo`                 | `/boot/overlays/`                  |
| `lib/modules/`                    | `/rootfs/lib/modules/`             |
