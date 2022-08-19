#include <stdcasa/UtilJ.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/MSUtil.h>
#include <casacore/ms/MSSel/MSSelection.h>

#include <utility>

using namespace std;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {

void
FrequencySelection::addCorrelationSlices (const Vector <Vector <Slice> > & slices)
{
    correlationSlices_p = slices;
}

void
FrequencySelection::filterByWindow (Int windowId) const
{
    filterWindowId_p = windowId;
}

Int
FrequencySelection::filterWindow() const
{
    return filterWindowId_p;
}

String
FrequencySelection::frameName (Int referenceFrame)
{
    String result;

    if (referenceFrame >= 0 && referenceFrame < MFrequency::N_Types){

        result = MFrequency::showType (referenceFrame);
    }
    else if (referenceFrame == ByChannel){
    }
    else{

        ThrowIf (true, String::format ("Unknown frame of reference: id=%d", referenceFrame));
    }

    return result;
}

Vector <Slice>
FrequencySelection::getCorrelationSlices (Int polarizationId) const
{
    if (correlationSlices_p.nelements() == 0){
        return Vector<Slice> (); // empty vector implies all elements
    }

    Assert (polarizationId >= 0 && polarizationId < (int) correlationSlices_p.nelements());

    return correlationSlices_p [polarizationId];
}

Int
FrequencySelection::getFrameOfReference () const
{
    return referenceFrame_p;
}

void
FrequencySelectionUsingChannels::add (Int spectralWindow, Int firstChannel,
                                      Int nChannels, Int increment)
{
    Assert (spectralWindow >= 0);
    Assert (firstChannel >= 0);
    Assert (nChannels > 0);
    Assert (increment != 0 || nChannels == 1);

    elements_p.push_back (Element (spectralWindow, firstChannel, nChannels, increment));
}

FrequencySelectionUsingChannels::FrequencySelectionUsingChannels (const FrequencySelectionUsingChannels & other)
: FrequencySelection (other),
  elements_p (other.elements_p),
  filtered_p (other.filtered_p),
  refinements_p (nullptr)
{
    if (other.refinements_p){
        refinements_p.reset (new FrequencySelectionUsingFrame (* other.refinements_p.get()));
    }
}

void
FrequencySelectionUsingChannels::add (const MSSelection & msSelectionConst,
                                      const MeasurementSet * ms)
{
    // Add in the frequency selection from the provided MSSelection object
    //
    // Meanings of columns in the "matrix" (actually used as a parallel array)

    enum {SpectralWindowId, FirstChannel, StopChannel, Step};

    MSSelection & msSelection = const_cast <MSSelection &> (msSelectionConst);
        // Needed because many msSelection methods are not const for some reason

    Matrix<Int> channelList = msSelection.getChanList();

    for (Int row = 0; row < (Int) channelList.nrow(); row ++){

        Int nChannels = channelList (row, StopChannel) - channelList (row, FirstChannel);
        nChannels = nChannels / channelList (row, Step) + 1;

        add (channelList (row, SpectralWindowId),
             channelList (row, FirstChannel),
             nChannels,
             channelList (row, Step));
    }

    // Extract and add the correlation selection.

    Vector <Vector<Slice> > correlationSlices;
    msSelection.getCorrSlices (correlationSlices, ms);

    addCorrelationSlices (correlationSlices);

}

FrequencySelectionUsingChannels::const_iterator
FrequencySelectionUsingChannels::begin () const
{
    filtered_p.clear();

    for (Elements::const_iterator i = elements_p.begin();
         i != elements_p.end();
         i ++){

        if (filterWindow () < 0 || i->spectralWindow_p == filterWindow()){
            filtered_p.push_back (* i);
        }
    }

    return filtered_p.begin();
}


FrequencySelection *
FrequencySelectionUsingChannels::clone () const
{
    return new FrequencySelectionUsingChannels (* this);
}

Bool
FrequencySelectionUsingChannels::empty () const
{
    return elements_p.empty();
}


FrequencySelectionUsingChannels::const_iterator
FrequencySelectionUsingChannels::end () const
{
    return filtered_p.end();
}

set<int>
FrequencySelectionUsingChannels::getSelectedWindows () const
{
    set <int> result;
    for (Elements::const_iterator i = elements_p.begin();
         i != elements_p.end();
         i ++){

        result.insert (i->spectralWindow_p);

    }

    return result;
}

Int
FrequencySelectionUsingChannels::getNChannels (Int spectralWindowId) const
{

    Int result = 0;

    if (elements_p.empty()){

    }
    else {

        for (Elements::const_iterator i = elements_p.begin();
                i != elements_p.end();
                i ++){

            if (i->spectralWindow_p == spectralWindowId){
                result += i->nChannels_p;
            }
        }
    }
    return result;
}

void
FrequencySelectionUsingChannels::refine (const FrequencySelectionUsingFrame & refiningSelection)
{
    refinements_p.reset (new FrequencySelectionUsingFrame (refiningSelection));
}

void
FrequencySelectionUsingChannels::applyRefinement (std::function <casacore::Slice (int, double, double)> spwcFetcher) const
{
    // Loop through each of the parts of the refining selection.

    for (auto refiningElement : * refinements_p.get()){

        // Get the spectral window and then look for parts of the original selection which
        // apply to the same spectral window.

        int spectralWindow = refiningElement.getSpectralWindow();

        for (auto & originalSelection : elements_p){

            if (originalSelection.spectralWindow_p != spectralWindow){
                continue; // wrong window, so skip it
            }

            // Convert the element into channels in the current spectral window.

            Slice s = spwcFetcher (spectralWindow,
                                   refiningElement.getBeginFrequency(),
                                   refiningElement.getEndFrequency());

            int firstRefiningChannel = s.start();
            int lastRefiningChannel = s.end();

            // Refine the original selection so that it only applies to the intersection between
            // the refining channel range and the original range.
            //
            // This only applies if the two intersect; otherwise the original selection element
            // is left unchanged.

            refineSelection (originalSelection, firstRefiningChannel, lastRefiningChannel);
        }
    }

    refinements_p.reset (nullptr); // All done so destroy them.
}

bool
FrequencySelectionUsingChannels::refinementNeeded () const
{
    return !! refinements_p;
}

void
FrequencySelectionUsingChannels::refineSelection (FrequencySelectionUsingChannels::Element & originalElement,
                                                  int firstRefiningChannel, int lastRefiningChannel) const
{
    int originalLastChannel = originalElement.firstChannel_p +
        (originalElement.nChannels_p - 1) * originalElement.increment_p;

    // If the two ranges do not overlap, then skip this operation since there's not basis
    // for refinement.  Presumably the refinement operation will apply to a different
    // selection range in this spectral window.

    if (firstRefiningChannel > originalLastChannel ||
        lastRefiningChannel < originalElement.firstChannel_p)
    {
        return;  // No overlap, so refinement not possible
    }

    // Refine the channel interval of the existing selection.

    int newFirstChannel = max (originalElement.firstChannel_p, firstRefiningChannel);
    int newLastChannel = min (originalLastChannel, lastRefiningChannel);

    originalElement.firstChannel_p = newFirstChannel;
    originalElement.nChannels_p = (newLastChannel - newFirstChannel) / originalElement.increment_p + 1;
}

size_t
FrequencySelectionUsingChannels::size () const
{
    return elements_p.size();
}


String
FrequencySelectionUsingChannels::toString () const
{
    String s = String::format ("{frame='%s' {", frameName (getFrameOfReference()).c_str());

    for (Elements::const_iterator e = elements_p.begin();
         e != elements_p.end();
         e ++){

        s += String::format ("(spw=%d, 1st=%d, n=%d, inc=%d)",
                            e->spectralWindow_p,
                            e->firstChannel_p,
                            e->nChannels_p,
                            e->increment_p);
    }
    s += "}}";

    return s;
}

FrequencySelectionUsingFrame::FrequencySelectionUsingFrame (MFrequency::Types frameOfReference)
: FrequencySelection (frameOfReference)
{}

void
FrequencySelectionUsingFrame::add (Int spectralWindow, Double bottomFrequency,
                                   Double topFrequency)
{
    elements_p.push_back (Element (spectralWindow, bottomFrequency, topFrequency));
}

//void
//FrequencySelectionUsingFrame::add (Int spectralWindow, Double bottomFrequency,
//                                   Double topFrequency, Double increment)
//{
//    elements_p.push_back (Elements (spectralWindow, bottomFrequency, topFrequency, increment));
//}


FrequencySelectionUsingFrame::const_iterator
FrequencySelectionUsingFrame::begin () const
{
    filtered_p.clear();

    for (Elements::const_iterator i = elements_p.begin();
         i != elements_p.end();
         i ++){

        if (filterWindow () < 0 || i->spectralWindow_p == filterWindow()){
            filtered_p.push_back (* i);
        }
    }

    return filtered_p.begin();
}

FrequencySelection *
FrequencySelectionUsingFrame::clone () const
{
    return new FrequencySelectionUsingFrame (* this);
}

Bool
FrequencySelectionUsingFrame::empty () const
{
    return elements_p.empty();
}


FrequencySelectionUsingFrame::const_iterator
FrequencySelectionUsingFrame::end () const
{
    return filtered_p.end();
}

set<int>
FrequencySelectionUsingFrame::getSelectedWindows () const
{
    set<int> result;
    for (Elements::const_iterator i = elements_p.begin();
         i != elements_p.end();
         i ++){

        result.insert (i->spectralWindow_p);
    }

    return result;
}

Double
FrequencySelectionUsingFrame::Element::getBeginFrequency () const
{
    return beginFrequency_p;
}

Double
FrequencySelectionUsingFrame::Element::getEndFrequency () const
{
    return endFrequency_p;
}

int
FrequencySelectionUsingFrame::Element::getSpectralWindow () const
{
    return spectralWindow_p;
}

String
FrequencySelectionUsingFrame::toString () const
{
    String s = String::format ("{frame='%s' {", frameName (getFrameOfReference()).c_str());

    for (Elements::const_iterator e = elements_p.begin();
         e != elements_p.end();
         e ++){

        s += String::format ("(spw=%d, 1st=%g, n=%g, inc=%g)",
                             e->spectralWindow_p,
                             e->beginFrequency_p,
                             e->endFrequency_p,
                             e->increment_p);
    }

    s += "}}";

    return s;
}
 std::map<int, std::pair<int, int>  >  FrequencySelectionUsingFrame::getChannelRange (const casacore::MeasurementSet& ms) const {
	 
	 map<int, pair<int, int> > retval;
	 MFrequency::Types freqframe=MFrequency::castType(getFrameOfReference());
	 Vector<Int> outspw;
	 Vector<Int> outstart;
	 Vector<Int> outnchan;
	 for (Elements::const_iterator j = elements_p.begin(); j != elements_p.end(); j++){
		//cerr << "elements " << (j->spectralWindow_p) <<  "  " << (j->beginFrequency_p)<< "  " <<  (j->endFrequency_p)<< endl;

		Int outstart;
		Int outnchan;
		Int spw=j->spectralWindow_p;
		Double begFreq=j->beginFrequency_p;
		Double endFreq=j->endFrequency_p;
		//cerr << "spw " << spw << " BegFreq " << begFreq << "  EndFreq " << endFreq << endl;
		MSUtil::getChannelRangeFromFreqRange(outstart,
  			  outnchan, ms, spw, begFreq, endFreq, 1.0e-6,freqframe); 
		if(outstart > -1){
			auto  old=retval.find(spw);
			if(old != retval.end()){
				pair<int, int> oldrange=old->second;
				if(oldrange.second >= outstart){
						if(outnchan > (oldrange.second-outstart+1+oldrange.first))
							oldrange.first=outnchan;
						else
							oldrange.first=oldrange.second-outstart+1+oldrange.first;
						oldrange.second=outstart;
				}
				else{
				    if((oldrange.first -(outstart - oldrange.second)) <outnchan){	
						oldrange.first=outnchan+outstart-oldrange.second+1;
					}
				}
				old->second=oldrange;
			}
			else{
	 
				retval[int(spw)]=make_pair(int(outnchan), int(outstart));
			}
		}
	 }
	 return retval;
	 
 }
FrequencySelections::FrequencySelections ()
: filterWindow_p (-1)
{}

FrequencySelections::FrequencySelections (const FrequencySelections & other)
{
    for (Selections::const_iterator s = other.selections_p.begin();
         s != other.selections_p.end(); s++){
        selections_p.push_back ((* s)->clone());
    }

    filterWindow_p = other.filterWindow_p;
    selectedWindows_p = other.selectedWindows_p;
}

FrequencySelections::~FrequencySelections ()
{
    for (Selections::const_iterator s = selections_p.begin();
         s != selections_p.end(); s++){
        delete (* s);
    }
}

void
FrequencySelections::add (const FrequencySelection & selection)
{
    if (! selections_p.empty()){
        ThrowIf (getFrameOfReference() != selection.getFrameOfReference(),
                 String::format ("Frequency selection #%d has incompatible frame of reference %d:%s "
                                "(!= %d:%s)",
                                selections_p.size() + 1,
                                selection.getFrameOfReference(),
                                FrequencySelection::frameName (selection.getFrameOfReference()).c_str(),
                                getFrameOfReference(),
                                FrequencySelection::frameName (getFrameOfReference()).c_str()));
    }

    selections_p.push_back (selection.clone());
    Int msIndex = selections_p.size() - 1;
    set<int> windows = selection.getSelectedWindows();

    for (set<int>::const_iterator w = windows.begin(); w != windows.end(); w++){
        selectedWindows_p.insert (make_pair (msIndex, * w));
    }

}

FrequencySelections *
FrequencySelections::clone () const
{
    return new FrequencySelections (* this);
}

const FrequencySelection &
FrequencySelections::get (Int msIndex) const
{
    if (selections_p.empty()){
        return defaultSelection_p;
    }

    ThrowIf (msIndex < 0 || msIndex >= (int) selections_p.size(),
             String::format ("MS index, %d, out of bounds [0,%d]", msIndex, selections_p.size() - 1));

    return * selections_p [msIndex];
}


Int
FrequencySelections::getFrameOfReference () const
{
    if (selections_p.empty()){
        return FrequencySelection::ByChannel;
    }
    else {
        return selections_p.front()->getFrameOfReference();
    }
}

Bool
FrequencySelections::isSpectralWindowSelected (Int msIndex, Int spectralWindowId) const
{
    // Empty selections means everything is selected

    if (selections_p.empty()){
        return true;
    }

    Assert (msIndex >= 0 && msIndex < (int) selections_p.size() && selections_p [msIndex] != 0);

    if (selections_p [msIndex]->empty()){
        return true;
    }

    SelectedWindows::const_iterator swi =
        selectedWindows_p.find (make_pair (msIndex, spectralWindowId));

    return swi != selectedWindows_p.end();
}


Int
FrequencySelections::size () const
{
    return (Int) selections_p.size();
}

} // end namespace vi

using namespace casacore;
} // end namespace casa
