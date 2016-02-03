# RasPiArduino [![Build Status](https://travis-ci.org/me-no-dev/RasPiArduino.svg?branch=master)](https://travis-ci.org/me-no-dev/RasPiArduino)

[![Join the chat at https://gitter.im/me-no-dev/RasPiArduino](https://badges.gitter.im/me-no-dev/RasPiArduino.svg)](https://gitter.im/me-no-dev/RasPiArduino?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
Arduino Framework for RaspberryPI


### Instructions for the PI
* Install Raspbian on your RaspberryPI
* Disable Serial Console on boot by changing /boot/cmdline.txt to
```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
```

* Disable loading sound kernel module by commenting _dtparam=audio=on_ in /boot/config.txt
* Enable password login for root
```bash
sudo su
passwd
_enter the new root password twice_
nano /etc/ssh/sshd_config
_change PermitRootLogin to yes_
```
* Copy tools/arduino.service to /etc/avahi/services/arduino.service and restart avahi-daemon
* Copy all files from tools/arpi_bins to /usr/local/bin
* Install telent by running _sudo apt-get install telnet_
* Create symbolic link for _run-avrdude_ by running _sudo ln -s /usr/local/bin/run-avrdude /usr/bin/run-avrdude_
* Do not load i2c uart or spi kernel drivers
* Optionally add _/usr/local/bin/run-sketch_ to /etc/rc.local to start the sketch on boot

### Instructions for Arduino IDE
* Open the hardware folder of Arduino IDE
* Create a folder named "RaspberryPi"
* Clone the repository to a folder named "piduino"
```bash
cd RaspberryPi
git clone https://github.com/me-no-dev/RasPiArduino piduino
```
* Copy the toolchain to tools/arm-none-linux-gnueabi (WIP)
* Restart Arduino IDE and select the PI from the list of network boards

