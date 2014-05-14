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
the csvtools are a set of tools design to enable the
insertion of a csv contant to most of the given SQL 
servers which the main focus of the tools is about 
Mysql/Mariadb, PostgreSQL and ODBC.
the csvtools also supply a set of tools to change the 
CSV file or to different file types or to split and
comabin any the type of CSV files.


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
%{_bindir}/csvcombine
%{_bindir}/csvsplit
%{_mandir}/man1/csv2mysql.1.gz
%{_mandir}/man1/csv2odbc.1.gz
%{_mandir}/man1/csv2pgsql.1.gz
%{_mandir}/man1/csv2sqlite.1.gz
%{_mandir}/man1/csvsplit.1.gz
%{_mandir}/man1/csvcombine.1.gz
%{_includedir}/csvtools.h
%{_libdir}/libcsvtools.a
%{_libdir}/libcsvtools.la
%{_libdir}/libcsvtools.so
%{_libdir}/libcsvtools.so.0
%{_libdir}/libcsvtools.so.0.0.0

%doc README INSTALL



%changelog
* Tue May 13 2014 Oren Oichman 	<two.oes@gmail.com> 1.1.60-1
- adding the csvplit and the csvcombine tools to the main RPM
