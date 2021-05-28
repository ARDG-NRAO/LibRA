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

}
