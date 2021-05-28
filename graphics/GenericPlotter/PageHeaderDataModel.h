#ifndef PAGEHEADERDATAMODEL_H_
#define PAGEHEADERDATAMODEL_H_

#include <casa/Utilities/CountedPtr.h>


namespace casa {

// The purpose of this class is simply to provide
// a base for implementations, and not to define an abstract interface
// of what should be a 'Data Model' for a 'Page Header'.
// A Plotter may act under the control of a Controller.
// A Controller may provide a way to access internal data
// aiming at being displayed in a Page Header.
// A Plotter may - given a way to access external data - render that data
// on a Page Header.
class PageHeaderDataModel {
protected:
	PageHeaderDataModel() {}
	virtual ~PageHeaderDataModel() {}
};

typedef casacore::CountedPtr<PageHeaderDataModel> PageHeaderDataModelPtr;

}


#endif /* PAGEHEADERDATAMODEL_H_ */
