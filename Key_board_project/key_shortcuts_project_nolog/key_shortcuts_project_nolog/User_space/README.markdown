# System Control Kernel Module

## Overview
The `sys_control` kernel module is a Linux kernel driver that provides interactive control over system parameters—screen brightness, system volume, and CPU governor—using keyboard shortcuts. It features real-time ASCII feedback in the terminal and logs all actions to `/var/log/sys_control.log` for debugging and tracking. The module was developed for Ubuntu with kernel 6.8.0-58-generic as part of a Linux Device Driver training project.

### Contributors
- **Shubham**: Designed and implemented brightness control.
- **Akash R**: Developed volume control, replacing the initial Wi-Fi control.
- **Aravinth**: Implemented CPU governor control.

## Features
- **Brightness Control (Mode 0, by Shubham)**:
  - Adjusts screen brightness via `/sys/class/backlight/intel_backlight/brightness`.
  - Key bindings: `Alt + Up` (increase), `Alt + Down` (decrease).
  - Feedback: ASCII progress bar (e.g., `Brightness: [#####     ] 50% (by Shubham)`).
- **Volume Control (Mode 1, by Akash R)**:
  - Adjusts system volume using `amixer` via a sysfs interface (`/sys/kernel/sys_control/volume_cmd`).
  - Key bindings: `Alt + Up` (increase by 5%), `Alt + Down` (decrease by 5%).
  - Feedback: ASCII progress bar (e.g., `Volume: [#####     ] 50% (by Akash R)`).
- **CPU Governor Control (Mode 2, by Aravinth)**:
  - Toggles CPU governor between `performance` and `powersave` based on brightness level.
  - No direct key bindings for adjustment; set automatically in mode 2.
  - Feedback: Text (e.g., `CPU Governor: performance (by Aravinth)`).
- **Mode Switching**:
  - Key binding: `Ctrl + M` cycles through modes (0: brightness, 1: volume, 2: CPU governor).
- **Logging**:
  - All actions (mode switches, brightness/volume changes, CPU governor toggles) are logged to `/var/log/sys_control.log` with timestamps and contributor credits.
- **Feedback**:
  - Real-time ASCII feedback displayed in the terminal via `display_control.c`.
  - Feedback includes contributor names for each mode.

## Files
- **`sys_control.c`**: Kernel module source code.
- **`display_control.c`**: User-space program for displaying ASCII feedback.
- **`volume_control.sh`**: User-space script to handle volume commands via `amixer`.
- **`Makefile`**: Build script for compiling the kernel module.
- **`README.md`**: This documentation.

## Requirements
- **OS**: Ubuntu with kernel 6.8.0-58-generic.
- **Dependencies**:
  - `linux-headers-$(uname -r)`: Kernel headers for compilation.
  - `alsa-utils`: For `amixer` (volume control).
  - `gcc`: For compiling `display_control.c`.
- **Permissions**:
  - Write access to `/sys/class/backlight/*/brightness` and `/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`.
  - Write access to `/var/log/sys_control.log`.

## Installation
1. **Clone or Save Files**:
   - Save `sys_control.c`, `display_control.c`, `volume_control.sh`, and `Makefile` in a directory (e.g., `~/key_shortcuts_project/Kernal_space`).
   - Make the script executable:
     ```bash
     chmod +x volume_control.sh
     ```

2. **Install Dependencies**:
   ```bash
   sudo apt update
   sudo apt install linux-headers-$(uname -r) alsa-utils gcc
   ```

3. **Compile the Module**:
   ```bash
   make clean
   make
   ```

4. **Compile the User-Space Program**:
   ```bash
   gcc -o display_control display_control.c
   ```

5. **Set Permissions**:
   - Ensure write access to backlight and governor files:
     ```bash
     sudo chmod 666 /sys/class/backlight/*/brightness
     sudo chmod 666 /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
     ```
   - Ensure log file is writable:
     ```bash
     sudo touch /var/log/sys_control.log
     sudo chmod 666 /var/log/sys_control.log
     ```

## Usage
1. **Load the Module**:
   ```bash
   sudo insmod sys_control.ko
   ```
   - Verify:
     ```bash
     ls /dev/sys_control
     ls /sys/kernel/sys_control/
     ```

2. **Run the Volume Script**:
   ```bash
   ./volume_control.sh &
   ```
   - Note the PID:
     ```bash
     jobs -l
     ```

3. **Run the Feedback Program**:
   ```bash
   ./display_control
   ```

4. **Control System Parameters**:
   - **Brightness (Mode 0)**:
     - Press `Ctrl + M` until `Brightness: [...] (by Shubham)` appears.
     - `Alt + Up`: Increase brightness (~5% steps).
     - `Alt + Down`: Decrease brightness.
   - **Volume (Mode 1)**:
     - Press `Ctrl + M` to reach `Volume: [...] (by Akash R)`.
     - `Alt + Up`: Increase volume (5% steps).
     - `Alt + Down`: Decrease volume.
     - Test with:
       ```bash
       aplay /usr/share/sounds/alsa/Front_Center.wav
       ```
   - **CPU Governor (Mode 2)**:
     - Press `Ctrl + M` to reach `CPU Governor: ... (by Aravinth)`.
     - Governor toggles based on brightness level.

5. **View Logs**:
   ```bash
   cat /var/log/sys_control.log
   ```
   Example log entries:
   ```
   [1698765432.123456789] System Control initialized (by Shubham, Akash R, Aravinth)
   [1698765433.234567890] Brightness increased to 500 (by Shubham)
   [1698765434.345678901] Switched to mode 1 (Volume (by Akash R))
   [1698765435.456789012] Volume up to 60% (by Akash R)
   [1698765436.567890123] CPU Governor set to performance (by Aravinth)
   ```

6. **Clean Up**:
   - Stop `display_control`: `Ctrl+C`.
   - Stop `volume_control.sh`:
     ```bash
     kill <PID>
     ```
   - Unload module:
     ```bash
     sudo rmmod sys_control
     ```
   - Clean builds:
     ```bash
     make clean
     ```

## Issues Faced
The development process encountered several challenges, resolved iteratively:

### 1. Brightness Control (by Shubham)
- **Issue**: Incorrect backlight path caused brightness control failures.
  - **Symptom**: `Failed to read brightness settings` in `dmesg`.
  - **Solution**: Verified path with `ls /sys/class/backlight/` and updated `BRIGHTNESS_PATH` to `intel_backlight` or system-specific path (e.g., `amdgpu_bl0`). Added permissions: `sudo chmod 666 /sys/class/backlight/*/brightness`.
- **Issue**: Compilation errors due to mismatched kernel headers.
  - **Symptom**: `make` failed with missing symbols.
  - **Solution**: Installed `linux-headers-$(uname -r)` and ensured `gcc-12` matched the kernel’s compiler.
- **Contributor**: Shubham designed the brightness control logic and ensured compatibility with sysfs.

### 2. Volume Control (by Akash R)
- **Issue**: Initial ALSA kernel API integration failed.
  - **Symptom**: Compilation errors (`snd_ctl_scalar_info`, `snd_card_create` undefined).
  - **Cause**: Missing or incompatible ALSA kernel headers for kernel 6.8.0-58-generic.
  - **Solution**: Switched to a sysfs-based approach with `/sys/kernel/sys_control/volume_cmd` and a user-space script (`volume_control.sh`) using `amixer`.
- **Issue**: Volume not changing despite feedback updates.
  - **Symptom**: `Volume: [...]` updated, but `amixer` commands didn’t execute.
  - **Cause**: Script not running, incorrect `amixer` control (`Master` vs. `PCM`), or sysfs file permissions.
  - **Solution**:
    - Ensured script was running (`./volume_control.sh &`).
    - Tested `amixer scontrols` to identify correct control (`Master`).
    - Set permissions: `sudo chmod 666 /sys/kernel/sys_control/volume_cmd`.
    - Added `kobject_uevent` to notify user-space of sysfs changes.
- **Issue**: Feedback not syncing with actual volume.
  - **Symptom**: Module’s `control_state.volume` didn’t match `amixer` output.
  - **Solution**: Updated `volume_control.sh` to read current volume from `amixer` (not fully synced, but feedback is indicative).
- **Contributor**: Akash R replaced Wi-Fi control with volume control, implementing the sysfs/script approach.

### 3. CPU Governor Control (by Aravinth)
- **Issue**: Governor not changing on some systems.
  - **Symptom**: `/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor` write failed.
  - **Cause**: Missing CPUFreq kernel support or permissions.
  - **Solution**: Verified CPUFreq (`zcat /proc/config.gz | grep CPU_FREQ`) and set permissions: `sudo chmod 666 /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`.
- **Issue**: Limited governor options.
  - **Symptom**: `performance` or `powersave` not available.
  - **Solution**: Checked `scaling_available_governors` and ensured fallback to available governors.
- **Contributor**: Aravinth implemented the CPU governor toggle logic based on brightness levels.

### 4. General Issues
- **Issue**: Keyboard events not detected.
  - **Solution**: Verified input device registration (`cat /proc/bus/input/devices`) and ensured `input_register_handler` worked.
- **Issue**: Feedback not displaying.
  - **Solution**: Ensured `/dev/sys_control` was created and `display_control.c` was reading correctly.
- **Issue**: Log file permissions.
  - **Solution**: Added `sudo chmod 666 /var/log/sys_control.log` to ensure kernel could write logs.

## Future Improvements
- **Volume Sync**: Fully sync `control_state.volume` with `amixer` output by reading volume in the kernel.
- **Additional Controls**: Add fan speed or screen contrast control.
- **Custom Keys**: Allow user-defined key bindings via module parameters.
- **Log Rotation**: Implement log file rotation to prevent `/var/log/sys_control.log` from growing indefinitely.

## License
This project is licensed under the GNU General Public License (GPL).

## Acknowledgments
- Thanks to Shubham, Akash R, and Aravinth for their contributions.
- Developed as part of Mirafra LDD Batch 2 training.