# RasPiArduino
Arduino Framework for RaspberryPI


### Instructions for the PI
1. Install Raspbian on your RaspberryPI
2. Disable Serial Console on boot by changing /boot/cmdline.txt to
```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
```

3. Disable loading sound kernel module by commenting _dtparam=audio=on_ in /boot/config.txt
4. Enable password login for root
```bash
sudo su
passwd
_enter the new root password twice_
nano /etc/ssh/sshd_config
_change PermitRootLogin to yes_
```
5. Copy tools/arduino.service to /etc/avahi/services/arduino.service and restart avahi-daemon
6. Copy all files from tools/arpi_bins to /usr/local/bin
7. Install telent by running _sudo apt-get install telnet_
8. Create symbolic link for _run-avrdude_ by running _sudo ln -s /usr/local/bin/run-avrdude /usr/bin/run-avrdude_
9. Do not load i2c uart or spi kernel drivers
10. Optionally add _/usr/local/bin/run-sketch_ to /etc/rc.local to start the sketch on boot

### Instructions for Arduino IDE
1. Open the hardware folder of Arduino IDE
2. Create a folder named "RaspberryPi"
3. Clone the repository to a folder named "piduino"
```bash
cd RaspberryPi
git clone https://github.com/me-no-dev/RasPiArduino piduino
```
4. Copy the toolchain to tools/arm-none-linux-gnueabi (WIP)
5. Restart Arduino IDE and select the PI from the list of network boards

Lots of work still needs to be done before this project can be concidered stable!
Many improvements are still to come.
