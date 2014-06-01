#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([netstream], [1.0], [catchgreen@163.com])
AC_CONFIG_SRCDIR([netstream.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_RANLIB
AM_PROG_AR
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_SEARCH_LIBS(pthread_create, pthread)
#AC_SEARCH_LIBS(dlopen, dl)
#AC_SEARCH_LIBS(snmp_sess_init, netsnmp)
AC_SEARCH_LIBS(xmlStrcmp, xml2)

# Checks for libraries.

# Checks for header files.
AC_SUBST(LIB_XML_INCLUDE, /usr/include/libxml2/)
AC_SUBST(LIB_SNMP_INCLUDE, /usr/local/snmp/include/)
AC_SUBST(LIB_SNMP_LIB, /usr/local/snmp/lib/)
AC_DEFINE(OSPF_VERSION,,support the ospf protocol)
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero getcwd gettimeofday localtime_r memset select socket strchr strdup strerror])

AC_CONFIG_FILES(Makefile
				utils/Makefile
				comm/Makefile
				lib/Makefile)

AC_OUTPUT
