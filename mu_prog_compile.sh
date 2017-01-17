#!/bin/bash
trap "" HUP

#PID=$(ps ax | grep aMussel | head -n 1 | awk '{print $1}')
#sudo kill $PID

#sudo killall pigpiod
#if [ -f /var/run/pigpio.pid ]; then
#	echo "Entered" > tmp
#	PID=$(cat /var/run/pigpio.pid)
#	echo $PID
#	sudo kill -9 $PID
#fi



# get last ssh ip address
# sship=$(last | grep pts | head -n 1 | awk '{print $3}')
sship=$(cat /var/log/auth.log | grep --text sshd | grep --text "Accepted password" | tail -n 1 | awk '{print $11}')
echo $sship
echo "SSH IP = $sship" >> /home/pi/amussel/mu_i2c_programming/cred.log


# get my ip
#myip=$(ifconfig wlan0 | awk '/t addr:/{gsub(/.*:/,"",$2);print$2}')
myip=$(hostname -I | cut -f1 -d' ')
echo $myip
echo "MY IP = $myip" >> /home/pi/amussel/mu_i2c_programming/cred.log

sshport=$(hostname | sed -n -e 's/.*amussel//p' | head -n 1)
sshport=$((sshport+10000))
echo $sshport
echo "SSH PORT = $sshport" >> /home/pi/amussel/mu_i2c_programming/cred.log

# start compiling
cd /home/pi/amussel/mu_i2c_programming

#curr_dir=$(pwd)
#cd $curr_dir

# delete residual files
rm prog*


gcc -Wall -pthread -lpigpiod_if2 -lrt *.c -o prog_mu > /home/pi/amussel/mu_i2c_programming/cmpl.log

# remove used StringImage.h
#rm StringImage.h

# start programming and send ip address as an argument
sudo ./prog_mu "${sship}" "${sshport}" "${myip}" > /home/pi/amussel/mu_i2c_programming/prog.log &
