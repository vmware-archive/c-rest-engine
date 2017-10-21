# !/bin/bash
TOPDIR=`pwd`
IPADDR="$(ifconfig | grep -A 1 'eth0' | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"
PORT="81"

# Compile from source in the same directory
gcc -o $TOPDIR/infiteData $TOPDIR/infiteData.c

$TOPDIR/infiteData $IPADDR $PORT "15000" "TEST 1"

$TOPDIR/infiteData $IPADDR $PORT "1" "TEST 2"

$TOPDIR/infiteData $IPADDR $PORT "10" "TEST 3"

$TOPDIR/infiteData $IPADDR $PORT "512" "TEST 4"

$TOPDIR/infiteData $IPADDR $PORT "4095" "TEST 5"

$TOPDIR/infiteData $IPADDR $PORT "5000" "TEST 6"

$TOPDIR/infiteData $IPADDR $PORT "10000" "TEST 7"

$TOPDIR/infiteData $IPADDR $PORT "32000" "TEST 8"

$TOPDIR/infiteData $IPADDR $PORT "1024" "TEST 9"

rm -f $TOPDIR/infiteData

