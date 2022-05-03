//# VisImagingWeight.cc: imaging weight calculation for a give buffer
//# Copyright (C) 2011
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


#include <casa/Utilities/CountedPtr.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <casa/Logging/LogIO.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/TableUtil.h>
#include <components/ComponentModels/ComponentList.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <ms/MeasurementSets/MSSource.h>
#include <ms/MSSel/MSSourceIndex.h>
#include <ms/MeasurementSets/MSSourceColumns.h>

#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines2/VisModelData.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/SimpleComponentFTMachine.h>
#include <synthesis/TransformMachines2/GridFT.h>
//#include <synthesis/TransformMachines/rGridFT.h>
#include <synthesis/TransformMachines2/MosaicFTNew.h>
#include <synthesis/TransformMachines2/WProjectFT.h>
//#include <synthesis/TransformMachines/MultiTermFT.h>
#include <synthesis/TransformMachines2/MultiTermFTNew.h>
#include <synthesis/TransformMachines2/SetJyGridFT.h>

namespace {

  casa::VisModelDataI * createRefImVisModelData (){
    return new casa::refim::VisModelData ();
  }

  // FIXME compiler warning re: unused variable initializeVisModelDataFactory 
  bool initializeVisModelDataFactory = casa::VisModelDataI::setFactory (createRefImVisModelData, 1);

}



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace refim {//namespace for refactor

using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;
using namespace casacore;
using namespace casa::vi;
  VisModelData::VisModelData(): clholder_p(0), ftholder_p(0), flatholder_p(0), version_p(-1){
  
  cft_p=new SimpleComponentFTMachine();
  }

  VisModelData::~VisModelData(){


  }

VisModelDataI *
VisModelData::clone ()
{
    return new VisModelData (* this);
}


  Bool VisModelData::hasAnyModel(const MeasurementSet& thems, Vector<Int>& fieldids){
    Bool retval=False;
    fieldids.resize();
    MSColumns msc(thems);
    Vector<Int> fields=msc.fieldId().getColumn();
    const Sort::Order order=Sort::Ascending;
    const Int option=Sort::HeapSort | Sort::NoDuplicates;
    Int nfields=GenSort<Int>::sort (fields, order, option);\
    if (nfields>0) {
      for (Int j=0; j< 2; ++j){
    	const Table* thetab=&thems;
    	if (j==1)
	  thetab=&(thems.source());
    	for (Int k=0; k< nfields; ++k){
    			if(thetab->keywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
    		{
		  String elkey=thetab->keywordSet().asString("definedmodel_field_"+String::toString(fields[k]));
		  if(thetab->keywordSet().isDefined(elkey)){
		    fieldids.resize(fieldids.nelements()+1, True);
		    fieldids[fieldids.nelements()-1]=fields[k];
    		  }
    		}
    	}
      }
    }
    if(fieldids.nelements() >0)
      retval=True;

    return retval;
  }

void VisModelData::listModel(const MeasurementSet& thems){
 
  //Table newTab(thems);

  MSColumns msc(thems);
  Vector<String> fldnames=msc.field().name().getColumn();
  Vector<Int> fieldids;
  LogIO logio;
  if(hasAnyModel(thems, fieldids)){
      for (uInt k=0; k< fieldids.nelements(); ++k){
    			
	logio << " " << fldnames[fieldids[k]] << " (id = " << fieldids[k] << ")" << LogIO::POST;
    			  
      }
    }
    else{
      logio <<  " None." << LogIO::POST;
    }
  
    
}

void VisModelData::deleteDiskImage(MeasurementSet& theMS, const String& theKey){
  TableRecord theRec;
  if(!VisModelData::getModelRecord(theKey, theRec, theMS))
    return;
  if(theRec.isDefined("numft")){
    Int numft=0;
    numft=theRec.asInt("numft");
    for (Int k=0; k < numft; ++k){
      String ftname=String("ft_")+String::toString(k);
      const Record& ftrec=theRec.asRecord(ftname);
      if(ftrec.asRecord("container").isDefined("diskimage")){
	String diskim=ftrec.asRecord("container").asString("diskimage");
	if(TableUtil::canDeleteTable(diskim))
	  TableUtil::deleteTable(diskim);
      }

    }
  }


}
void VisModelData::removeRecordByKey(MeasurementSet& theMS, const String& theKey){


  deleteDiskImage(theMS, theKey);
  if(Table::isReadable(theMS.sourceTableName()) &&theMS.source().nrow() > 0 ){
    if(theMS.source().keywordSet().isDefined(theKey)){
      Int rowid=theMS.source().keywordSet().asInt(theKey);
      TableRecord elrec;
      //Replace the model with an empty record
      MSSourceColumns srcCol(theMS.source());
      srcCol.sourceModel().put(rowid, elrec);
      theMS.source().rwKeywordSet().removeField(theKey);
    }
  }
  //Remove from Main table if it is there
  if(theMS.rwKeywordSet().isDefined(theKey))
    theMS.rwKeywordSet().removeField(theKey);
}

void VisModelData::clearModel(const MeasurementSet& thems){
  //  Table newTab(thems);
  MeasurementSet& newTab=const_cast<MeasurementSet& >(thems);
  if(!newTab.isWritable())
    return;
  auto part_block = newTab.getPartNames(true);
  Vector<String> theParts(part_block.begin( ),part_block.end( ));
  if(theParts.nelements() > 1){
    for (uInt k=0; k < theParts.nelements(); ++k){
      MeasurementSet subms(theParts[k], newTab.lockOptions(), Table::Update);
      clearModel(subms);
    }
    return;
  }
  Bool alreadyLocked=newTab.hasLock(True);
  if(!alreadyLocked)
    newTab.lock(True);
  if(Table::isReadable(newTab.sourceTableName())){
    if(!newTab.source().isWritable())
      return;
    newTab.source().lock(True);   
  }
  LogIO logio;
  logio << "Clearing all model records in MS header."
	  << LogIO::POST;

  MSColumns msc(thems);
  Vector<Int> fields=msc.fieldId().getColumn();
  Vector<String> fldnames=msc.field().name().getColumn();
  const Sort::Order order=Sort::Ascending;
  const Int option=Sort::HeapSort | Sort::NoDuplicates;
  Int nfields=GenSort<Int>::sort (fields, order, option);
  for (Int k=0; k< nfields; ++k){
    String elkey;
    Int srow;
    //if(newTab.rwKeywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
      if(isModelDefined(fields[k], thems, elkey, srow))
      {
	logio << " " << fldnames[fields[k]] << " (id = " << fields[k] << ") deleted." << LogIO::POST;
	//Remove from Source table
	removeRecordByKey(newTab, elkey);
	if(srow > -1){
	  if(thems.source().keywordSet().isDefined("definedmodel_field_"+String::toString(fields[k]))){
	    newTab.source().rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
	  }
	}
	//Remove from Main table if it is there
	if(newTab.rwKeywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
	  newTab.rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
      }
      else{
	///remove keys that point to nowehere
	//cerr << "The key " << elkey << endl;
	if(!newTab.source().isNull() && newTab.source().rwKeywordSet().isDefined(elkey))
	  newTab.source().rwKeywordSet().removeField(elkey);
	if(newTab.rwKeywordSet().isDefined(elkey))
	  newTab.rwKeywordSet().removeField(elkey);
	if( !newTab.source().isNull() && newTab.source().rwKeywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
	  newTab.source().rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
	if(newTab.rwKeywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
	   newTab.rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
      }
  }
  if(!alreadyLocked)
    newTab.unlock();
  if(Table::isReadable(newTab.sourceTableName())){
    newTab.source().unlock();   
  }
  ////Cleaning out orphaned image disk models
  String srctable=thems.source().isNull() ? "" : thems.source().tableName();
  Vector<String> possibleFT(2); 
  possibleFT(0)=srctable+"/FT_MODEL*";
  possibleFT(1)=theParts[0]+"/FT_MODEL*";
  
  Vector<String> ftmods=Directory::shellExpand(possibleFT);
  for (uInt kk=0; kk < ftmods.nelements(); ++kk){
    if(TableUtil::canDeleteTable(ftmods[kk])){
      TableUtil::deleteTable(ftmods[kk]);
    }
  }

}
  void VisModelData::clearModel(const MeasurementSet& thems, const String field, const String specwindows){
    MeasurementSet& newTab=const_cast<MeasurementSet& >(thems);
    if(!newTab.isWritable())
      return;
  auto part_block = newTab.getPartNames(true);
  Vector<String> theParts(part_block.begin( ),part_block.end( ));
  if(theParts.nelements() > 1){
    for (uInt k =0; k < theParts.nelements(); ++k){
      MeasurementSet subms(theParts[k], newTab.lockOptions(), Table::Update);
      clearModel(subms, field, specwindows);
    }
    return;
  }
  if(!newTab.isWritable())
    return;
  Bool alreadyLocked=newTab.hasLock(True);
  if(!alreadyLocked)
    newTab.lock(True);
  if(Table::isReadable(newTab.sourceTableName())){
    if(!Table::isWritable(newTab.sourceTableName()))
      return;
    newTab.source().lock(True);   
  }

  MSColumns msc(thems);
  Vector<String> fldnames=msc.field().name().getColumn();
  Int nfields=0;
  Vector<Int> fields(0);
  {
    // Parse field specification
    MSSelection mss;
    mss.setFieldExpr(field);
    TableExprNode ten=mss.toTableExprNode(&thems);
    fields=mss.getFieldList();
    nfields=fields.nelements();
  }


  if (nfields==0)
    // Call the method that deletes them all
    VisModelData::clearModel(thems);
  else {

    //Now we have the two cases the whole field or specific spws
    // only delete the specified ones
    Vector<Int> spws(0);
    Int nspws=0;
    {
      // Parse field specification
      MSSelection mss;
      mss.setFieldExpr(field);
      mss.setSpwExpr(specwindows);
      TableExprNode ten=mss.toTableExprNode(&thems);
      spws=mss.getSpwList();
      nspws=spws.nelements();
    }

   
    LogIO logio;
    logio << "Clearing model records in MS header for selected fields." 
	  << LogIO::POST;
    
    for (Int k=0; k< nfields; ++k){
      String elkey;
      Int srow;
      if(isModelDefined(fields[k], newTab, elkey, srow))
	
	{
	
	  if(nspws==0){
	    logio << " " << fldnames[fields[k]] << " (id = " << fields[k] << ") deleted." << LogIO::POST;
	    removeRecordByKey(newTab, elkey);
	    if(srow > -1 ){
	      if(newTab.source().keywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
		newTab.source().rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));	      
	    }
	    if(newTab.rwKeywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
	      newTab.rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
	  }
	  else{
	    //if(newTab.rwKeywordSet().isDefined(elkey)){
	    
	    TableRecord conteneur;  
	    getModelRecord(elkey, conteneur, newTab); 
	   
	    //=newTab.rwKeywordSet().asRecord(elkey);
	    if(removeSpw(conteneur, spws, fields)){
	    	putRecordByKey(newTab, elkey, conteneur, srow);
	    }
	    else{
	    	removeRecordByKey(newTab, elkey);
	    	if(newTab.source().keywordSet().isDefined("definedmodel_field_"+String::toString(fields[k])))
	    		newTab.source().rwKeywordSet().removeField("definedmodel_field_"+String::toString(fields[k]));
	    }
	      /*
	      Vector<Int> defspws=conteneur.asArrayInt("spws");
	      Vector<Int> newdefspw(defspws.nelements(), -1);
	      Int counter=0;
	      for(uInt k=0; k < defspws.nelements(); ++k){
		for (Int j=0; j < nspws; ++j){
		  if(defspws[k] != spws[j]){
		    newdefspw[counter]=defspws[k];
		    ++counter;
		  }
		}
	      }
	      if(counter==0){
		//Now we have to remove this ftm or cft
		newTab.rwKeywordSet().removeField(elkey);
	      }
	      else{
		conteneur.define("spws", newdefspw);
		updatespw(conteneur, 
	      }
	      */
	  }
	      
	
	    
	}
      else
	logio << " " << fldnames[fields[k]] << " (id = " << fields[k] << ") not found." << LogIO::POST;
    }
    
  }
  if(!alreadyLocked)
    newTab.unlock();
  if(Table::isReadable(newTab.sourceTableName())){
    newTab.source().unlock();   
  }
  

  }

  Bool VisModelData::removeSpw(TableRecord& therec, const Vector<Int>& spws, const Vector<Int>& fields){
     Int numft=0;
     Vector<String> numtype(2);
     Vector<String> keyval(2);
     numtype[0]=String("numft");
     numtype[1]=String("numcl");
     keyval[0]="ft_";
     keyval[1]="cl_";
     
     for (Int j=0; j< 2; ++j){
    	 if(therec.isDefined(numtype[j])){
    		 numft=therec.asInt(numtype[j]);
    		 Vector<String> toberemoved(numft);
    		 Int numrem=0;
    		 for (Int k=0; k < numft; ++k){
    			 RecordInterface& ftrec=therec.asrwRecord(keyval[j]+String::toString(k));
			 if(!ftrec.isDefined("version")){
			   Bool hasField=true;
			   if(fields.nelements() >0){
			     hasField=false;
			     Vector<Int> fieldsinrec;
			     ftrec.get("fields", fieldsinrec);
			     if(anyEQ(fieldsinrec, fields))
			       hasField=true;
			   }
			   if(hasField && !removeSpwFromMachineRec(ftrec, spws)){
			     toberemoved[numrem]=String(keyval[j]+String::toString(k));
			     ++numrem;
			   }
			 }
			 else{
			   //Version 2
			   const Matrix<Int>& ftcombind=ftrec.asArrayInt("indexcombination");
			   Matrix<Int> newComb;
			   for(uInt row=0; row < ftcombind.nrow(); ++row){
			     Vector<Int> rowcomb=ftcombind.row(row);	
			     Bool rowTobeRemoved=False;
			     for (uInt ff=0; ff < fields.nelements() ; ++ff){
			       for (uInt ss=0; ss < spws.nelements(); ++ss){
				 if(rowcomb[0]==fields[ff] && rowcomb[1] == spws[ss] )
				   rowTobeRemoved=True;
				 
				 
			       }
			     }
			     if(!rowTobeRemoved){
			       newComb.resize(newComb.nrow()+1,4, True);
			       newComb.row(newComb.nrow()-1)=rowcomb;
			     }
			   }
			   if(newComb.nrow()==0){
			     toberemoved[numrem]=String(keyval[j]+String::toString(k));
			     ++numrem;	
			   }
			   else{
			     ftrec.define("indexcombination", newComb);
			   }
			   
			   
			 }
    		 }
    		 if(numrem > 0){
    			 for (Int k=0; k < numrem; ++k)
    				 removeFTFromRec(therec, toberemoved[k], k==(numrem-1));
    		 }
    	 }
     }
     numft=0; Int numcl=0;
     if(therec.isDefined("numft")) numft=therec.asInt("numft");
     if(therec.isDefined("numcl")) numcl=therec.asInt("numcl");
     if (numft==0 && numcl==0)
    	 return false;
     return true;
  }

  Bool VisModelData::removeFTFromRec(TableRecord& therec, const String& keyval, const Bool relabel){


    String *splitkey=new String[2];
    Int nsep=split(keyval, splitkey, 2, String("_"));
    if (nsep <1 || !therec.isDefined(keyval)) 
      return false;
    String eltype=splitkey[0];
    //Int modInd=String::toInt(splitkey[1]);
    Int numcomp= (eltype==String("ft")) ? therec.asInt("numft"): therec.asInt("numcl");
    therec.removeField(keyval);
    
    numcomp=numcomp-1;
    if(eltype=="ft")
      therec.define("numft", numcomp);
    else
      therec.define("numcl", numcomp);
    if(relabel){

    
    	eltype=eltype+String("_");
    	Int id=0;
    	for(uInt k=0; k < therec.nfields(); ++k){
    		if(therec.name(k).contains(eltype)){
    			therec.renameField(eltype+String::toString(id), k);
    			++id;
    		}
    	}
    }

    delete [] splitkey;
    return true;
  } 

  Bool VisModelData::removeSpwFromMachineRec(RecordInterface& ftclrec, const Vector<Int>& spws){
    Vector<Int> defspws=ftclrec.asArrayInt("spws");
    Vector<Int> newdefspw(defspws.nelements(), -1);
    Int counter=0;
    for(uInt k=0; k < defspws.nelements(); ++k){
      for (uInt j=0; j < spws.nelements(); ++j){
	if(defspws[k] == spws[j]){
	  defspws[k]=-1;
	  ++counter;
	}
      }
    }
    if(defspws.nelements() == uInt(counter)){
      //Now we have to remove this ft or cl model

      return false;

    }    
    newdefspw.resize(defspws.nelements()-counter);
    counter=0;
    for (uInt k=0; k < defspws.nelements(); ++k){
      if(defspws[k] != -1){
	newdefspw[counter]=defspws[k];
	++counter;
      }
    }
    
    ftclrec.define("spws", newdefspw);
    return true;
  }

  Bool VisModelData::addToRec(TableRecord& therec, const Vector<Int>& spws){

    Int numft=0;
    Int numcl=0;
    Vector<Bool> hasSpw(spws.nelements(), false);
    if(therec.isDefined("numft")){
      numft=therec.asInt("numft");
      Vector<Int> ft_toremove(numft, 0);
      for(Int k=0; k < numft; ++k){
	const Record& ftrec=therec.asRecord("ft_"+String::toString(k));
	const Vector<Int>& ftspws=ftrec.asArrayInt("spws");
	for (uInt i=0; i<spws.nelements(); ++i){
	  for (uInt j=0; j<ftspws.nelements(); ++j){
	    if(spws[i]==ftspws[j]){
	      hasSpw[i]=true;	   
	      ft_toremove[k]=1;
	    }
	  }	
	}
      }
      if(sum(ft_toremove) >0){
	for(Int k=0; k < numft; ++k){
	  if(ft_toremove[k]==1)
	    therec.removeField("ft_"+String::toString(k));
	}
	numft=numft-sum(ft_toremove);
	therec.define("numft", numft);
	Int id=0;
	for(uInt k=0; k < therec.nfields(); ++k){
	  if(therec.name(k).contains("ft_")){
	    therec.renameField("ft_"+String::toString(id), k);
	    ++id;
	  }
	}
      }
    }
    if(therec.isDefined("numcl")){
      numcl=therec.asInt("numcl");
      Vector<Int> cl_toremove(numcl, 0);
      for(Int k=0; k < numcl; ++k){
	const Record& clrec=therec.asRecord("cl_"+String::toString(k));
	const Vector<Int>& clspws=clrec.asArrayInt("spws");
	for (uInt i=0; i<spws.nelements(); ++i){
	  for (uInt j=0; j<clspws.nelements(); ++j){
	    if(spws[i]==clspws[j]){
	      hasSpw[i]=true;	    
	      cl_toremove[k]=1;
	    }
	  }	
	}
      }
      if(sum(cl_toremove) >0){
	for(Int k=0; k < numcl; ++k){
	  if(cl_toremove[k]==1)
	    therec.removeField("cl_"+String::toString(k));
	}
	numcl=numcl-sum(cl_toremove);
	therec.define("numcl", numcl);
	Int id=0;
	for(uInt k=0; k < therec.nfields(); ++k){
	  if(therec.name(k).contains("cl_")){
	    therec.renameField("cl_"+String::toString(id), k);
	    ++id;
	  }
	}
      }
    }
    return (!allTrue(hasSpw) || ((numft+numcl)>0));
  }

Bool VisModelData::addToRec(TableRecord& therec, const Matrix<Int>& combind){
  if(therec.nfields() >0 && ((!therec.isDefined("version")) || (therec.asInt("version") !=2)))
    throw(AipsError("cannot combine with older version virtual model column;\n please clear it before proceeding"));
  Int numft=0;
  Int numcl=0;
  Vector<Bool> hasComb(combind.nrow(), false);
  if(therec.isDefined("numft")){
    numft=therec.asInt("numft");
    Vector<Int> ft_toremove(numft, 0);
    for(Int k=0; k < numft; ++k){
      const Record& ftrec=therec.asRecord("ft_"+String::toString(k));
      const Matrix<Int>& ftcombind=ftrec.asArrayInt("indexcombination");
      for (uInt i=0; i<combind.nrow(); ++i){
	for (uInt j=0; j<ftcombind.nrow(); ++j){
	  if(allEQ(combind.row(i), ftcombind.row(j))){
	    //We could upgrade this to just delete the intent from the ft
	    hasComb[i]=true;	   
	    ft_toremove[k]=1;
	  }
	}	
      }
    }
    if(sum(ft_toremove) >0){
      for(Int k=0; k < numft; ++k){
	if(ft_toremove[k]==1)
	  therec.removeField("ft_"+String::toString(k));
      }
      numft=numft-sum(ft_toremove);
      therec.define("numft", numft);
      Int id=0;
      for(uInt k=0; k < therec.nfields(); ++k){
	if(therec.name(k).contains("ft_")){
	  therec.renameField("ft_"+String::toString(id), k);
	  ++id;
	}
      }
    }
  }
  if(therec.isDefined("numcl")){
    numcl=therec.asInt("numcl");
    Vector<Int> cl_toremove(numcl, 0);
    for(Int k=0; k < numcl; ++k){
      const Record& clrec=therec.asRecord("cl_"+String::toString(k));
      const Matrix<Int>& clcombind=clrec.asArrayInt("indexcombination");
      for (uInt i=0; i<combind.nrow(); ++i){
	for (uInt j=0; j<clcombind.nrow(); ++j){
	  if(allEQ(combind.row(i),clcombind.row(j))){
	    hasComb[i]=true;	    
	    cl_toremove[k]=1;
	  }
	}	
      }
      }
    if(sum(cl_toremove) >0){
      for(Int k=0; k < numcl; ++k){
	if(cl_toremove[k]==1)
	  therec.removeField("cl_"+String::toString(k));
      }
      numcl=numcl-sum(cl_toremove);
      therec.define("numcl", numcl);
      Int id=0;
      for(uInt k=0; k < therec.nfields(); ++k){
	if(therec.name(k).contains("cl_")){
	  therec.renameField("cl_"+String::toString(id), k);
	  ++id;
	}
      }
    }
  }
  return (!allTrue(hasComb) || ((numft+numcl)>0));
}
 
  
Bool VisModelData::isModelDefined(const Int fieldId, const MeasurementSet& thems, String& thekey, Int& sourceRow){
  sourceRow=-1;
  String modelkey=String("definedmodel_field_")+String::toString(fieldId);
  thekey="";
  if(Table::isReadable(thems.sourceTableName()) && thems.source().nrow() > 0 && (thems.source().keywordSet().isDefined(modelkey))){
      {
      thekey=thems.source().keywordSet().asString(modelkey);
      if(thems.source().keywordSet().isDefined(thekey))
	sourceRow=thems.source().keywordSet().asInt(thekey);
    }
  }else{
    if(thems.keywordSet().isDefined(modelkey)){
      thekey=thems.keywordSet().asString(modelkey);  
    }
  }
  if(thekey != "" )
    return VisModelData::isModelDefined(thekey, thems);
  return false;
  
}

  Bool VisModelData::isModelDefined(const String& elkey, const MeasurementSet& thems){
    //Let's try the Source table
    if(Table::isReadable(thems.sourceTableName()) &&thems.source().nrow() > 0 ){
      if(thems.source().keywordSet().isDefined(elkey))
	return true;      
    }
    //Let's try the Main table 
    if(thems.keywordSet().isDefined(elkey))
      return true;
    return false;
  }

  Bool VisModelData::getModelRecord(const String& theKey, TableRecord& theRec, const MeasurementSet& theMs){
    //Let's try the Source table
    if(Table::isReadable(theMs.sourceTableName()) &&theMs.source().nrow() > 0 && (theMs.source().keywordSet().isDefined(theKey))){
	{
	//Get the row for the model 
        Int row=theMs.source().keywordSet().asInt(theKey);
	//MSSourceColumns srcCol(theMs.source());
     
	ScalarColumn<TableRecord> scol(theMs.source(), "SOURCE_MODEL");
	scol.get(row, theRec);
      }
      return true;
    }
    //Let's try the Main table 
    if(theMs.keywordSet().isDefined(theKey)){
      theRec=theMs.keywordSet().asRecord(theKey);
      return true;
    }
    return false;


  }

  void VisModelData::putRecordByKey(MeasurementSet& theMS, const String& theKey, const TableRecord& theRec, const Int sourceRowNum){
    //Prefer the Source table first    
    if( (sourceRowNum> -1) && Table::isReadable(theMS.sourceTableName()) && Int(theMS.source().nrow()) > sourceRowNum ){
      MSSource& mss=theMS.source();
      if(!mss.isColumn(MSSource::SOURCE_MODEL) ){
	mss.addColumn(ScalarRecordColumnDesc("SOURCE_MODEL"), true);
      }
      if(mss.rwKeywordSet().isDefined(theKey))
	mss.rwKeywordSet().removeField(theKey);
      mss.rwKeywordSet().define(theKey, sourceRowNum);
      MSSourceColumns srcCol(mss);
      srcCol.sourceModel().put(sourceRowNum, theRec);
      return;
    }
    //Oh well no source table so will add it to the main table
    theMS.rwKeywordSet().defineRecord(theKey, theRec);
    

  }

  Bool VisModelData::putModelRecord(const Vector<Int>& fieldIds, TableRecord& theRec, MeasurementSet& theMS){
    auto part_block = theMS.getPartNames(true);
    Vector<String> theParts(part_block.begin( ),part_block.end( ));
    if(theParts.nelements() > 1){
      Bool retval=true;
      for (uInt k =0; k < theParts.nelements(); ++k){
    	  MeasurementSet subms(theParts[k], theMS.lockOptions(), Table::Update);
    	  retval= retval && putModelRecord(fieldIds, theRec, subms);
      }
      return retval;
    }
 
    String elkey="model";
    for (uInt k=0; k < fieldIds.nelements();  ++k){
      elkey=elkey+"_"+String::toString(fieldIds[k]);
    }
    if(theMS.rwKeywordSet().isDefined(elkey))
      theMS.rwKeywordSet().removeField(elkey);
    Int row=-1;
    //Prefer the Source table first    
    MSFieldColumns fCol(theMS.field());
    if(Table::isReadable(theMS.sourceTableName()) && (theMS.source().nrow() > 0) &&  (!fCol.sourceId().isNull()) && (fCol.sourceId().get(fieldIds[0]) != -1) ){
      //
      row=0;
      MSSource& mss=theMS.source();
     
      Int sid=fCol.sourceId().get(fieldIds[0]);
      auto rows=MSSourceIndex(mss).getRowNumbersOfSourceID(sid);
      if(rows.nelements() > 0) 
	row=rows[0];
      else{
	LogIO logio;
	logio << "Invalid Source_id "+String::toString(sid)+" found in FIELD table\n" 
	      <<"Model is being written at Source ID 0 position which will be erased\n" 
	      << "Fix the FIELD table before proceeding " 
	      <<  LogIO::WARN << LogIO::POST;
	
      }
	  
	
      
      putRecordByKey(theMS, elkey, theRec, row);
      for (uInt k=0; k < fieldIds.nelements();  ++k){
	mss.rwKeywordSet().define("definedmodel_field_"+String::toString(fieldIds[k]), elkey);
      }
      return true;
      
    }
    //Oh well no source table so will add it to the main table
    putRecordByKey(theMS, elkey, theRec, -1);
    for (uInt k=0; k < fieldIds.nelements();  ++k){
      theMS.rwKeywordSet().define("definedmodel_field_"+String::toString(fieldIds[k]), elkey);	
    }

    return true;
  }


void VisModelData::putModel(const MeasurementSet& thems, const RecordInterface& rec, const Vector<Int>& validfieldids, const Vector<Int>& spws, const Vector<Int>& starts, const Vector<Int>& nchan,  const Vector<Int>& incr, Bool iscomponentlist, Bool incremental){

  LogIO logio;

  try{
    //A field can have multiple FTmachines and ComponentList associated with it 
    //For example having many flanking images for the model
    //For componentlist it may have multiple componentlist ...for different spw
  //Timer tim;
  //tim.mark();
    Int counter=0;
    Record modrec;
    modrec.define("fields", validfieldids);
    modrec.define("spws", spws);
    modrec.define("start", starts);
    modrec.define("nchan", nchan);
    modrec.define("incr", incr);
    modrec.defineRecord("container", rec);
    String elkey="model";
    for (uInt k=0; k < validfieldids.nelements();  ++k){
      elkey=elkey+"_"+String::toString(validfieldids[k]);
    }
    TableRecord outRec; 
    Bool addtorec=false;
    MeasurementSet& newTab=const_cast<MeasurementSet& >(thems);
    newTab.lock(True);
    if(Table::isReadable(newTab.sourceTableName())){
      newTab.source().lock(True);   
    }
    if(isModelDefined(elkey, newTab)){ 
      getModelRecord(elkey, outRec, thems);
      //if incremental no need to check & remove what is in the record
      if(!incremental)
    	  addtorec=addToRec(outRec, spws);
    }

    ///////even if it is not defined some other field model might be sitting on that
    //////model key
    Int hasSourceRecord=firstSourceRowRecord(validfieldids[0], thems, outRec);
    if(hasSourceRecord > -1 && outRec.nfields() > 0)
      addtorec=true;
    //cerr << "has Source " << hasSourceRecord << endl;
    ////

    incremental=incremental || addtorec;
    if(iscomponentlist){
      modrec.define("type", "componentlist");
      if(outRec.isDefined("numcl"))
	counter=incremental ? outRec.asInt("numcl") : 0;
            
    }
    else{
      modrec.define("type", "ftmachine");
      if(outRec.isDefined("numft"))
	counter=incremental ? outRec.asInt("numft") : 0;
    }
    iscomponentlist ? outRec.define("numcl", counter+1) : outRec.define("numft", counter+1); 
  
    //for (uInt k=0; k < validfieldids.nelements();  ++k){
    //  newTab.rwKeywordSet().define("definedmodel_field_"+String::toString(validfieldids[k]), elkey);
      
    // }
    iscomponentlist ? outRec.defineRecord("cl_"+String::toString(counter), modrec):
      outRec.defineRecord("ft_"+String::toString(counter), modrec);
    //////////////////;
    //for (uInt k=0; k < newTab.rwKeywordSet().nfields() ; ++k){
    //  cerr << "keys " << k << "  is  " << newTab.rwKeywordSet().name(k) << " type " << newTab.rwKeywordSet().dataType(k) << endl;
    //}
    ////////////////////////
    //if image for a given key is on disk and not incrementing ...lets remove it
    if(!incremental) 
      deleteDiskImage(newTab, elkey);
    putModelRecord(validfieldids, outRec, newTab);  
    //if(newTab.rwKeywordSet().isDefined(elkey))
    //	newTab.rwKeywordSet().removeField(elkey);
    //newTab.rwKeywordSet().defineRecord(elkey, outRec);
    
    // tim.show("Time taken to save record ");
 
      /*
      String subName=newTab.tableName()+"/"+elkey;
      if(Table::isReadable(subName) && !Table::canDeleteTable(subName, true))
      	throw(AipsError("Cannot save model into MS"));
      else if ((Table::isReadable(subName) && Table::canDeleteTable(subName, true)))
	Table::deleteTable(subName, true);
      TableDesc td1 ("td1", TableDesc::New);
      //td1.addColumn (ArrayColumnDesc<Int> ("SPECTRAL_WINDOW_ID"));
      //td1.addColumn(ScalarColumnDesc<TableRecord>("MODEL"));
      SetupNewTable newtab1 (subName, td1, Table::New);
      Table tab1 (newtab1);
      newTab.rwKeywordSet().defineTable(elkey, tab1);
      tab1.rwKeywordSet().defineRecord(elkey, outRec);
      */
      //ArrayColumn<Int> spwCol(tab1, "SPECTRAL_WINDOW_ID");
      //ScalarColumn<TableRecord> modCol(tab1, "MODEL");
      //newTab.addRow(1,false);
      //spwCol.put(0,spws);
      //modCol.put(0,outRec);
      //newTab.flush();
    // MSSource& mss=newTab.source();
    //  cerr << "has model_source " << mss.isColumn(MSSource::SOURCE_MODEL) << endl;
    //  if(!mss.isColumn(MSSource::SOURCE_MODEL) ){
    //mss.addColumn(ScalarRecordColumnDesc("SOURCE_MODEL"), true);
    //  }
    //  MSSourceColumns srcCol(mss);
    //  srcCol.sourceModel().put(0, outRec);
    newTab.unlock();
    if(Table::isReadable(newTab.sourceTableName())){
      newTab.source().unlock();   
    }
  }
  catch(...){
    logio << "Could not save virtual model data column due to an artificial virtual model size limit. \nYou may need to use the scratch column if you need model visibilities" << LogIO::WARN << LogIO::POST ;
   const_cast<MeasurementSet& >(thems).unlock(); 
  }

}
  void VisModelData::putModel(const MeasurementSet& thems,const RecordInterface& rec, const Matrix<Int>& indexComb, const Matrix<Int>& chanSel,Bool iscomponentlist, Bool incremental){
    LogIO logio;
    
    try{
      //A field can have multiple FTmachines and ComponentList associated with it 
      //For example having many flanking images for the model
      //For componentlist it may have multiple componentlist ...for different spw
      //Timer tim;
      //tim.mark();
      Int counter=0;
      Record modrec;
      modrec.define("version", Int(2));
      Vector<Int> validfieldids;
      validfieldids=indexComb.column(0);
      Int nfields=GenSort<Int>::sort (validfieldids, Sort::Ascending, 
				      Sort::QuickSort|Sort::NoDuplicates);
      validfieldids.resize(nfields, True);
      modrec.define("indexcombination", indexComb);
      //cerr << "modrec after indexcombination " << modrec << endl;
      modrec.define("channelselection", chanSel);
      modrec.defineRecord("container", rec);
      String elkey="model";
      for (uInt k=0; k < validfieldids.nelements();  ++k){
	elkey=elkey+"_"+String::toString(validfieldids[k]);
      }
      TableRecord outRec; 
      Bool addtorec=false;
      MeasurementSet& newTab=const_cast<MeasurementSet& >(thems);
      newTab.lock(True);
      if(Table::isReadable(newTab.sourceTableName())){
        newTab.source().lock(True);   
      }
      //cerr << elkey << " incr " << incremental << endl;
      if(isModelDefined(elkey, newTab)){ 
	getModelRecord(elkey, outRec, thems);
	//if incremental no need to check & remove what is in the record
	if(!incremental)
	  addtorec=addToRec(outRec, indexComb);
	//cerr << "addToRec " << addtorec << endl;
      }
      else{
	///////even if it is not defined some other field model might be sitting on that
	//////model key
	Int hasSourceRecord=firstSourceRowRecord(validfieldids[0], thems, outRec);
	if(hasSourceRecord > -1 && outRec.nfields() > 0)
	  addtorec=true;
	//cerr << "has Source " << hasSourceRecord << " addToRec " << addtorec << endl;
	////
      }
      if(outRec.nfields() >0 && ((!outRec.isDefined("version")) || (outRec.asInt("version") !=2)))
	throw(AipsError("cannot combine with older version virtual model column;\n please clear it before proceeding"));
      outRec.define("version", Int(2));
      incremental=incremental || addtorec;
      if(iscomponentlist){
	modrec.define("type", "componentlist");
	if(outRec.isDefined("numcl"))
	  counter=incremental ? outRec.asInt("numcl") : 0;
	
      }
      else{
	modrec.define("type", "ftmachine");
	if(outRec.isDefined("numft"))
	  counter=incremental ? outRec.asInt("numft") : 0;
      }
      iscomponentlist ? outRec.define("numcl", counter+1) : outRec.define("numft", counter+1); 
      
      //for (uInt k=0; k < validfieldids.nelements();  ++k){
      //  newTab.rwKeywordSet().define("definedmodel_field_"+String::toString(validfieldids[k]), elkey);
      
      // }
      iscomponentlist ? outRec.defineRecord("cl_"+String::toString(counter), modrec):
	outRec.defineRecord("ft_"+String::toString(counter), modrec);
      //////////////////;
      //for (uInt k=0; k < newTab.rwKeywordSet().nfields() ; ++k){
      //  cerr << "keys " << k << "  is  " << newTab.rwKeywordSet().name(k) << " type " << newTab.rwKeywordSet().dataType(k) << endl;
      //}
      ////////////////////////
      //if image for a given key is on disk and not incrementing ...lets remove it
      if(!incremental) 
	deleteDiskImage(newTab, elkey);
      putModelRecord(validfieldids, outRec, newTab);  
      newTab.unlock();
      if(Table::isReadable(newTab.sourceTableName())){
        newTab.source().unlock();   
    }
    }
    catch(...){
      logio << "Could not save virtual model data for some reason \nYou may need clear the model and redo or  use the scratch column if you need model visibilities" << LogIO::WARN << LogIO::POST ;
      const_cast<MeasurementSet& >(thems).unlock(); 
    }
    
  }

  void VisModelData::modifyDiskImagePath(Record& rec, const VisBuffer2& vb){
  Record& ftmac= rec.rwSubRecord("container");
  if(ftmac.isDefined("diskimage")){
    String theDiskImage=ftmac.asString("diskimage");
    if(File(theDiskImage).exists())
      return;
    String subPathname[30];
    String sep = "/";
    uInt nw = split(theDiskImage, subPathname, 20, sep);
    String theposs=(subPathname[nw-1]);
    ///String msname=vb.msName(false);
    ///As the above does not work ..returns some bogus reference table
    String msname=(vb.ms()).antenna().tableName();
    msname.erase(msname.length()-8);
    for (uInt i=nw-2 ; i>0; --i){ 
      if(File(msname+"/"+theposs).exists()){
		theDiskImage=msname+"/"+theposs;
		ftmac.define("diskmage", theDiskImage);
		return;
      }
      theposs=subPathname[i]+"/"+theposs;
    }
  }
}



  void VisModelData::addModel(const RecordInterface& rec,  const Vector<Int>& /*msids*/, const VisBuffer2& vb){
    


    Int indexft=-1;
    if(rec.isDefined("numft")){
      Int numft=rec.asInt("numft");
      if(numft >0){
	for(Int ftk=0; ftk < numft; ++ftk){
	  Record ftrec(rec.asRecord("ft_"+String::toString(ftk)));
	  modifyDiskImagePath(ftrec, vb);
	  Vector<Int>fields;
	  Vector<Int> spws;
	  ftrec.get("fields", fields);
	  ftrec.get("spws", spws);
	  if(anyEQ(spws, vb.spectralWindows()(0))){
	    indexft=ftholder_p.nelements();
	    ftholder_p.resize(indexft+1, false, true);
	    ftholder_p[indexft].resize(1);
	    ftholder_p[indexft][0]=NEW_FT(ftrec.asRecord("container"));
	    if(!(ftholder_p[indexft][0]))
	      throw(AipsError("Unknown FTMachine saved in virtual model"));
	    ftholder_p[indexft][0]->initMaps(vb);
	    
	    for( uInt fi=0; fi < fields.nelements(); ++fi){
	      for(uInt spi=0; spi < spws.nelements(); ++spi){
		Int indx=-1;
		Int ftindx=-1;
		if(hasModel(vb.msId(), fields[fi], spws[spi]) && (ftindex_p(spws[spi], fields[fi], vb.msId()) >= 0 )){
		 
		  indx=ftindex_p(spws[spi], fields[fi], vb.msId());
		  ftindx=ftholder_p[indx].nelements();
		  Bool alreadyAdded=false;
		  for (Int kk=1; kk < ftindx; ++kk){
		    alreadyAdded= alreadyAdded || (ftholder_p[indexft][0]==ftholder_p[indx][kk]);
		  }
		  if(!alreadyAdded){
		    ftholder_p[indx].resize(ftindx+1, true);
		    ftholder_p[indx][ftindx]=ftholder_p[indexft][0];
		  }
		}
		else{
		  ftindex_p(spws[spi], fields[fi], vb.msId())=indexft;
		}
	      }
	    }
	  }
	  else{
	    if(hasModel(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0)) < 0)
	      ftindex_p(vb.spectralWindows()(0), vb.fieldId()(0), vb.msId())=-2;
	  }

	  
	}
      }	      
    }
    Int indexcl=-1;
    if(rec.isDefined("numcl")){
      Int numcl=rec.asInt("numcl");
      if(numcl >0){
	for(Int clk=0; clk < numcl; ++clk){
	  Vector<Int>fields;
	  Vector<Int> spws;
	  Record clrec(rec.asRecord("cl_"+String::toString(clk)));
	  clrec.get("fields", fields);
	  clrec.get("spws", spws);
	  if(anyEQ(spws, vb.spectralWindows()(0))){
	    indexcl=clholder_p.nelements();
	    clholder_p.resize(indexcl+1, false, true);
	    clholder_p[indexcl].resize(1);
	    clholder_p[indexcl][0]=new ComponentList();
	    String err;
	    if(!((clholder_p[indexcl][0])->fromRecord(err, clrec.asRecord("container"))))
	      throw(AipsError("Component model failed to load for field "+String::toString(fields)));
	    for( uInt fi=0; fi < fields.nelements(); ++fi){
	      for(uInt spi=0; spi < spws.nelements(); ++spi){
		Int indx=-1;
		Int clindx=-1;
		if(hasModel(vb.msId(), fields[fi], spws[spi]) && (clindex_p(spws[spi], fields[fi], vb.msId()) >= 0 )){
		  indx=clindex_p(spws[spi], fields[fi], vb.msId());
		  clindx=clholder_p[indx].nelements();
		  Bool alreadyAdded=false;
		  for (Int kk=1; kk < clindx; ++kk){
		    alreadyAdded= alreadyAdded || (clholder_p[indexcl][0]==clholder_p[indx][kk]);
		  } 
		  if(!alreadyAdded){
		    clholder_p[indx].resize(clindx+1, true);
		    clholder_p[indx][clindx]=clholder_p[indexcl][0];
		  }
		}
		else{
		  //		  cerr << "spws, field, msid " << spws[spi] << " " << fields[fi] << " " <<  vb.msId() << " index " << indexcl << endl;
		  clindex_p(spws[spi], fields[fi], vb.msId())=indexcl;
		}
	      }
	    }
	  }
	  else{
	    if(hasModel(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0)) < 0)
	      clindex_p(vb.spectralWindows()(0), vb.fieldId()(0), vb.msId())=-2;
	  }

	}
      }
    }


  }

  FTMachine* VisModelData::NEW_FT(const Record& ftrec){
    String name=ftrec.asString("name");
    if(name=="GridFT")
      return new GridFT(ftrec);
    //if(name=="rGridFT")
    //  return new rGridFT(ftrec);
    if(name=="WProjectFT")
      return new WProjectFT(ftrec);
    //if(name=="MultiTermFT")
    //  return new MultiTermFT(ftrec);
    if(name=="MosaicFT")
      return new MosaicFT(ftrec);
    if(name=="MosaicFTNew")
      return new MosaicFTNew(ftrec);
    if(name=="SetJyGridFT")
      return new SetJyGridFT(ftrec);
    if(name=="MultiTermFTNew")
      return new MultiTermFTNew(ftrec);
    //When the following have constructors from Record they should be uncommented
    //   if(name=="AWProjectFT")
    //  return new AWProjectFT(ftrec);
    //if(name=="AWProjectWBFT")
    //  return new  AWProjectWBFT(ftrec);
    //if(name=="MultiTermAWProjectWBFT")
    //  return new MultiTermAWProjectWBFT(ftrec);
    return NULL;
  }

  Int VisModelData::hasModel(Int msid, Int field, Int spw){

    IPosition oldcubeShape=ftindex_p.shape();
    if(oldcubeShape(0) <(spw+1) || oldcubeShape(1) < (field+1) || oldcubeShape(2) < (msid+1)){
      Cube<Int> newind(max((spw+1), oldcubeShape(0)), max((field+1),oldcubeShape(1)) , max((msid+1), oldcubeShape(2)));
      newind.set(-1);
      newind(IPosition(3, 0,0,0), (oldcubeShape-1))=ftindex_p;
      ftindex_p.assign(newind);
      newind.set(-1);
      newind(IPosition(3, 0,0,0), (oldcubeShape-1))=clindex_p;
      clindex_p.assign(newind);
    }

    if( (clindex_p(spw, field, msid) + ftindex_p(spw, field, msid)) < -2)
      return -2;
    else if( (clindex_p(spw, field, msid) ==-1)  &&  (ftindex_p(spw, field, msid) ==-1))
      return -1;
    return 1;


  }

   void VisModelData::initializeToVis(){
    

  }



  void VisModelData::getMatchingMachines(Vector<CountedPtr<FTMachine> >& ft, Vector<CountedPtr<ComponentList> >& cl, const vi::VisBuffer2& vb){
    if(isVersion2()){
      Vector<Vector<Int> > combindx;
      getUniqueIndicesComb(vb, combindx);
      if(combindx.nelements() != 1)
	throw(AipsError("Cannot deal with multiple intent per visbuffer "));
      std::vector<Int> indexInBuf=combindx[0].tovector();
      indexInBuf.push_back(vb.msId());
      auto itFTMap = ftindex2_p.find(indexInBuf);
      if(itFTMap != ftindex2_p.end() ) {
	if((itFTMap->second) < 0){
	  ft.resize(0);
	}
	else{
	  ft=ftholder_p[itFTMap->second];
		  }
      }
      auto itCLMap = clindex2_p.find(indexInBuf);
      if(itCLMap != clindex2_p.end() ) {
	if((itCLMap->second)< 0){
	  cl.resize(0);
	}
	else{
	  //cerr << "matching " << Vector<Int>(itCLMap->first) << "   " <<  itCLMap->second << endl;
			cl=clholder_p[itCLMap->second];
			//		cerr << "returned length  " << cl.nelements() << endl;
	}
      }
      ///Now let's deal with a key that has not been visited before
      if((itCLMap == clindex2_p.end()) && (itFTMap == ftindex2_p.end() )){
	//cerr << "no matching holder " <<  Vector<Int>(indexInBuf) << " num of cl " << clholder_p.nelements() << endl;
		  updateHolders(vb, indexInBuf);
		  getMatchingMachines(ft, cl, vb);
		  
      }
    }
    else{
      cl=getCL(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0));
      ft=getFT(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0));
    }
    return;
	  
  }
  void VisModelData::updateHolders(const vi::VisBuffer2& vb, const std::vector<Int>& indexInBuf){
	  Int fieldId=vb.fieldId()[0];
	  const MeasurementSet& thems= vb.ms();
	  Int snum=-1;
	  Bool hasmodkey=False;
	  String modelkey;
	  //cerr <<"IndexInBuf " << Vector<Int>(indexInBuf) << " nelem " << ftindex2_p.size() << "   " << clindex2_p.size() << endl; 
	  hasmodkey=isModelDefined(fieldId, thems, modelkey, snum);
	  if(!hasmodkey){
	    //cerr << "NOHAS KEY fieldId " << fieldId << endl;
		  clindex2_p[indexInBuf]=-1;
		  ftindex2_p[indexInBuf]=-1;
		  return;
	  }
	  //if we have already filled for this field
	 for (auto it=ftindex2_p.begin(); it != ftindex2_p.end(); ++it){
	    //  cerr << Vector<Int>(it->first) << "   val " << it->second << endl;
	   if((it->first)[0]==fieldId){
	     clindex2_p[indexInBuf]=-2;
	     ftindex2_p[indexInBuf]=-2;
	     return;
	   }
	     
	 } 
	  //We do have this key
	  TableRecord therec;
	  getModelRecord(modelkey, therec, thems);
	  if(!therec.isDefined("version") || (therec.asInt("version")!=2)){
		///Set something versioning so as it can go to do version 1 way 
		version_p=1;
		clindex2_p[indexInBuf]=-1;
		ftindex2_p[indexInBuf]=-1;
		return;
	  }
	  else{
	    version_p=2;
	    Int indexft=-1;
	    if(therec.isDefined("numft")){
	      Int numft=therec.asInt("numft");
	      if(numft >0){
		for(Int ftk=0; ftk < numft; ++ftk){
		  Record ftrec(therec.asRecord("ft_"+String::toString(ftk)));
		  modifyDiskImagePath(ftrec, vb);
		  const Matrix<Int>& ftcombind=ftrec.asArrayInt("indexcombination");
		  //cerr << " ftcombind " << ftcombind << endl;
		  indexft=ftholder_p.nelements();
		  ftholder_p.resize(indexft+1, false, true);
		  ftholder_p[indexft].resize(1);
		  ftholder_p[indexft][0]=NEW_FT(ftrec.asRecord("container"));
		  if(!( ftholder_p[indexft][0]))
		    throw(AipsError("Unsupported FTMachine found in virtual MODEL_DATA column")); 
		  ftholder_p[indexft][0]->initMaps(vb);
		  for(uInt row=0; row < ftcombind.nrow(); ++row){
		    std::vector<int> key=ftcombind.row(row).tovector();
		    key.push_back(int(vb.msId()));
		    
		    if(ftindex2_p.count(key) >0){
		      Int numftforkey=ftholder_p[ftindex2_p[key]].nelements();
		      Int indx=ftindex2_p[key];
		      Bool alreadyAdded=false;
		      for (Int kk=1; kk < numftforkey; ++kk){
			alreadyAdded= alreadyAdded || (ftholder_p[indexft][0]==ftholder_p[indx][kk]);
		      }
		      if(!alreadyAdded){
			ftholder_p[indx].resize(numftforkey+1, true);
			ftholder_p[indx][numftforkey]=ftholder_p[indexft][0];
		      }
		    }
		    else{
		      ftindex2_p[key]=indexft;
		    }
		  }
		  
		}
	      }
	    }
	    if(ftindex2_p.count(indexInBuf)==0)
	      ftindex2_p[indexInBuf]=-2;
	    Int indexcl=-1;
	    ////////////
	    //cerr <<"map " << endl;
	    //for (auto it=clindex2_p.begin(); it != clindex2_p.end(); ++it){
	    // cerr << Vector<Int>(it->first) << "   val " << it->second << endl;

	    //}
	    ///////////
	    if(therec.isDefined("numcl")){
		  Int numcl=therec.asInt("numcl");
		  //cerr << "numcl " << numcl << endl;
		  if(numcl >0){
		    for(Int clk=0; clk < numcl; ++clk){
	  
		      Record clrec(therec.asRecord("cl_"+String::toString(clk)));
		      const Matrix<Int>& clcombind=clrec.asArrayInt("indexcombination");
		      
		      
		      indexcl=clholder_p.nelements();
		      clholder_p.resize(indexcl+1, false, true);
		      clholder_p[indexcl].resize(1);
		      clholder_p[indexcl][0]=new ComponentList();
		      String err;
		      if(!((clholder_p[indexcl][0])->fromRecord(err, clrec.asRecord("container"))))
			throw(AipsError("Component model failed to load for field "+String::toString(clcombind.column(0))));
		      for(uInt row=0; row < clcombind.nrow(); ++row){
			std::vector<int> key=clcombind.row(row).tovector();
			
			key.push_back(int(vb.msId()));
			//cerr << "key " <<  Vector<Int>(key) << endl;
			if(clindex2_p.count(key) >0){
			  Int numclforkey=clholder_p[clindex2_p[key]].nelements();
			  Int indx=clindex2_p[key];
			  Bool alreadyAdded=false;
			  for (Int kk=1; kk < numclforkey; ++kk){
			    alreadyAdded= alreadyAdded || (clholder_p[indexcl][0]==clholder_p[indx][kk]);
			  }
			  if(!alreadyAdded){
			    clholder_p[indx].resize(numclforkey+1, true);
			    clholder_p[indx][numclforkey]=clholder_p[indexcl][0];
			  }
			}
			else{
			  clindex2_p[key]=indexcl;
			}
		      }
		
		    }
		  }
		}
		if(clindex2_p.count(indexInBuf)==0)
		  clindex2_p[indexInBuf]=-2;
	  }
	  //cerr <<"update holder " << clholder_p.nelements() << endl; 
	 
	  
  }
  void VisModelData::getUniqueIndicesComb(const vi::VisBuffer2& vb, Vector< Vector<Int> >& retval){
	   const Vector<Int>& state = vb.stateId();
	   const Vector<Int>& scan=vb.scan();
	   const Vector<Double>& t=vb.time();
	   const Vector<Int>& fldid=vb.fieldId();
	   const Vector<Int>& spwid=vb.spectralWindows();
	   Vector<uInt>  uniqIndx;
	   uInt nTimes=GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates);
	   Vector<Int> comb(4);
	   
	   for (uInt k=0; k < nTimes; ++k){
	     comb(0)=fldid(uniqIndx[k]);
	     comb(1)=spwid(uniqIndx[k]);
	     comb(2)=scan(uniqIndx[k]);
	     comb(3)=state(uniqIndx[k]);
	     Bool hasComb=False;
	     if(retval.nelements() >0){
	       for (uInt j=0; j < retval.nelements(); ++j){
		 if(allEQ(retval[j], comb))
		   hasComb=True;
	       }
				   
	     }
	     if(!hasComb){
	       retval.resize(retval.nelements()+1, True);
	       retval[retval.nelements()-1]=comb;
	     }
	   }
	   
  }

  void VisModelData::init(const VisBuffer2& vb){
    if(version_p < 1){
      updateHolders(vb, std::vector<Int>({vb.fieldId()(0), vb.spectralWindows()(0), vb.scan()(0), vb.stateId()(0), vb.msId()}));

    }

  }
  
  
  Bool VisModelData::getModelVis(VisBuffer2& vb){

    //Vector<CountedPtr<ComponentList> >cl=getCL(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0));
    //Vector<CountedPtr<FTMachine> > ft=getFT(vb.msId(), vb.fieldId()(0), vb.spectralWindows()(0));
    Vector<CountedPtr<ComponentList> >cl(0);
    Vector<CountedPtr<FTMachine> > ft(0);
    getMatchingMachines(ft, cl, vb);
    //Fill the buffer with 0.0; also prevents reading from disk if MODEL_DATA exists
    ///Oh boy this is really dangerous...
    //nCorr etc are public..who know who changed these values before reaching here.
    Cube<Complex> mod(vb.nCorrelations(), vb.nChannels(), vb.nRows(), Complex(0.0));
    vb.setVisCubeModel(mod);
    Bool incremental=false;
    //cerr << "CL nelements visbuff " << cl.nelements() << endl;
    if( cl.nelements()>0){
      //     cerr << "In cft " << cl.nelements() << endl;
      for (uInt k=0; k < cl.nelements(); ++k)
	if(!cl[k].null()){
	  cft_p->get(vb, *(cl[k]), -1); 
	  //cerr << "max " << max(vb.visCubeModel()) << endl;
	  //cout << "max " << max(vb.visCubeModel()) << endl;
	  incremental=true;
	}
    }
    if(ft.nelements()>0){
      Cube<Complex> tmpModel;
      if(incremental || ft.nelements() >1)
	tmpModel.assign(vb.visCubeModel());
      Bool allnull=true;
      for (uInt k=0; k < ft.nelements(); ++k){
	if(!ft[k].null()){
	  if(k >0) vb.setVisCubeModel(Cube<Complex> (vb.nCorrelations(), vb.nChannels(), vb.nRows(), Complex(0.0)));
    
	  ft[k]->get(vb, -1);
	  if(ft.nelements()>1 || incremental){
	    tmpModel+=vb.visCubeModel();
	  }
	  allnull=false;
	}
      }
      //cerr << "min max after ft " << min(vb.visCubeModel()) << max(vb.visCubeModel()) << endl;
      if(!allnull){
	if(ft.nelements()>1 || incremental)
	  vb.setVisCubeModel(tmpModel);
	incremental=true;
      }      
    }
    if(!incremental){
      //No model was set so....
      ///Set the Model to 1.0 for parallel hand and 0.0 for x-hand
      
      vb.setVisCubeModel(Complex(1.0));
      Cube<Complex> modelCube(vb.visCubeModel());
      Vector<Stokes::StokesTypes> corrType=vb.getCorrelationTypesSelected();
      uInt nCorr = corrType.nelements();
      for (uInt i=0; i<nCorr; i++) {
	  if(corrType[i]==Stokes::RL || corrType[i]==Stokes::LR ||
	     corrType[i]==Stokes::XY || corrType[i]==Stokes::YX){
		  modelCube.yzPlane(i)=0.0;
	  }
      }
    }
    
    return true;
    
  }


  Vector<CountedPtr<ComponentList> > VisModelData::getCL(const Int msId, const Int fieldId, const Int spwId){
    if(!hasModel(msId, fieldId, spwId))
      return Vector<CountedPtr<ComponentList> >(0);
    Int indx=clindex_p(spwId, fieldId, msId);
    //  cerr << "indx " << indx << "   " << clholder_p[indx].nelements() <<  " spw " << spwId << " field " << fieldId << endl;
    if(indx <0)
      return Vector<CountedPtr<ComponentList> >(0);
    // cerr << "CL " << clholder_p[indx][0]->summarize(0) << endl;
    return clholder_p[indx];
	

  }

  Vector<CountedPtr<FTMachine> >VisModelData::getFT(const Int msId, const Int fieldId, Int spwId){

    if(!hasModel(msId, fieldId, spwId))
      return Vector<CountedPtr<FTMachine> >(0);
    Int indx=ftindex_p(spwId, fieldId, msId);
    //cerr << "indx " << indx << endl;
    if(indx <0)
      return Vector<CountedPtr<FTMachine> >(0);
    return ftholder_p[indx];
  }

  Int VisModelData::firstSourceRowRecord(const Int field, const MeasurementSet& theMS, TableRecord& rec){
    Int row=-1;
    
    //Prefer the Source table first    
    MSFieldColumns fCol(theMS.field());
    if(Table::isReadable(theMS.sourceTableName()) && (theMS.source().nrow() > 0) &&  (!fCol.sourceId().isNull()) && (fCol.sourceId().get(field) != -1) ){
    
      const MSSource& mss=theMS.source();
      
      Int sid=fCol.sourceId().get(field);
      auto rows=MSSourceIndex(mss).getRowNumbersOfSourceID(sid);
      if(rows.nelements() > 0) 
	row=rows[0];
      const TableRecord& keywords=mss.keywordSet();
      Bool rowIsUsed=false;
      for (uInt n=0; n< keywords.nfields(); ++n){
	if(keywords.dataType(n) == TpInt){
	  if(row==keywords.asInt(n)){
	    rowIsUsed=true;
	  }
	}
      }
      //nobody is using that row ..any record there must be dangling
      if(!rowIsUsed)
	return -1;
      if(row >-1 && mss.isColumn(MSSource::SOURCE_MODEL)){
	ScalarColumn<TableRecord> scol(theMS.source(), "SOURCE_MODEL");
	scol.get(row, rec);
      }
    }
    return row;
  }


}// end namespace refim
}//# NAMESPACE CASA - END

