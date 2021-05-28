#include <msvis/MSVis/VisModelDataI.h>
#include <casa/Exceptions.h>

using namespace casacore;
namespace casa {

VisModelDataI::Factory VisModelDataI::factory_p = 0;
VisModelDataI::Factory VisModelDataI::factory2_p = 0;

VisModelDataI *
VisModelDataI::create ()
{
  // ThrowIf (factory_p == 0, "No VisModelDataI::factory available");

  if (factory_p == nullptr){
	  return nullptr;
  }

  return factory_p ();
}

VisModelDataI *
VisModelDataI::create2 ()
{
//  ThrowIf (factory2_p == 0, "No VisModelDataI::factory available");

  if (factory2_p == nullptr){
      return nullptr;
  }

  return factory2_p ();
}

bool 
VisModelDataI::setFactory (Factory factory, Int whichone)
{
	if(whichone==0)
		factory_p = factory;
	else if(whichone==1)
		factory2_p= factory;
  return true;
}

}
