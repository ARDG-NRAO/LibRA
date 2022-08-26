//# grpcInteractiveClean.h: perform interactive masking using grpc + casa viewer
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef SYNTHESIS_GRPCINTERACTIVEITERATION_H
#define SYNTHESIS_GRPCINTERACTIVEITERATION_H

#include <list>
#include <mutex>
#include <thread>
#include <stdcasa/record.h>
#include <casacore/casa/Containers/Record.h>
#include "img.grpc.pb.h"

namespace casa {

    struct grpcInteractiveCleanState {
        grpcInteractiveCleanState( );
        void reset( );

        int    Niter;
        int    CycleNiter;
        int    InteractiveNiter;

        float Threshold;
        float CycleThreshold;
        float InteractiveThreshold;
	  
        bool IsCycleThresholdAuto;
        bool IsThresholdAuto;
        // Determines if IsCycleThresholdAuto can ever be true.
        // Necessary because tclean implicitly updates IsCycleThresholdAuto during certain grpcInteractiveClean functions, but for deconvolve we don't want this value to change.
        bool IsCycleThresholdMutable;

        float CycleFactor;
        float LoopGain;
    
        bool  StopFlag;
        bool  PauseFlag;
        bool  InteractiveMode;
        bool  UpdatedModelFlag;

        int   IterDone;
        int   InteractiveIterDone;
        int   MaxCycleIterDone;
        int   MajorDone;
        int   StopCode;

        //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        float Nsigma; 
        float MaxPsfSidelobe;
        float MinPsfFraction;
        float MaxPsfFraction;
        float PeakResidual;

        //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        casacore::Array<double> SummaryMinor;
        casacore::Array<int>    SummaryMajor;

        //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        float MinorCyclePeakResidual;
        float PrevPeakResidual;
        float NsigmaThreshold;

        int PrevMajorCycleCount;
        float PeakResidualNoMask;
        float PrevPeakResidualNoMask;
        float MinPeakResidualNoMask;
        float MinPeakResidual;
        float MaskSum;

        //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        float MadRMS;

    };

    class grpcInteractiveCleanGui {
    private:
        struct CleanState {
            int image_id;
            int prev_image_id;
            int mask_id;
            int prev_mask_id;
            CleanState( ) : image_id(0), prev_image_id(0),
                            mask_id(0), prev_mask_id(0) { }
        };

        pid_t viewer_pid;
        bool viewer_started;
        std::string viewer_uri;
        std::unique_ptr<rpc::img::view::Stub> viewer_proxy;
        std::map<int,CleanState> clean_state;

    protected:
        std::string get_distro_data_path( );
        std::string get_python_path( );
        std::string get_viewer_path( );
        std::string get_fifo( );

        bool spawn_viewer( );
        bool stop_viewer( );
        bool launch( );

        void unload( int );
        static bool clone( const std::string &imageName, const std::string &newImageName );
        static float maskSum( const std::string &maskname );

    public:
        grpcInteractiveCleanGui( );
        ~grpcInteractiveCleanGui( );

        bool alive( );
        int open_panel( std::list<std::tuple<std::string,bool,bool,int>> images );
        void close_panel( int );
        int interactivemask( int panel, const std::string &image, const std::string &mask,
                             int &niter, int &cycleniter, std::string &thresh,
                             std::string &cyclethresh, const bool forceReload=false );

    };

    class grpcInteractiveCleanManager {
    private:
        grpcInteractiveCleanState state;
        std::mutex state_mtx;
        grpcInteractiveCleanGui gui;

        // filled at start of interaction... zero out after interactive clean completes...
        int clean_panel_id;
        // image name, multiterm, finished, exit code (if finished)
        std::list<std::tuple<std::string,bool,bool,int>> clean_images;

    protected:
        template<typename T> T access( T data, std::function< T ( T, grpcInteractiveCleanState & )> csect ) {
            std::lock_guard<std::mutex> state_lck(state_mtx);
            return csect(data,state);
        }

        void pushDetails( );
        void updateCycleThreshold( grpcInteractiveCleanState & );
        void resetMinorCycleInitInfo( grpcInteractiveCleanState & );
        void mergeMinorCycleSummary( const casacore::Array<casacore::Double>&, grpcInteractiveCleanState &, casacore::Int immod );

        /**********
        casacore::Int interactivemask(const casacore::String& image, const casacore::String& mask, 
			casacore::Int& niter, casacore::Int& cycleniter, casacore::String& thresh, casacore::String& cyclethresh, 
			const casacore::Bool forceReload=false);
        ******/
    public:
        grpcInteractiveCleanManager( ) : clean_panel_id(-1) { }

        casacore::Record getMinorCycleControls( );
        casacore::Record getDetailsRecord( bool includeSummary=true );
        casacore::Record getSummaryRecord( ) { return getDetailsRecord(true); }
        int cleanComplete( bool lastcyclecheck=false, bool reachedMajorLimit=false );
        /* Note:  Incrementing the Major cycle will reset the cycleIterDone */
        void incrementMajorCycleCount( );
        void resetMinorCycleInitInfo( );
        void addSummaryMajor( );

        void mergeCycleInitializationRecord( casacore::Record &initRecord );
        void mergeCycleExecutionRecord( casacore::Record&, casacore::Int immod );

        void changeStopFlag( bool stopEnabled );

        void setControlsFromRecord(const casac::record &);
        void setControls( int niter, int ncycle, float threshold );

        void setIterationDetails(const casac::record &);
        casacore::Record pauseForUserInteraction( );
        void closePanel( );

    };

    struct grpcInteractiveClean {
        static grpcInteractiveCleanManager &getManager( );
    };

} //# NAMESPACE CASA - END

#endif

