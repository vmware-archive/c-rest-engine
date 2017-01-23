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
        port=$(cat ../docs/SampleRestEngineConfig.cfg | grep Port | cut -d' ' -f2)
        version=$(cat package/rpm/vmware-rest.spec | grep Version | cut -d' ' -f2)
	release=$(cat package/rpm/vmware-rest.spec | grep Release | cut -d' ' -f2)
	pushd rpmbuild/RPMS/x86_64
	rpm -ivh vmware-rest-$version-$release.x86_64.rpm
	popd
        iptables -A INPUT -p tcp --dport $port -j ACCEPT
	iptables -A OUTPUT -p tcp --dport $port -j ACCEPT
	cp ../docs/SampleRestEngineConfig.cfg /tmp/restconfig.txt
	cp ../docs/RestEngineStart.tcf /opt/vmware/sbin/
	cp ../docs/RestEngineStop.tcf /opt/vmware/sbin/
	cp ../docs/RestStartStopRepeat.tcf /opt/vmware/sbin/
    fi
else 
    if [ $install = true ]; then
	echo "Build failed!"
    else 
	echo "Build failed, abandoning installation!"
    fi
fi
popd
