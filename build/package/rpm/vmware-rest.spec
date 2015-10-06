Name:    vmware-rest
Summary: VMware REST Library
Version: 1.0.0
Release: 0
Group:   Applications/System
Vendor:  VMware, Inc.
License: VMware
URL:     http://www.vmware.com
BuildArch: x86_64
Requires:  coreutils >= 8.22, openssl >= 1.0.1
BuildRequires:  coreutils >= 8.22, openssl-devel >= 1.0.1

%if "%{_debug}" == "1"
%define __strip /bin/true
%define _enable_debug yes
%else
%define _enable_debug no
%endif

%description
VMware REST Library

%build
export CFLAGS="-Wno-unused-but-set-variable -Wno-pointer-sign -Wno-implicit-function-declaration -Wno-address -Wno-enum-compare"
cd build
autoreconf -mif ..
../configure \
    --prefix=%{_prefix} \
    --libdir=%{_lib64dir} \
    --with-ssl=/usr \
    --enable-debug=%{_enable_debug}

make

%install

[ %{buildroot} != "/" ] && rm -rf %{buildroot}/*
cd build && make install DESTDIR=$RPM_BUILD_ROOT



%post

    /sbin/ldconfig

%files
%defattr(-,root,root)
%{_sbindir}/vmrestd
%{_bindir}/rest-cli
%{_lib64dir}/libvmrestclient.so*
%{_lib64dir}/librestengine.*
%{_lib64dir}/libtransport.*

%exclude %{_includedir}/vmrest.h
%exclude %{_lib64dir}/libvmrestclient.la
%exclude %{_lib64dir}/libvmrestclient.a

# %doc ChangeLog README COPYING

%changelog

