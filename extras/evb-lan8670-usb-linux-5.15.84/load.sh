#!/bin/bash

#Uncomment the below line if needed.
#modprobe usbnet

mac=$( lsmod | grep -o "smsc95xx_t1s" )
if [ "$mac" != "smsc95xx_t1s" ]; then
  echo -
else
  sudo rmmod smsc95xx_t1s
  sleep 1
fi

phy=$( lsmod | grep -o "microchip_t1s" )
if [ "$phy" != "microchip_t1s" ]; then
  echo -
else
  sudo rmmod microchip_t1s
  sleep 1
fi

sudo insmod microchip_t1s.ko enable=1 node_id=0 node_count=8 max_bc=0 burst_timer=128 to_timer=32
sudo insmod smsc95xx_t1s.ko

sudo ip addr add dev eth1 192.168.42.100/24
