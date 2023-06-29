`Arduino_10BASE_T1S`
====================

### How-to-[`EVB-LAN8670-USB`](https://www.microchip.com/en-us/development-tool/EV08L38A)
**Note**: Keep [this application note](https://microchip.my.site.com/s/article/EVB-LAN8670-USB-Enablement-for-Debian-Ubuntu-Raspbian) in mind when building for Debian based systems. You may need to revert back to legacy network configuration tools.

* Build kernel driver:
```bash
cd extras/evb-lan8670-usb-linux-5.19
make
```
* Load kernel driver (configuration of 10BASE-T1S PHY within `load.sh`):
```bash
./load.sh
```
* `dmesg` output when connecting the USB dongle:
```bash
[  +0,124387] usb 1-2.2: New USB device found, idVendor=184f, idProduct=0051, bcdDevice= 2.00
[  +0,000014] usb 1-2.2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  +0,000006] usb 1-2.2: Product: 10BASE-T1S
[  +0,000004] usb 1-2.2: Manufacturer: MCHP
[  +0,000005] usb 1-2.2: SerialNumber: 0000465
[  +0,003908] smsc95xx v2.0.0
[  +0,056568] LAN867X Rev.B1 usb-001:014:00: PLCA mode enabled. Node Id: 0, Node Count: 8, Max BC: 0, Burst Timer: 128, TO Timer: 32
[  +0,000236] LAN867X Rev.B1 usb-001:014:00: attached PHY driver (mii_bus:phy_addr=usb-001:014:00, irq=191)
[  +0,001013] smsc95xx_t1s 1-2.2:1.0 eth1: register 'smsc95xx_t1s' at usb-0000:00:14.0-2.2, smsc95xx USB 2.0 Ethernet, 00:1e:c0:d1:b9:4b
```
* You can take a look at the registered Ethernet device via `ip link show eth1`:
```bash
ip link show eth1
3: eth1: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 3c:e1:a1:b8:e9:76 brd ff:ff:ff:ff:ff:ff
```

### How-to-`iperf`
```bash
iperf -c 192.168.0.100 -u -b 10M
iperf -s 192.168.0.100 -u -b 10M
```
