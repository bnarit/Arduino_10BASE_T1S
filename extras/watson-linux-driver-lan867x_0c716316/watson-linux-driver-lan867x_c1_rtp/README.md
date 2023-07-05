# LAN867x 10BASE-T1S PHY Linux Driver

This document describes the procedure for compiling and configuring the LAN867x driver in the Raspberry Pi 4. This driver package provides support for the Linux kernel versions **6.1.21** and **5.15.84**.

## Setup the hardware
- Microchip LAN867x product website link:
    - [LAN8670](https://www.microchip.com/en-us/product/lan8670)
    - [LAN8671](https://www.microchip.com/en-us/product/lan8671)
    - [LAN8672](https://www.microchip.com/en-us/product/lan8672)
    - [EVB-LAN8670-USB](https://www.microchip.com/en-us/development-tool/EV08L38A)
- Raspberry Pi 4 software:
    - [RPI Software](https://www.raspberrypi.com/software/)
## Prerequisites
If the Raspberry Pi OS is freshly installed, then the below prerequisites are mandatory before using the driver package. Please make sure the Raspberry Pi is connected with internet because some dependencies need to be installed.
- Please make sure the **current date and time** of Raspberry Pi is up to date. Can be checked with the below command,

```
    $ date
```
- If they are not up to date and if you want to set it manually then use the below example and modify the fields for the current date and time,
```
    $ sudo date -s "Thursday 25 May 2023 10:44:43 AM IST"
```
Link to refer: [set-date-time-raspberry-pi](https://raspberrytips.com/set-date-time-raspberry-pi/)
- Please make sure the **apt-get is up to date**. Run the below command to update apt-get,
```
    $ sudo apt-get update
```
- Make sure the **linux headers** are installed in Pi, if not use the below command to install it,
```
    $ sudo apt-get install raspberrypi-kernel-headers
```
## Support for Linux kernel version 6.1.21 with Raspberry Pi 4
These procedures are tested in **Raspberry Pi 4 with Linux Kernel 6.1.21**.

**Note:** The latest RPI software image uses 64bit image in default. But the above command installs Linux headers for 32bit image. So it may be required to disable 64bit image in the config.txt which enables 32bit image automatically. Refer the below links for more information.

Link to refer: [rpi_config_txt](https://www.raspberrypi.com/documentation/computers/config_txt.html#arm_64bit)

Link to refer: [rpi_config_txt_howto](https://medium.com/for-linux-users/how-to-make-your-raspberry-pi-4-faster-with-a-64-bit-kernel-77028c47d653)
## Prepare Driver
- Extract the downloaded software package into your local directory using the below command,

```
    $ unzip lan867x-linux-driver-1v0.zip
    $ cd lan867x-linux-driver-1v0/linux-v6.1.21-support/
    $ make
    $ sudo insmod microchip_t1s.ko enable=1 node_id=0 node_count=8 max_bc=0 burst_timer=128 to_timer=32
```
## Support for Linux kernel version 5.15.84 with Raspberry Pi 4
These procedures are tested in **Raspberry Pi 4 with Linux Kernel 5.15.84**.
## Prepare Driver
- Extract the downloaded software package into your local directory using the below command,
```
    $ unzip lan867x-linux-driver-1v0.zip
    $ cd lan867x-linux-driver-1v0/linux-v5.15.84-support/
    $ make
    $ sudo insmod microchip_t1s.ko enable=1 node_id=0 node_count=8 max_bc=0 burst_timer=128 to_timer=32
    $ sudo insmod smsc95xx_t1s.ko
```
**Note:** If the microchip_t1s.ko is loaded without any module parameters then the default settings of PLCA will be configured as mentioned below in the **Configure Driver** section.

**Note:** Use the below command to enable better performance in Raspberry Pi,
```
    $ echo performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor > /dev/null
```
## Configure Driver
- Driver can take the following module parameters for configuring the PLCA settings,
    - enable (1-enable plca and 0-disable plca)
    - node_id (0-255 plca node id)
    - node_count (0-255 plca node count)
    - max_bc (0-255 max burst count)
    - burst_timer (0-255 burst timer)
    - to_timer (0-255 to timer)
- Default PLCA settings if they are not configured through module parameters,
    - enable=1
    - node_id=0
    - node_count=8
    - max_bc=0
    - burst_timer=128
    - to_timer=32
- Example configuration,
```
    $ sudo insmod microchip_t1s.ko enable=1 node_id=0 node_count=8 max_bc=0 burst_timer=128 to_timer=32
```
- Example ethernet configuration,
```
    $ sudo ip addr add dev eth1 192.168.1.100/24
```
## Note
- If you are using EVB-LAN8670-USB in the Raspberry Pi 4 with Linux kernel 6.1.21, then the support for the MAC is already in the smsc95xx.c driver. So EVB-LAN8670-USB has to be connected after loading the above driver otherwise the smsc95xx will load the generic phy driver. If the EVB-LAN8670-USB is already connected before loading the above driver then you can use the below commands to bind the driver to the above driver. In this case you don't need to unplug and replug the EVB-LAN8670-USB.
```
    $ echo <usb_device_name> | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind > /dev/null
    $ sudo insmod microchip_t1s.ko
    $ echo <usb_device_name> | sudo tee /sys/bus/usb/drivers/smsc95xx/bind > /dev/null
```
- Example,
```
    $ echo 1-1.3:1.0 | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind > /dev/null
    $ sudo insmod microchip_t1s.ko
    $ echo 1-1.3:1.0 | sudo tee /sys/bus/usb/drivers/smsc95xx/bind > /dev/null
```
- As mentioned above this driver has been developed and tested against Linux kernel versions 6.1.21 and 5.15.84. If you are using different versions of the Linux kernel then please contact Microchip support team for the backport patches supported for that corresponding version.
- Mainlining the LAN867x PHY driver (microchip_t1s.c) in the upstream is in progress. Hopefully the next latest release of the Linux kernel will have the support for this driver.
