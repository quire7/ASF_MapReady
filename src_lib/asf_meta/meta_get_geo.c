/****************************************************************
FUNCTION NAME:  meta_get_*

DESCRIPTION:
   Extract relevant parameters from CEOS.
   These are the external entry points for
   the geolocation-related routines.

RETURN VALUE:
   
SPECIAL CONSIDERATIONS:

PROGRAM HISTORY:
  1.0 - O. Lawlor.  9/10/98.  CEOS Independance.
****************************************************************/
#include "asf.h"
#include "asf_meta.h"
#include "jpl_proj.h"
#include "ddr.h"

/*Geolocation Calls:*/

/* Converts a given line and sample to that for
the non-multilooked, non-windowed, equivalent image.*/
void meta_get_orig(void *fake_ddr,
	int y, int x,int *yOrig,int *xOrig)
{
	struct DDR *ddr=(struct DDR *)fake_ddr;
	*xOrig=(x)*ddr->sample_inc + (ddr->master_sample-1);
	*yOrig=(y)*ddr->line_inc + (ddr->master_line-1);
}

/* Converts given line and sample to geodetic
latitude and longitude. Works with all image types.
*/
void meta_get_latLon(meta_parameters *sar,
	double yLine, double xSample,double elev,double *lat,double *lon)
{
	if (sar->geo->type=='S'||sar->geo->type=='G')
	{ /*Slant or ground range.  Use state vectors and doppler.*/
		double slant,doppler,time;
		meta_get_timeSlantDop(sar,yLine,xSample,
			&time,&slant,&doppler);
		meta_timeSlantDop2latLon(sar,
			time,slant,doppler,elev,
			lat,lon);
	} else if (sar->geo->type=='P')
	{	/*Map-Projected. Use projection information to calculate lat & lon.*/
		double px,py;
		px=sar->geo->proj->startX+sar->geo->proj->perX*xSample;
		py=sar->geo->proj->startY+sar->geo->proj->perY*yLine;
		proj_to_ll(sar->geo,px,py,lat,lon);
	} else
	{	/*Bogus image type.*/
		printf("Error! Invalid image type '%c' passed to meta_get_latLon!\n",
			sar->geo->type);
		exit(1);
	}
}

/*Converts the given time, slant range, doppler, and elevation
off earth's surface into a latitude and longitude.*/
void meta_timeSlantDop2latLon(meta_parameters *sar,
	double time, double slant,double dop,double elev,
	double *lat,double *lon)
{
	double ignored;
	stateVector stVec;
	stVec=meta_get_stVec(sar,time);
	fixed2gei(&stVec,0.0);/*Subtract Earth's spin.*/
		
	getLatLongMeta(stVec,sar,slant,dop,elev,
		lat,lon,&ignored);
}

/* Converts a given line and sample in image into time,
slant-range, and doppler.  Works with all image types.
*/  
void meta_get_timeSlantDop(meta_parameters *sar,
	double yLine,double xSample,double *time,double *slant,double *dop)
{
	if (sar->geo->type=='S'||sar->geo->type=='G')
	{ /*Slant or ground range.  These are easy.*/
		*slant=meta_get_slant(sar,yLine,xSample);
		*time=meta_get_time(sar,yLine,xSample);
		if (dop != NULL)
		{
			if (sar->geo->deskew==1)
				*dop=0.0;
			else
				*dop=meta_get_dop(sar,yLine,xSample);
		}
	} else  if (sar->geo->type=='P') /*Map-projected image.  These are more difficult.*/
	{
		double lat,lon;
		meta_get_latLon(sar,yLine,xSample,0.0,&lat,&lon);
		latLon2timeSlant(sar,lat,lon,time,slant,dop);
	} else
	{	/*Bogus image type.*/
		printf("Error! Invalid image type '%c' passed to meta_get_latLon!\n",
			sar->geo->type);
		exit(1);
	}
}
