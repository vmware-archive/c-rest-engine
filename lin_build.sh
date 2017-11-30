#!/bin/bash
# script to build for Linux and optionally install the resulted RPM
# Author: Danut Moraru
# Test for gerrit

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
        port=$(cat ../docs/SampleRestEngineConfigPlain.cfg | grep Port | cut -d' ' -f2 | cut -d'p' -f1 | cut -d'P' -f1)
        version=$(cat package/rpm/vmware-rest.spec | grep Version | cut -d' ' -f2)
        release=$(cat package/rpm/vmware-rest.spec | grep Release | cut -d' ' -f2)
        pushd rpmbuild/RPMS/x86_64
        rpm -ivh vmware-rest-$version-$release.x86_64.rpm
        popd
        iptables -C INPUT -p tcp --dport $port -j ACCEPT
        if [ $? -ne 0 ]; then
            iptables -A INPUT -p tcp --dport $port -j ACCEPT
            iptables -A OUTPUT -p tcp --dport $port -j ACCEPT
        fi
        cp ../docs/SampleRestEngineConfigPlain.cfg /tmp/restconfig.txt
        cp ../docs/SampleRestEngineConfigPlain.cfg /root/restconfig.txt
    fi
else 
    if [ $install = true ]; then
        echo "Build failed!"
    else 
        echo "Build failed, abandoning installation!"
    fi
fi
popd
