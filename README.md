# pbdADIOS

pbdADIOS is a package to connect R with ADIOS parallel middleware 
for IO. Staging and in situ capability.

pbdADIOS requires:
  * R version 3.1.0 or higher with pbdMPI
  * [ADIOS 1.9.0 library](https://www.olcf.ornl.gov/center-projects/adios/)

More information about pbdADIOS can be found in the
[ADIOS User's Manual](http://users.nccs.gov/~pnorbert/ADIOS-UsersManual-1.7.0.pdf).

# Install
This file is for a Linux, Mac OS X and Solaris 

1. 'pbdMPI' Installation:
    ```
    See 'pbdMPI/INSTALL' for the details.
    ```

2.  MXML installation:

    Install MXML. Download MXML from : http://www.msweet.org/projects.php?Z3
    ```
    ./configure --prefix=SpecifyInstallPath  
    make
    make install 
    ```

3. ADIOS  Installation:

    Install ADIOS. Download from : https://www.olcf.ornl.gov/center-projects/adios/
    ```
    ./configure CFLAGS="-w -fpic" --prefix=SpecifyInstallPath  
          --disable-fortran
          --without-hdf5
          --without-phdf5
          --without-netcdf
          --without-nc4par
          --without-dataspaces 
          --without-flexpath
          --without-lustre 
          --enable-shared=yes 
          --with-mxml=Specify_MXML_InstallPath 
          --with-pic
    make
    make install
    ```

4. 'pbdADIOS' Installation: 

    Download and install 'pbdADIOS'
    ```
    R CMD INSTALL pbdADIOS_0.1-0.tar.gz --configure-args="--with-adios-home=Specify_ADIOS_InstallPath"
    ```


# Usage

  ```
  \# load pbdADIOS lib
  library(pbdADIOS, quiet = TRUE)

  \# print variables and attributes
  bpls("attributes.bp")

  \# dump all variables
  bp.dump("attributes.bp")

  \# read a variable
  bp.read("attributes.bp", "temperature")

  finalize() # pbdMPI final
  ```
More examples in demo directory.