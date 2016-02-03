# RasPiArduino [![Build Status](https://travis-ci.org/me-no-dev/RasPiArduino.svg?branch=master)](https://travis-ci.org/me-no-dev/RasPiArduino)

[![Join the chat at https://gitter.im/me-no-dev/RasPiArduino](https://badges.gitter.im/me-no-dev/RasPiArduino.svg)](https://gitter.im/me-no-dev/RasPiArduino?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
Arduino Framework for RaspberryPI


### Instructions for the PI
* Install Raspbian Jessie on your RaspberryPI
* Disable Serial Console on boot by changing /boot/cmdline.txt to
```bash
cat > /boot/cmdline.txt <<EOL
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
EOL
```

* Disable loading sound kernel module by commenting _dtparam=audio=on_ in /boot/config.txt
```
#dtparam=audio=on
```

* Enable password login for root
```bash
sudo su
passwd
```
_enter the new root password twice_
```bash
exit
sudo nano /etc/ssh/sshd_config
```
_change PermitRootLogin to yes_

* Change the hostane for your Pi (optional)
```bash
sudo echo "piduino" > /etc/hostname
```

* Setup WiFi (optional)
```bash
sudo cat > /etc/wpa_supplicant/wpa_supplicant.conf <<EOL
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
network={
    ssid="your-ssid"
    psk="your-pass"
}
EOL
```

* Setup avahi service to allow updating the sketch from ArduinoIDE
```bash
sudo cat > /etc/avahi/services/arduino.service <<EOL
<?xml version="1.0" standalone='no'?><!--*-nxml-*-->
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
  <name replace-wildcards="yes">%h</name>
  <service>
    <type>_arduino._tcp</type>
    <port>22</port>
    <txt-record>board=bplus</txt-record>
  </service>
</service-group>
EOL

sudo service avahi-daemon restart
```

* Install telent and git
```bash
sudo apt-get install telnet git
```

* Copy all files from tools/arpi_bins to /usr/local/bin
```bash
git clone https://github.com/me-no-dev/RasPiArduino.git piduino
chmod +x piduino/tools/arpi_bins/*
sudo cp piduino/tools/arpi_bins/* /usr/local/bin
rm -rf piduino
```

* Create symbolic link for _run-avrdude_
```bash
sudo ln -s /usr/local/bin/run-avrdude /usr/bin/run-avrdude
```

* Synchronize time and start sketch on boot (optional)
```bash
sudo cat > /etc/rc.local <<EOL
#!/bin/sh -e

_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi

# Sync Time
ntpdate-debian -u
# Start Sketch
/usr/local/bin/run-sketch
exit 0
EOL
```

* Prevent some RealTek USB WiFi from sleep (optional) (EU)
```bash
sudo echo "options 8192cu rtw_power_mgnt=0 rtw_enusbss=1 rtw_ips_mode=1" > /etc/modprobe.d/8192cu.conf
sudo echo "options r8188eu rtw_power_mgnt=0 rtw_enusbss=1 rtw_ips_mode=1" > /etc/modprobe.d/r8188eu.conf
```

* Do not load i2c uart or spi kernel drivers

### Instructions for Arduino IDE
* Open the hardware folder of Arduino IDE
* Create a folder named "RaspberryPi"
* Clone the repository to a folder named "piduino"
```bash
cd RaspberryPi
git clone https://github.com/me-no-dev/RasPiArduino piduino
```
* Copy the toolchain to tools/arm-linux-gnueabihf (WIP)
* Restart Arduino IDE and select the RaspberryPI from the list of boards
* Compile a sketch
* Select the RespberryPi from the list of Ports (will show the IP address)
* Upload your sketch and see it go


### If everything went well
#### Selecting the board from the list of ports
![Select Pi Port](doc/pi_select.png)

#### Password prompt berfore upload
![Enter Pi Pass](doc/pi_pass.png)

#### Monitoring the sketch
![Sketch Monitor](doc/pi_monitor.png)


### Links to external tutorials
* [VIDEO: Setup Arduino IDE for Windows](https://www.youtube.com/watch?v=lZvhtfUlY8Y)
