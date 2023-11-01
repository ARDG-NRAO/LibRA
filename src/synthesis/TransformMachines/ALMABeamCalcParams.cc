#include <synthesis/TransformMachines/BeamCalc.h>

using namespace casacore;
namespace casa{

  Double ALMABandMinFreqDefaults[ALMABeamCalc_NumBandCodes] =
    {
      31.3E9, 67E9, 84E9, 125E9, 163E9, 211E9, 275E9, 385E9, 602E9, 787E9
    };
  Double ALMABandMaxFreqDefaults[ALMABeamCalc_NumBandCodes] =
    {
      45E9, 90E9, 116E9, 163E9, 211E9, 275E9, 373E9, 500E9, 720E9, 950E9
    };

  BeamCalcGeometry ALMABeamCalcGeometryDefaults[ALMABeamCalc_NumBandCodes] = 
    {
      /* Format of each entry:
	 
      struct BeamCalcGeometry	all dimensions in meters, GHz 
      {
      char name[16];		name of antenna, e.g., VLA 
      Double sub_h;		subreflector vertex height above primary vertex 
      Double feedpos[3];		position of feed 
      Double subangle;		angle subtended by the subreflector 
      Double legwidth;		strut width 
      Double legfoot;		distance from optic axis of leg foot 
      Double legapex;		hight of leg intersection 
      Double Rhole;		radius of central hole 
      Double Rant;		antenna radius 
      Double reffreq;		a reference frequency 
      Double taperpoly[5];	polynomial expanded about reffreq 
      Int ntaperpoly;		number of terms in polynomial 
      Double astigm_0;          astigmatism: coefficient of Zernike Polyn. Z6 a.k.a. 0-90
      Double astigm_45;         astigmatism: coefficient of Zernike Polyn. Z5 a.k.a. 45-135
      */

      /* BeamCalc_ALMA_1 */
      {
	"ALMA",
	"ALMA_1",
	6.177, // distance surface subreflector to surface of primary reflector
	{0.0,0.0,-1.377}, // feed position (m), assume centered
	7.16, // angular diameter (deg) of subreflector from 6.177 m distance
	0.09, // leg width (m) 
	5.00, // dist. opt. axis to leg foot (m) APPROXIMATE! T.B.C. 
	5.14, // height of leg intersection (m) APPROXIMATE! T.B.C. 
	0.375, // radius of central hole (m)
	6.0, // antenna radius
	31.3, // ref. freq (GHz), lower band edge
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1, // T.B.C.
	0.,0.
      },
      /* BeamCalc_ALMA_2 */
      {
	"ALMA",
	"ALMA_2",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	67.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1 ,
	0.,0.
      },
      /* BeamCalc_ALMA_3 */
      {
	"ALMA",
	"ALMA_3",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	84.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1, 
	0.,0.
      },
      /* BeamCalc_ALMA_4 */
      {
	"ALMA",
	"ALMA_4",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	125.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0. 
      },
      /* BeamCalc_ALMA_5 */
      {
	"ALMA",
	"ALMA_5",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	163.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_ALMA_6 */
      {
	"ALMA",
	"ALMA_6",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	211.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0. 
      },
      /* BeamCalc_ALMA_7 */
      {
	"ALMA",
	"ALMA_7",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	275.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_ALMA_8 */
      {
	"ALMA",
	"ALMA_8",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	385.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_ALMA_9 */
      {
	"ALMA",
	"ALMA_9",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	602.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_ALMA_10 */
      {
	"ALMA",
	"ALMA_10",
	6.177, 
	{0.0,0.0,-1.377}, 
	7.16, 
	0.09, 5.00, 5.14, 
	0.375,
	6.0, 
	787.0,
	{4.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0. 
      }
    };
};
