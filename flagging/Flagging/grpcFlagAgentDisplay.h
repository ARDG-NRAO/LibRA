//# FlagAgentDisplay.h: This file contains the interface definition of the FlagAgentDisplay class.
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

#ifndef FlagAgentDisplay_H_
#define FlagAgentDisplay_H_

#include <mutex>
#include <vector>
#include <future>
#include <flagging/Flagging/FlagAgentBase.h>
#ifdef USE_GRPC
#include <grpc++/server.h>
#include "plotserver.grpc.pb.h"
#include "plotserver_events.grpc.pb.h"

namespace casa { //# NAMESPACE CASA - BEGIN

    struct grpcFlagAgentState {
        grpcFlagAgentState( );
        // GUI parameters
        std::string userChoice_p;
        std::string userFixA1_p, userFixA2_p;
        casacore::Int skipScan_p, skipSpw_p, skipField_p;

        std::string antenna1_p;
        std::string antenna2_p;

        std::mutex set_values;			// protect state within object
        bool input_received;			// state object has received input
        bool input_needed;				// chaged to true when the controlling
										// thread is waiting for input
        std::promise<bool> output;		// control thread needs input
    };

    class grpcFlagAgentResponse : public ::rpc::gui::plotserver_events::Service {
    public:
        grpcFlagAgentResponse( std::shared_ptr<grpcFlagAgentState> s ) : state(s) { }

        ::grpc::Status button( ::grpc::ServerContext *context,
                               const ::rpc::gui::ButtonEvent *req,
                               ::google::protobuf::Empty* );
        ::grpc::Status check( ::grpc::ServerContext *context,
                              const ::rpc::gui::CheckEvent *req,
                              ::google::protobuf::Empty* );
        ::grpc::Status radio( ::grpc::ServerContext *context,
                              const ::rpc::gui::RadioEvent *req,
                              ::google::protobuf::Empty* );
        ::grpc::Status linetext( ::grpc::ServerContext *context,
                                 const ::rpc::gui::LineTextEvent *req,
                                 ::google::protobuf::Empty* );
        ::grpc::Status slidevalue( ::grpc::ServerContext *context,
                                   const ::rpc::gui::SlideValueEvent *req,
                                   ::google::protobuf::Empty* );
        ::grpc::Status exiting( ::grpc::ServerContext *context,
                                const ::google::protobuf::Empty*,
                                ::google::protobuf::Empty* );
        ::grpc::Status closing( ::grpc::ServerContext *context,
                                const ::rpc::gui::ClosingEvent *req,
                                ::google::protobuf::Empty* );
    protected:
        std::shared_ptr<grpcFlagAgentState> state;
    };

    class FlagAgentDisplay : public FlagAgentBase {
    public:
      
        FlagAgentDisplay(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false);
        ~FlagAgentDisplay();

        // Make plots and either display or write to a file
        casacore::Bool displayReports(FlagReport &combinedReport);
    
        // Get a report/summary
        FlagReport getReport();
    
    protected:

        std::string discover_executable( );

        struct plotter_t {
            bool active( ) const { return active_; }
            pid_t pid;
            std::string plot_uri;
            std::unique_ptr<rpc::gui::plotserver::Stub> plot;
            std::unique_ptr<rpc::gui::plotserver_events::Service> response;
            bool launch(std::string plotserver_path);
            plotter_t(std::shared_ptr<grpcFlagAgentState> state);
        protected:
            bool active_;
            bool start_response_manager( );
            std::string get_casaplotserver_path( ) const;
            std::string get_distro_data_path( ) const;
            std::string get_fifo( ) const;
            std::unique_ptr<grpcFlagAgentResponse> response_svc;
            std::unique_ptr<grpc::Server> response_server;
            std::string response_uri;
            bool plot_started_;
        };

        // shutdown a plotserver process
        bool done( std::shared_ptr<plotter_t> plotter );
        // create panel
        int create_panel( std::shared_ptr<plotter_t> plot, int parent, bool new_row );
        // erase
        void erase( std::shared_ptr<plotter_t> plot, int panel );
        // set labels
        void setlabel( std::shared_ptr<plotter_t> plot, int panel, std::string xlabel, std::string ylabel, std::string title );
        // load dock
        int create_dock( std::shared_ptr<plotter_t> plot, int panel, std::string xml );
        int raster( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &data, ssize_t sizex, ssize_t sizey );
        int line( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &xdata, const std::vector<float> &ydata, std::string color, std::string label );
        int scatter( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &xdata,
                     const std::vector<float> &ydata, std::string color, std::string label,
                     std::string symbol, int symbol_size, int dot_size );

        // Compute flags for a given (time,freq) map
        bool computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer, VisMapper &visibilities,FlagMapper &flag,casacore::Int antenna1,casacore::Int antenna2,std::vector<casacore::uInt> &rows);

        void preProcessBuffer(const vi::VisBuffer2 &visBuffer);
    
        // Choose how to step through the baselines in the current chunk
        void iterateAntennaPairsInteractive(antennaPairMap *antennaPairMap_ptr);
    
        // Parse configuration parameters
        void setAgentParameters(casacore::Record config);

    private:

        static constexpr int TIMEOUT = 80000;
    
        casacore::Bool setDataLayout();
        casacore::Bool setReportLayout();
        casacore::Bool buildDataPlotWindow();
        casacore::Bool buildReportPlotWindow();


        void getChunkInfo(const vi::VisBuffer2 &visBuffer);
        casacore::Bool skipBaseline(std::pair<casacore::Int,casacore::Int> antennaPair);
 
        void getUserInput();
        void getReportUserInput();
    
        void DisplayRaster(casacore::Int xdim, casacore::Int ydim, casacore::Vector<casacore::Float> &data, int frame);
        void DisplayLine(casacore::Int xdim, casacore::Vector<casacore::Double> &xdata, casacore::Vector<casacore::Float> &ydata, casacore::String label, casacore::String color, casacore::Bool hold, int frame);
        void DisplayScatter(casacore::Int xdim, casacore::Vector<casacore::Double> &xdata, casacore::Vector<casacore::Float> &ydata, casacore::String label, casacore::String color, casacore::Bool hold, int frame);
        void DisplayLineScatterError(std::shared_ptr<plotter_t> plotter, casacore::String &plottype, casacore::Vector<casacore::Float> &xdata, casacore::Vector<casacore::Float> &ydata, casacore::String &errortype, casacore::Vector<casacore::Float> &error, casacore::String label, casacore::String color, int frame);

        // Plotter members
        std::shared_ptr<grpcFlagAgentState> gui_state;
        std::shared_ptr<plotter_t> dataplotter_p;
        std::shared_ptr<plotter_t> reportplotter_p;
        bool active( ) const { return (dataplotter_p && dataplotter_p->active( )) || (reportplotter_p && reportplotter_p->active( )); }

        // Control parameters
        casacore::Bool pause_p;

        // visBuffer state variables
        casacore::Int fieldId_p;
        casacore::String fieldName_p;
        casacore::Int scanStart_p;
        casacore::Int scanEnd_p;
        casacore::Int spwId_p;
        casacore::uInt nPolarizations_p;
        casacore::Vector<casacore::Double> freqList_p;

        casacore::Bool dataDisplay_p, reportDisplay_p; // show per chunk plots and/or end-of-casacore::MS plots
        casacore::String reportFormat_p;
        casacore::Bool stopAndExit_p;
        // casacore::Bool reportReturn_p;
        casacore::Bool showBandpass_p;

        std::vector<int> panels_p;
        std::vector<int> report_panels_p;

        const char *dock_xml_p, *report_dock_xml_p;
//    casacore::Bool showBandpass_p;

    // Control parameters
//    casacore::Bool stopAndExit_p;
//    casacore::Bool pause_p;
//    casacore::Bool dataDisplay_p, reportDisplay_p; // show per chunk plots and/or end-of-casacore::MS plots
//    casacore::Bool reportReturn_p;
//    casacore::String reportFormat_p;
    
    // visBuffer state variables
//    casacore::Int fieldId_p;
//    casacore::String fieldName_p;
//    casacore::Int scanStart_p;
//    casacore::Int scanEnd_p;
//    casacore::Int spwId_p;
//    casacore::String antenna1_p;
//    casacore::String antenna2_p;
    
//    casacore::uInt nPolarizations_p;
//    casacore::Vector<casacore::Double> freqList_p;
    
    // GUI parameters
//    casacore::String userChoice_p;
//    casacore::String userFixA1_p, userFixA2_p;
    
//    casacore::Int skipScan_p, skipSpw_p, skipField_p;

        casacore::Vector<casacore::String> plotColours_p;
    
    
    };
  
  
} //# NAMESPACE CASA - END

#endif //# USE_GRPC

#endif /* FLAGAGENTDISPLAY_H_ */

