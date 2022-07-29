/*
 * VisibilityProcessing.h
 *
 *  Created on: Feb 8, 2011
 *      Author: jjacobs
 */

#ifndef VISIBILITYPROCESSING_H_
#define VISIBILITYPROCESSING_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Exceptions/Error.h>
#include <stdcasa/UtilJ.h>
#include "VisBuffer.h"
#include "VisibilityIterator.h"

#include <memory>
#include <tuple>
#include <map>
#include <set>
#include <vector>


/*

Visibility Processing Framework Class Summary
=============================================

SubchunkIndex - Index of a subchunk.  Consists of the chunk number,
                the subchunk number and the iteration number.  All three
                are zero-based.  The interation number is nonzero if a chunk
                is reprocessed. A subchunk is used to identify a VisBuffer
                relative to the VisibilityIterator managed by the VpEngine.
VbPtr - Smart pointer of a VisBuffer.
VisibilityProcessor - A visibility processing node in data flow graph
    VisibilityProcessorStub - A do-nothing node used for unit testing
    VpContainer - A VP which contains a graph of VPs.  It handles moving
                  data between its input and output ports to the appropriate
                  input and output ports of the data flow graph it contains.
    SplitterVp - Has a single input port and outputs identical copies of it
                 through its output ports.
    WriterVp - Takes an input and writes it out to the VisibilityIterator provided
               when it was constructed.  Optionally passes the input data to its
               output port.
VpEngine - Object that executes a data flow graph of VisibilityProcessors on data
           accessed via a VisibilityIterator.

VpPort - A data port into or out of (or both) a VisibiltyProcessor
VpPorts - A collection of VpPort objects
VpData - A collection of visibility data; it works like an associative array
         pairing a VpPort with a VisBuffer.

*/

namespace casa {

namespace asyncio {
    class PrefetchColumns;
};

namespace vpf {

class VisibilityProcessor;
class VpContainer;
class VpEngine;

class SubchunkIndex {

    friend class SubchunkIndex_Test;

public:

    enum {Invalid = -1};

    SubchunkIndex (casacore::Int chunkNumber = Invalid, casacore::Int subChunkNumber = Invalid, casacore::Int iteration = Invalid);

    // Comparison Operators
    //
    // Comparison is in lexicographic order by chunk, subchunk and iteration.

    casacore::Bool operator< (const SubchunkIndex & other) const;
    casacore::Bool operator== (const SubchunkIndex & other) const { return ! (* this < other || other < * this);}
    casacore::Bool operator!= (const SubchunkIndex & other) const { return ! (* this == other);}

    casacore::Int getChunkNumber () const;
    casacore::Int getIteration () const;
    casacore::Int getSubchunkNumber () const;

    casacore::String toString () const;

private:

    casacore::Int chunkNumber_p;        // -1 for invalid
    casacore::Int iteration_p;          // -1 for invalid
    casacore::Int subChunkNumber_p;
};

class VbPtr : public std::shared_ptr<casa::VisBuffer> {

public:

    VbPtr () : std::shared_ptr<casa::VisBuffer> () {}
    explicit VbPtr (casa::VisBuffer * vb) : std::shared_ptr<casa::VisBuffer> (vb) {}

    // Assignment operator setting VbPtr to a normal pointer.  Ownership is passed to the
    // VbPtr so caller must ensure that delete is not called on the VisBuffer.

    VbPtr & operator= (casa::VisBuffer * vb)
    {
        std::shared_ptr<casa::VisBuffer>::operator= (VbPtr (vb));
        return * this;
    }
};

class VpPort {

    friend class VpContainer;
    friend class VpPort_Test;

public:

    // Normally ports are either input or output ports.  However, the ports
    // of a VpContainer do double duty serving as an input to the container and
    // an outputted to the input of a contained VP, or vice versa.

    typedef enum {Unknown, Input = 1, Output = 2, InOut = Input | Output} Type;

    VpPort ();
    VpPort (VisibilityProcessor * vp, const casacore::String & name, Type type);
    ~VpPort () {}

    casacore::Bool operator< (const VpPort & other) const;
    casacore::Bool operator== (const VpPort & other) const;

    casacore::Bool empty () const;
    casacore::String getFullName () const; // returns Vp0.Vp1...VpN.portName
    casacore::String getName () const; // returns portName
    Type getType () const; // Returns the port's type as something from the Type enum
    casacore::Bool isConnectedInput () const; // true if port has been connected up as an input
    casacore::Bool isConnectedOutput () const; // true if port has been connected up as an output

    // Used to check the type of the port as defined in the Type enum.  InOut ports
    // return true for both casacore::Input and Output types.

    bool isType (Type t) const;

    //casacore::String toString() const;

protected:

    const VisibilityProcessor * getVp () const;
    VisibilityProcessor * getVp ();
    void setConnectedInput ();
    void setConnectedOutput ();

private:

    casacore::Bool connectedInput_p;
    casacore::Bool connectedOutput_p;
    casacore::String name_p;
    VisibilityProcessor * visibilityProcessor_p; // [use]
    Type type_p;

};

class VpPorts : public std::vector<VpPort> {

    friend class VisibilityProcessor;
    friend class VpPorts_Test;

public:

    casacore::Bool contains (const casacore::String & name) const;
    casacore::Bool contains (const VpPort & port) const;
    VpPort get (const casacore::String & name) const;
    casacore::String toString () const;

protected:

    VpPort & getRef (const casacore::String & name);

    template <typename Itr>
    static
    Itr
    find(const casacore::String & name, Itr begin, Itr end)
    {
        Itr i;

        for (i = begin; i != end; i++){
            if (i->getName() == name){
                break;
            }
        }

        return i;
    }

};

namespace asyncio {
    class PrefetchColumns;
}


class VpData: public std::map<VpPort, VbPtr> {

    friend class VpData_Test;

public:

    VpData ();
    VpData (const VpPort & port, VbPtr);

    void add (const VpPort & port, VbPtr); // Adds a (port,VbPtr) to the collection

    // Returns the (port,VbPtr) pairs for the requested set of ports.  An execption
    // is thrown if a requested port is not present unless missingIsOk is set to true.

    VpData getSelection (const VpPorts &, bool missingIsOk = false) const;
    casacore::String getNames () const; // Returns a comma-separated list of the port names.
};


class VisibilityProcessor {

    friend class VpContainer;
    friend class WriterVp;

public:

    VisibilityProcessor( const VisibilityProcessor& ) = delete;
    VisibilityProcessor& operator=( const VisibilityProcessor& ) = delete;

    typedef enum {
        Normal,
        RepeatChunk
    } ChunkCode;

    typedef enum {
        Subchunk,    // casacore::Normal processing of a subchunk
        EndOfChunk,  // Called after all subchunks of a chunk have been processed
        EndOfData    // Called after all chunks have been processed
    } ProcessingType;

    typedef std::tuple <ChunkCode, VpData> ProcessingResult;

    VisibilityProcessor ();
    VisibilityProcessor (const casacore::String & name,
                         const std::vector<casacore::String> & inputNames,
                         const std::vector<casacore::String> & outputNames = std::vector<casacore::String>(),
                         casacore::Bool makeIoPorts = false);
    virtual ~VisibilityProcessor () {}

    // chunkStart is called to inform the VP that a new chunk is starting.

    void chunkStart (const SubchunkIndex &);

    // Called to cause the VP to process the provided inputs.  It will be called
    // in three different contexts as indicated by the ProcessingType.

    ProcessingResult doProcessing (ProcessingType processingType,
                                   VpData & inputData,
                                   VpEngine * vpEngine,
                                   const SubchunkIndex & subChunkIndex);

    // Returns a pointer to the containing VP or NULL if this VP is top-level.

    const VpContainer * getContainer () const { return NULL;}

    // The full name of a VP is a dotted list of the names of all the containing
    // VPs ending with the name of this VP (e.g., vp0.vp1...vpN.thisVp).

    casacore::String getFullName () const;

    // Returns the input port having the specified name.  Exception if port is undefined.

    VpPort getInput (const casacore::String & name) const;

    // Returns a collection of the input ports for this VP; optionally only the
    // connected ports are returned.

    VpPorts getInputs (casacore::Bool connectedOnly = false) const;

    // Returns the name of this VP

    casacore::String getName () const;

    // Returns the number of Subchunks processed (mainly for testing)

    casacore::Int getNSubchunksProcessed () const;

    // Returns the number of unique Subchunks (i.e., iteration ignored) processed.
    // (mainly for testing)

    casacore::Int getNSubchunksUniqueProcessed () const;

    // Returns the output port having the specified name.  Exception if port is undefined.

    VpPort getOutput (const casacore::String & name) const;

    // Returns a collection of the output ports for this VP; optionally only the
    // connected ports are returned.

    VpPorts getOutputs (casacore::Bool connectedOnly = false) const;

    // Returns the collection of columns that need to be prefetched if this node
    // is used with async I/O.

    virtual casa::asyncio::PrefetchColumns getPrefetchColumns () const;

    // Called by the framework when the processing is about to begin (i.e., prior
    // to the first VisBuffer being fed into the graph.

    void processingStart ();

    // Called to ask the VP to check its validity (i.e., are all needed inputs connected,
    // etc.).

    void validate ();

protected:

    // The public API contains many methods that are not virtual.  However, where subclass-specific
    // behavior is potentially useful, a corresponding xxxImpl method is provided.  This allows the
    // framework to perform certain required housekeeping options while allowing the subclass
    // to perform custom operations.

    // Called on the object when a new chunk is about to be started.

    virtual void chunkStartImpl (const SubchunkIndex &) {}


    // Defines the set of possible input ports for this VP

    VpPorts definePorts (const std::vector<casacore::String> & portNames, VpPort::Type type, const casacore::String & typeName);

    // Requests processing of the provided (possibly empty) input data.  This is called on each
    // subchunk (then inputData will be nonempty) and at the end of a chunk and the end of the
    // entire data set.  These last two call types allow the VP to output any data that it might have
    // been accumulating across multiple subchunks, etc.

    virtual ProcessingResult doProcessingImpl (ProcessingType processingType,
                                               VpData & inputData,
                                               const SubchunkIndex & subChunkIndex) = 0;

    // Returns a collection of the ports that are not connected using the provided connection
    // method; some ports may also be excluded from this list by name.

    VpPorts portsUnconnected (const VpPorts & ports, casacore::Bool (VpPort::* isConnected) () const,
                              const std::vector<casacore::String> & except = std::vector<casacore::String> ()) const;

    // Called when data processing is about to beging; this allows the VP to perform any
    // initialization that it desires now that it is completely connected into the graph.

    virtual void processingStartImpl () {}

    // Methods to ease the validation process.

    void throwIfAnyInputsUnconnected (const std::vector<casacore::String> & exceptThese = std::vector<casacore::String> ()) const;
    void throwIfAnyInputsUnconnectedExcept (const casacore::String & exceptThisOne) const;
    void throwIfAnyOutputsUnconnected (const std::vector<casacore::String> & exceptThese = std::vector<casacore::String> ()) const;
    void throwIfAnyOutputsUnconnectedExcept (const casacore::String & exceptThisOne) const;
    void throwIfAnyPortsUnconnected () const;

    // Called to allow the node to validate its initial state.  An casacore::AipsError should be thrown if
    // this node decides that it is invalid.

    virtual void validateImpl () = 0;

private:

    VpPort & getInputRef (const casacore::String & name);
    VpPort & getOutputRef (const casacore::String & name);
    void setContainer (const VpContainer *);

    ROVisibilityIterator * getVi (); // returns the VI used for this data set
    VpEngine * getVpEngine(); // returns the engine executing this VP

    const VpContainer * container_p; // [use]
    casacore::String name_p; // name of this VP
    casacore::Int nSubchunks_p; // number of subchunks processed
    casacore::Int nSubchunksUnique_p; // number of unique subchunks processed
    VpEngine * vpEngine_p; // pointer to VpEngine processing this VP (can be null)
    VpPorts vpInputs_p; // collection of input ports
    VpPorts vpOutputs_p; // collection of output ports
};

std::ostream & operator<< (std::ostream & os, const VisibilityProcessor::ProcessingType & processingType);
casacore::String toString (VisibilityProcessor::ProcessingType p);

class VisibilityProcessorStub : public VisibilityProcessor {

    // Used to allow definition of a VP variable for use in testing.
    // Should never be actually operated on.

public:

    VisibilityProcessorStub (const casacore::String & name)
    : VisibilityProcessor (name, utilj::Strings(), utilj::Strings())
    {}

    ProcessingResult doProcessingImpl (ProcessingType /*processingType*/,
                                       VpData & /*inputData*/,
                                       const SubchunkIndex & /*subChunkIndex*/);

    void validateImpl ();


};

//class SimpleVp: public VisibilityProcessor {
//
//public:
//
//    SimpleVp (const casacore::String & name, const casacore::String & input = "In", const casacore::String & output = "");
//    virtual ~SimpleVp ();
//
//protected:
//
//    class SimpleResult : public std::tuple<ChunkCode, VisBuffer *> {};
//
//    virtual ProcessingResult doProcessingImpl (ProcessingType processingType,
//                                               VpData & inputData,
//                                               const SubchunkIndex & subChunkIndex);
//    virtual void validateImpl ();
//
//private:
//
//};

class SplitterVp : public VisibilityProcessor {

public:

    SplitterVp (const casacore::String & name,
                const casacore::String & inputName,
                const std::vector<casacore::String> & outputNames);

    ~SplitterVp () {}

protected:

    ProcessingResult doProcessingImpl (ProcessingType processingType ,
                                       VpData & inputData,
                                       const SubchunkIndex & subChunkIndex);

    void validateImpl ();
};

class WriterVp: public VisibilityProcessor {

public:

    // Creates a WriterVp node.  If the vi argument is NULL then the
    // flow graph's VI is used.  The advanceVi argument is used to
    // direct the node to advance the VI after each write (i.e., perform
    // a vi++ operation); advancing the flow graph's VI will cause a
    // run time exception.

    WriterVp (const casacore::String & name,
              VisibilityIterator * vi = NULL,
              casacore::Bool advanceVi = false,
              const casacore::String & input = "In",
              const casacore::String & output = "Out");

    // This paradoxical method allows the user to create a single data flow graph
    // and then programmatically decide at run time whether data should be actually
    // output on this particular run.

    casacore::Bool setDisableOutput (casacore::Bool disableIt);

protected:

    ProcessingResult doProcessingImpl (ProcessingType processingType,
                                       VpData & inputData,
                                       const SubchunkIndex & subChunkIndex);

    void validateImpl ();

private:

    casacore::Bool advanceVi_p; // true is VI is to be advanced after each write.
                      // N.B., advancing the flow graphs VI is prohibited
    casacore::Bool disableOutput_p; // true if output is disabled.
    VisibilityIterator * vi_p; // VI to use for output.
};

class VpContainer : public VisibilityProcessor {

    friend class VisibilityProcessing;

public:

    // Creates a VpContainer object providing the specified inputs and outputs.
    // These inputs and outputs will potentially be connected to the inputs and
    // outputs of the VPs that are contained in the container.

    VpContainer (const casacore::String & name,
                 const std::vector<casacore::String> & inputs = std::vector<casacore::String> (1, "In"),
                 const std::vector<casacore::String> & outputs = std::vector<casacore::String> ());

    virtual ~VpContainer () {}

    // Adds a VP to the container.  Exception if VP is already in the container.

    virtual void add (VisibilityProcessor * processor);

    // Connects the specified output to the specified input.  The VP pointer may be
    // omitted if the port belongs to the container.

    virtual void connect (VisibilityProcessor * sourceVp, const casacore::String &  sourcePortName,
                          VisibilityProcessor * sinkVp, const casacore::String &  sinkPortName);
    virtual void connect (const casacore::String &  sourcePortName,
                          VisibilityProcessor * sinkVp, const casacore::String &  sinkPortName);
    virtual void connect (VisibilityProcessor * sourceVp, const casacore::String &  sourcePortName,
                          const casacore::String &  sinkPortName);

    virtual void chunkStart (const SubchunkIndex & sci);

    // Fills the container with the specified set of VPs.  The container must be
    // empty prior to this call.

    virtual void fillWithSequence (VisibilityProcessor * first, ...); // Last one NULL

    // Returns the columns that are required to be prefetched if async I/O is used.

    virtual casa::asyncio::PrefetchColumns getPrefetchColumns () const;

protected:

    typedef std::vector<VisibilityProcessor *> VPs; // VPs are used (not owned)
    typedef VPs::const_iterator const_iterator;
    typedef VPs::iterator iterator;

    iterator begin();
    const_iterator begin() const;

    casacore::Bool contains (const VisibilityProcessor *) const;
    virtual ProcessingResult doProcessingImpl (ProcessingType processingType,
                                               VpData & inputData,
                                               const SubchunkIndex & subChunkIndex);
    casacore::Bool empty () const;
    iterator end();
    const_iterator end() const;
    virtual void processingStartImpl ();
    size_t size() const;
    virtual void validateImpl ();

private:

    typedef std::map<VpPort, VpPort> Network;
    typedef std::set<VpPort> NetworkReverse;
    typedef std::tuple<VisibilityProcessor *, VpData> ReadyVpAndData;

    class VpSet : public std::set<VisibilityProcessor *> {
    public:

        template <typename In>
        VpSet (In begin, In end) : std::set<VisibilityProcessor *> (begin, end) {}
        casacore::String getNames () const;
    };

    Network network_p; // connections between the ports of the connected nodes
    NetworkReverse networkReverse_p; // connections of contets except indexed in
                                     // backwards order.
    VPs vps_p; // the VPs contained by this container.

    ReadyVpAndData findReadyVp (VpSet & vpsWaiting, VpData & inputs, bool flushing) const;
    ReadyVpAndData findReadyVpFlushing (VpSet & vpsWaiting, VpData & inputs) const;
    ReadyVpAndData findReadyVpNormal (VpSet & vpsWaiting, VpData & inputs) const;
    bool follows (const VisibilityProcessor * a, const VisibilityProcessor * b) const;
    bool followsSet (const VisibilityProcessor * a, const VpSet & vpSet) const;
    void orderContents ();
    void remapPorts (VpData & data, const VisibilityProcessor *);
    std::pair<VpPort,VpPort> validateConnectionPorts (VisibilityProcessor * sourceVp,
                                                 const casacore::String &  sourcePortName,
                                                 VisibilityProcessor * sinkVp,
                                                 const casacore::String &  sinkPortName);
};

class VpEngine {

    friend class VisibilityProcessor;

public:

    VpEngine () : vi_p (NULL) {}

    // Process the data set swept by the VisibilityIterator using the
    // VisibilityProcessor provided with the optionally specified port
    // as the input.

    void process (VisibilityProcessor & processor,
                  ROVisibilityIterator & vi,
                  const casacore::String & inputPortName);

    void process (VisibilityProcessor & processor,
                  ROVisibilityIterator & vi,
                  const VpPort & inputPort = VpPort ());

    static casacore::Int getLogLevel ();
    static void log (const casacore::String & format, ...);
    static casacore::String getAipsRcBase ();

private:

    ROVisibilityIterator * vi_p; // [use]

    static casacore::Int logLevel_p;
    static casacore::LogIO * logIo_p;
    static casacore::Bool loggingInitialized_p;
    static casacore::LogSink * logSink_p;

    static casacore::Bool initializeLogging ();

    ROVisibilityIterator * getVi ();

};

} // end namespace vpu

} // end namespace casa


/*

    VisibilityProcessor vp1;
    VisibilityProcessor vp2;
    VpuContainer vpc1;

    vpc1.add (vp1);
    vpc1.add (vp2);

    vpc1.connect (vp1.getOutput (Out), vp2.getInput (In));
    vpc1.connect (vpc1.getInput (In), vp1.getInput (In));
    vpc1.connect (vp2.getOutput (Out), vpc1.getOutput (Out));

    VpuContainer vpc2;
    VpuContainer vpc0;

    vpc0.add (vpc1, vpc2);
    vpc0.connect (vpc1.getOutput (Out), vpc2.getOutput (In));
    vpc0.connect (vpc0.getOutput (In), vpc1.getInput (In));
    vpc0.connect (vpc1.getOutput (Out), vpc0.getOutput (Out));

    vpc0.validate ();

 */



#endif /* VISIBILITYPROCESSING_H_ */
