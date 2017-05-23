Name:    vmware-rest
Summary: VMware REST Library
Version: 1.0.0
Release: 3
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
%exclude %{_sbindir}/*
%{_includedir}/vmrest.h
%{_lib64dir}/librestengine.*

# %doc ChangeLog README COPYING

%changelog
*   Mon May 22 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.0-3
-   Get/Set data API signature change.
*   Fri May 19 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.0-2
-   Fixing ESX crash issue.
*   Fri May 12 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.0-1
-   Fixing version to 1.0.X format.
*   Tue Apr 18 2017 Kumar Kaus
*   Tue May 09 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0-1
-   Initianl release version.
*   Tue Apr 18 2017 Kumar Kaushik <kaushikk@vmware.com> 0.9-1
-   First pre-release version

