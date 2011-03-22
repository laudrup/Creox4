%define name creox
%define	version 0.2.2rc2
%define release 1
%define prefix /usr

%define builddir $RPM_BUILD_DIR/%{name}-%{version}

Summary:   The Realtime Sound Effector
Name:      %{name}
Version:   %{version}
Release:   %{release}
Prefix:    %{prefix}
Copyright: GPL v2
Vendor:    Jozef Kosoru <jozef.kosoru@pobox.sk>
URL:       http://www.uid0.sk/zyzstar/projects/creox/downloads/creox-0.2.1.tar.bz2
Packager:  Jozef Kosoru <jozef.kosoru@pobox.sk>
Group:     Multimedia
Source:    %{name}-%{version}.tar.bz2
BuildRoot: /tmp/build-%{name}-%{version}

%description
The Realtime Sound Effector

%prep
rm -rf %{builddir}

%setup
touch `find . -type f`
if [ -z "$KDEDIR" ]; then
	export KDEDIR=%{prefix}
fi
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                --prefix=$KDEDIR --with-install-root=$RPM_BUILD_ROOT \
                $LOCALFLAGS

%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT
chmod u+s $RPM_BUILD_ROOT/$KDEDIR/bin/%{name}

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.%{name}
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}

%post
chmod u+s $KDEDIR/bin/%{name}

%postun

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{builddir}
rm -f $RPM_BUILD_DIR/file.list.%{name}


%files -f ../file.list.%{name}
