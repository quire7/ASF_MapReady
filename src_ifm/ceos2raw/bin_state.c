/****************************************************************
FUNCTION NAME:
Interface to the "bin_state" structure.
This structure holds the state information used
during the decoding/analysis of an LZP binary file.

Also contains metadata interfaces for same.

SYNTAX:

PARAMETERS:
    NAME:       TYPE:           PURPOSE:
    --------------------------------------------------------

DESCRIPTION:

RETURN VALUE:

SPECIAL CONSIDERATIONS:

PROGRAM HISTORY:

	????/??		Orion Lawlor - Original Creation
	10/2000		R. Gens      - Modified updateAGC_window
				       to allow for bit errors

****************************************************************/

#include "decoder.h"
#include "missing.h"

/*********************************
new_bin_state:
	Allocate, set to defaults, and return
a new bin_state structure.
*/
bin_state *new_bin_state(void)
{
	int i;
	bin_state *s=(bin_state *)MALLOC(sizeof(bin_state));
	strcpy(s->satName,"Unknown");
	strcpy(s->beamMode,"STD");
	s->zeroDopSteered=0;
	s->nPulseInAir=0;
	s->nSamp=0;
	s->nBeams=1;/*Normally, you have just one beam*/
	s->slope=0; /*chirp slope, Hz/sec.*/
	s->frequency=0; /*radar wavelength, in m.*/
	s->fs=0;/*Range sampling frequency, Hz*/
	s->prf=0;/*Pulse Repetition Freqency*/
	s->prf_code=-1;
	s->dwp=0;
	s->dwp_code=-1;
	s->range_gate=0;
	s->time_code=0;
	
	s->binary=NULL;
	s->curFrame=0;
	s->bytesPerFrame=0;
	s->bytesInFile=0;
	s->missing=NULL;
	
	s->nValid=0;
	s->estDop=0.0;
	s->I_BIAS=s->Q_BIAS=0.0;
	
	s->re=6363000.989; /*approximate earth radius at scene center.*/
	s->vel=7463.989; /*satellite velocity, m/s.*/
	s->ht=792000.989; /*satellite height above earth, m.*/
	s->pulsedur=0; /*chirp length, in sec.*/
	s->lookDir='R';
	s->azres=8.0;    /* Desired azimuth resolution (m)*/
	s->nLooks=5;     /* Number of looks to square up data */
	
	s->dotFMT=NULL;
	for (i=0;i<MAX_BEAMS;i++)
		s->firstFrame[i]=NULL;
	
	return s;
}
/************************
Delete_bin_state: destroys the given bin_state structure.
*/
void delete_bin_state(bin_state *s)
{
	int i;
	strcpy(s->satName,"Deleted...");
	if (s->binary)
		FCLOSE(s->binary);
	if (s->dotFMT)
		FCLOSE(s->dotFMT);
	for (i=0;i<MAX_BEAMS;i++)
		if (s->firstFrame[i]!=NULL)
			FREE(s->firstFrame[i]);
	if (s->missing!=NULL)
		freeMissing(s);
	FREE(s);
}

/************************
Writes the satellite * fields into the given meta_parameters
*/
void updateMeta(bin_state *s,meta_parameters *meta)
{
/*Update fields for which we have decoded header info.*/
	meta->ifm->er=s->re;
	meta->ifm->ht=s->re+s->ht;
	meta->ifm->nLooks=s->nLooks;
	meta->ifm->orig_nLines=30000;/*Guess 30K lines- doesn't really matter.*/
	meta->ifm->orig_nSamples=s->nSamp;
	
	meta->geo->type='S';/*Slant range product*/
	meta->geo->deskew=0;/*Not doppler deskewed*/
	meta->geo->lookDir=s->lookDir;
	meta->geo->rngPixTime=1.0/s->fs;
	meta->geo->azPixTime=1.0/s->prf;
	meta->geo->xPix=meta->geo->rngPixTime*(speedOfLight/2.0);
	meta->geo->yPix=meta->geo->azPixTime*s->vel* /*Orbital velocity*/
		(meta->ifm->er/meta->ifm->ht);/*Swath velocity*/
	meta->geo->timeShift=meta->geo->slantShift=0.0;
	meta->geo->slantFirst=s->range_gate*speedOfLight/2.0;
	meta->geo->wavelen=speedOfLight/s->frequency;

	if (meta->info==NULL)
	{
		meta->info=(extra_info *)MALLOC(sizeof(extra_info));
		meta->info->orbit=-1;
		meta->info->bitErrorRate=-1;
		sprintf(meta->info->satBinTime,"%f",s->time_code);
		sprintf(meta->info->satClkTime,"0");
	}
	strcpy(meta->info->sensor,s->satName);
	strcpy(meta->info->mode,s->beamMode);
	strcpy(meta->info->processor,"ASF Lz/ceos2raw");
	meta->info->prf = s->prf;
}

/********************************
AddStateVector:
	Updates the Earth Radius, spacecraft hieght,
velocity, etc. using the given state vector.
Format:
	stVec[0-2]: Earth-Fixed position, in meters.
	stVec[3-5]: Earth-Fixed velocity, in meters/second.
*/

void addStateVector(bin_state *s,stateVector *stVec)
{
	double latCen;/*Geocentric latitude of state vector, in radians.*/
	double er;/*Radius of earth under state vector, in m.*/
	
        /* Use state vector to estimate latitude.
	 ---------------------------------------*/
	latCen=atan(stVec->pos.z/
		sqrt(stVec->pos.x*stVec->pos.x+stVec->pos.y*stVec->pos.y));

        /* Use the latitude to determine earth's (ellipsoidal) radius.
	 -----------------------------------------------------------*/
	er=er_polar/sqrt(1-ecc2/(1+tan(latCen)*tan(latCen)));

        /* Now write all these parameters into satellite structure.
         --------------------------------------------------------*/
	s->re=er;
	s->ht=vecMagnitude(stVec->pos)-er;
	s->vel=vecMagnitude(stVec->vel);
	
	printf("Updating for more accurate earth radius (%.2f), \n"
		"height (%.2f), and velocity (%.2f).\n",
		s->re,s->ht,s->vel);
	
	/* Estimate the doppler value at beam center
         ------------------------------------------*/
        if (!s->zeroDopSteered)
	{
	  GEOLOCATE_REC *g=init_geolocate(stVec);
	  g->side=s->lookDir;
	  g->lambda=speedOfLight/s->frequency;
	  s->estDop=yaw2doppler(g,s->range_gate*speedOfLight/2.0,1.10)/s->prf;
	  printf("Estimated doppler: %f PRF\n",s->estDop);
	  free_geolocate(g);
	}
}

/********************************
updateAGC_window:
	Writes the given AGC value (floating-point amplitude
amplification that should be applied to this image) and 
window position (starting offset of this row in pixels) to
the .fmt file. Called by decoding routines.
*/
void updateAGC_window(bin_state *s,float amplify,float startOff)
{
	static float sto_amp=-1000.0,sto_off=-1000.0,diff=5.0;
	diff=abs((10*log(sto_amp*sto_amp)/log(10))-(10*log(amplify*amplify)/log(10)));
	if (s->dotFMT==NULL) return;/*Skip it if no .fmt file exists*/
	if (((sto_amp!=amplify)&&(diff<3))||(sto_off!=startOff))
	{/*Amplification factor or window position has changed! Update .fmt*/
		sto_amp=amplify;
		sto_off=startOff;
		fprintf(s->dotFMT,"%-8d %12.3f %10.5g\n",s->nLines,startOff,amplify);
		fflush(s->dotFMT);/*Flush the format file*/
	}
}



