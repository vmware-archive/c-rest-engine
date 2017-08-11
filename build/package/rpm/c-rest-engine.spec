Name:          c-rest-engine
Summary:       Minimal http(s) server library
Version:       1.0.3
Release:       3%{?dist}
Group:         Applications/System
Vendor:        VMware, Inc.
License:       Apache 2.0
URL:           http://www.github.com/vmware/c-rest-engine
BuildArch:     x86_64
BuildRequires: coreutils >= 8.22
BuildRequires: openssl-devel >= 1.0.1
Requires:      coreutils >= 8.22
Requires:      openssl >= 1.0.1

%if "%{_debug}" == "1"
%define __strip /bin/true
%define _enable_debug yes
%else
%define _enable_debug no
%endif

%description
c-rest-engine is a minimal embedded http(s) server written in C.
Its primary intent is to enable REST(Representational State Transfer)
API support for C daemons.

%package devel
Summary:       c-rest-engine dev files
Requires:      coreutils >= 8.22
Requires:      openssl-devel >= 1.0.1
Requires:      %{name} = %{version}-%{release}

%description devel
development libs and header files for c-rest-engine

%build
cd build
autoreconf -mif ..
../configure \
    --prefix=%{_prefix} \
    --libdir=%{_lib64dir} \
    --with-ssl=/usr \
    --enable-debug=%{_enable_debug} \
    --disable-static

make

%install

[ %{buildroot} != "/" ] && rm -rf %{buildroot}/*
cd build && make install DESTDIR=$RPM_BUILD_ROOT
find %{buildroot} -name '*.la' -delete

%post
/sbin/ldconfig

%files
%defattr(-,root,root)
%{_lib64dir}/*.so.*
%exclude %{_sbindir}/vmrestd
#%{_sbindir}/vmrestd

%files devel
%{_includedir}/vmrest.h
%{_lib64dir}/*.so

%changelog
*   Fri Aug 11 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.3-3
-   Fixing all known coverity bugs.
*   Fri Aug 04 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.3-2
-   Applying security fixes for set SSL info.
*   Thu Jul 20 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.2-2
-   Providing API for set SSL info, Bug#1864924
*   Mon Jun 19 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.2-1
-   Updating to version 1.0.2
*   Fri Jun 16 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.1-4
-   Relaxing maximum URI length.
*   Thu Jun 08 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.1-3
-   Fixing file upload.
*   Wed Jun 07 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.1-2
-   Fixing chunk encoded packet issue in getData.
*   Fri May 26 2017 Kumar Kaushik <kaushikk@vmware.com> 1.0.1-1
-   Updating the spec version.
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

