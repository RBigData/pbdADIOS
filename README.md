# pbdADIOS

pbdADIOS is a package to connect R with ADIOS parallel middleware 
for IO. Staging and in situ capability.

pbdADIOS requires:
  * R version 3.1.0 or higher with pbdMPI
  * [ADIOS 1.7.0 library](https://www.olcf.ornl.gov/center-projects/adios/)

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

To build on sith and rhea:

```
module swap PE-pgi PE-gnu
module load adios/1.7.0
module load r
export ADIOS_DIR=/sw/redhat6/adios/1.7.0/rhel6_gnu4.7.1_wrappers
R CMD INSTALL pbdADIOS --configure-args="--with-adios-home=$ADIOS_DIR"
```

To run on sith (note: change some hard coded paths to your locations!):

```
qsub run_sith.sh
```

`run_sith.sh` contains:

```
#!/bin/sh
#PBS -N adiosis
#PBS -l nodes=1:ppn=32
#PBS -l walltime=0:30:00
#PBS -e pbs.err
#PBS -o pbs.log

source /etc/profile.d/modules.sh
module swap PE-pgi PE-gnu
module load r
module load adios/1.7.0

cd $MEMBERWORK/stf006/heat

time mpirun -np 4 --mca mpi_warn_on_fork 0 \
    Rscript /ccs/home/ost/adios/pbdADIOS/tests/test_heat.r
```