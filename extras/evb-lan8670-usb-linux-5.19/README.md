# LAN867x 10BASE-T1S PHY Linux Driver

This document describes the procedure for compiling and configuring the LAN867x driver. This driver is updated for supporting **Linux kernel version 5.19**. If you are using different versions of the Linux kernel then please contact Microchip support team for the backport patches supported for that corresponding version.

## Prepare Driver
- Extract the downloaded software package into your local directory using the below command,

```
    $ unzip evb-lan8670-usb-linux-5.19.zip
    $ cd evb-lan8670-usb-linux-5.19/
    $ make
    $ sudo insmod microchip_t1s.ko
    $ sudo insmod smsc95xx_t1s.ko
```
**Note:** Loading microchip_t1s.ko module without any module parameters will configure the default PLCA settings mentioned below. Please refer the below **Configure Driver** section for more details.
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
**Note:** A sample load.sh file is included in the software package for your reference.
