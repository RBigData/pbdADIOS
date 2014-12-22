## autoconf configure.ac && autoreconf -if

################################################################################
# This file is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or distribute 
# this file, for any purpose, and by any means.
#
# In jurisdictions that recognize copyright laws, the author or authors of this 
# file dedicate any and all copyright interest in the file to the public
# domain. We make this dedication for the benefit of the public at large and to 
# the detriment of our heirs and successors. We intend this dedication to be an 
# overt act of relinquishment in perpetuity of all present and future rights to 
# this file under copyright law.
#
# THE FILE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
# WITH THIS FILE OR THE USE OR OTHER DEALINGS IN THIS FILE.
#
# For more information, please refer to http://unlicense.org/
################################################################################

AC_PREREQ([2.69])
AC_INIT([pbdADIOS], [0.1-0])
# AC_CONFIG_SRCDIR([src/R_adios_read.c])
# AC_CONFIG_SRCDIR([src/])


# ADIOS
AC_ARG_WITH([adios-home],
            AC_HELP_STRING([--with-adios-home=ADIOS_PATH],
                           [location of ADIOS installation]),
            [adios_include_path=$withval])



ADIOS_PATH="${adios_include_path}"
if test ! -f "${ADIOS_PATH}/include/adios.h" ; then
  AC_MSG_ERROR([Value of --with-adios-home does not contain include/adios.h])
fi

ADIOS_CPPFLAGS=-I${ADIOS_PATH}/include
# ADIOS_LDFLAGS=`${ADIOS_PATH}/bin/adios_config -l`
ADIOS_LDFLAGS="/Users/pragnesh/5.1.1/SGN_pbdADIOS/adios-1.8.0/lib/libadios.a /Users/pragnesh/5.1.1/SGN_pbdADIOS/adios-1.8.0/lib/libadiosread.a -lmxml"

# pbdMPI
R_SCMD="${R_HOME}/bin/Rscript -e"

MPI_CPPFLAGS=`${R_SCMD} "source('./R/get_conf.r');get.conf('PKG_CPPFLAGS','"${R_ARCH}"')"`
MPI_LDFLAGS=`${R_SCMD} "source('./R/get_conf.r');get.conf('PKG_LIBS','"${R_ARCH}"')"`


# Report
echo " "
echo "***************** Results of pbdADIOS package configure *****************"
echo " "
echo ">> EXT_LDFLAGS = ${EXT_LDFLAGS}"
echo ">> MPI_CPPFLAGS = ${MPI_CPPFLAGS}"
echo ">> MPI_LDFLAGS = ${MPI_LDFLAGS}"
echo ">> ADIOS_CPPFLAGS = ${ADIOS_CPPFLAGS}"
echo ">> ADIOS_LDFLAGS = ${ADIOS_LDFLAGS}"
echo " "
echo "*************************************************************************"
echo " "


# Substitute templates
AC_SUBST(MPI_CPPFLAGS)
AC_SUBST(MPI_LDFLAGS)
AC_SUBST(ADIOS_CPPFLAGS)
AC_SUBST(ADIOS_LDFLAGS)

AC_OUTPUT(src/Makevars)

