#include <synthesis/TransformMachines/BeamCalc.h>

using namespace casacore;
namespace casa{

  Double VLABandMinFreqDefaults[VLABeamCalc_NumBandCodes] = // (Hz)
    {
      1.34E9, 4.5E9, 8.0E9, 14.4E9, 22.0E9, 40.0E9, 0.1E9  
    };
  Double VLABandMaxFreqDefaults[VLABeamCalc_NumBandCodes] = 
    {
      1.73E9, 5.0E9, 8.8E9, 15.4E9, 24.0E9, 50.0E9, 0.3E9      
    };
  Double EVLABandMinFreqDefaults[EVLABeamCalc_NumBandCodes] = 
    {
      0.9E9, 2.0E9, 4.0E9, 8.0E9, 12.0E9, 18.0E9, 26.5E9, 40.0E9 
    };
  Double EVLABandMaxFreqDefaults[EVLABeamCalc_NumBandCodes] = 
    {
      2.0E9, 4.0E9, 8.0E9, 12.0E9, 18.0E9, 26.5E9, 40.0E9, 50.0E9
    };

  BeamCalcGeometry VLABeamCalcGeometryDefaults[VLABeamCalc_NumBandCodes] = 
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
      
      
      /* BeamCalc_VLA_L */
      {
	"VLA",
	"VLA_L",
	8.47852,
	{0.68968367, 0.68968367, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	1.5,
	{10.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_C */
      {
	"VLA",
	"VLA_C",
	8.47852,
	{-0.412205, -0.883976, 1.67640}, 
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	5.0,
	{12.8, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_X */
      {
	"VLA",
	"VLA_X",
	8.47852,
	{-0.6269493, 0.7471691, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	8.0,
	{11.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_U */
      {
	"VLA",
	"VLA_U",
	8.47852,
	{0.4122049, -0.883976, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	15.0,
	{11.5, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_K */
      {
	"VLA",
	"VLA_K",
	8.47852,
	{-0.937576, 0.268846, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	22.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_Q */
      {
	"VLA",
	"VLA_Q",
	8.47852,
	{-0.9723533, -0.076526, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	45.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_VLA_4 */ // Just a place holder - values are not correct
      {
	"VLA",
	"VLA_4",
	8.47852,
	{-0.9723533, -0.076526, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	45.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      }
      
    };

  BeamCalcGeometry EVLABeamCalcGeometryDefaults[EVLABeamCalc_NumBandCodes] = 
    {
      
      /* BeamCalc_EVLA_L */
      {
	"VLA", // use the same surface file as for VLA
	"EVLA_L",
	8.47852,
	{-0.10026, 0.97019, 1.67640}, 
	9.26,
	0.27, 7.55, 10.93876,
	2.0,//3.25,//@1.5GHz, Default:2.0,
	12.5,
	1.5,
	{10.0, 2.0, 0.0, 0.0, 0.0}, 2,//{13.44, 0.00062, 0.00039, 0.0, 0.0}, 3,//	{10.0, 2.0, 0.0, 0.0, 0.0}, 2,
	0.,0.
      },
      /* BeamCalc_EVLA_S */
      {
	"VLA",
	"EVLA_S",
	8.47852,
	{0.196123, -0.955439, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	3.87,//@2GHz, Default: 2.0,
	12.5,
	3.0,
	{15.837, -0.004, 0.00023, 0.0, 0.0}, 3,//@2GHz; Default: {12.75, 0.75, 0.0, 0.0, 0.0}, 2,
	0.,0.
      },
      /* BeamCalc_EVLA_C */
      {
	"VLA",
	"EVLA_C",
	8.47852,
	{0.94300, -0.249152, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	6.0,
	{12.75, 0.375, 0.0, 0.0, 0.0}, 2,
	0.,0.
      },
      /* BeamCalc_EVLA_X */
      {
	"VLA",
	"EVLA_X",
	8.47852,
	{0.893100, 0.392044, 1.67640}, 
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	10.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_EVLA_U */
      {
	"VLA",
	"EVLA_U",
	8.47852,
	{-0.657688, -0.720260, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	15.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_EVLA_K */
      {
	"VLA",
	"EVLA_K",
	8.47852,
	{-0.877393, -0.426039, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	22.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_EVLA_A */
      {
	"VLA",
	"EVLA_A",
	8.47852,
	{-0.933238, 0.283539, 1.67640}, 
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	33.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      },
      /* BeamCalc_EVLA_Q */
      {
	"VLA",
	"EVLA_Q",
	8.47852,
	{-0.9723533, -0.0765259, 1.67640},
	9.26,
	0.27, 7.55, 10.93876,
	2.0,
	12.5,
	45.0,
	{13.0, 0.0, 0.0, 0.0, 0.0}, 1,
	0.,0.
      }
    };

};

