/****************************************************************
FUNCTION NAME:

SYNTAX:

PARAMETERS:
    NAME:	TYPE:		PURPOSE:
    --------------------------------------------------------

DESCRIPTION:

RETURN VALUE:

SPECIAL CONSIDERATIONS:

PROGRAM HISTORY:

****************************************************************/
#include "asf.h"

char asap_input[61][80] = 
{"20                            L",
"20                            M",
"0                             IRES",
"0                             ISUN",
"0                             IMOON",
"1                             IEPHEM",
"0                             IDRAG",
"1                             IDENS",
"0                             ISRP",
"0                             IORB",
"0                             IPRINT",
"0                             INODE",
"0                             IPLOT",
"   7144.79531873405990D0      ORB(1), A",
"      0.00054686326616D0         (2), E",
"     98.52701943570180D0         (3), I",
"    310.17540974788739D0         (4), NODE",
"    323.46202236587749D0         (5), W",
"    138.51252152135388D0         (6), M",
"1.D-12                        RELERR",
"1.D-12                        ABSERR",
"060.D0                        STEP",
"19950401.D0                   TINT(1)",
"153300.0000D0                     (2)",
"19950401.D0                   TFIN(1)",
"153700.0000D0                     (2)",
"19950401.D0                   TREF(1)",
"153300.0000D0                     (2)",
"3.9860045D5                   GE",
"6378.140D0                    RE",
".4178074216D-2                RATE",
"99.652865509D0                PM",
".8182D-1                      ELLIP",
"6468.14D0                     RATM",
"0.D0                          RDENS",
"0.D0                          RHT",
"0.D0                          SHT",
"1.D0                          ALTMAX",
"1.D0                          WT",
"10.D-6                        AREAD",
"10.D-6                        AREAS",
"2000.D0                       SCMASS",
"2.D0                          CDRAG",
"6.6D-3                        CSRP",
".13271244D12                  GS",
"0.D0                          ES(1)",
"0.D0                          ES(2)",
"0.D0                          ES(3)",
"0.D0                          ES(4)",
"0.D0                          ES(5)",
"0.D0                          ES(6)",
"0.D0                          ES(7)",
".490279D4                     GM",
"0.D0                          EM(1)",
"0.D0                          EM(2)",
"0.D0                          EM(3)",
"0.D0                          EM(4)",
"0.D0                          EM(5)",
"0.D0                          EM(6)",
"0.D0                          EM(7)",
"    2    0                 -.10826271D-2                          0.D0"};

make_5_file()
 {
   FILE *fp;
   int  i;
   
   fp = fopen("5.input","wb");
   for (i=0; i< 61; i++)
     {
       fprintf(fp,"%s\n",asap_input[i]);
     }
   fclose(fp);
 }

