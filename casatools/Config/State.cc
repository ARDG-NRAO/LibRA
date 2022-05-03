#include <numeric>
#include <limits.h>
#include <unistd.h>
#include <stdexcept>
#include <casatools/Config/State.h>

namespace casatools {
    static bool shutting_down = false;

    State &get_state( )  {
        if ( shutting_down )
            throw std::runtime_error("casatools::get_state( ) called after shutdown initiated");
        if ( casacore::AppStateSource::fetch( ).initialized( ) == false )
            casacore::AppStateSource::initialize( new State );
        return dynamic_cast<State&>(casacore::AppStateSource::fetch( ));
    }

    void State::shutdown( ) {
        shutting_down = true;        
    }

    std::string State::resolve(const std::string &subdir) const {

        // return immediately if subdir is null string
        if ( subdir.size( ) <= 0 ) return subdir;

        // if subdir already resolves to a valid path, try to return a
        // fully qualified path...
        struct stat statbuf;
        if ( stat( subdir.c_str( ), &statbuf ) == 0 ) {
            if ( subdir[0] == '/' ) return subdir;
            char buf[PATH_MAX];
            if ( getcwd(buf,sizeof(buf)) ) {
                return std::string(buf) + "/" + subdir;
            }
            return subdir;
        }

        // otherwise, if data path must be searched leave it
        // to the base class resolve method...
        return casacore::AppState::resolve( subdir );
    }

}
