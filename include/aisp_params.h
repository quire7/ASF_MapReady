/****************************************************************************
*								            *
*   aisp_params.h   Parameters needed to run AISP                           *
*   Copyright (C) 1997  Alaska SAR Facility		   	    	    *
*									    *
*	Alaska SAR Facility			APD Lab Web Site:	    *	
*	Geophysical Institute			www.asf.alaska.edu/apd	    *
*       University of Alaska Fairbanks					    *
*	P.O. Box 757320				APD Email:		    *
*	Fairbanks, AK 99775-7320		apd@asf.alaska.edu	    *
*									    *
****************************************************************************/

/*-------------------------------------------------------------------------*/
/*   This structure contains all of the parameters needed to run asp.c     */
/*-------------------------------------------------------------------------*/
struct AISP_PARAMS {
  char	in1[256];	     /* First input file basename		   */
  char	out[256];	     /* Output File basename                       */
  char	CALPRMS[255];        /* Calibration Parameters file (if desired)   */
  char	srm[4];		     /* Secondary range migration correction (y/n) */
  char	iqflip[4];	     /* Flip i/q or offset video (y/n flip;o off)  */ 
  int	iflag;		     /* Debugging Flag 	   			   */
  int	npatches;	     /* Number of range input patches 		   */
  int	na_valid;	     /* Number of valid points in the Azimuth	   */
  int	deskew; 	     /* Deskew flag				   */
  int	isave;		     /* Start range bin				   */
  int	nla;		     /* Number of range bins to process		   */
  int	hamFlag;	     /* Use a Hamming window for az ref func.      */
  int	kaiFlag;	     /* Use a Kaiser window for az ref func.       */
  int	pwrFlag;	     /* Create a power image			   */
  int	sigmaFlag;	     /* Create a sigma naught image		   */
  int	gammaFlag;	     /* Create a gamma naught image		   */
  int	betaFlag;	     /* Create a beta naught image		   */
  int	ifirstline;	     /* First line to read (from 0)  		   */
  int	ifirst;		     /* First sample pair to use (start 0)	   */
  int	nlooks;		     /* Number of looks in the azimuth		   */
  int	nbytes;		     /* Number of input bytes per line (w/ header) */
  int	ngood;		     /* Number of input bytes per line (no header) */
  float	fd;		     /* Doppler centroid quadratic coefs (Hz/prf)  */
  float	fdd;		     /* Doppler centroid quadratic coefs (Hz/prf)  */
  float	fddd;	             /* Doppler centroid quadratic coefs (Hz/prf)  */
  float	re;		     /* Earth Radius (m)			   */
  float	vel;		     /* Body fixed S/C velocity (m/s)		   */
  float	ht;		     /* Spacecraft height (m)			   */
  float	r00;		     /* Range of first sample in raw data file (m) */
  float	prf;		     /* Pulse Repitition Frequency (pps)	   */
  float	azres;		     /* Desired azimuth resolution (m)		   */
  float	fs;		     /* Range sampling rate (Hz)	  	   */
  float	slope;		     /* Chirp Slope (Hz/s)			   */
  float	pulsedur;	     /* Pulse Duration (s)			   */
  float	nextend;	     /* Chirp Extension Points			   */
  float	wavl;		     /* Radar Wavelength			   */
  float	rhww;		     /* Range spectrum wt. (1.0=none;0.54=hamming) */
  float	pctbw;		     /* Fraction of range bandwidth to remove  	   */
  float	pctbwaz;	     /* Fraction of azimuth bandwidth to remove	   */
  float	sloper, interr,	     /* 1st Patch Slope and Inter. for range       */
	slopea, intera;      /* 1st Patch Slope and Inter. for azimuth     */
  float	dsloper, dinterr,    /* Delta slope,inter per patch in range       */
	dslopea, dintera;    /* Delta slope,inter per patch in azimith     */
  float	caltone;	     /* Caltone % of sample rate		   */
  float	timeOff,slantOff; /*Characteristic Timing Offset (manually entered.*/
  float	xmi;		     /* bias value for i values			   */
  float	xmq;		     /* bias value for q values			   */
};
void print_params(const char *in,struct AISP_PARAMS *a,const char *sourceProgram);
void read_params(const char *in,struct AISP_PARAMS *);

