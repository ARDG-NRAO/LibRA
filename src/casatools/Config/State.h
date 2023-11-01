//# State.h: application state for CASAtools python module
//# Copyright (C) 2017
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
//#                        Charlottesville, VA 22903-2475 USA
//#
#ifndef CASATOOLS_CONFIG_STATE_H
#define CASATOOLS_CONFIG_STATE_H
#include <sys/stat.h>
#include <list>
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits.h>
#include <stdlib.h>
#include <casacore/casa/System/AppState.h>
#include <casatools/Proc/Registrar.h>

namespace casatools {   /** namespace for CASAtools classes within "CASA code" **/

    class State: public casacore::AppState {
      public:

        State( ) { }

        virtual bool initialized( ) const {
            return true;
        }

        // use the data path to find the filename...
        virtual std::string resolve(const std::string &filename) const;

        virtual std::list<std::string> dataPath( ) const {
            return data_path;
        }

        // get directory containing IERS measures data
        // an exception is thrown if it does not exist or
        // does not contain the IERS tables
        //   * should contain IERS tables
        std::string measuresDir( ) const {
            return distro_data_path;
        }

        virtual std::string pythonPath( ) const {
            return python_path;
        }

        //   * should contain viewer colormap tables
        //   * should contain IERS tables
        virtual std::string distroDataPath( ) const {
            return distro_data_path;
        }

        virtual std::string logPath( ) const {
            return log_path;
        }

        virtual bool noGui( ) const {
            return no_gui;
        }

        virtual bool agg( ) const {
            return do_agg;
        }

        virtual bool pipeline( ) const {
            return do_pipeline;
        }

        void clearDataPath( ) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            data_path.clear( );
        }

        void setDataPath(const std::vector<std::string> &new_list) {
            std::list<std::string> existing_paths;
            struct stat s;

            // accept only strings that are the path to a directory
            std::copy_if( new_list.begin( ), new_list.end( ), std::back_inserter(existing_paths),
                          [&s]( std::string d ) {
                              return (stat(d.c_str( ),&s) == 0)  && (s.st_mode & S_IFDIR);
                          } );

            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);

            // always clear the existing path
            data_path.clear( );

            // convert the paths to fully qualified paths
            std::transform( existing_paths.begin( ), existing_paths.end( ),
                            std::back_inserter( data_path ),
                            [ ]( const std::string &f ) {
                                char *expanded = realpath(f.c_str( ), NULL);
                                std::string result( expanded ? expanded : "" );
                                free(expanded);
                                return result;
                            } );
        }

        void setPythonPath(const std::string &pypath) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            python_path = pypath;
        }

        void setDistroDataPath(const std::string &path) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            distro_data_path = path;
        }

        void setLogPath(const std::string &logpath) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            log_path = logpath;
        }

        void setNoGui(bool nogui) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            no_gui = nogui;
        }

        void setAgg(bool agg) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            do_agg = agg;
        }

        void setPipeline(bool pipeline) {
            // protect critical section...
            std::lock_guard<std::mutex> guard(data_path_mutex);
            do_pipeline = pipeline;
        }

        // get map of registrations
        std::list<ServiceId> services( ) { return registrar.services( ); }
        // returns true if a registration for 'id' was found
        bool removeService( std::string id ) { return registrar.remove(id); }
        // returns assigned identifier (likely based upon the proposed_id)
        ServiceId addService( std::string proposed_id, std::string uri, const std::list<std::string> &types ) {
            return registrar.add(ServiceId(proposed_id, uri, types));
        }
        ServiceId addService( const ServiceId &new_service ) {
            return registrar.add(new_service);
        }
        

        std::string registryURI( ) {
            return registrar.uri( );
        }

        // do any necessary shutdown functions,
        void shutdown( );
        
      private:
        std::mutex data_path_mutex;
        std::list<std::string> data_path;
        std::string python_path;
        std::string log_path;
        std::string distro_data_path;			// path to data as provide by casadata pkg
        std::string measures_dir;
        bool no_gui, do_agg, do_pipeline;
        Registrar registrar;
    };

    extern State &get_state( );

}


#endif
