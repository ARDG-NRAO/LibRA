/*
 * NROData.h
 *
 *  Created on: May 16, 2016
 *      Author: wataru kawasaki
 */

#ifndef SINGLEDISH_FILLER_NRODATA_H_
#define SINGLEDISH_FILLER_NRODATA_H_

#define NRO_ARYMAX 35
#define SCNLEN_HEADER_SIZE 424

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct NRODataObsHeader {
  string LOFIL0;
  string VER0;
  string GROUP0;
  string PROJ0;
  string SCHED0;
  string OBSVR0;
  string LOSTM0;
  string LOETM0;
  int ARYNM0;
  int NSCAN0;
  string TITLE0;
  string OBJ0;
  string EPOCH0;
  double RA00;
  double DEC00;
  double GL00;
  double GB00;
  int NCALB0;
  int SCNCD0;
  string SCMOD0;
  double VEL0;
  string VREF0;
  string VDEF0;
  string SWMOD0;
  double FRQSW0;
  double DBEAM0;
  double MLTOF0;
  double CMTQ0;
  double CMTE0;
  double CMTSOM0;
  double CMTNODE0;
  double CMTI0;
  string CMTTMO0;
  double SBDX0;
  double SBDY0;
  double SBDZ10;
  double SBDZ20;
  double DAZP0;
  double DELP0;
  int CBIND0;
  int NCH0;
  int CHRANGE0[2];
  double ALCTM0;
  double IPTIM0;
  double PA0;
  string RX0[NRO_ARYMAX];
  double HPBW0[NRO_ARYMAX];
  double EFFA0[NRO_ARYMAX];
  double EFFB0[NRO_ARYMAX];
  double EFFL0[NRO_ARYMAX];
  double EFSS0[NRO_ARYMAX];
  double GAIN0[NRO_ARYMAX];
  string HORN0[NRO_ARYMAX];
  string POLTP0[NRO_ARYMAX];
  double POLDR0[NRO_ARYMAX];
  double POLAN0[NRO_ARYMAX];
  double DFRQ0[NRO_ARYMAX];
  string SIDBD0[NRO_ARYMAX];
  int REFN0[NRO_ARYMAX];
  int IPINT0[NRO_ARYMAX];
  int MULTN0[NRO_ARYMAX];
  double MLTSCF0[NRO_ARYMAX];
  string LAGWIN0[NRO_ARYMAX];
  double BEBW0[NRO_ARYMAX];
  double BERES0[NRO_ARYMAX];
  double CHWID0[NRO_ARYMAX];
  int ARRY0[NRO_ARYMAX];
  int NFCAL0[NRO_ARYMAX];
  double F0CAL0[NRO_ARYMAX];
  double FQCAL0[NRO_ARYMAX][10];
  double CHCAL0[NRO_ARYMAX][10];
  double CWCAL0[NRO_ARYMAX][10];
  int SCNLEN0;
  int SBIND0;
  int IBIT0;
  string SITE0;
  string TRK_TYPE;
  string SCAN_COORD;
  int NBEAM;
  int NPOL;
  int NSPWIN;
  int CHMAX;
  string VERSION;
  int16_t ARRYTB[36];
  string POLNAME[12];
  string CDMY1;
};

struct NRODataScanData {
  string LSFIL0;
  int ISCN0;
  string LAVST0;
  string SCNTP0;
  double DSCX0;
  double DSCY0;
  double SCX0;
  double SCY0;
  double PAZ0;
  double PEL0;
  double RAZ0;
  double REL0;
  double XX0;
  double YY0;
  string ARRYT0;
  float TEMP0;
  float PATM0;
  float PH200;
  float VWIND0;
  float DWIND0;
  float TAU0;
  float TSYS0;
  float BATM0;
  int LINE0;
  int IDMY1[4];
  double VRAD0;
  double FRQ00;
  double FQTRK0;
  double FQIF10;
  double ALCV0;
  double OFFCD0[2][2];
  int IDMY0;
  int IDMY2;
  double DPFRQ0;
  string ARRYSCN;
  string CDMY1;
  double SFCTR0;
  double ADOFF0;
  string LDATA;
};
 
} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_NROOBSHEADER_H_ */
