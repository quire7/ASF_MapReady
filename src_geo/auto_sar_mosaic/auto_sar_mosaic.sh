#!/bin/sh
#
# NAME: auto_sar_mosaic - create a single mosaic from  two  or  more
#     SAR images
#
# SYNOPSIS:
#     auto_sar_mosaic [-c] [-h] [-p pixel_size] [ -i] [-f fill]
#            <projfile> <projkey> <output_file> <input files...>
#
# DESCRIPTION:
#     auto_sar_mosaic will assemble SAR CEOS images, from any swath
#     or frame, into a large mosaic in LAS format.
#       
# EXTERNAL ASSOCIATES:
#    NAME:               USAGE:
#    ---------------------------------------------------------------
#
# FILE REFERENCES:
#    NAME:               USAGE:
#    ---------------------------------------------------------------
#	
# PROGRAM HISTORY:
#   Mike Shindle  
#   1995 June 1
#
#   1.0 - original program.
#   1.1 - added calibrate. included option to turn calibrate off.
#       - added version number info.
#       - added resample capability.
#   2.0 - added nongeocoded image capability. 
#   2.1 - handles new resample, sartpsgrid, & .tlr files
#   2.2 - check for projection file
#   2.3 - added imageid functionality
#   2.4 - added the ability to force lon. degrees to be in degrees east
#   2.5 - changed command line syntax, use -b replaces -d.
#         before running projprm, checks to see if GEO projection exists
#   2.51 - modified to use new metadata program, T. Logan 5/96
#   2.6 - Corrected a possible bug caused by using [] test conditions.
#         This is a documented "feature" of the Bourne Shell that was
#         corrected in the Korn Shell by using the [  ] test conditions.
#   3.0 - No longer uses date string.
#   4.0 - Replace calls to sarautoreg with calls to geocode
#   4.1 - Added fill option
#
# HARDWARE/SOFTWARE LIMITATIONS:
#	A projection file must be created before running  this  pro-
#    	gram.  Use the projprm(1) program to create such a file.
#     	See  projprm  for  more details.
#
# ALGORITHM DESCRIPTION:
#
# ALGORITHM REFERENCES:
#
# BUGS
#	
#****************************************************************************
#								            *
#   auto_sar_mosaic - create a single mosaic from  two  or  more SAR images *
#   Copyright (C) 2001  ASF Advanced Product Development    	    	    *
#									    *
#   This program is free software; you can redistribute it and/or modify    *
#   it under the terms of the GNU General Public License as published by    *
#   the Free Software Foundation; either version 2 of the License, or       *
#   (at your option) any later version.					    *
#									    *
#   This program is distributed in the hope that it will be useful,	    *
#   but WITHOUT ANY WARRANTY; without even the implied warranty of    	    *
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   	    *
#   GNU General Public License for more details.  (See the file LICENSE     *
#   included in the asf_tools/ directory).				    *
#									    *
#   You should have received a copy of the GNU General Public License       *
#   along with this program; if not, write to the Free Software		    *
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
#									    *
#       ASF Advanced Product Development LAB Contacts:			    *
#	APD E-mail:	apd@asf.alaska.edu 				    *
# 									    *
#	Alaska SAR Facility			APD Web Site:	            *	
#	Geophysical Institute			www.asf.alaska.edu/apd	    *
#      	University of Alaska Fairbanks					    *
#	P.O. Box 757320							    *
#	Fairbanks, AK 99775-7320					    *
#								  	    *
#**************************************************************************/
#

# set initial flags
calibrate_flag=1
mosaic_flag=1
id_flag=0
out_mosaic=""
colorConcat=""
TableFile=tableids.$$
pixFlag=""
fillGeocode=""
fillConcat=""

#
# check_error- enter here to trap run-time errors
#
check_error ()
{
	if [ ! $? -eq 0 ]
	then
		echo "auto_sar_mosaic: Last command returned in error."
		rm sarreg.tmp.*
		exit 1
	fi
}

# define function print_usage
print_usage ()
{ 
      echo ;
      echo "Usage: auto_sar_mosaic [-c] [-h] [-p pixel_size] [ -i] [-f fill]" ;
      echo "            <projfile> <projkey> <output_file> <input files...>" ;
      echo ;
      echo "\t-c\t\tdo not calibrate files" ;
      echo "\t-h\t\tCreate a color concatenated image\n\t\t\t(red, green, blue, red...)" ;
      echo "\t-p\t\tsets output pixel spacing" ;
      echo "\t-i\t\tmosaic image ids only, do not create an image mosaic" ;
      echo "\t-f\t\tset background of images to <fill>" ;
      echo "\tprojfile\t\tprojection file (from projprm)" ;
      echo "\tprojkey \t\tprojection key" ;
      echo "\toutput_file\tLAS output mosaic image file name" ;
      echo "\tinput_files\tSAR CEOS images" ;
      echo ;
      echo "Auto_sar_mosaic assembles a set of SAR CEOS input files into";
      echo "a large mosaic, in LAS format.";
      echo ;
      echo "Version 4.1, ASF SAR TOOLS\n" ;
      exit 1 ;
} 

# obtain command line args
if [ $# -eq 0 ]
then
        echo "No parameters passed."
	print_usage
fi

keepGoing=1
while [ $keepGoing -eq 1 ]
do
  case $1 in
    -c)
      calibrate_flag=0
      shift;;
    -p)
      pixFlag="-p "$2
      shift
      shift;;
    -i)
      id_flag=1
	mosaic_flag=0
      shift
      ;; 
    -f)
      fillGeocode="-background $2"
      fillConcat="-m $2"
      shift
      shift;;
    -h)
      colorConcat="-c" 
      shift;;
     *)
      keepGoing=0
      ;;
  esac
done

# make sure they did everything right
# if date-time string & out_mosaic are not passed, exit.
if [ $# -lt 4 ]
then
  echo "Not enough required parameters passed."
  echo "I see only "$#" non-flag parameters."
  print_usage
fi

projfile=$1
projkey=$2
out_mosaic=$3

shift
shift
shift

infile_list=""

while [ ! $# -eq 0 ]
do
# Extract out image base name; add name to list
	baseName=`echo $1 | awk -F. '{print $1}'`
	infile_list=$infile_list" "$baseName
	shift
done

file_list=""
for infile in $infile_list
do
    outfile=$infile"_geo"
    file_list="$file_list $outfile"
    echo "Working on ###### $infile - $outfile ######"
    echo "---------------------------------------------------------"
    echo "Ingesting $infile ..."
    if [ $calibrate_flag -eq 1 ]
    then
    	calibrate $infile las_tmp
    else
    	sarin $infile las_tmp
    fi
    check_error
    echo "Doing "geocode $fillGeocode $pixFlag $resampFlag las_tmp las_tmp $projfile $projkey $outfile
    geocode $fillGeocode $pixFlag $resampFlag las_tmp las_tmp $projfile $projkey $outfile
    check_error
    rm las_tmp.*
done

# if creating image id's, copy each geocoded image file into 
# a id number file. In this case, the id number will equal its
# position in command line order
if [  $id_flag -eq 1 ]
then
  echo "\nCreating id images"
  idtable $TableFile $file_list
  check_error
  
  # copy over the ddr; adding image name to list
  idimages=""
  for i in $file_list
  do
    cp $i.ddr id$i.ddr
    idimages="$idimages id$i"
  done

  # concat idimages together
  echo "\nCombining id images into one single image\n" 
  concat $fillConcat idt$out_mosaic $idimages
  check_error

  # remove unwanted individual strip images
  for i in $idimages
  do
    rm $i.img $i.ddr
  done
fi

# if creating an image mosaic, follow the normal process
if [  $mosaic_flag -eq 1 ]
then
  # concat images together. place in out_file.
  echo "Combining multiple images into single image with concat"
  echo " "
  concat $colorConcat $out_mosaic $file_list 
  check_error
fi

# Delete temporary LAS files
for i in $file_list
do
   rm $i.*
done

# Cleanup & exit
exit 0

