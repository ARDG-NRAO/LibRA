 /*
 * VisibilityProcessing.cc
 *
 *  Created on: Apr 20, 2011
 *      Author: jjacobs
 */

#include "VisibilityProcessing.h"
#include "VisBufferAsync.h"
#include "VisibilityIteratorImplAsync.h"
#include "UtilJ.h"

#include <tuple>

#include <casa/System/AipsrcValue.h>

#include <algorithm>
#include <list>
#include <stdarg.h>
#include <limits>
#include <memory>
#include <numeric>

using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::asyncio;
using namespace casacore;
using namespace casa::utilj;
using namespace std;
using std::shared_ptr;

using namespace casacore;
namespace casa {

namespace vpf {

#define Log(level, ...) \
    {if (level <= VpEngine::getLogLevel()) \
         VpEngine::log (__VA_ARGS__);}


//SimpleVp::SimpleVp (const String & name, const String & input, const String & output)
//: VisibilityProcessor (name,
//                       vector<String> (1, input),
//                       output.empty() ? vector<String> () : vector<String> (1, output))
//{}
//
//SimpleVp::~SimpleVp ()
//{}
//
//void
//SimpleVp::validateImpl ()
//{
//    throwIfAnyPortsUnconnected ();
//}


SplitterVp::SplitterVp (const String & name,
                        const String & inputName,
                        const vector<String> & outputNames)
: VisibilityProcessor (name, vector<String> (1, inputName), outputNames)
{
    ThrowIf (inputName.size() != 1, "Exactly one input is required.");
    ThrowIf (outputNames.size () < 1, "At least one output is required.");
}



VisibilityProcessor::ProcessingResult
SplitterVp::doProcessingImpl (ProcessingType processingType ,
                              VpData & inputData,
                              const SubchunkIndex & /*subChunkIndex*/)
{
    if (processingType == VisibilityProcessor::EndOfChunk ||
        processingType == VisibilityProcessor::EndOfData){
        return ProcessingResult();
    }

    VpPort inputPort = getInputs () [0];
    VpPorts outputs = getOutputs();
    VbPtr inputVbPtr = inputData [inputPort];
    VpData vpData;

    // Handle the first, required output by reusing the input VB

    vpData [outputs [0]] = inputVbPtr; // reuse in input

    // Handle any additional outputs.

    for (int i = 1; i < (int) outputs.size(); i++){

        VpPort port = outputs [i];

        // Make a copy of the input and add it as an output

        vpData [port] = VbPtr (inputVbPtr->clone());

    }

    ProcessingResult processingResult (Normal, vpData);

    return processingResult;
}

void
SplitterVp::validateImpl ()
{
    throwIfAnyInputsUnconnected ();
    ThrowIf (getOutputs (true).empty(),
             String::format ("SplitterVp %s has no outputs connected.", getFullName().c_str()));
}


SubchunkIndex::SubchunkIndex (Int chunkNumber, Int subChunkNumber, Int iteration)
: chunkNumber_p (chunkNumber), iteration_p (iteration), subChunkNumber_p (subChunkNumber)
{}

Bool
SubchunkIndex::operator< (const SubchunkIndex & other) const
{
    Bool result = std::make_tuple (chunkNumber_p, subChunkNumber_p, iteration_p) <
                  std::make_tuple (other.chunkNumber_p, other.subChunkNumber_p, other.iteration_p);

    return result;
}

Int
SubchunkIndex::getChunkNumber () const
{
    return chunkNumber_p;
}

Int
SubchunkIndex::getIteration () const
{
    return iteration_p;
}

Int
SubchunkIndex::getSubchunkNumber () const
{
    return subChunkNumber_p;
}

String
SubchunkIndex::toString() const
{
    return String::format ("(%d,%d,%d)", chunkNumber_p, subChunkNumber_p, iteration_p);
}

VisibilityProcessorStub::ProcessingResult
VisibilityProcessorStub::doProcessingImpl (ProcessingType /*processingType*/,
                                           VpData & /*inputData*/,
                                           const SubchunkIndex & /*subChunkIndex*/)
{
    Throw ("Stub does not permit processing.");
}

void
VisibilityProcessorStub::validateImpl ()
{
    Throw ("Stub does not permit validation.");
}


//VisBuffer *
//VbPtr::get ()
//{
//    return vb_p; // use with caution
//}
//

//VisBuffer *
//VbPtr::release ()
//{
//    freeOnDelete_p = false;
//    VisBuffer * vb = vb_p;
//    vb_p = NULL;
//
//    return vb;
//}

VisibilityProcessor::VisibilityProcessor ()
: container_p (NULL),
  nSubchunks_p (0),
  nSubchunksUnique_p (0),
  vpEngine_p (0)
{}


VisibilityProcessor::VisibilityProcessor (const String & name,
                                          const vector<String> & inputNames,
                                          const vector<String> & outputNames,
                                          Bool makeIoPorts)
: container_p (NULL),
  name_p (name),
  nSubchunks_p (0),
  nSubchunksUnique_p (0),
  vpEngine_p (0)

{
    VpPort::Type portType = makeIoPorts ? VpPort::InOut : VpPort::Input;
    vpInputs_p = definePorts (inputNames, portType, "input");
    portType = makeIoPorts ? VpPort::InOut : VpPort::Output;
    vpOutputs_p = definePorts (outputNames, portType, "output");
}

void
VisibilityProcessor::throwIfAnyInputsUnconnected (const vector<String> & exceptThese) const
{
    VpPorts inputs = getInputs ();

    VpPorts unconnectedPorts = portsUnconnected (inputs, & VpPort::isConnectedInput, exceptThese);

    ThrowIf (! unconnectedPorts.empty(),
             String::format ("Vp '%s' has unconnected inputs: %s",
                     getName().c_str(),
                     unconnectedPorts.toString().c_str()));
}

void
VisibilityProcessor::throwIfAnyInputsUnconnectedExcept (const String & name) const
{
    throwIfAnyInputsUnconnected (vector<String> (1, name));
}


void
VisibilityProcessor::throwIfAnyOutputsUnconnected (const vector<String> & exceptThese) const
{
    VpPorts outputs = getOutputs ();

    VpPorts unconnectedPorts = portsUnconnected (outputs, & VpPort::isConnectedOutput, exceptThese);

    ThrowIf (! unconnectedPorts.empty(),
             String::format ("Vp '%s' has unconnected outputs: %s",
                     getName().c_str(),
                     unconnectedPorts.toString().c_str()));
}

void
VisibilityProcessor::throwIfAnyOutputsUnconnectedExcept (const String & name) const
{
    throwIfAnyOutputsUnconnected (vector<String> (1, name));
}


void
VisibilityProcessor::throwIfAnyPortsUnconnected () const
{
    VpPorts inputs = getInputs ();

    VpPorts unconnectedInputs = portsUnconnected (inputs, & VpPort::isConnectedInput);

    VpPorts outputs = getOutputs ();

    VpPorts unconnectedOutputs = portsUnconnected (outputs, & VpPort::isConnectedOutput);

    if (! unconnectedInputs.empty () || ! unconnectedOutputs.empty()){

        String message = String::format ("Vp '%s' has");


        if (! unconnectedInputs.empty()){

            // String::format up the inputs portion of the message, if applicable

            message += String::format (" unconnected inputs: %s",
                                unconnectedInputs.toString().c_str());
        }

        if (! unconnectedInputs.empty()){

            // String::format up the inputs portion of the message, if applicable.
            // Determine the appropriate text to join up the previous text.

            string conjunction = (unconnectedInputs.empty()) ? ""
                                                             : "\n and";

             message += String::format ("%s has unconnected outputs: %s",
                                conjunction.c_str (),
                                unconnectedOutputs.toString().c_str());
        }

        message += ".";

        ThrowIf (true, message);

    }

}


VpPorts
VisibilityProcessor::portsUnconnected (const VpPorts & ports, Bool (VpPort::* isConnected) () const,
                                       const vector<String> & except) const
{
    VpPorts unconnectedPorts;

    for (VpPorts::const_iterator port = ports.begin(); port != ports.end(); port ++){

        Bool connected = ((* port).*isConnected)();

        if (! connected){

            // See if it's in the list of exceptions

            if (! except.empty() && find (except.begin(), except.end(), port->getName()) == except.end()){
                unconnectedPorts.push_back (* port);
            }
        }

    }

    return unconnectedPorts;
}

void
VisibilityProcessor::chunkStart (const SubchunkIndex & sci)
{
    chunkStartImpl (sci);
}


VpPorts
VisibilityProcessor::definePorts (const vector<String> & portNames, VpPort::Type type, const String & typeName)
{

    VpPorts vpPorts;

    for (vector<String>::const_iterator portName = portNames.begin();
         portName != portNames.end();
         portName ++){

        ThrowIf (vpPorts.contains (* portName),
                 String::format ("VisibilityProcessor %s already has an %s port '%s'",
                         getName().c_str(), typeName.c_str(), portName->c_str()));

        vpPorts.push_back (VpPort (this, * portName, type));
    }

    return vpPorts;
}

VisibilityProcessor::ProcessingResult
VisibilityProcessor::doProcessing (ProcessingType processingType,
                                   VpData & inputData,
                                   VpEngine * vpEngine,
                                   const SubchunkIndex & subchunkIndex)
{

    vpEngine_p = vpEngine;
    pair<Int,Int> originalViPosition = getVi()->getSubchunkId ();

    if (processingType == Subchunk && subchunkIndex != SubchunkIndex::Invalid){
        nSubchunks_p ++;
        if (subchunkIndex.getIteration() == 0){
            nSubchunksUnique_p ++;
        }
    }

    ProcessingResult result;

    try {
        result = doProcessingImpl (processingType, inputData, subchunkIndex);

    }
    catch (AipsError & e){

        vpEngine_p = 0;

        Rethrow (e, String::format ("Error in doProcessing of VP '%s': %s on %s", getName().c_str(),
                            toString (processingType).c_str(), subchunkIndex.toString().c_str()));
    }

    pair<Int,Int> currentViPosition = getVi()->getSubchunkId ();

    ThrowIf (currentViPosition != originalViPosition,
             String::format ("VisibilityIterator moved during processing in VP '%s'", getName().c_str()));

    vpEngine_p = 0;

    return result;

}


String
VisibilityProcessor::getFullName () const
{

    list<String> names;
    names.push_front (getName());

    const VisibilityProcessor * parent = getContainer();

    while (parent != NULL){

        names.push_front (parent->getName());
        parent = parent->getContainer();
    }

    String fullName = utilj::join (names, ".");

    return fullName;
}

VpPorts
VisibilityProcessor::getInputs (Bool connectedOnly) const
{
    VpPorts result;

    if (connectedOnly){

        // Copy over the inputs that are input connected.
        // Need to negate predicate to get the right results --STL is strange sometimes.

        remove_copy_if (vpInputs_p.begin(), vpInputs_p.end(), back_inserter (result),
                        not1 (mem_fun_ref (& VpPort::isConnectedInput)));
    }
    else{
        result = vpInputs_p;
    }

    return result;
}

VpPort
VisibilityProcessor::getInput (const String & name) const
{
    ThrowIf (! vpInputs_p.contains (name),
             String::format ("Vp '%s' has no input port '%s'", getName().c_str(), name.c_str()));

    return vpInputs_p.get (name);
}

VpPort &
VisibilityProcessor::getInputRef (const String & name)
{
    ThrowIf (! vpInputs_p.contains (name),
             String::format ("Vp '%s' has no input port '%s'", getName().c_str(), name.c_str()));

    return vpInputs_p.getRef (name);
}

String
VisibilityProcessor::getName () const
{
    return name_p;
}

Int
VisibilityProcessor::getNSubchunksProcessed () const
{
    return nSubchunks_p;
}

Int
VisibilityProcessor::getNSubchunksUniqueProcessed () const
{
    return nSubchunksUnique_p;
}


VpPort
VisibilityProcessor::getOutput (const String & name) const
{
    ThrowIf (! vpOutputs_p.contains (name),
             String::format ("Vp '%s' has no output port '%s'", getName().c_str(), name.c_str()));

    return vpOutputs_p.get (name);
}

VpPort &
VisibilityProcessor::getOutputRef (const String & name)
{
    ThrowIf (! vpOutputs_p.contains (name),
             String::format ("Vp '%s' has no output port '%s'", getName().c_str(), name.c_str()));

    return vpOutputs_p.getRef (name);
}

VpPorts
VisibilityProcessor::getOutputs (Bool connectedOnly) const
{
    VpPorts result;

    if (connectedOnly){

        // Copy over the outputs that are output connected.
        // Need to negate predicate to get the right results --STL is strange sometimes.

        remove_copy_if (vpOutputs_p.begin(), vpOutputs_p.end(), back_inserter (result),
                        not1 (mem_fun_ref (& VpPort::isConnectedOutput)));
    }
    else{
        result = vpOutputs_p;
    }

    return result;
}

PrefetchColumns
VisibilityProcessor::getPrefetchColumns () const
{
    return PrefetchColumns ();
}

ROVisibilityIterator *
VisibilityProcessor::getVi ()
{
    return vpEngine_p->getVi();
}

VpEngine *
VisibilityProcessor::getVpEngine ()
{
    return vpEngine_p;
}

void
VisibilityProcessor::processingStart ()
{
    nSubchunks_p = 0;
    nSubchunksUnique_p = 0;

    validate();

    processingStartImpl ();
}

void
VisibilityProcessor::setContainer (const VpContainer * container)
{
    assert (container != NULL);

    ThrowIf (container_p != NULL,
             String::format ("Attempting to add VisibiltyProcessor '%s' into '%s'; previously added to '%s'",
                     getName().c_str(), container->getFullName().c_str(), container_p->getFullName().c_str()));

    container_p = container;
}

String
toString (VisibilityProcessor::ProcessingType p)
{
    static const char * names [] = {"Subchunk", "EndOfChunk", "EndOfData"};

    return names [p];
}

void
VisibilityProcessor::validate ()
{
    validateImpl ();
}

VpContainer::VpContainer (const String & name, const vector<String> & inputs, const vector<String> & outputs)
: VisibilityProcessor (name, inputs, outputs, true)
{}

void
VpContainer::add (VisibilityProcessor * vp)
{
    ThrowIf (contains (vp),
             String::format ("Visibility processor %s already in container %s",
                            vp->getName().c_str(), getName().c_str()));

    vp->setContainer (this);
    vps_p.push_back (vp);
}

void
VpContainer::chunkStart (const SubchunkIndex & sci)
{
    iterator i;
    try{
        for (i = begin(); i != end(); i++){
            (*i)->chunkStart (sci);
        }
    }
    catch (AipsError & e){
        Rethrow (e, String::format ("Error during chunkStart for container '%s' VP '%s'",
                            getName().c_str(), (*i)->getName().c_str()));
    }
}

void
VpContainer::connect (const String &  sourcePortName,
                      VisibilityProcessor * sinkVp, const String &  sinkPortName)
{
    connect (this, sourcePortName, sinkVp, sinkPortName);
}

void
VpContainer::connect (VisibilityProcessor * sourceVp, const String &  sourcePortName,
                      const String &  sinkPortName)
{
    connect (sourceVp, sourcePortName, this, sinkPortName);
}

pair<VpPort, VpPort>
VpContainer::validateConnectionPorts (VisibilityProcessor * sourceVp,
                                      const String &  sourcePortName,
                                      VisibilityProcessor * sinkVp,
                                      const String &  sinkPortName)
{
    // Does the owning VP really support these ports?

    ThrowIf (sourceVp != this && ! sourceVp->getOutputs ().contains (sourcePortName),
             String::format ("Visibility processor %s in %s does not have output %s",
                     sourceVp->getName().c_str(), getName().c_str(),
                     sourcePortName.c_str()));

    ThrowIf (sourceVp == this && ! getInputs().contains (sourcePortName),
             String::format ("Visibility processor container %s in %s does not have input %s",
                     sourceVp->getName().c_str(), getName().c_str(),
                     sourcePortName.c_str()));

    ThrowIf (sinkVp != this && ! sinkVp->getInputs ().contains (sinkPortName),
             String::format ("Visibility processor %s in %s does not have input %s",
                     sinkVp->getName().c_str(), getName().c_str(),
                     sinkPortName.c_str()));

    ThrowIf (sinkVp == this && ! getOutputs().contains (sinkPortName),
             String::format ("Visibility processor container %s in %s does not have output %s",
                     sinkVp->getName().c_str(), getName().c_str(),
                     sinkPortName.c_str()));

    // Are the ports already in use?

    VpPort sink = (sinkVp == this) ? sinkVp->getOutput (sinkPortName)
                                   : sinkVp->getInput (sinkPortName);
    VpPort source = (sourceVp == this) ? sourceVp->getInput (sourcePortName)
                                       : sourceVp->getOutput (sourcePortName);

    ThrowIf (utilj::containsKey (source, network_p),
             String::format ("Output %s already in use for visibility processor %s in %s",
                     source.getName().c_str(), sourceVp->getName().c_str(), getName().c_str()));

    ThrowIf (utilj::containsKey (sink, networkReverse_p),
             String::format ("Input %s already in use for visibility processor %s in %s",
                     sink.getName().c_str(), sinkVp->getName().c_str(), getName().c_str()));

    return make_pair (source, sink);
}

void
VpContainer::connect (VisibilityProcessor * sourceVp, const String &  sourcePortName,
                      VisibilityProcessor * sinkVp, const String &  sinkPortName)
{
    // Validate the requested connection
    // =================================

    // Do they refer to a VP in this container?

    ThrowIf (! contains (sourceVp) && sourceVp != this,
             String::format ("No such visibility processor %s in %s.",
                     sourceVp->getName().c_str(), getName().c_str()));
    ThrowIf (! contains (sinkVp) && sinkVp != this,
             String::format ("No such visibility processor %s in %s.",
                     sinkVp->getName().c_str(), getName().c_str()));

    VpPort sink, source;
    std::tie (source, sink) = validateConnectionPorts (sourceVp, sourcePortName, sinkVp, sinkPortName);

    // See if this is a connection to the container inputs or outputs or
    // a normal connection between VPs

    Bool containerConnect = (source.getType() == sink.getType()) &&
                            ((source.isType(VpPort::Input) && sourceVp == this) ||
                             (sink.isType(VpPort::Output) && sinkVp == this));

    Bool normalConnect = source.isType (VpPort::Output) && sink.isType (VpPort::Input);

    Bool selfConnect = sourceVp == sinkVp; // detects loop back

    ThrowIf (! (normalConnect ||  containerConnect) || selfConnect,
             String::format ("Cannot connect %s:%s to %s:%s in %s", sourceVp->getName ().c_str(),
                     source.getName ().c_str (), sinkVp->getName().c_str (),
                     sink.getName ().c_str (), getName().c_str()));

    // The validation is over, so actually do the connection.

    network_p [source] = sink;
    networkReverse_p.insert (sink);

    // Inform the real ports (i.e., not the copies) that they are connected
    // N.B.: Container ports are in/out and are intended to be doubly connected,
    //       from the inside and from the outside of the container.

    if (source.isType (VpPort::Input)){
        sourceVp->getInputRef (source.getName()).setConnectedOutput ();
    }
    else{
        sourceVp->getOutputRef (source.getName()).setConnectedOutput ();
    }

    if (sink.isType (VpPort::Output)){
        sinkVp->getOutputRef (sink.getName()).setConnectedInput ();
    }
    else{
        sinkVp->getInputRef (sink.getName()).setConnectedInput ();
    }
}

VpContainer::iterator
VpContainer::begin()
{
    return iterator (vps_p.begin());
}

VpContainer::const_iterator
VpContainer::begin() const
{
    return const_iterator (vps_p.begin());
}



Bool
VpContainer::contains (const VisibilityProcessor * vp) const
{
    Bool foundIt = find (vps_p.begin(), vps_p.end(), vp) != vps_p.end();

    return foundIt;
}

VisibilityProcessor::ProcessingResult
VpContainer::doProcessingImpl (ProcessingType processingType, VpData & data, const SubchunkIndex & sci)
{

    VpSet vpsWaiting (vps_p.begin(), vps_p.end()); // Set of pending VPs
    ChunkCode overallChunkCode = Normal;           // container result for this data
    VisibilityProcessor * vp;                      // Currently executing VP
    VpData vpInputs;                               // Inputs to be fed to current Vp

    remapPorts (data, this);

    Log (3, "VpContainer::doProcessing: '%s' starting execution with inputs {%s}.\n",
         getName().c_str(), data.getNames().c_str());

    try{

        do {

            // Find a VP which can compute given the current set of inputs

            Bool flushing = processingType != Subchunk;
            std::tie (vp, vpInputs) = findReadyVp (vpsWaiting, data, flushing);

            if (vp != NULL){

                Log (3, "VpContainer::doProcessing: '%s' starting execution of %s.\n",
                     getName().c_str(), vp->getName().c_str());

                // Have the ready VP process its inputs and
                // potentially produce more outputs

                ChunkCode chunkCode;
                VpData outputs;

                std::tie (chunkCode, outputs) =
                        vp->doProcessing (processingType, vpInputs, getVpEngine(), sci);

                Log (3, "VpContainer::doProcessing: execution of %s output {%s}.\n",
                     vp->getName().c_str(), outputs.getNames().c_str());

                if (processingType == EndOfChunk && chunkCode == RepeatChunk){

                    // If any VP in this iteration requests a chunk repeat,
                    // then that's the overall result.

                    overallChunkCode = RepeatChunk;
                }

                // Remove the VP from the set of pending VPs, remove
                // the data this VP consumed as inputs and add any outputs
                // it produced to the set of available data.

                vpsWaiting.erase (vp);
                for (VpData::const_iterator i = vpInputs.begin(); i != vpInputs.end(); i++){
                    data.erase (i->first);
                }
                remapPorts (outputs, vp);
                data.insert (outputs.begin(), outputs.end());

            }

        } while (vp != NULL);

    }
    catch (AipsError & e){
        Rethrow (e, String::format ("Error while container '%s' processing VP '%s'",
                            getName().c_str(), (vp != NULL) ? vp->getName().c_str() : "NULL"));
    }

    if (vpsWaiting.empty()){
        Log (3, "VpContainer::doProcessing: '%s' executed all VPs.\n", getName().c_str());
    }
    else{
        Log (3, "VpContainer::doProcessing: '%s' did not execute VPs: {%s}.\n",
             getName().c_str(), vpsWaiting.getNames().c_str());
    }

    return ProcessingResult (overallChunkCode, data);
}

Bool
VpContainer::empty () const
{
    return vps_p.empty();
}

VpContainer::iterator
VpContainer::end()
{
    return iterator (vps_p.end());
}

VpContainer::const_iterator
VpContainer::end() const
{
    return const_iterator (vps_p.end());
}

void
VpContainer::fillWithSequence (VisibilityProcessor * first, ...)
{
    ThrowIf (! vps_p.empty (),
             String::format ("fillWithSequence performed on non-empty container %s", getName().c_str()));

	va_list vaList;

	VisibilityProcessor * vp = first;

	va_start (vaList, first);

	while (vp != NULL){

	    add (vp);

	    vp = va_arg (vaList, VisibilityProcessor *);
	}

	va_end (vaList);

	for (VPs::iterator vp = vps_p.begin ();
	     vp != vps_p.end();
	     vp ++){

	    VPs::iterator vp2 = vp + 1;
	    if (vp2 == vps_p.end()){
	        break;
	    }

	    ThrowIf ((* vp)->getOutputs().empty(),
	             String::format ("Visibility processor %s has no outputs.", (* vp)->getName().c_str()));
	    ThrowIf ((* vp2)->getInputs().empty(),
	             String::format ("Visibility processor %s has no inputs.", (* vp2)->getName().c_str()));

	    connect (* vp, (* vp)->getOutputs().front().getName(),
	             * vp2, (* vp2)->getInputs().front().getName());

	}

	// Connect up containers input to the input of the first VP

	ThrowIf (vps_p.front()->getInputs().empty(),
	         String::format ("First node in sequence, %s, has no inputs",
	                 vps_p.front()->getName().c_str()));

	connect (getInputs().front().getName(),
	         vps_p.front(), vps_p.front()->getInputs().front().getName());

	if (! getOutputs().empty()  && ! vps_p.back()->getOutputs().empty()){

	    // Connect up output of last node with output of container

	    connect (vps_p.back(), vps_p.back()->getInputs().front().getName(),
	             getOutputs().front().getName());

	}
}

VpContainer::ReadyVpAndData
VpContainer::findReadyVp (VpSet & vps, VpData & data, Bool flushing) const
{
    if (flushing){
        return findReadyVpFlushing (vps, data);
    }
    else{
        return findReadyVpNormal (vps, data);
    }
}

VpContainer::ReadyVpAndData
VpContainer::findReadyVpFlushing (VpSet & vpsWaiting, VpData & data) const
{

    VisibilityProcessor * readyVp = NULL;

    // The first vp in the ordered list to also be in the waiting set is the
    // one we want.

    for (VPs::const_iterator vp = vps_p.begin(); vp != vps_p.end(); vp ++){
        if (vpsWaiting.find (* vp) != vpsWaiting.end()){
            readyVp = * vp;
            break;
        }
    }

    if (readyVp == NULL){

        ThrowIf (! vpsWaiting.empty(), "Could not find ready VP during flush (bug)");

        return ReadyVpAndData (NULL, VpData());
    }

    // The set of input data will be all of the inputs desired by the node that
    // are available.  They may not be present if an upstream node didn't have any
    // data to flush out.

    VpPorts connectedInputList = readyVp -> getInputs (true);

    ReadyVpAndData result = ReadyVpAndData (readyVp, data.getSelection (connectedInputList, true));

    return result;
}

VpContainer::ReadyVpAndData
VpContainer::findReadyVpNormal (VpSet & vps, VpData & data) const
{
    ReadyVpAndData result (NULL, VpData());

    set<VpPort> dataPorts;
    for (VpData::const_iterator d = data.begin(); d != data.end(); d++){
        dataPorts.insert (d->first);
    }

    for (VpSet::const_iterator vp = vps.begin(); vp != vps.end(); vp ++){

        VpPorts connectedInputList = (* vp)->getInputs (true);

        set<VpPort> connectedInputSet (connectedInputList.begin(), connectedInputList.end());

        // Subtract from the needed input ports, the set of available data ports.
        // When the comes up empty then the VP can execute.

        VpPorts diff;
        set_difference (connectedInputSet.begin(), connectedInputSet.end(),
                        dataPorts.begin(), dataPorts.end(),
                        back_inserter (diff));

        if (diff.empty()){

            result = ReadyVpAndData (* vp, data.getSelection (connectedInputList));

            break;
        }

    }

    return result;
}

bool
VpContainer::follows (const VisibilityProcessor * a, const VisibilityProcessor * b) const
{
    // Go through the interconnection network and see if one of processor b's outputs go to
    // processor a's inputs

    Bool result = false;
    for (Network::const_iterator arc = network_p.begin(); arc != network_p.end(); arc ++){
        if (arc->first.getVp() == b && arc->second.getVp() == a){
            result = true;
            break;
        }
    }

    return result;
}

bool
VpContainer::followsSet (const VisibilityProcessor * a, const VpSet & vpSet) const
{
    Bool result = false;

    for (VpSet::const_iterator vp = vpSet.begin(); vp != vpSet.end(); vp++){
        if (follows (a, * vp)){
            result = true;
            break;
        }
    }

    return result;
}

casa::asyncio::PrefetchColumns
VpContainer::getPrefetchColumns () const
{
    PrefetchColumns result;

    for (VPs::const_iterator vp = vps_p.begin(); vp != vps_p.end(); vp ++){

        result = result + (* vp)->getPrefetchColumns();

    }

    return result;
}


void
VpContainer::orderContents ()
{
    // Order the VPs in this container using their dependencies.  Nodes that are only dependent
    // on the container will be first, then nodes dependent on the first set of nodes, etc.

    VpSet unorderedVps (vps_p.begin(), vps_p.end()); // VPs not assigned an order as of yet
    VPs orderedVps; // sorted list of VPs

    while (! unorderedVps.empty()){

        VPs nextClass;

        // Create the next class of VPs which are only dependent on previous classes of VPs.
        // These will be VPs which are not dependent on any of the currently unordered nodes.

        for (VpSet::const_iterator vp = unorderedVps.begin(); vp != unorderedVps.end(); vp ++){
            if (! followsSet (* vp, unorderedVps)){
                nextClass.push_back (* vp);
                orderedVps.push_back (* vp);
            }
        }

        // Remove the VPs that are

        for (VPs::const_iterator vp = nextClass.begin(); vp != nextClass.end(); vp++){
            unorderedVps.erase (* vp);
        }

        // If no nodes were found then there must be a cycle and the loop will never
        // terminate!

        ThrowIf (nextClass.size() == 0, String::format ("VpContainer %s contains a cycle", getName().c_str()));

    }

    vps_p = orderedVps;
}

void
VpContainer::processingStartImpl ()
{
    iterator i;
    try{
        for (i = begin(); i != end(); i++){
            (*i)->processingStart ();
        }
    }
    catch (AipsError & e){
        Rethrow (e, String::format ("Error during processingStart for container '%s' VP '%s'",
                            getName().c_str(), (*i)->getName().c_str()));
    }
}

void
VpContainer::remapPorts (VpData & data, const VisibilityProcessor * vp)
{
    vector<VpPort> oldPorts = mapKeys (data);

    for (vector<VpPort>::const_iterator oldPort = oldPorts.begin();
         oldPort != oldPorts.end();
         oldPort ++){

        Network::const_iterator newPortItr = network_p.find (* oldPort);

        if (newPortItr != network_p.end()){

            VpPort newPort = newPortItr->second;

            assert (! utilj::containsKey (newPort, data));

            data [newPort] = data [* oldPort];
            data.erase (* oldPort);

        }
        else{
            ThrowIf (true,
                     String::format ("Vp '%s' produced unused output '%s'",
                             vp->getFullName().c_str(), oldPort->getName().c_str()));
        }
    }
}

size_t
VpContainer::size() const
{
    return vps_p.size();
}



void
VpContainer::validateImpl()
{
    iterator i;
    try{
        for (i = begin(); i != end(); i++){
            (*i)->validate ();
        }
    }
    catch (AipsError & e){
        Rethrow (e, String::format ("Error during validate for container '%s' VP '%s'",
                            getName().c_str(), (*i)->getName().c_str()));
    }

    orderContents(); // put vps_p into dependency order
}

String
VpContainer::VpSet::getNames () const
{
    String nameList = utilj::join (begin(), end(), mem_fun (& VisibilityProcessor::getName), ",");

    return nameList;
}

VpData::VpData ()
{}

VpData::VpData (const VpPort & port, VbPtr vb)
{
    add (port, vb);
}

void
VpData::add (const VpPort & port, VbPtr vb)
{
    ThrowIf (utilj::containsKey (port, * this),
            String::format ("VpData::add: data already present for port %s.", port.getFullName ().c_str()));

    (* this) [port] = vb;
}

String
VpData::getNames () const
{
    string names = join (begin(), end(),
                     compose (mem_fun_ref (& VpPort::getName),
                              casa::utilj::firstFunctor<VpPort, VbPtr>()),
                     ",");

    return names;
}

VpData
VpData::getSelection (const VpPorts & ports, bool missingIsOk) const
{
    VpData result;

    for (VpPorts::const_iterator port = ports.begin(); port != ports.end(); port ++){

        const_iterator data = find (* port);

        if (data != end()){
            result [* port] = data->second;
        }
        else{
            assert (missingIsOk);
            UnusedVariable (missingIsOk);
        }
    }

    return result;
}


Int VpEngine::logLevel_p = std::numeric_limits<int>::min();
LogIO * VpEngine::logIo_p = NULL;
LogSink * VpEngine::logSink_p = NULL;
Bool VpEngine::loggingInitialized_p = false;

Bool
VpEngine::initializeLogging()
{

    AipsrcValue<Int>::find (logLevel_p, getAipsRcBase () + ".debug.logLevel",
                            std::numeric_limits<int>::min());

    if (logLevel_p >= 0){

        if (logSink_p == 0){
            logSink_p = new LogSink(LogMessage::NORMAL, false);
        }

        logIo_p = new LogIO (LogOrigin ("VisibilityProcessing"));
        * logIo_p << "VisibilityProcessing logging enabled; level=" << logLevel_p << endl << LogIO::POST;

    }

    loggingInitialized_p = true;

    return true;
}

String
VpEngine::getAipsRcBase ()
{
    return "VpFramework";
}

Int
VpEngine::getLogLevel ()
{
    return logLevel_p;
}

ROVisibilityIterator *
VpEngine::getVi ()
{
    return vi_p;
}

void
VpEngine::log (const String & formatString, ...)
{
    if (! loggingInitialized_p){
        initializeLogging ();
    }

    va_list vaList;

    va_start (vaList, formatString);

    String result = formatV (formatString.c_str(), vaList);

    va_end (vaList);

    (* logIo_p) << result << endl << LogIO::POST;
}

void
VpEngine::process (VisibilityProcessor & processor,
                   ROVisibilityIterator & vi,
                   const String & inputPortName)
{
    ThrowIf (! processor.getInputs ().contains (inputPortName),
             String::format ("VisibilityProcessor %s does not have an input port '%s'",
                            processor.getName().c_str(), inputPortName.c_str()));

    process (processor, vi, processor.getInput (inputPortName));
}

void
VpEngine::process (VisibilityProcessor & processor,
                   ROVisibilityIterator & vi,
                   const VpPort & inputPortProvided)
{
    Log (1, "VpEngine::process starting on processor '%s'", processor.getName().c_str());

    vi_p = & vi;

    VisBufferAutoPtr vbTemp (vi);
    VbPtr vb (vbTemp.release());

    VpPort inputPort = inputPortProvided;

    if (inputPort.empty()){ // Take single input to VP as default if not specified

        VpPorts inputs = processor.getInputs ();

        ThrowIf (inputs.size() != 1,
                 String::format ("Vp '%s' must have exactly one input or an input must be specified explicitly",
                         processor.getName().c_str()));

        inputPort = inputs.front();
    }

    // connect up input and then validate

    processor.processingStart ();

    Int chunkNumber = 0;
    Int subchunkNumber = 0;

    try {

        for (vi.originChunks ();
                vi.moreChunks();
                vi.nextChunk (), chunkNumber ++){

            Int iteration = 0;
            VisibilityProcessor::ChunkCode chunkCode = VisibilityProcessor::Normal;

            do {  // The VP can request repeating a chunk

                Log (2, "VpEngine::process: Starting chunk %d (iteration=%d)\n",
                     chunkNumber, iteration);

                processor.chunkStart (SubchunkIndex (chunkNumber, SubchunkIndex::Invalid, iteration));

                subchunkNumber = 0;

                for (vi.origin (); vi.more (); ++ vi, subchunkNumber ++){

                    vb->dirtyComponentsClear();
                    VisibilityProcessor::ProcessingResult ignored;

                    SubchunkIndex sci (chunkNumber, subchunkNumber, iteration);

                    Log (2, "VpEngine::process: Starting Subchunk %s \n",
                         sci.toString ().c_str());

                    VpData data (inputPort, vb);
                    ignored = processor.doProcessing (VisibilityProcessor::Subchunk,
                                                      data,
                                                      this,
                                                      sci);
                }

                VpData noData;
                VpData ignored;
                std::tie (chunkCode, ignored) =
                        processor.doProcessing (VisibilityProcessor::EndOfChunk,
                                                noData,
                                                this,
                                                SubchunkIndex (chunkNumber, SubchunkIndex::Invalid, iteration));

                iteration ++;

            } while (chunkCode == VisibilityProcessor::RepeatChunk);

        }
    }
    catch (AipsError & e){
    }

    VisibilityProcessor::ProcessingResult ignored;
    VpData noData;

    ignored = processor.doProcessing (VisibilityProcessor::EndOfData,
                                      noData,
                                      this,
                                      SubchunkIndex ());

    Log (1, "VpEngine::process completed for processor '%s'", processor.getName().c_str());

}

VpPort::VpPort()
: connectedInput_p (false),
  connectedOutput_p (false),
  name_p (""),
  visibilityProcessor_p (NULL),
  type_p (Unknown)
{}


VpPort::VpPort (VisibilityProcessor * vp, const String & name, VpPort::Type type)
: connectedInput_p (false),
  connectedOutput_p (false),
  name_p (name),
  visibilityProcessor_p (vp),
  type_p (type)
{}

Bool
VpPort::operator== (const VpPort & other) const
{
    Bool result = other.getVp() == getVp() && other.getName() == getName();

    return result;
}

Bool
VpPort::operator< (const VpPort & other) const
{
    Bool result = other.getVp() < getVp() ||
                  (other.getVp() == getVp() && other.getName() < getName());

    return result;
}


Bool
VpPort::empty () const
{
    return visibilityProcessor_p == NULL;
}

String
VpPort::getFullName () const
{
    String vpName = "*NULL*";
    if (getVp() != NULL){
        vpName = getVp()->getFullName();
    }

    return String::format ("%s:%s", vpName.c_str(), getName().c_str());
}

String
VpPort::getName () const
{
    return name_p;
}

VpPort::Type
VpPort::getType () const
{
    return type_p;
}

VisibilityProcessor *
VpPort::getVp ()
{
    return visibilityProcessor_p;
}

const VisibilityProcessor *
VpPort::getVp () const
{
    return visibilityProcessor_p;
}

Bool
VpPort::isConnectedInput () const
{
    return connectedInput_p;
}

Bool
VpPort::isConnectedOutput () const
{
    return connectedOutput_p;
}

Bool
VpPort::isType (Type t) const
{
    return (type_p & t) != 0;
}

void
VpPort::setConnectedInput ()
{
    AssertAlways (! empty() && ! connectedInput_p && isType (Input));

    connectedInput_p = true;
}

void
VpPort::setConnectedOutput ()
{
    AssertAlways (! empty() && ! connectedOutput_p && isType (Output));

    connectedOutput_p = true;
}

Bool
VpPorts::contains (const String & name) const
{
    Bool foundIt = find (name, begin(), end()) != end();

    return foundIt;
}

Bool
VpPorts::contains (const VpPort & port) const
{
    Bool foundIt = std::find (begin(), end(), port) != end();

    return foundIt;
}

VpPort
VpPorts::get (const String & name) const
{
    const_iterator i = find (name, begin(), end());
    ThrowIf (i == end(), "No such port '" + name + "'");

    return * i;
}

VpPort &
VpPorts::getRef (const String & name)
{
    iterator i = find (name, begin(), end());
    ThrowIf (i == end(), "No such port '" + name + "'");

    return * i;
}

String
VpPorts::toString () const
{

    String result;

    result = utilj::containerToString (begin(), end(), & VpPort::getName);

    return result;
}


WriterVp::WriterVp (const String & name,
                    VisibilityIterator * vi,
                    Bool advanceVi,
                    const String & input,
                    const String & output)
: VisibilityProcessor (name, vector<String> (1, input), vector<String> (1, output)),
  advanceVi_p (advanceVi),
  disableOutput_p (false),
  vi_p (vi)
{
    ThrowIf (advanceVi_p && vi == NULL,
             String::format ("Parameter advanceVi can only be true if a VI is provided for WriterVp '%s',",
                     name.c_str()));
}

VisibilityProcessor::ProcessingResult
WriterVp::doProcessingImpl (ProcessingType /*processingType*/,
                            VpData & inputData,
                            const SubchunkIndex & /*subChunkIndex*/)
{
    if (inputData.empty()){
        return ProcessingResult();  // Nothing to write
    }

    VpPort inputPort = getInputs () [0];

    ThrowIf (! utilj::containsKey (inputPort, inputData),
             String::format ("Input data not found for port '%s' in VP '%s'",
                     inputPort.getName().c_str(),
                     getName().c_str()));

    // Get the (writable) VisibilityIterator

    VisibilityIterator * vi = vi_p;

    if (vi == NULL){

        // If a VI wasn't provided, then use the one being swept by the VI engine

        vi = dynamic_cast <VisibilityIterator *> (getVi());
    }


    ThrowIf (vi == NULL, String::format ("No writable VI found in VP '%s'", getName().c_str()));

    // Write out the data to the VI



    try{
        if (! disableOutput_p){
            vi->writeBack (inputData [inputPort].get());

            if (advanceVi_p){

                // Advance VI to the next position.  If the current chunk is exhausted then
                // advance the chunk and reset to the chunk's origin.

                (* vi) ++;
                if (! vi->more()){
                    vi->nextChunk();
                    if (vi->moreChunks()){
                        vi->origin();
                    }
                }
            }
        }
    }
    catch (AipsError & e){
        Rethrow (e, String::format ("While '%s' writing VB to disk", getName().c_str()));
    }

    inputData [inputPort] -> dirtyComponentsClear();

    // Output the data if the output of this VP is connected.

    VpData outputData; // Start out with empty outputs

    VpPorts outputs = getOutputs(true); // get connected outputs

    if (! outputs.empty()){

        // An output was connected.

        outputData [outputs [0]] = inputData [inputPort];
    }

    ProcessingResult processingResult (Normal, outputData);

    return processingResult;
}

Bool
WriterVp::setDisableOutput (Bool disableIt)
{
    Bool old = disableOutput_p;
    disableOutput_p = disableIt;

    return old;
}

void
WriterVp::validateImpl()
{
    throwIfAnyInputsUnconnected ();
}



ostream &
operator<< (ostream & os, const VisibilityProcessor::ProcessingType & processingType)
{
    os << toString (processingType);

    return os;
}


} // end namespace vpu

using namespace casacore;
} // end namespace casa
