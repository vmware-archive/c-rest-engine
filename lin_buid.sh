#!/bin/bash
# script to build for Linux and optionally install the resulted RPM
# Author: Danut Moraru

install=false
while [ $# -gt 0 ]
do
    case "$1" in
        -i)  install=true;;
    esac
    shift
done

pushd build
make -f Makefile.bootstrap DEBUG=1
if [ $? -eq 0 ]; then
    if [ $install = true ]; then
	rpm -e vmware-rest
	pushd rpmbuild/RPMS/x86_64
	rpm -ivh vmware-rest-1.0.0-0.x86_64.rpm
	popd
	port=$(cat ../docs/SampleRestEngineConfig.cfg | grep Port | cut -d' ' -f2)
        iptables -A INPUT -p tcp --dport $port -j ACCEPT
	cp ../docs/SampleRestEngineConfig.cfg /tmp/restconfig.txt
	cp ../docs/SampleRestEngineStartCmd.txt /opt/vmware/sbin/
    fi
else 
    if [ $install = true ]; then
	echo "Build failed!"
    else 
	echo "Build failed, abandoning installation!"
    fi
fi
popd
