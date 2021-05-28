//# VLAIlluminationConvFunc.h: Definition for VLAIlluminationConvFunc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_BEAMCALC_H
#define SYNTHESIS_BEAMCALC_H

//#include <casa/complex.h>
#include <images/Images/TempImage.h>
#include <casa/Exceptions.h>
#include <casa/Logging/LogIO.h>

namespace casa
{

#define MAXGEOM 2000

  typedef struct 	/* all dimensions in meters, GHz */
  {
    char name[16];	/* name of antenna, e.g., VLA */
    char bandName[16];
    casacore::Double sub_h;	/* subreflector vertex height above primary vertex */
    casacore::Double feedpos[3];	/* position of feed */
    casacore::Double subangle;	/* angle subtended by the subreflector */
    casacore::Double legwidth;	/* strut width */
    casacore::Double legfoot;		/* distance from optic axis of leg foot */
    casacore::Double legapex;		/* hight of leg intersection */
    casacore::Double Rhole;		/* radius of central hole */
    casacore::Double Rant;		/* antenna radius */
    casacore::Double reffreq;		/* a reference frequency */
    casacore::Double taperpoly[5];	/* polynomial expanded about reffreq */
    casacore::Int ntaperpoly;		/* number of terms in polynomial */
    
    casacore::Double astigm_0;     /* astigmatism: coefficient of Zernike Polyn. Z6 a.k.a. 0-90 */
    casacore::Double astigm_45;    /* astigmatism: coefficient of Zernike Polyn. Z5 a.k.a. 45-135 */

    /* to be added later
       casacore::Double focus;
       casacore::Double dfeedpos[3];
       casacore::Double dsub_z;
    */
  } BeamCalcGeometry;
  
  typedef struct 
  {
    casacore::Int oversamp;			/* average this many points per cell */
    casacore::TempImage<casacore::Complex> *aperture;	/* Jones planes [Nx,Ny,NStokes,NFreq]*/
    casacore::Double x0, y0;			/* center of cell 0, 0, meters */
    casacore::Double dx, dy;			/* increment in meters */
    casacore::Int nx, ny;			/* calculation plane size in cells */
    /* last cell is at coordinates:
       X = x0 + (nx-1)*dx
       Y = y0 + (ny-1)*dy
    */
    casacore::Double pa;			/* Parallactic angle, radians */
    casacore::Double freq;			/* GHz */
    casacore::Int band;
  } ApertureCalcParams;

  /*
   * calcAntenna parameters
   */
  typedef struct
  {
    casacore::Double sub_h;               /* height of subreflector (on axis) */
    casacore::Double feed[3];             /* position of the feed */
    casacore::Double feeddir[3];          /* unit vector pointing along feed */
    casacore::Double pfeeddir[3];         /* same as above after pathology applied */
    casacore::Double radius;              /* antenna radius (m) */
    casacore::Double K;
    casacore::Double deltar;
    casacore::Double zedge;               /* height at the edge of the dish */
    casacore::Double bestparabola;        /* best fit parabola quadratic coef */
    casacore::Double ftaper;              /* taper of feed */
    casacore::Double thmax;               /* maximum angle of feed */
    casacore::Double fa2pi;
    casacore::Double legwidth;
    casacore::Double legfoot, legfootz;
    casacore::Double legapex;
    casacore::Double legthick;
    casacore::Double hole_radius;
    casacore::Double freq, lambda;
    casacore::Double astigm_0;     /* astigmatism: coefficient of Zernike Polyn. Z6 a.k.a. 0-90 */
    casacore::Double astigm_45;    /* astigmatism: coefficient of Zernike Polyn. Z5 a.k.a. 45-135 */
    casacore::Double dir[3];
    casacore::Double hhat[3], vhat[3];   /* unit vectors orthogonal to dir */
    casacore::Double z[MAXGEOM];
    casacore::Double m[MAXGEOM];
    casacore::Double k[3];
    casacore::Int ngeom;
    char name[16];
    casacore::Int gridsize;
  } calcAntenna;
  
  typedef struct
  {
    casacore::Double subrot[3][3];        /* 3x3 matrix rotating x,y,z or nx,ny,nz */
    casacore::Double feedrot[3][3];       /* 3x3 matrix rotating x,y,z or nx,ny,nz */
    casacore::Double subshift[3];         /* 3 length vector */
    casacore::Double feedshift[3];        /* 3 length vector */
    casacore::Double subrotpoint[3];      /* 3 vector describing point to rotate sub. */
    casacore::Double az_offset;           /* azimuth pointing offset (radians) */
    casacore::Double el_offset;           /* elevation pointing offset (radians) */
    casacore::Double phase_offset;        /* DC offset in phase (radians) */
    casacore::Double focus;               /* meters out of focus toward subreflector */
  } Pathology;
  
  typedef struct
  {
    casacore::Double aper[6];             /* aperture x, y, z, nx, ny, nz */
    casacore::Double dish[6];             /* dish x, y, z, nx, ny, nz */
    casacore::Double sub[6];              /* subreflector x, y, z, nx, ny, nz */
    casacore::Double feed[3];             /* feed x, y, z */
  } Ray;

  enum VLABeamCalcBandCode{
    BeamCalc_VLA_L = 0,
    BeamCalc_VLA_C,
    BeamCalc_VLA_X,
    BeamCalc_VLA_U,
    BeamCalc_VLA_K,
    BeamCalc_VLA_Q,
    BeamCalc_VLA_4,
    
    VLABeamCalc_NumBandCodes	/* this line last */
  };

  enum EVLABeamCalcBandCode{
    BeamCalc_EVLA_L = 0,
    BeamCalc_EVLA_S,
    BeamCalc_EVLA_C,
    BeamCalc_EVLA_X,
    BeamCalc_EVLA_U,
    BeamCalc_EVLA_K,
    BeamCalc_EVLA_A,
    BeamCalc_EVLA_Q,
    BeamCalc_EVLA_4,
    
    EVLABeamCalc_NumBandCodes	/* this line last */
  };


  enum ALMABeamCalcBandCode{
    BeamCalc_ALMA_1 = 0,
    BeamCalc_ALMA_2,
    BeamCalc_ALMA_3,
    BeamCalc_ALMA_4,
    BeamCalc_ALMA_5,
    BeamCalc_ALMA_6,
    BeamCalc_ALMA_7,
    BeamCalc_ALMA_8,
    BeamCalc_ALMA_9,
    BeamCalc_ALMA_10,
    
    ALMABeamCalc_NumBandCodes	/* this line last */
  };

  extern casacore::Double VLABandMinFreqDefaults[VLABeamCalc_NumBandCodes];
  extern casacore::Double VLABandMaxFreqDefaults[VLABeamCalc_NumBandCodes];
  extern BeamCalcGeometry VLABeamCalcGeometryDefaults[VLABeamCalc_NumBandCodes];
  extern casacore::Double EVLABandMinFreqDefaults[EVLABeamCalc_NumBandCodes];
  extern casacore::Double EVLABandMaxFreqDefaults[EVLABeamCalc_NumBandCodes];
  extern BeamCalcGeometry EVLABeamCalcGeometryDefaults[EVLABeamCalc_NumBandCodes];
  extern casacore::Double ALMABandMinFreqDefaults[ALMABeamCalc_NumBandCodes];
  extern casacore::Double ALMABandMaxFreqDefaults[ALMABeamCalc_NumBandCodes];
  extern BeamCalcGeometry ALMABeamCalcGeometryDefaults[ALMABeamCalc_NumBandCodes];

  class BeamCalc
  {
  public:

    // This is a SINGLETON class
    static BeamCalc* Instance();

    void setBeamCalcGeometries(const casacore::String& obsName, // (the observatory name, e.g. "ALMA" or "ACA")
			       const casacore::String& antennaType = "STANDARD",
			       const casacore::MEpoch& obsTime = casacore::MEpoch(casacore::Quantity(50000., "d")),
			       const casacore::String& otherAntRayPath=""); // override the AntennaResponses casacore::Table in Observatories

    casacore::Int getBandID(const casacore::Double freq, // in Hz 
			    const casacore::String& obsname,
			    const casacore::String& bandName,
			    const casacore::String& antennaType="STANDARD",
			    const casacore::MEpoch& obsTime = casacore::MEpoch(casacore::Quantity(50000., "d")),
			    const casacore::String& otherAntRayPath=""); // override the AntennaResponses casacore::Table in Observatories 

    casacore::Int calculateAperture(ApertureCalcParams *ap);
    casacore::Int calculateAperture(ApertureCalcParams *ap, const casacore::Int& whichStokes);
    casacore::Int calculateApertureLinPol(ApertureCalcParams *ap, const casacore::Int& whichStokes);

  protected:
    BeamCalc();
    
  private:

  void computePixelValues(const ApertureCalcParams *ap, const calcAntenna *a, const Pathology *p,
			  const casacore::Double &L0, casacore::Complex *Er, casacore::Complex *El, 
			  const casacore::Int &i, const casacore::Int &j);
  void computePixelValues(const ApertureCalcParams *ap, 
			  const calcAntenna *a, const Pathology *p,
			  const casacore::Double &L0,
			  casacore::Complex *Er, casacore::Complex *El,
			  const casacore::Int &i, const casacore::Int &j,
			  const casacore::Int& whichStokes);
  void computePixelValuesLinPol(const ApertureCalcParams *ap, 
				const calcAntenna *a, const Pathology *p,
				const casacore::Double &L0,
				casacore::Complex *Ex, casacore::Complex *Ey,
				const casacore::Int &i, const casacore::Int &j,
				const casacore::Int& whichStokes);
    //normalizes a "vector" of 3 Doubles in the vector sense
    inline void norm3(casacore::Double *v)
    {
      casacore::Double s;
      s = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
      v[0] /= s;
      v[1] /= s;
      v[2] /= s;
    }

    calcAntenna *newAntenna(casacore::Double sub_h, casacore::Double feed_x, casacore::Double feed_y, casacore::Double feed_z,
			    casacore::Double ftaper, casacore::Double thmax, const char *geomfile);

    void deleteAntenna(calcAntenna *a);

    void Antennasetfreq(calcAntenna *a, casacore::Double freq);

    void Antennasetdir(calcAntenna *a, const casacore::Double *dir);

    // sets feeddir after pathology is considered 
    void alignfeed(calcAntenna *a, const Pathology *p);

    void getfeedbasis(const calcAntenna *a, casacore::Double B[3][3]);

    void Efield(const calcAntenna *a, const casacore::Complex *pol, casacore::Complex *E);

    casacore::Int Antennasetfeedpattern(calcAntenna *a, const char *filename, casacore::Double scale);

    calcAntenna *newAntennafromApertureCalcParams(ApertureCalcParams *ap);

    casacore::Int dishvalue(const calcAntenna *a, casacore::Double r, casacore::Double *z, casacore::Double *m);

    casacore::Int astigdishvalue(const calcAntenna *a, casacore::Double x, casacore::Double y, casacore::Double *z, casacore::Double *m); 

    // Returns position of subreflector piece (x, y, z) and
    // its normal (u, v, w)
    casacore::Int subfromdish(const calcAntenna *a, casacore::Double x, casacore::Double y, casacore::Double *subpoint);

    casacore::Int dishfromsub(const calcAntenna *a, casacore::Double x, casacore::Double y, casacore::Double *dishpoint);

    void printAntenna(const calcAntenna *a);

    Ray* newRay(const casacore::Double *sub);

    void deleteRay(Ray *ray);

    Pathology* newPathology();

    Pathology* newPathologyfromApertureCalcParams(ApertureCalcParams *ap);

    void deletePathology(Pathology *P);

    void normvec(const casacore::Double *a, const casacore::Double *b, casacore::Double *c);

    casacore::Double dAdOmega(const calcAntenna *a, const Ray *ray1, const Ray *ray2,
		    const Ray *ray3, const Pathology *p);

    casacore::Double dOmega(const calcAntenna *a, const Ray *ray1, const Ray *ray2,
		  const Ray *ray3, const Pathology *p);

    casacore::Double Raylen(const Ray *ray);

    void Pathologize(casacore::Double *sub, const Pathology *p);

    void applyPathology(Pathology *P, calcAntenna *a);

    void intersectdish(const calcAntenna *a, const casacore::Double *sub, const casacore::Double *unitdir,
		       casacore::Double *dish, casacore::Int niter);

    void intersectaperture(const calcAntenna *a, const casacore::Double *dish, 
			   const casacore::Double *unitdir, casacore::Double *aper);

    casacore::Double feedfunc(const calcAntenna *a, casacore::Double theta);

    casacore::Double feedgain(const calcAntenna *a, const Ray *ray, const Pathology *p);

    Ray* trace(const calcAntenna *a, casacore::Double x, casacore::Double y, const Pathology *p);

    void tracepol(casacore::Complex *E0, const Ray *ray, casacore::Complex *E1);

    casacore::Int legplanewaveblock(const calcAntenna *a, casacore::Double x, casacore::Double y);

    casacore::Int legplanewaveblock2(const calcAntenna *a, const Ray *ray);

    casacore::Int legsphericalwaveblock(const calcAntenna *a, const Ray *ray);

    void copyBeamCalcGeometry(BeamCalcGeometry* to, BeamCalcGeometry* from);

    static BeamCalc* instance_p;

    casacore::String obsName_p;
    casacore::String antType_p;
    casacore::MEpoch obsTime_p;
    casacore::uInt BeamCalc_NumBandCodes_p;
    casacore::Vector<BeamCalcGeometry> BeamCalcGeometries_p;
    casacore::Vector<casacore::Double> bandMinFreq_p; // in Hz
    casacore::Vector<casacore::Double> bandMaxFreq_p; // in Hz
    casacore::String antRespPath_p;

    static const casacore::Double METER_INCH;
    static const casacore::Double INCH_METER;
    static const casacore::Double NS_METER;
    static const casacore::Double METER_NS;
    static const casacore::Double DEG_RAD;
    static const casacore::Double RAD_DEG;

  };
  
};

#endif
