#!/bin/bash

ARCH=$(uname -a)
CPUP=$(grep "physical id" /proc/cpuinfo | sort -u | wc -l)
CPUV=$(grep -c "processor" /proc/cpuinfo)

RAM_TOTAL=$(free -m | awk '/Mem:/ {print $2}')
RAM_USED=$(free -m | awk '/Mem:/ {print $3}')
RAM_PERCENT=$(echo "scale=2; $RAM_USED/$RAM_TOTAL*100" | bc)

DISK_TOTAL=$(df -BM --total | awk '/total/ {print $2}' | sed 's/M//')
DISK_USED=$(df -BM --total | awk '/total/ {print $3}' | sed 's/M//')
DISK_PERCENT=$(echo "scale=2; $DISK_USED/$DISK_TOTAL*100" | bc)

CPU_LOAD=$(top -bn1 | grep '^%Cpu' | cut -c 9- | xargs | awk '{printf("%.1f%%"), $1 + $3}')

LB=$(who -b | awk '{print $3 " " $4}')

LVMU=$(lsblk | grep -q lvm && echo yes || echo no)

TCPC=$(ss -t state ESTABLISHED | wc -l)

ULOG=$(users | wc -w)

IP=$(hostname -I)
MAC=$(ip link show | awk '/link\/ether/ {print $2}')

CMND=$(journalctl _COMM=sudo | grep COMMAND | wc -l)

wall "  #Architecture: $ARCH
  #CPU physical: $CPUP
  #vCPU: $CPUV
  #Memory Usage: $RAM_USED/${RAM_TOTAL}MB ($RAM_PERCENT%)
  #Disk Usage: $DISK_USED/${DISK_TOTAL}MB ($DISK_PERCENT%)
  #CPU load: $CPU_LOAD
  #Last boot: $LB
  #LVM use: $LVMU
  #Connections TCP: $TCPC ESTABLISHED
  #User log: $ULOG
  #Network: IP $IP ($MAC)
  #Sudo: $CMND cmd"

