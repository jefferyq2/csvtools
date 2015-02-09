Name:		csvtools
Version:	1.1.61
Release:	1%{?dist}
Summary:	a CSV parsing and database inserting tools

Group:		System Environment/Base
License:	GPLv2+
URL:		None
Source0:	%{name}-%{version}.tar.gz

BuildRequires:  mysql-devel postgresql-devel unixODBC-devel sqlite-devel
Requires:	csvtools-libs postgresql-libs unixODBC mysql-libs sqlite

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

#%package -n csvtools
#Summary: a CSV parsing and database inserting binary tools
#Requires: csvtools-libs
#%description -n csvtools
#the set of the binary toosl to be uses with the csvtools 
#package.
#the tools are csv2mysql , csv2pgsql , csv2odbc , csv2sqlite
#csvcombine and csvsplit

%files 
#%files -n csvtools
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

%package -n csvtools-libs
Summary: A set of libraries for the csvtools package

%description -n csvtools-libs
a set of libraries and a header file for C to be used
by the csvtools binary

%files -n csvtools-libs
%{_includedir}/csvtools.h
%{_libdir}/libcsvtools.a
%{_libdir}/libcsvtools.la
%{_libdir}/libcsvtools.so
%{_libdir}/libcsvtools.so.0
%{_libdir}/libcsvtools.so.0.0.0

%doc README INSTALL



%changelog
* Sun Jun 15 2014 Oren Oichman  <two.oes@gmail.com> 1.1.60-1
- spiting the packages of the binary and the libraries

* Tue May 13 2014 Oren Oichman 	<two.oes@gmail.com> 1.1.60-1
- adding the csvplit and the csvcombine tools to the main RPM

