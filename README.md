`Arduino_10BASE-T1S`
====================

### How-to-[`EVB-LAN8670-USB`](https://www.microchip.com/en-us/development-tool/EV08L38A)
**Note**: Keep [this application note](https://microchip.my.site.com/s/article/EVB-LAN8670-USB-Enablement-for-Debian-Ubuntu-Raspbian) in mind when building for Debian based systems. You may need to revert back to legacy network configuration tools.

* Build the kernel driver:
```bash
wget https://ww1.microchip.com/downloads/aemDocuments/documents/AIS/ProductDocuments/CodeExamples/EVB-LAN8670-USB_Linux_Driver_0v5.zip
unzip EVB-LAN8670-USB_Linux_Driver_0v5.zip
patch EVB-LAN8670-USB_Linux_Driver_0v5.patch
cd t1s-usb
make
```
* `dmesg` output when connecting the USB dongle:
```bash
[  +0,124966] usb 1-2.1: New USB device found, idVendor=184f, idProduct=0051, bcdDevice= 2.00
[  +0,000014] usb 1-2.1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  +0,000007] usb 1-2.1: Product: 10BASE-T1S
[  +0,000005] usb 1-2.1: Manufacturer: MCHP
[  +0,000004] usb 1-2.1: SerialNumber: 0000465
[  +0,004384] smsc95xx v2.0.0
[  +0,133179] Microchip 10BASE-T1S LAN867X PHY usb-001:012:00: attached PHY driver (mii_bus:phy_addr=usb-001:012:00, irq=POLL)
[  +0,000529] smsc95xx_t1s 1-2.1:1.0 eth2: register 'smsc95xx_t1s' at usb-0000:00:14.0-2.1, smsc95xx USB 2.0 Ethernet, 00:1e:c0:d1:b9:4b
```
* Load driver using `t1s.sh` (node id can be supplied via parameter):
```bash
chmox +x t1s.sh
./t1s.sh 0 
```
loads the driver and configures the adapter as node 0 (PLCA coordinator) and sets also the IP address for the T1S adapter (via ifconfig) to `192.168.0.10`.
```bash
ip link show eth1
3: eth1: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 3c:e1:a1:b8:e9:76 brd ff:ff:ff:ff:ff:ff
```
