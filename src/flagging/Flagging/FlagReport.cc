//# FlagReport.cc: This file contains the implementation of the FlagReport class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <flagging/Flagging/FlagReport.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


////////////////////////////////////
/// FlagReport implementation ///
////////////////////////////////////

// All FlagReports must have 'type' and 'name' defined.
FlagReport::FlagReport(String type,String name, String title, String xlabel, 
        String ylabel):Record(),logger_p()
{
    logger_p.origin(LogOrigin("FlagReport",__FUNCTION__,WHERE));

    // Type of report. Options : none, list, plotraster, plotpoints
    if( ! ( type == "list" || type == "none" || type == "plotraster"
            || type == "plotpoints") )
    {
        logger_p << LogIO::WARN << "Invalid FlagReport type : " << type << ". Setting to 'none' " << LogIO::POST;
        type="none";
    }

    define( RecordFieldId("type") , type );

    if( type == "list" ) // List of reports
    {
        define( RecordFieldId("nreport") , (Int)0 );
    }
    else // One report
    {
        define( RecordFieldId("name") , name );
    }

    // One report of type "plot"
    if( type == "plotraster" ||  type == "plotpoints")
    {
        define( RecordFieldId("title") , title );
        define( RecordFieldId("xlabel") , xlabel );
        define( RecordFieldId("ylabel") , ylabel );
        define( RecordFieldId("ndata") , (Int)0 );
    }

}

FlagReport::FlagReport(String type, String name, const Record &other)
{

    assign(other);
    logger_p = casacore::LogIO();

    // Type of report. Options : none, summary
    if( ! ( type == "summary" || type == "rflag" || type == "none" ) )
    {
        logger_p << LogIO::WARN << "Invalid FlagReport type : " << type << ". Setting to 'none' " << LogIO::POST;
        type="none";
    }

    if( isDefined("type") )
    {
        logger_p << LogIO::WARN << "Overwriting field 'type' of input record by that supplied in this FlagReport constructor : " << type << LogIO::POST;
    }
    define( RecordFieldId("type") , type );

    if( isDefined("name") )
    {
        logger_p << LogIO::WARN << "Overwriting field 'name' of input record by that supplied in this FlagReport constructor : " << type << LogIO::POST;
    }
    define( RecordFieldId("name") , name );

}

FlagReport::FlagReport(const Record &other)
{
    assign(other);
    logger_p = casacore::LogIO();

    if( ! isDefined( "type" ) )
    {
        // add a type='none' field, and send for validation checks.
        define( RecordFieldId("type") , "none" );
    }

    if( ! isDefined( "name" ) )
    {
        // add a type='none' field, and send for validation checks.
        define( RecordFieldId("name") , "UnknownAgent" );
    }

}

FlagReport::~FlagReport()
{
}

// Add a FlagReport of any type to a FlagReport of type "list".
// If it is a single report, add as a subRecord
// If it is a list of reports, append this list, and re-index.
Bool
FlagReport::addReport(FlagReport inpReport)
{
    logger_p.origin(LogOrigin("FlagReport",__FUNCTION__,WHERE));

    // Verify and read type of current record
    if( !verifyFields() )
    {
        return false;
    }

    // Reports can be added only to flagreports of type 'list'
    if( reportType() != String("list"))
    {
        logger_p << LogIO::WARN << "Current FlagReport must be of type 'list' " << LogIO::POST;
        return false;
    }

    // Now that we know it's of type list, read nreport
    Int numReport = nReport();

    // React to different types of input FlagReports
    if( inpReport.reportType() != String("list")) // It's not a list.
    {
        if ( inpReport.reportType() != String("none") ) // It's not empty either. Add as a subRecord.
        {
            defineRecord( RecordFieldId(String("report")+String::toString(numReport)) , inpReport );
            define( RecordFieldId("nreport") , (Int)(numReport+1) );
        }
    }
    else // It's a list. Append the input list to the current one, re-indexing appropriately
    {
        Int nInpReps = inpReport.nReport();

        for(Int rep=0;rep<nInpReps;rep++)
        {
            defineRecord( RecordFieldId(String("report")+String::toString(numReport+rep)) , inpReport.asRecord( (String("report")+String::toString(rep)) ) );
        }
        define( RecordFieldId("nreport") , (Int)(numReport + nInpReps) );
    }
    return true;
}// end of addReport

//----------------------------------------------------------------------------------------------
Bool
FlagReport::addData(Array<Float> data)
{
    logger_p.origin(LogOrigin("FlagReport",__FUNCTION__,WHERE));

    String thisType = reportType();
    Bool retval = true;

    if( thisType != "plotraster" )
    {
        logger_p << LogIO::WARN << "Current FlagReport must be of type 'plotraster' " << LogIO::POST;
        //		return false;
        retval = false;
    }
    else
    {
        Int numData = nData();
        if(numData == 1)
        {
            logger_p << LogIO::WARN << "Cannot overlay raster plots." << LogIO::POST;
            //			return false;
            retval = false;

        }
        else
        {
            define( RecordFieldId(String("data")+String::toString(numData)) , data );
            define( RecordFieldId("ndata") , (Int)(numData+1) );
            retval = true;
        }
    }

    return retval;
}

//----------------------------------------------------------------------------------------------
Bool
FlagReport::addData(String plottype, Vector<Float> xdata, Vector<Float> ydata, String errortype, Vector<Float> error, String label)
{
    logger_p.origin(LogOrigin("FlagReport",__FUNCTION__,WHERE));

    String thisType = reportType();
    if( thisType != "plotpoints" )
    {
        logger_p << LogIO::WARN << "Current FlagReport must be of type 'plotpoints' " << LogIO::POST;
        return false;
    }
    else
    {
        Int numData = nData();
        define( RecordFieldId(String("plottype")+String::toString(numData)) , plottype );
        define( RecordFieldId(String("xdata")+String::toString(numData)) , xdata );
        define( RecordFieldId(String("ydata")+String::toString(numData)) , ydata );
        define( RecordFieldId(String("label")+String::toString(numData)) , label );
        define( RecordFieldId("ndata") , (Int)(numData+1) );

        if( error.nelements() > 0 )
        {
            define( RecordFieldId(String("error")+String::toString(numData)) , error );
            define( RecordFieldId(String("errortype")+String::toString(numData)) , errortype );
        }

    }

    return true;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

String
FlagReport::reportType()
{
    String thisType;
    if( ! isDefined("type") )
    {
        ostringstream xxx;
        print(xxx);
        cout << "Report with no type : " << xxx.str() << endl;
    }
    get( RecordFieldId("type") , thisType );
    return thisType;
}

Int
FlagReport::nReport()
{
    if( reportType() != "list" )
    {
        return -1;
    }
    else
    {
        Int nrep;
        get( RecordFieldId("nreport") , nrep );
        return nrep;
    }
}

Int
FlagReport::nData()
{
    String thisType = reportType();

    if( thisType != "plotraster" &&  thisType != "plotpoints" )
    {
        return -1;
    }
    else
    {
        Int ndat;
        get( RecordFieldId("ndata") , ndat );
        return ndat;
    }
}

Bool
FlagReport::accessReport(Int index, FlagReport &outReport)
{
    // Verify and read type of current record
    if( !verifyFields() )
    {
        return false;
    }

    if( reportType() != "list" )
    {
        return false;
    }

    if( index < 0 || index >= nReport() )
    {
        return false;
    }

    String repName(String("report")+String::toString(index));
    outReport = subRecord( RecordFieldId(repName) );
    return true;
}


//----------------------------------------------------------------------------------------------

// Check that all required fields have valid values.
Bool
FlagReport::verifyFields()
{
    logger_p.origin(LogOrigin("FlagReport",__FUNCTION__,WHERE));

    if( ! (isDefined("type") ) )
    {
        logger_p << LogIO::WARN << "Invalid FlagReport state ! No type is defined." << LogIO::POST;
        return false;
    }

    // Read type of current record
    String thisType;
    get( RecordFieldId("type") , thisType );

    // For a FlagReport of type "list", check that nreport exists and is correct.
    if(thisType == "list")
    {
        if( ! isDefined("nreport") )
        {
            logger_p << LogIO::WARN << "No 'nreport' defined" << LogIO::POST;
            return false; // nreport field is not defined
        }
        else // check that nreport subRecords exist, and verify each one.
        {
            Int nReps;
            get( RecordFieldId("nreport") , nReps );
            for(Int rep=0; rep<nReps; rep++)
            {
                String repname = String("report")+String::toString(rep);
                if( ! isDefined(repname) )
                {
                    logger_p << LogIO::WARN << "Report : " << repname << " is not defined" << LogIO::POST;
                    return false; // Does not contain a subReport
                }
                else
                {
                    FlagReport subRep(subRecord( RecordFieldId(repname) ));
                    if( ! subRep.verifyFields() )
                    {
                        logger_p << LogIO::WARN << "Invalid subRecord for " << repname << LogIO::POST;
                        return false; // subReport is invalid
                    }
                }
            }
        }
    }
    else if(thisType=="plotraster" || thisType=="plotpoints")
    {
        if( !isDefined("name") || !isDefined("title") ||
                !isDefined("xlabel") || !isDefined("ylabel") ||
                !isDefined("ndata") )
        {
            logger_p << LogIO::WARN << "Invalid FlagReport of type " << thisType << LogIO::POST;
            return false;
        }

        Int numData;
        get( RecordFieldId("ndata") , numData);

        for(Int dat=0;dat<numData;dat++)
        {
            if(  (thisType=="plotraster" && ! isDefined(String("data")+String::toString(dat)) )  ||
                    ( (thisType=="plotpoints")  && (!isDefined(String("xdata")+String::toString(dat)) || !isDefined(String("ydata")+String::toString(dat)) ) ) )
            {
                logger_p << LogIO::WARN << "Data for  " << dat << " is not defined" << LogIO::POST;
                return false; // Does not contain data.
            }

        }

    }
    // else it is of type 'summary' or 'none' and we don't check anything there.

    return true;

}// end of verifyFields()


} //# NAMESPACE CASA - END

