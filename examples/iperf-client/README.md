:floppy_disk: `iperf-client`
=============================

This example sketch can be used to measure 10BASE-T1S network performance using this software stack. 

### How-to-compile/upload
```bash
arduino-cli compile -b arduino:renesas_uno:minima -v examples/iperf-client -u -p /dev/ttyACM0
```

### How-to-`iperf`
The following commands assume that you've set up 10BASE-T1S interface on your PC (i.e. [`EVB-LAN8670-USB`](https://www.microchip.com/en-us/development-tool/EV08L38A)) with an IP address of `192.168.42.100`.

Open a serial monitor on your PC in order to start running the sketch:
```bash
cat /dev/ttyACM0
```
```bash
IP	192.168.42.101
MAC	FC:C2:3D:46:26:F8
...
```
Verify that you can ping your board running this sketch from your PC:
```bash
ping 192.168.42.101
```
```bash
PING 192.168.42.101 (192.168.42.101) 56(84) bytes of data.
64 bytes from 192.168.42.101: icmp_seq=1 ttl=255 time=3.16 ms
64 bytes from 192.168.42.101: icmp_seq=2 ttl=255 time=3.26 ms
...
```
Start `iperf` on your PC:
```bash
iperf --client 192.168.42.101 --udp --bandwidth 10M
```
