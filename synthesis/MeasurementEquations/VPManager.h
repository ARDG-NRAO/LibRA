//# VPManager functionality sits here; 
//# Copyright (C) 1996-2011
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright by ESO (in the framework of the ALMA collaboration)
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef SYNTHESIS_VPMANAGER_H
#define SYNTHESIS_VPMANAGER_H

#include <map>
#include <mutex>
#include <synthesis/MeasurementEquations/AntennaResponses.h>

namespace casacore{

  class Record;
}

namespace casa {
  //Forward declarations

  class VPManager
    {
      
    public: 
      // no need of enums here use the one from PBMathInterface
      //      enum Type{NONE, COMMONPB, AIRY, GAUSS, POLY, IPOLY, COSPOLY,
      //		NUMERIC, IMAGE, ZERNIKE, MAX=ZERNIKE};

      // this is a SINGLETON class
      static VPManager* Instance();

      void reset(casacore::Bool verbose=false);

      casacore::Bool saveastable(const casacore::String& tablename);

      casacore::Bool loadfromtable(const casacore::String& tablename);

      casacore::Bool summarizevps(const casacore::Bool verbose);


      casacore::Bool setcannedpb(const casacore::String& tel, 
		       const casacore::String& other, 
		       const casacore::Bool dopb,
		       const casacore::String& commonpb,
		       const casacore::Bool dosquint, 
		       const casacore::Quantity& paincrement, 
		       const casacore::Bool usesymmetricbeam,
		       casacore::Record& rec);

      casacore::Bool setpbairy(const casacore::String& telescope, const casacore::String& othertelescope, 
		     const casacore::Bool dopb, const casacore::Quantity& dishdiam, 
		     const casacore::Quantity& blockagediam, 
		     const casacore::Quantity& maxrad, 
		     const casacore::Quantity& reffreq, 
		     casacore::MDirection& squintdir, 
		     const casacore::Quantity& squintreffreq, const casacore::Bool dosquint, 
		     const casacore::Quantity& paincrement, 
		     const casacore::Bool usesymmetricbeam,
		     casacore::Record& rec);

      casacore::Bool setpbcospoly(const casacore::String& telescope, const casacore::String& othertelescope,
			const casacore::Bool dopb, const casacore::Vector<casacore::Double>& coeff,
			const casacore::Vector<casacore::Double>& scale,
			const casacore::Quantity& maxrad,
			const casacore::Quantity& reffreq,
			const casacore::String& isthispb,
			casacore::MDirection& squintdir,
			const casacore::Quantity& squintreffreq, const casacore::Bool dosquint,
			const casacore::Quantity& paincrement,
			const casacore::Bool usesymmetricbeam,
			casacore::Record& rec);

      casacore::Bool setpbgauss(const casacore::String& tel, const casacore::String& other, const casacore::Bool dopb,
		      const casacore::Quantity& halfwidth, const casacore::Quantity maxrad, 
		      const casacore::Quantity& reffreq, const casacore::String& isthispb, 
		      casacore::MDirection& squintdir, const casacore::Quantity& squintreffreq,
		      const casacore::Bool dosquint, const casacore::Quantity& paincrement, 
		      const casacore::Bool usesymmetricbeam, casacore::Record& rec);

      
     

      casacore::Bool setpbinvpoly(const casacore::String& telescope, const casacore::String& othertelescope,
			const casacore::Bool dopb, const casacore::Vector<casacore::Double>& coeff,
			const casacore::Quantity& maxrad,
			const casacore::Quantity& reffreq,
			const casacore::String& isthispb,
			casacore::MDirection& squintdir,
			const casacore::Quantity& squintreffreq, const casacore::Bool dosquint,
			const casacore::Quantity& paincrement,
			const casacore::Bool usesymmetricbeam,
			casacore::Record& rec);

      casacore::Bool setpbinvpoly(const casacore::String& telescope, const casacore::String& othertelescope,
			const casacore::Bool dopb, const casacore::Matrix<casacore::Double>& coeff,
			const casacore::Vector<casacore::Double>& freqs,
			const casacore::Quantity& maxrad,
			const casacore::Quantity& reffreq,
			const casacore::String& isthispb,
			casacore::MDirection& squintdir,
			const casacore::Quantity& squintreffreq, const casacore::Bool dosquint,
			const casacore::Quantity& paincrement,
			const casacore::Bool usesymmetricbeam,
			casacore::Record& rec);

      casacore::Bool setpbnumeric(const casacore::String& telescope, const casacore::String& othertelescope,
			const casacore::Bool dopb, const casacore::Vector<casacore::Double>& vect,
			const casacore::Quantity& maxrad,
			const casacore::Quantity& reffreq,
			const casacore::String& isthispb,
			casacore::MDirection& squintdir,
			const casacore::Quantity& squintreffreq, const casacore::Bool dosquint,
			const casacore::Quantity& paincrement,
			const casacore::Bool usesymmetricbeam,
			casacore::Record &rec);

      ///antnames => Antenna names for which this PB image applies. "*" is for all 
      casacore::Bool setpbimage(const casacore::String& telescope, const casacore::String& othertelescope, 
		      const casacore::Bool dopb, const casacore::String& realimage, 
		      const casacore::String& imagimage, const casacore::String& compleximage, const casacore::Vector<casacore::String>& antnames,  casacore::Record& rec);
      ///////Returns the image beam that the vpmananger has in state
      ////// antnames contains the antenna names for which each element of the image beams apply
      casacore::Bool imagepbinfo(casacore::Vector<casacore::Vector<casacore::String> >& antnames, casacore::Vector<casacore::Record>& imagebeams);

      casacore::Bool setpbpoly(const casacore::String& telescope, const casacore::String& othertelescope,
		     const casacore::Bool dopb, const casacore::Vector<casacore::Double>& coeff,
		     const casacore::Quantity& maxrad,
		     const casacore::Quantity& reffreq,
		     const casacore::String& isthispb,
		     casacore::MDirection& squintdir,
		     const casacore::Quantity& squintreffreq, const casacore::Bool dosquint,
		     const casacore::Quantity& paincrement,
		     const casacore::Bool usesymmetricbeam,
		     casacore::Record &rec);
      

      casacore::Bool setpbantresptable(const casacore::String& telescope, const casacore::String& othertelescope,
			     const casacore::Bool dopb, const casacore::String& tablepath);
                            // no record filled, need to access via getvp()

      // set the default voltage pattern for the given telescope
      casacore::Bool setuserdefault(const casacore::Int vplistfield,
			  const casacore::String& telescope,
			  const casacore::String& antennatype="");

      casacore::Bool getuserdefault(casacore::Int& vplistfield,
			  const casacore::String& telescope,
			  const casacore::String& antennatype="");

      casacore::Bool getanttypes(casacore::Vector<casacore::String>& anttypes,
		       const casacore::String& telescope,
		       const casacore::MEpoch& obstime,
		       const casacore::MFrequency& freq, 
		       const casacore::MDirection& obsdirection); // default: Zenith
			      
      // return number of voltage patterns satisfying the given constraints
      casacore::Int numvps(const casacore::String& telescope,
		 const casacore::MEpoch& obstime,
		 const casacore::MFrequency& freq, 
		 const casacore::MDirection& obsdirection=casacore::MDirection(casacore::Quantity( 0., "deg"), // default is the Zenith
							   casacore::Quantity(90., "deg"), 
							   casacore::MDirection::AZEL)
		 ); 


      // get the voltage pattern satisfying the given constraints
      casacore::Bool getvp(casacore::Record &rec,
		 const casacore::String& telescope,
		 const casacore::MEpoch& obstime,
		 const casacore::MFrequency& freq, 
		 const casacore::String& antennatype="", 
		 const casacore::MDirection& obsdirection=casacore::MDirection(casacore::Quantity( 0., "deg"), // default is the Zenith
							   casacore::Quantity(90., "deg"), 
							   casacore::MDirection::AZEL)
		 ); 

      // get a general voltage pattern for the given telescope and ant type if available
      casacore::Bool getvp(casacore::Record &rec,
		 const casacore::String& telescope,
		 const casacore::String& antennatype=""
		 ); 

      // get a set of all necessary beams and their properties for a given antenna list
      casacore::Bool getvps(casacore::Vector<casacore::Record> & unique_out_rec_list, // the list of unique beam records
		  casacore::Vector<casacore::Vector<casacore::uInt> >& beam_index, // indices to the above vectors in sync with AntennaNames
		  const casacore::String& telescope,
		  const casacore::Vector<casacore::MEpoch>& inpTimeRange, // only elements 0 and 1 are used; if 1 is not present it is assumed to be inf
		  const casacore::Vector<casacore::MFrequency>& inpFreqRange, // must contain at least one element; beams will be provided for each element 
		  const casacore::Vector<casacore::String>& AntennaNames, // characters 0 and 1 are used for ALMA to determine the antenna type
		  const casacore::MDirection& obsdirection=casacore::MDirection(casacore::Quantity( 0., "deg"), // default is the Zenith
							    casacore::Quantity(90., "deg"), 
							    casacore::MDirection::AZEL)
		  );

      casacore::Bool vpRecIsIdentical(const casacore::Record& rec0, const casacore::Record& rec1);

    protected:
      VPManager(casacore::Bool verbose=false);

    private:
      static VPManager* instance_p;

      static std::recursive_mutex mutex_p;

      casacore::Record vplist_p; 
      std::map<casacore::String, casacore::Int > vplistdefaults_p; 
      AntennaResponses aR_p; 

      inline casacore::String antennaDescription(const casacore::String& telescope,
				       const casacore::String& antennatype){
	if(antennatype.empty()) return telescope;
	return telescope + " " + antennatype;
      };

      inline casacore::String telFromAntDesc(const casacore::String& antDesc){
	casacore::String tempstr = antDesc;
	if(tempstr.contains(" ")) return tempstr.before(" ");
	return tempstr;
      };

      inline casacore::String antTypeFromAntDesc(const casacore::String& antDesc){
	casacore::String tempstr = antDesc;
	if(tempstr.contains(" ")) return tempstr.after(" ");
	tempstr = "";
	return tempstr;
      };

    };

} //# NAMESPACE CASA - END
#endif
