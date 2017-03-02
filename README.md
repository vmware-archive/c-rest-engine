VMWare REST (Trident)
=====================
Trident is a minimal embedded HTTP(S) server written in C. Its primary intent is to 
enable REST(Representational State Transfer) API support for C daemons.

Trident can also be easily consumed via copenAPI project to adhere open API
specification by linux foundation previously known as swagger API specs.

Platforms
---------

Trident provides library for following platforms

1. Linux(.so)

2. Windows(.dll)


Design Goals
-------------

1. Provides REST RPC mechanism for services and daemons written in ANSI C.

2. Provides both Secure Socket Layer(SSL) and plain text communicatiom 
   over wire.

3. Should be small in size.

4. High performance.

5. Easily extensive.


Prerequisites
-------------

Trident uses following open source project.

Linux:

1.  Openssl (Linux)

Windows:

1. OpensSL (Windows)

2. rd-platform-sdk-windows (VMWare internal POSIX library implementation)


Source code
-----------

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git clone https://github.com/vmware/trident.git
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Build
-----

These build instructions are to build Trident on VMware's Photon Linux
distribution.

1.  Clone trident git repository onto your Photon (Full) installation.

2.  "cd build"

3.  For RPM build: 
    "make -f Makefile.bootstrap DEBUG=1"

    For object file in same directory:
    "autoreconf -mif .. && ../configure && make"

4.  As part of a successful build, the following RPMs should be created in the

    1.  trident/build/rpmbuild/RPMS/x86_64/vmware-rest-1.0.1-6.x86_64.rpm

Installation
------------

1. While building from source:

Use RPM install command to install the above generated RPM.

"rpm -ivh trident/build/rpmbuild/RPMS/x86_64/vmware-rest-1.0.1-6.x86_64.rpm"

The installed libraries will go in following folder 
"/opt/vmware/lib64/"

NOTE: If previous version of same RPM is already installed please delete it
using the following command "rpm -e vmware-rest-1.0.1-6"

2. Using package manager on Photon OS Linux distribution.

Pre-built library for trident are available through the following YUM
repositories that can be configured on your Photon deployment.

After the following YUM repositories have been configured, it should be possible
to install the trident using "tdnf install vmware-rest".

### Trident YUM repository

Create the file "/etc/yum.repos.d/photonSupport.repo" with the following contents.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[lightwave]
name=VMware Lightwave 1.0(x86_64)
baseurl=https://dl.bintray.com/vmware/photonSupport
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY
gpgcheck=0
enabled=1
skip_if_unavailable=True
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

