`Arduino_10BASE-T1S`
====================

### How-to-[`EVB-LAN8670-USB`](https://www.microchip.com/en-us/development-tool/EV08L38A)
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
Microchip 10BASE-T1S LAN867X PHY usb-001:013:00: attached PHY driver (mii_bus:phy_addr=usb-001:013:00, irq=POLL)
smsc95xx_t1s 1-2.1:1.0 eth0: register 'smsc95xx_t1s' at usb-0000:00:14.0-2.1, smsc95xx USB 2.0 Ethernet, 00:1e:c0:d1:b9:4b
smsc95xx_t1s 1-2.1:1.0 enx001ec0d1b94b: renamed from eth0
```
* Patch `t1s.sh` to contain name of your ethernet interface
 ```diff
<       sudo ip addr flush dev enx001ec0d1b94b
<       sudo ip addr change 192.168.0.$ipadr/24 dev enx001ec0d1b94b
---
>       sudo ip addr flush dev eth1
>       sudo ip addr change 192.168.0.$ipadr/24 dev eth1
117c117
<       ip -4 addr | grep "enx001ec0d1b94b"
---
>       ip -4 addr | grep "eth1"
```
* Load driver using `t1s.sh` (node id can be supplied via parameter):
```bash
chmox +x t1s.sh
./t1s.sh 0 
```
loads the driver and configures the adapter as node 0 (PLCA coordinator) and sets also the IP address for the T1S adapter (via ifconfig) to `192.168.0.10`.
