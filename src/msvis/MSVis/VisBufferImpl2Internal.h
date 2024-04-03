/*
 * VisBufferImpl2Internal.h
 *
 *  Created on: Aug 22, 2013
 *      Author: jjacobs
 */

#ifndef VISBUFFERIMPL2INTERNAL_H_
#define VISBUFFERIMPL2INTERNAL_H_

#include <msvis/MSVis/ViImplementation2.h>
#include <cassert>
#include <sstream>

namespace casa {

namespace ms {
    class Vbi2MsRow;
}

namespace vi {

//////////////////////////////////////////////////////////
//
// Auxiliary Classes are contained in the "vb" namespace.
//
// These include VbCacheItemBase, VbCacheItem, VisBufferCache
// and VisBufferState.




// Possible array shapes of data coming from the main table cells.

typedef enum {NoCheck, Nr, NfNr, NcNr, NcNfNr, Ns, NsNcNr, NsNcNfNr, NcNfNcatNr, I3Nr, N_ShapePatterns} ShapePattern;

class VisBufferCache;

class VbCacheItemBase {

    // Provides a common base class for all of the cached value classes.
    // This is required because the actualy value classes use a template
    // to capture the underlying value type.

    friend class VisBufferImpl2;

public:

    VbCacheItemBase (bool isMutable)
    : isKey_p (false),
      isMutable_p (isMutable),
      vbComponent_p (VisBufferComponent2::Unknown),
      vb_p (0) {}

    virtual ~VbCacheItemBase () {}

    virtual void appendRows (casacore::rownr_t nRowsToAdd, casacore::Bool truncate = false) = 0;
    virtual void clear (casacore::Bool clearStatusOnly = false) = 0;
    virtual void clearDirty () = 0;
    virtual void copyRowElement (casacore::Int sourceRow, casacore::Int destinationRow) = 0;
    virtual void fill () const = 0;
    VisBufferComponent2
    getComponent () const
    {
        return vbComponent_p;
    }
    virtual casacore::Bool isArray () const = 0;
    virtual casacore::Bool isDirty () const = 0;
    virtual casacore::Bool isPresent () const = 0;
    virtual casacore::Bool isShapeOk () const = 0;
    virtual void resize (casacore::Bool /*copyValues*/) {}
    virtual void setDirty () = 0;
    virtual casacore::String shapeErrorMessage () const = 0;

protected:

    virtual void copy (const VbCacheItemBase * other, casacore::Bool fetchIfNeeded) = 0;

    VisBufferImpl2 * getVb () const
    {
        return vb_p;
    }

    virtual void initialize (VisBufferCache * cache, VisBufferImpl2 * vb, VisBufferComponent2 component,
                             casacore::Bool isKey = true);

    casacore::Bool isKey () const { return isKey_p;}
    bool isMutable () const { return isMutable_p; }

    virtual void setAsPresent (casacore::Bool isPresent = true) const = 0;
    void setIsKey (casacore::Bool isKey)
    {
        isKey_p = isKey;
    }

private:

    casacore::Bool isKey_p;
    const bool isMutable_p;
    VisBufferComponent2 vbComponent_p;
    VisBufferImpl2 * vb_p; // [use]

};

typedef std::vector<VbCacheItemBase *> CacheRegistry;

template <typename T, casacore::Bool IsComputed = false>
class VbCacheItem : public VbCacheItemBase {

    friend class VisBufferImpl2;

public:

    typedef T DataType;
    typedef void (VisBufferImpl2::* Filler) (T &) const;

    VbCacheItem (bool isMutable = false)
    : VbCacheItemBase (isMutable), isPresent_p (false)
    {}

    virtual ~VbCacheItem () {}

    virtual void appendRows (casacore::rownr_t, casacore::Bool)
    {
        // Noop for scalars
    }

    virtual void
    clear (casacore::Bool clearStatusOnly)
    {
        if (! clearStatusOnly) {
            clearValue (item_p);
        }
        setAsPresent (false);
        clearDirty ();
    }

    virtual void
    clearDirty ()
    {
        isDirty_p = false;
    }

    virtual void copyRowElement (casacore::Int /*sourceRow*/, casacore::Int /*destinationRow*/) {} // noop


    virtual void
    fill () const
    {
        const VisBufferImpl2 * vb = getVb();

        ThrowIf (! vb->isAttached (),
                 casacore::String::format ("Can't fill VisBuffer component %s: Not attached to VisibilityIterator",
                                 VisBufferComponents2::name (getComponent()).c_str()));

        ThrowIf (! IsComputed && ! vb->isFillable (),
                 casacore::String::format ("Cannot fill VisBuffer component %s: %s",
                                 VisBufferComponents2::name (getComponent()).c_str(),
                                 vb->getFillErrorMessage ().c_str()));

        (vb ->* filler_p) (item_p);
    }

    const T &
    get () const
    {
        if (! isPresent()){
            fill ();
            setAsPresent ();
            isDirty_p = false;
        }

        return item_p;
    }

    T &
    getRef (casacore::Bool fillIfAbsent = true)
    {
        if (! isPresent() && fillIfAbsent){
            fill ();
        }
        setAsPresent();

        // Caller is getting a modifiabled reference to the
        // datum (otherwise they would use "get"): assume
        // that it will be used to modify the datum and mark
        // it as dirty.

        isDirty_p = true;

        return item_p;
    }

    void
    initialize (VisBufferCache * cache, VisBufferImpl2 * vb, Filler filler,
                VisBufferComponent2 component = VisBufferComponent2::Unknown,
                casacore::Bool isKey = true)
    {
        VbCacheItemBase::initialize (cache, vb, component, isKey);
        filler_p = filler;
    }

    casacore::Bool isArray () const
    {
        return false;
    }

    casacore::Bool
    isDirty () const
    {
        return isDirty_p;
    }

    casacore::Bool
    isPresent () const
    {
        return isPresent_p;
    }

    virtual casacore::Bool
    isShapeOk () const
    {
        return true;
    }

    virtual void
    set (const T & newItem)
    {
        ThrowIf (! isMutable () && ! getVb()->isWritable (), "This VisBuffer is readonly");

        ThrowIf (isKey() && ! getVb()->isRekeyable (),
                 "This VisBuffer is does not allow row key values to be changed.");

        // Set operations to a rekeyable VB are allowed to change the shapes of the
        // values.  When T derives from casacore::Array, the assign method will use casacore::Array::assign
        // which resizes the destination value to match the source value.  For nonkeyable
        // VBs, the normal operator= method is used which for Arrays will throw an
        // exception when a shape incompatibility exists between the source and destination.

        if (isKey ()){
            assign (item_p, newItem);
        }
        else{
            item_p = newItem;
        }

        ThrowIf (! isShapeOk (), shapeErrorMessage() );

        setAsPresent();
        isDirty_p = true;
    }


    template <typename U>
    void
    set (const U & newItem)
    {
        ThrowIf (! isMutable () && ! getVb()->isWritable (), "This VisBuffer is readonly");

        ThrowIf (isKey () && ! getVb()->isRekeyable (),
                 "This VisBuffer is does not allow row key values to be changed.");

        item_p = newItem;

        ThrowIf (! isShapeOk (), shapeErrorMessage() );

        setAsPresent();
        isDirty_p = true;
    }

    template <typename U>
    void
    setSpecial (const U & newItem)
    {
        // For internal use for items which aren't really demand-fetched

        item_p = newItem;
        setAsPresent();
        isDirty_p = false;
    }

    virtual void
    setDirty ()
    {
        isDirty_p = true;
    }

    virtual casacore::String
    shapeErrorMessage () const
    {
        ThrowIf (true, "Scalar shapes should not have shape errors.");

        return casacore::String();
    }

protected:

    void
    assign (T & lhs, const T & rhs)
    {
        lhs = rhs;
    }

    template <typename E>
    static void clearValue (casacore::Array <E> & value){
        value.resize();
    }

    static void clearValue (casacore::Int & value){
        value = 0;
    }

    static void clearValue (casacore::MDirection & value){
        value = casacore::MDirection ();
    }


//    virtual void
//    copy (const VbCacheItemBase * otherRaw, casacore::Bool markAsCached)
//    {
//        // Convert generic pointer to one pointint to this
//        // cache item type.
//
//        const VbCacheItem * other = dynamic_cast <const VbCacheItem *> (otherRaw);
//        Assert (other != 0);
//
//        // Capture the cached status of the other item
//
//        isPresent_p = other->isPresent_p;
//
//        // If the other item was cached then copy it over
//        // otherwise clear out this item.
//
//        if (isPresent_p){
//            item_p = other->item_p;
//        }
//        else {
//            item_p = T ();
//
//            if (markAsCached){
//                isPresent_p = true;
//            }
//        }
//    }

    virtual void
    copy (const VbCacheItemBase * otherRaw, casacore::Bool fetchIfNeeded)
    {
        const VbCacheItem<T, IsComputed> * other =
            dynamic_cast <const VbCacheItem<T, IsComputed> *> (otherRaw);
        copyAux (other, fetchIfNeeded);
    }

    void
    copyAux (const VbCacheItem<T, IsComputed> * other, bool fetchIfNeeded)
    {
        if (other->isPresent()){

            item_p = other->item_p;
            setAsPresent ();
            isDirty_p = false;
        }
        else if (fetchIfNeeded){
            set (other->get());
        }
        else {

            setAsPresent (false);
            isDirty_p = false;
        }
    }

    T &
    getItem () const
    {
        return item_p;
    }

    void
    setAsPresent (casacore::Bool isPresent = true) const
    {
        isPresent_p = isPresent;
    }

private:

    Filler       filler_p;
    mutable casacore::Bool isDirty_p;
    mutable casacore::Bool isPresent_p;
    mutable T    item_p;
};

template <typename T, casacore::Bool IsComputed = false>
class VbCacheItemArray : public VbCacheItem<T, IsComputed> {
public:

    typedef typename VbCacheItem<T>::Filler Filler;
    typedef typename T::IteratorSTL::value_type ElementType;

    VbCacheItemArray(bool isMutable = false)
    : VbCacheItem<T, IsComputed> (isMutable), capacity_p (0), shapePattern_p (NoCheck) {}
    virtual ~VbCacheItemArray () {}

    virtual void appendRows (casacore::rownr_t nRows, casacore::Bool truncate)
    {

        // Only used when time averaging

        casacore::IPosition shape = this->getItem().shape();
        casacore::Int nDims = shape.size();

        if (nDims == 0 || shapePattern_p == NoCheck){
            // This item is empty or unfillable so leave it alone.
        }
        else if (truncate){

            // Make any excess rows disappear with a little hack to
            // avoid a copy.  This leaves the storage unchanged and merely
            // changes the associated bookkeeping values.

            AssertCc ((ssize_t)nRows <= shape.last());

            shape.last() = nRows;

            this->getItem().adjustLastAxis (shape);

        }
        else{

            // The array needs to resized to hold nRows worth of data.  If the
            // shape of the existing array is the same as the existing one ignoring
            // the number of rows then we expect the array

            this->setAsPresent(); // This VB is being filled manually
            casacore::IPosition desiredShape = this->getVb()->getValidShape (shapePattern_p);
            casacore::IPosition currentShape = getShape();

            // Determine if the existing shape is the same as the desired shape
            // ignoring rows.  If is the same, then the existing data will need
            // to be copied in the event that the array needs to be resized
            // (i.e., reallocated).

            casacore::Bool shapeOk = true; // will ignore last dimension
            for (casacore::uInt i = 0; i < currentShape.nelements() - 1; i++){
                shapeOk = shapeOk && desiredShape [i] == currentShape [i];
            }

            desiredShape.last() = nRows;

	    if (shapeOk){

	      // Only the number of rows differs from the current shape.  
	      // This call will preserve any existing data.

	      this->getItem().adjustLastAxis (desiredShape, 20);
	    } 
	    else {

	      // Since the core shape is changing, the existing data is
              // not useful; this call will not preserve it.

	      this->getItem().reformOrResize (desiredShape);
	    }
        }
    }

    virtual void copyRowElement (casacore::Int sourceRow, casacore::Int destinationRow)
    {
        copyRowElementAux (this->getItem(), sourceRow, destinationRow);
    }

    virtual casacore::IPosition getShape() const
    {
        return this->getItem().shape();
    }


    void
    initialize (VisBufferCache * cache,
                VisBufferImpl2 * vb,
                Filler filler,
                VisBufferComponent2 component,
                ShapePattern shapePattern,
                casacore::Bool isKey)
    {
        VbCacheItem<T, IsComputed>::initialize (cache, vb, filler, component, isKey);
        shapePattern_p = shapePattern;
    }


    virtual casacore::Bool
    isShapeOk () const
    {
        // Check to see if the shape of this data item is consistent
        // with the expected shape.

        casacore::Bool result = shapePattern_p == NoCheck ||
                      this->getItem().shape() == this->getVb()->getValidShape (shapePattern_p);

        return result;
    }


    casacore::Bool isArray () const
    {
        return true;
    }

    void
    resize (casacore::Bool copyValues)
    {
        if (shapePattern_p != NoCheck){

            casacore::IPosition desiredShape = this->getVb()->getValidShape (shapePattern_p);

            this->getItem().resize (desiredShape, copyValues);
            capacity_p = desiredShape.last();

            if (! copyValues){
                //this->getItem() = typename T::value_type();
                std::fill(this->getItem().begin( ),this->getItem().end( ),typename T::value_type());
            }

        }
    }

    virtual void
    set (const T & newItem)
    {
        ThrowIf (! this->isMutable() && ! this->getVb()->isWritable (), "This VisBuffer is readonly");

        ThrowIf (this->isKey() && ! this->getVb()->isRekeyable (),
                 "This VisBuffer is does not allow row key values to be changed.");

        // Now check for a conformant shape.

        casacore::IPosition itemShape = newItem.shape();
        casacore::Bool parameterShapeOk = shapePattern_p == NoCheck ||
                                itemShape == this->getVb()->getValidShape (shapePattern_p);
        ThrowIf (! parameterShapeOk,
                 "Invalid parameter shape:: " + shapeErrorMessage (& itemShape));

        VbCacheItem<T,IsComputed>::set (newItem);
    }

    template <typename U>
    void
    set (const U & newItem)
    {
        if (! this->isPresent()){ // Not present so give it a shape
            set (T (this->getVb()->getValidShape (shapePattern_p)));
        }

        VbCacheItem<T,IsComputed>::set (newItem);
    }

    virtual casacore::String
    shapeErrorMessage (const casacore::IPosition * badShape = 0) const
    {

        ThrowIf (shapePattern_p == NoCheck,
                 "No shape error message for NoCheck type array");

        ThrowIf (isShapeOk () && badShape == 0,
                 "Shape is OK so no error message.");

        casacore::String badShapeString = (badShape != 0) ? badShape->toString()
                                                : this->getItem().shape().toString();

        std::ostringstream os;

        os << "VisBuffer::ShapeError: "
           << VisBufferComponents2::name (this->getComponent())
           << " should have shape "
           << this->getVb()->getValidShape(shapePattern_p).toString()
           << " but had shape "
           << badShapeString;

        return os.str();
    }

protected:

    void
    assign (T & dst, const T & src)
    {
        dst.assign (src);
    }

    static void
    copyRowElementAux (casacore::Cube<typename T::value_type> & cube, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        casacore::IPosition shape = cube.shape();
        casacore::Int nI = shape(1);
        casacore::Int nJ = shape(0);

        for (casacore::Int i = 0; i < nI; i++){
            for (casacore::Int j = 0; j < nJ; j++){
                cube (j, i, destinationRow) = cube (j, i, sourceRow);
            }
        }
    }

    static void
    copyRowElementAux (casacore::Matrix<typename T::value_type> & matrix, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        casacore::IPosition shape = matrix.shape();
        casacore::Int nJ = shape(0);

        for (casacore::Int j = 0; j < nJ; j++){
            matrix (j, destinationRow) = matrix (j, sourceRow);
        }
    }

    static void
    copyRowElementAux (casacore::Array<typename T::value_type> & array, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        casacore::IPosition shape = array.shape();
        AssertCc (shape.nelements() == 4);

        casacore::Int nH = shape(2);
        casacore::Int nI = shape(1);
        casacore::Int nJ = shape(0);

        for (casacore::Int h = 0; h < nH; h++){
            for (casacore::Int i = 0; i < nI; i++){
                for (casacore::Int j = 0; j < nJ; j++){
                    array (casacore::IPosition (4, j, i, h, destinationRow)) =
                        array (casacore::IPosition (4, j, i, h, sourceRow));
                }
            }
        }
    }

    static void
    copyRowElementAux (casacore::Vector<typename T::value_type> & vector, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        vector (destinationRow) = vector (sourceRow);
    }

private:

    casacore::Int capacity_p;
    ShapePattern shapePattern_p;
};

template <typename T, casacore::Bool IsComputed = false>
class VbCacheItemVectorInt : public VbCacheItem<T, IsComputed> {
public:

    typedef typename VbCacheItem<T>::Filler Filler;
    typedef typename T::value_type ElementType;

    VbCacheItemVectorInt(bool isMutable = false)
    : VbCacheItem<T, IsComputed> (isMutable), shapePattern_p (NoCheck) {}
    virtual ~VbCacheItemVectorInt () {}

    virtual void appendRows (casacore::Int nRows, casacore::Bool truncate) {(void)nRows;(void)truncate;} // no-op

    virtual void copyRowElement (casacore::Int sourceRow, casacore::Int destinationRow) {(void)sourceRow;(void)destinationRow;} //no-op

    void
    initialize (VisBufferCache * cache,
                VisBufferImpl2 * vb,
                Filler filler,
                VisBufferComponent2 component,
                ShapePattern shapePattern,
                casacore::Bool isKey) //DONE
    {
        VbCacheItem<T, IsComputed>::initialize (cache, vb, filler, component, isKey);
        shapePattern_p = shapePattern;
    }


    virtual casacore::Bool
    isShapeOk () const //DONE
    {
        // Check to see if the shape of this data item is consistent
        // with the expected shape.

        casacore::Bool result = shapePattern_p == NoCheck;
        result = result || this->getItem().size() ==
            this->getVb()->getValidVectorShapes(shapePattern_p).size();
        return result;
    }


    casacore::Bool isArray () const
    {
        return false;
    }

    void
    resize (casacore::Bool copyValues) //DONE
    {
        if (shapePattern_p != NoCheck){

            auto desiredShape = this->getVb()->getValidVectorShapes (shapePattern_p);

            this->getItem().resize(desiredShape.size(), copyValues);

            if (! copyValues){
                this->getItem() = typename T::value_type();
            }

        }
    }

    virtual void
    set (const T & newItem) //DONE
    {
        ThrowIf (! this->isMutable() && ! this->getVb()->isWritable (), "This VisBuffer is readonly");

        ThrowIf (this->isKey() && ! this->getVb()->isRekeyable (),
                 "This VisBuffer is does not allow row key values to be changed.");

        // Now check for a conformant shape.
        if(shapePattern_p != NoCheck)
            ThrowIf (this->getItem().size() != newItem.size(),
                    "Invalid parameter shape:: " + sizeErrorMessage (newItem.size()));

        VbCacheItem<T,IsComputed>::set (newItem);
    }

    template <typename U>
    void
    set (const U & newItem) //DONE
    {
        if (! this->isPresent()){ // Not present so give it a shape
            auto desiredShape = this->getVb()->getValidVectorShapes (shapePattern_p);
            T item(desiredShape.size());
            set (T (item));
        }

        VbCacheItem<T,IsComputed>::set (newItem);
    }

protected:


    virtual casacore::String
    sizeErrorMessage (casacore::Int badSize) const
    {
        std::ostringstream os;

        os << "VisBuffer::ShapeError: "
           << VisBufferComponents2::name (this->getComponent())
           << " should have shape "
           << this->getVb()->getValidVectorShapes(shapePattern_p).size()
           << " but had shape "
           << badSize;

        return os.str();
    }

    void
    assign (T & dst, const T & src) //DONE
    {
        dst.assign (src);
    }

private:

    ShapePattern shapePattern_p;
};

template <typename T, casacore::Bool IsComputed = false>
class VbCacheItemVectorArray : public VbCacheItem<T, IsComputed> {
public:

    typedef typename VbCacheItem<T>::Filler Filler;
    typedef typename T::value_type::value_type ElementType;

    VbCacheItemVectorArray(bool isMutable = false)
    : VbCacheItem<T, IsComputed> (isMutable), shapePattern_p (NoCheck) {}
    virtual ~VbCacheItemVectorArray () {}

    virtual void appendRows (casacore::Int nRows, casacore::Bool truncate)
    {
        casacore::IPosition shape = this->getItem().shape();
        (void)truncate;
        // Only used when time averaging
        if (shapePattern_p == NoCheck){
            // This item is empty or unfillable so leave it alone.
        }
        else
        {
            resize(true);
            resizeRows(nRows);
        }
    }

    virtual void copyRowElement (casacore::Int sourceRow, casacore::Int destinationRow)
    {
        copyRowElementAux (this->getItem(), sourceRow, destinationRow);
    }

    void
    initialize (VisBufferCache * cache,
                VisBufferImpl2 * vb,
                Filler filler,
                VisBufferComponent2 component,
                ShapePattern shapePattern,
                casacore::Bool isKey)
    {
        VbCacheItem<T, IsComputed>::initialize (cache, vb, filler, component, isKey);
        shapePattern_p = shapePattern;
    }


    virtual casacore::Bool
    isShapeOk () const
    {
        // Check to see if the shape of this data item is consistent
        // with the expected shape.

        casacore::Bool result = shapePattern_p == NoCheck;
        result = result || this->getItem().size() ==
            this->getVb()->getValidVectorShapes(shapePattern_p).size();
        if (result)
        {
            for (size_t iElem = 0 ; iElem < this->getItem().size(); ++iElem)
                result = result ||
                    this->getItem()[iElem].shape() == this->getVb()->getValidVectorShapes (shapePattern_p)[iElem];
        }
        return result;
    }


    casacore::Bool isArray () const
    {
        return true;
    }

    void
    resize (casacore::Bool copyValues)
    {
        if (shapePattern_p != NoCheck){

            auto desiredShape = this->getVb()->getValidVectorShapes (shapePattern_p);

            this->getItem().resize(desiredShape.size(), copyValues);
            for (size_t iElem = 0 ; iElem < this->getItem().size(); ++iElem)
                this->getItem()[iElem].resize(desiredShape[iElem], copyValues);

            if (! copyValues){
                for (size_t iElem = 0 ; iElem < this->getItem().size(); ++iElem)
                    this->getItem()[iElem] = typename T::value_type::value_type();
            }

        }
    }

    void
    resizeRows (casacore::Int newNRows)
    {
        if (shapePattern_p != NoCheck){
            // This will count the rows of each array and as soon as the number of rows exceeds
            // the requested one it will resize (shrink) that array. If the total number of rows is
            // less than the requested one, then the last array is resized (expanded).
            int nCumRows=0;
            size_t iElem;
            for (iElem = 0 ; iElem < this->getItem().size(); ++iElem)
            {
                auto & thisShape = this->getItem()[iElem].shape();
                nCumRows += thisShape.last();
                if (nCumRows > newNRows)
                {
                    casacore::IPosition newShape = thisShape;
                    newShape.last() = newNRows - (nCumRows - thisShape.last());
                    this->getItem()[iElem].resize (newShape, true);
                    break;
                }
            }
            if (nCumRows < newNRows)
            {
                casacore::IPosition lastArrayShape = this->getItem()[this->getItem().size()-1].shape();
                casacore::IPosition newShape = lastArrayShape;
                newShape.last() = newNRows - nCumRows;
                this->getItem()[this->getItem().size()-1].resize (newShape, true);
            }
            else
                this->getItem().resize(iElem, true);

            this->setDirty();
        }
    }

    virtual void
    set (const T & newItem)
    {
        ThrowIf (! this->isMutable() && ! this->getVb()->isWritable (), "This VisBuffer is readonly");

        ThrowIf (this->isKey() && ! this->getVb()->isRekeyable (),
                 "This VisBuffer is does not allow row key values to be changed.");

        // Now check for a conformant shape.
        if(shapePattern_p != NoCheck)
        {
            ThrowIf (this->getItem().size() != newItem.size(),
                    "Invalid parameter shape:: " + sizeErrorMessage (newItem.size()));
            for (size_t iElem = 0 ; iElem < this->getItem().size(); ++iElem)
                ThrowIf (this->getItem()[iElem].shape() != newItem[iElem].shape(),
                        "Invalid parameter shape:: " + shapeErrorMessage (iElem, &(newItem[iElem].shape())));
        }

        VbCacheItem<T,IsComputed>::set (newItem);
    }

    template <typename U>
    void
    set (const U & newItem)
    {
        if (! this->isPresent()){ // Not present so give it a shape
            auto desiredShape = this->getVb()->getValidVectorShape (shapePattern_p);
            T item(desiredShape.size());
            for (size_t iElem = 0 ; iElem < item.size(); ++iElem)
                item[iElem].resize(desiredShape[iElem]);

            set (T (item));
        }

        VbCacheItem<T,IsComputed>::set (newItem);
    }

protected:

    virtual casacore::String
    shapeErrorMessage (casacore::Int i, const casacore::IPosition * badShape = 0) const
    {

        ThrowIf (shapePattern_p == NoCheck,
                 "No shape error message for NoCheck type array");

        ThrowIf (isShapeOk () && badShape == 0,
                "Shape is OK so no error message.");

        casacore::String badShapeString = (badShape != 0) ? badShape->toString()
                                                : this->getItem()[i].shape().toString();

        std::ostringstream os;

        os << "VisBuffer::ShapeError: "
           << VisBufferComponents2::name (this->getComponent())
           << " should have shape "
           << this->getVb()->getValidVectorShapes(shapePattern_p)[i].toString()
           << " but had shape "
           << badShapeString;

        return os.str();
    }

    virtual casacore::String
    sizeErrorMessage (casacore::Int badSize) const
    {
        std::ostringstream os;

        os << "VisBuffer::ShapeError: "
           << VisBufferComponents2::name (this->getComponent())
           << " should have shape "
           << this->getVb()->getValidVectorShapes(shapePattern_p).size()
           << " but had shape "
           << badSize;

        return os.str();
    }

    static void
    copyRowElementAux (casacore::Vector<casacore::Cube<typename T::value_type::value_type>> & cubeVector, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        int nCumRows=0, sourceCube =-1, destinationCube = -1;
        int sourceCubeRow = 0, destinationCubeRow = 0;
        for (size_t iCube = 0 ; iCube < cubeVector.size(); ++iCube)
        {
            auto & thisShape = cubeVector[iCube].shape();
            nCumRows += thisShape.last();
            if (nCumRows < sourceCube)
            {
                sourceCube = iCube;
                sourceCubeRow = sourceRow - nCumRows;
            }
            if (nCumRows < destinationCube)
            {
                destinationCube = iCube;
                destinationCubeRow = destinationRow - nCumRows;
            }
        }

        ThrowIf (sourceCube != destinationCube || sourceCube == -1 || destinationCube == -1,
                 "Copying rows between incompatible cubes is not allowed");

        casacore::IPosition shape = cubeVector[sourceCube].shape();
        casacore::Int nI = shape(1);
        casacore::Int nJ = shape(0);

        for (casacore::Int i = 0; i < nI; i++){
            for (casacore::Int j = 0; j < nJ; j++){
                cubeVector[destinationCube] (j, i, destinationCubeRow) = cubeVector[sourceCube] (j, i, sourceCubeRow);
            }
        }
    }

    static void
    copyRowElementAux (casacore::Vector<casacore::Matrix<typename T::value_type::value_type>> & matrixVector, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        int nCumRows=0, sourceMatrix =-1, destinationMatrix = -1;
        casacore::rownr_t sourceMatrixRow = 0, destinationMatrixRow = 0;
        for (casacore::rownr_t iMatrix = 0 ; iMatrix < matrixVector.size(); ++iMatrix)
        {
            auto & thisShape = matrixVector[iMatrix].shape();
            nCumRows += thisShape.last();
            if (nCumRows < sourceMatrix)
            {
                sourceMatrix = iMatrix;
                sourceMatrixRow = sourceRow - nCumRows;
            }
            if (nCumRows < destinationMatrix)
            {
                destinationMatrix = iMatrix;
                destinationMatrixRow = destinationRow - nCumRows;
            }
        }

        ThrowIf (sourceMatrix != destinationMatrix || sourceMatrix == -1 || destinationMatrix == -1,
                 "Copying rows between incompatible matrices is not allowed");

        casacore::IPosition shape = matrixVector[sourceMatrix].shape();
        casacore::Int nJ = shape(0);

        for (casacore::Int j = 0; j < nJ; j++){
            matrixVector[destinationMatrix] (j, destinationMatrixRow) = matrixVector[sourceMatrix] (j, sourceMatrixRow);
        }
    }
   
    static void
    copyRowElementAux (casacore::Vector<casacore::Vector<typename T::value_type::value_type>> & vectorVector, casacore::Int sourceRow, casacore::Int destinationRow)
    {
        int nCumRows=0, sourceVector =-1, destinationVector = -1;
        int sourceVectorRow, destinationVectorRow;
        for (size_t iVector = 0 ; iVector < vectorVector.size(); ++iVector)
        {
            auto & thisShape = vectorVector[iVector].shape();
            nCumRows += thisShape.last();
            if (nCumRows < sourceVector)
            {
                sourceVector = iVector;
                sourceVectorRow = sourceRow - nCumRows;
            }
            if (nCumRows < destinationVector)
            {
                destinationVector = iVector;
                destinationVectorRow = destinationRow - nCumRows;
            }
        }

        ThrowIf (sourceVector != destinationVector || sourceVector == -1 || destinationVector == -1,
                 "Copying rows between incompatible matrices is not allowed");

        vectorVector[destinationVector] (destinationVectorRow) = vectorVector[sourceVector] (sourceVectorRow);
    }


private:

    ShapePattern shapePattern_p;
};




class VisBufferCache {

    // Holds the cached values for a VisBuffer object.

public:

    VisBufferCache (VisBufferImpl2 * vb);

    void appendComplete ();
    casacore::Int appendRow ();
    void initialize (VisBufferImpl2 * vb);
    void registerItem (VbCacheItemBase * item);

    // The values that are potentially cached.

    VbCacheItemArray <casacore::Vector<casacore::Int> > antenna1_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > antenna2_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > arrayId_p;
    VbCacheItemArray <casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >, true> cjones_p;
    VbCacheItemArray <casacore::Cube<casacore::Complex> > correctedVisCube_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Complex>>> correctedVisCubes_p;
//    VbCacheItemArray <casacore::Matrix<CStokesVector> > correctedVisibility_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > corrType_p;
    VbCacheItem <casacore::Int> dataDescriptionId_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > dataDescriptionIds_p;
    VbCacheItemArray <casacore::Vector<casacore::MDirection> > direction1_p; //where the first antenna/feed is pointed to
    VbCacheItemArray <casacore::Vector<casacore::MDirection> > direction2_p; //where the second antenna/feed is pointed to
    VbCacheItemArray <casacore::Vector<casacore::Double> > exposure_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > feed1_p;
    VbCacheItemArray <casacore::Vector<casacore::Float> > feed1Pa_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > feed2_p;
    VbCacheItemArray <casacore::Vector<casacore::Float> > feed2Pa_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > fieldId_p;
//    VbCacheItemArray <casacore::Matrix<casacore::Bool> > flag_p;
    VbCacheItemArray <casacore::Array<casacore::Bool> > flagCategory_p;
    VbCacheItemArray <casacore::Vector<casacore::Array<casacore::Bool>>> flagCategories_p;
    VbCacheItemArray <casacore::Cube<casacore::Bool> > flagCube_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Bool>>> flagCubes_p;
    VbCacheItemArray <casacore::Vector<casacore::Bool> > flagRow_p;
    VbCacheItemArray <casacore::Cube<casacore::Float> > floatDataCube_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Float>>> floatDataCubes_p;
    VbCacheItemArray <casacore::Matrix<casacore::Float> > imagingWeight_p;
    VbCacheItemArray <casacore::Cube<casacore::Complex> > modelVisCube_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Complex>>> modelVisCubes_p;
//    VbCacheItemArray <casacore::Matrix<CStokesVector> > modelVisibility_p;
    VbCacheItem <casacore::Int> nAntennas_p;
    VbCacheItem <casacore::Int> nChannels_p;
    VbCacheItem <casacore::Int> nCorrelations_p;
    VbCacheItem <casacore::Int> nRows_p;
    VbCacheItem <casacore::Int> nShapes_p;
    VbCacheItemVectorInt <casacore::Vector<casacore::rownr_t>> nRowsPerShape_p;
    VbCacheItemVectorInt <casacore::Vector<casacore::Int>> nChannelsPerShape_p;
    VbCacheItemVectorInt <casacore::Vector<casacore::Int>> nCorrelationsPerShape_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > observationId_p;
    VbCacheItem <casacore::MDirection> phaseCenter_p;
    VbCacheItem <casacore::Int> polFrame_p;
    VbCacheItem <casacore::Int> polarizationId_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > processorId_p;
    VbCacheItemArray <casacore::Vector<casacore::rownr_t> > rowIds_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > scan_p;
    VbCacheItemArray <casacore::Matrix<casacore::Float> > sigma_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Matrix<casacore::Float>>> sigmas_p;
    //VbCacheItemArray <casacore::Matrix<casacore::Float> > sigmaMat_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > spectralWindows_p;
    VbCacheItemArray <casacore::Vector<casacore::Int> > stateId_p;
    VbCacheItemArray <casacore::Vector<casacore::Double> > time_p;
    VbCacheItemArray <casacore::Vector<casacore::Double> > timeCentroid_p;
    VbCacheItemArray <casacore::Vector<casacore::Double> > timeInterval_p;
    VbCacheItemArray <casacore::Matrix<casacore::Double> > uvw_p;
    VbCacheItemArray <casacore::Cube<casacore::Complex> > visCube_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Complex>>> visCubes_p;
//    VbCacheItemArray <casacore::Matrix<CStokesVector> > visibility_p;
    VbCacheItemArray <casacore::Matrix<casacore::Float> > weight_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Matrix<casacore::Float>>> weights_p;
    //VbCacheItemArray <casacore::Matrix<casacore::Float> > weightMat_p;
    VbCacheItemArray <casacore::Cube<casacore::Float> > weightSpectrum_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Float>>> weightSpectra_p;
    VbCacheItemArray <casacore::Cube<casacore::Float> > sigmaSpectrum_p;
    VbCacheItemVectorArray <casacore::Vector<casacore::Cube<casacore::Float>>> sigmaSpectra_p;

    CacheRegistry registry_p;

    template <typename T, typename U>
    static void
    sortCorrelationItem (vi::VbCacheItem<T> & dataItem, casacore::IPosition & blc, casacore::IPosition & trc,
                         casacore::IPosition & mat, U & tmp, casacore::Bool sort)
    {

        T & data = dataItem.getRef ();
        U p1, p2, p3;

        if (dataItem.isPresent() && data.nelements() > 0) {

            blc(0) = trc(0) = 1;
            p1.reference(data (blc, trc).reform(mat));

            blc(0) = trc(0) = 2;
            p2.reference(data (blc, trc).reform(mat));

            blc(0) = trc(0) = 3;
            p3.reference(data (blc, trc).reform(mat));

            if (sort){ // casacore::Sort correlations: (PP,QQ,PQ,QP) -> (PP,PQ,QP,QQ)

                tmp = p1;
                p1 = p2;
                p2 = p3;
                p3 = tmp;
            }
            else {      // Unsort correlations: (PP,PQ,QP,QQ) -> (PP,QQ,PQ,QP)

                tmp = p3;
                p3 = p2;
                p2 = p1;
                p1 = tmp;
            }
        }
    }
};

class VisBufferState {

public:

    template<typename T>
    class FrequencyCache {
    public:

        typedef casacore::Vector<T> (ViImplementation2::* Updater) (casacore::Double, casacore::Int, casacore::Int, casacore::Int) const;

        FrequencyCache (Updater updater)
        : frame_p (-1),
          msId_p (-1),
          spectralWindowId_p (-1),
          time_p (-1),
          updater_p (updater)
        {}

        casacore::Int frame_p;
        casacore::Int msId_p;
        casacore::Int spectralWindowId_p;
        casacore::Double time_p;
        Updater updater_p;
        casacore::Vector<T> values_p;

        void
        flush ()
        {
            time_p = -1; // will be enough to cause a reload
        }

        void
        updateCacheIfNeeded (const ViImplementation2 * rovi,
                             casacore::Int rowInBuffer,
                             casacore::Int frame,
                             const VisBufferImpl2 * vb)
        {
            casacore::Int msId = vb->msId();
            casacore::Int spectralWindowId = vb->spectralWindows()(rowInBuffer);
            casacore::Double time = vb->time()(rowInBuffer);

            if (time == time_p && frame == frame_p && msId == msId_p &&
                spectralWindowId == spectralWindowId_p){
                return;
            }

            time_p = time;
            frame_p = frame;
            msId_p = msId;
            spectralWindowId_p = spectralWindowId;

            values_p.assign ((rovi ->* updater_p) (time_p, frame_p, spectralWindowId_p, msId_p));
        }
    };

    VisBufferState ()
    : appendCapacity_p (0),
      appendSize_p (0),
      areCorrelationsSorted_p (false),
      channelNumbers_p (& ViImplementation2::getChannels),
      dirtyComponents_p (),
      frequencies_p (& ViImplementation2::getFrequencies),
      isAttached_p (false),
      isFillable_p (false),
      isNewMs_p (false),
      isNewArrayId_p (false),
      isNewFieldId_p (false),
      isNewSpectralWindow_p (false),
      isRekeyable_p (false),
      isWritable_p (false),
      msId_p (-1),
      pointingTableLastRow_p (-1),
      validShapes_p (N_ShapePatterns),
      validVectorShapes_p (N_ShapePatterns),
      vi_p (0),
      visModelData_p (0),
      weightScaling_p ( )
    {}

    casacore::Int appendCapacity_p;
    casacore::Int appendSize_p;
    casacore::Bool areCorrelationsSorted_p; // Have correlations been sorted by sortCorr?
    FrequencyCache<casacore::Int> channelNumbers_p;
    casacore::Vector<casacore::Int> correlations_p;
    casacore::Vector<casacore::Stokes::StokesTypes> correlationsDefined_p;
    casacore::Vector<casacore::Stokes::StokesTypes> correlationsSelected_p;
    VisBufferComponents2 dirtyComponents_p;
    FrequencyCache<casacore::Double> frequencies_p;
    casacore::Bool isAttached_p;
    casacore::Bool isFillable_p;
    casacore::Bool isNewMs_p;
    casacore::Bool isNewArrayId_p;
    casacore::Bool isNewFieldId_p;
    casacore::Bool isNewSpectralWindow_p;
    casacore::Bool isRekeyable_p;
    casacore::Bool isWritable_p;
    casacore::Int msId_p;
    casacore::String msName_p;
    casacore::Bool newMs_p;
    mutable casacore::Int pointingTableLastRow_p;
    Subchunk subchunk_p;
    casacore::Vector<casacore::IPosition> validShapes_p;
    casacore::Vector<casacore::Vector<casacore::IPosition>> validVectorShapes_p;
    ViImplementation2 * vi_p; // [use]
    mutable VisModelDataI * visModelData_p;
    casacore::CountedPtr <WeightScaling> weightScaling_p;
};


}

}


#endif /* VISBUFFERIMPL2INTERNAL_H_ */
