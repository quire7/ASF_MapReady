/****************************************************************
NAME:  image add --  add images

SYNOPSIS:	rtc_add sarfile rtcfile outfile

DESCRIPTION:
  Adds the terrain corrected byte image named sarfile with the
  float valued image named rtcfile to produce the output file
  named outfile.

EXTERNAL ASSOCIATES:  None

FILE REFERENCES:
    NAME:                USAGE:
    ---------------------------------------------------------------
    rtcf_file		 Radiometric Terrain Correction Factors
			 (DEM image size file of floats)
    sar_file		 Input Terrain Corrected SAR image
    out_file	  	 Radiometrically Corrected SAR image

PROGRAM HISTORY:
    VERS:   DATE:   AUTHOR:       PURPOSE:
    ---------------------------------------------------------------
    1.0	    5/95    T. Logan	  Initial Creation
    1.1     8/95    T. Logan      Modified to accept pixel size
    2.0	    8/95    T. Logan	  Modified to use DEM space images
    3.0	    10/95   T. Logan      Ported to Solaris
    3.1    6/98     O. Lawlor     ASF/STEP      Removed non-ANSI Timer.

HARDWARE/SOFTWARE LIMITATIONS: 

ALGORITHM DESCRIPTION:

ALGORITHM REFERENCES:

BUGS:

****************************************************************/
/***************** Copyright Notice ***********************
                        English:
         You can freely use, modify, and re-distribute 
this code and/or binaries as long as you don't sell them,
and make sure to carry this notice along with them.
However, if you want to sell them, you can contact the 
University of Alaska Technology Corporation, below.


                        Legalese:
                 COPYRIGHT NOTIFICATION

(C) COPYRIGHT 1997 UNIVERSITY OF ALASKA. ALL RIGHTS RESERVED

This software discloses material protectable under copyright 
laws of the United States. Permission is hereby granted to 
use, reproduce, and prepare derivative works for noncommercial 
purposes at no charge, provided that this original copyright 
notice, and disclaimer are retained and any changes are 
clearly documented. Any entity desiring permission to 
incorporate this software or a work based on the software 
into a product for sale must contact the University of 
Alaska Technology Corporation.


This software was authored by:

RICK GURITZ      rguritz@images.alaska    (907)474-7886
Alaska SAR Facility, Geophysical Institute
P.O. Box 757320, University of Alaska Fairbanks
Fairbanks, Alaska 99775�7320
FAX: (907)474-5195

Any questions or comments on the software may be directed 
to one of the authors: Rick Guritz, Tom Logan, Mike Shindle,
Rob Fatland, Orion Lawlor, and Dorothy Corbett; or to
http://www.images.alaska.edu


NEITHER THE UNIVERSITY OF ALASKA NOR ANY SUBUNIT THEREOF, 
NOR ANY OF THEIR EMPLOYEES MAKES ANY WARRANTY, EXPRESS 
OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR 
RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR 
USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT, OR 
PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD 
NOT INFRINGE PRIVATELY OWNED RIGHTS.
LICENSING INQUIRES MAY BE DIRECTED TO THE UNIVERSITY 
OF ALASKA TECHNOLOGY DEVELOPMENT CORPORATION AT (907)451-0718.
************************************************************/
#include "asf.h"
#include "ddr.h"

#define VERSION 3.1

int main(argc,argv)
   int argc;
   char** argv;
{
char	rtcf_file[255], sar_file[255], out_file[255], cmd[255];
FILE	*fp_rtcf, *fp_sar, *fp_out;
struct  DDR ddr;
float   *rtcf_buf;
int	i, j, ns, nl;
float   min, max, d_ave, l_ave;
int	dark, light, neg;
unsigned char	*inbuf, *outbuf;
float	values[10];
int	count[10];
float   val;

StartWatch();

if (argc!=4)
  {
   printf("Usage: %s sarfile rtcfile outfile\n",argv[0]);
   printf("   inputs: sarfile.img  (.img extension assumed)\n");
   printf("           rtcfile.img  (.raw extension assumed)\n");
   printf("   output: outfile.img, Byte image in LAS 6.0 format\n");
   printf("\n   Version %.2f,  ASF STEP TOOLS\n",VERSION);
   exit(1);
  }

strcpy(sar_file,argv[1]);
strcat(strcpy(rtcf_file,argv[2]),".img");
strcat(strcpy(out_file,argv[3]),".img");

c_getddr(sar_file,&ddr);
strcat(sar_file,".img");
ns = ddr.ns;
nl = ddr.nl;
min = max = 1.0;
dark = light = neg = 0;
d_ave = l_ave = 0.0;
for (i = 0; i<10; i++) { values[i] = (float) i*0.2; count[i] = 0; }

printf("Applying radiometric terrain correction factors.\n");
printf("Factors: %s  Sar: %s  Output: %s\n",rtcf_file,sar_file,out_file);

inbuf = (unsigned char *) MALLOC (ns * sizeof(unsigned char));
outbuf = (unsigned char *) MALLOC (ns * sizeof(unsigned char));
rtcf_buf = (float *) MALLOC (ns * sizeof(float));

fp_rtcf = fopenImage(rtcf_file,"rb");
fp_sar = fopenImage(sar_file,"rb");
fp_out = fopenImage(out_file,"wb");

for (i = 0; i < nl; i++)
 {
   FREAD(rtcf_buf,ns*sizeof(float),1,fp_rtcf);
   FREAD(inbuf,ns,1,fp_sar);
   for (j = 0; j < ns; j++)
      if (inbuf[j]>0)
       {
        if (rtcf_buf[j]>(char)0)
          rtcf_buf[j] = log10(rtcf_buf[j])*10.0 + ((float)inbuf[j]/10.0 - 25.5);
        else rtcf_buf[j] = 0;
        val = 10.0*rtcf_buf[j]+255.0;
        if (val > 255) outbuf[j] = 255;
        else if (val < 0) outbuf[j] = 0;
        else outbuf[j] = val+0.5;
       }
      else outbuf[j] = 0;
   fwrite(outbuf,ns,1,fp_out);
 }

strcat(strcpy(sar_file,argv[1]),".ddr");
strcat(strcpy(out_file,argv[3]),".ddr");
sprintf(cmd,"cp %s %s\n",sar_file,out_file);
if (system(cmd) != 0)
  { printf("Failure copying metadata: Program Exited\n\n"); exit(1); }

fclose(fp_rtcf);
fclose(fp_sar);
fclose(fp_out);

StopWatch();

printf("\nRTC_ADD Successfully Completed \n");

exit(0);
}
