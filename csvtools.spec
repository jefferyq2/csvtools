Name:		csvtools
Version:	1.0.60
Release:	2%{?dist}
Summary:	a CSV parsing and database inserting tools

Group:		System Environment/Base
License:	GPLv2+
URL:		None
Source0:	%{name}-%{version}.tar.gz

BuildRequires:  mysql-devel postgresql-devel unixODBC-devel sqlite-devel
Requires:	postgresql-libs unixODBC mysql-libs sqlite

%description
the csvtools are a set of tools design to inable an
insetring of csv to most of the given SQL servers

the main focus of the tools is about Mysql and
PostgreSQL and ODBC.


%prep
%setup -q


%build
%configure --prefix=/usr \
	--with-odbc \
	--with-mysql \
	--with-pgsql \

make %{?_smp_mflags}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

install -m 755 -d %{buildroot}/%{_bindir}
install -m 755 -d %{buildroot}/%{_mandir}
#install -m 755  %{buildroot}/%{_bindir}/csv2mysql %{_bindir}/csv2mysql
#install -m 755 %{buildroot}/%{_bindir}/csv2mysql %{_bindir}/csv2pgsql
#install -m 755 %{buildroot}/%{_bindir}/csv2mysql %{_bindir}/csv2sqlite
#install -m 755 %{buildroot}/%{_bindir}/csv2mysql %{_bindir}/csv2odbc
#install -m 755 %{buildroot}/%{_mandir}/man1/csv2mysql.1.gz %{_mandir}/man1/csv2mysql.1.gz
#install -m 755 %{buildroot}/%{_mandir}/man1/csv2pgsql.1.gz %{_mandir}/man1/csv2pgsql.1.gz
#install -m 755 %{buildroot}/%{_mandir}/man1/csv2odbc.1.gz %{_mandir}/man1/csv2odbc.1.gz
#install -m 755 %{buildroot}/%{_mandir}/man1/csv2sqlite.1.gz %{_mandir}/man1/csv2sqlite.1.gz

#rm -rf %{buildroot}

%files
%{_bindir}/csv2mysql
%{_bindir}/csv2pgsql
%{_bindir}/csv2odbc
%{_bindir}/csv2sqlite
%{_mandir}/man1/csv2mysql.1.gz
%{_mandir}/man1/csv2odbc.1.gz
%{_mandir}/man1/csv2pgsql.1.gz
%{_mandir}/man1/csv2sqlite.1.gz
%{_includedir}/csvtools.h
%{_libdir}/libcsvtools.a
%{_libdir}/libcsvtools.la
%{_libdir}/libcsvtools.so
%{_libdir}/libcsvtools.so.0
%{_libdir}/libcsvtools.so.0.0.0

%doc README INSTALL



%changelog

