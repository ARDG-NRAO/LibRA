//# FlagAgentDisplay.cc: This file contains the implementation of the FlagAgentDisplay class.
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
//# $Id: rurvashi 28 Nov 2011$

#include <array>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <casatools/Config/State.h>

#include <flagging/Flagging/grpcFlagAgentDisplay.h>
#include <grpc++/grpc++.h>
#include "shutdown.grpc.pb.h"
#include "ping.grpc.pb.h"

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

constexpr int FlagAgentDisplay::TIMEOUT;

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// C++ is so ridiculous... trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

static bool isdir( const char *path ) {
    struct stat statbuf;
    int err = stat(path, &statbuf);
    if ( err == -1 ) return false;
    if ( S_ISDIR(statbuf.st_mode) ) return true;
    return false;
}

static std::string trim_trailing_slash( const char *str ) {
    char *temp = strdup(str);
    for ( int off = strlen(str) - 1; off >= 0; --off ) {
        if ( temp[off] == '/' ) temp[off] = '\0';
        else break;
    }
    std::string result = temp;
    free(temp);
    return result;
}

grpcFlagAgentState::grpcFlagAgentState( ) : userChoice_p("Continue"), userFixA1_p(""), userFixA2_p(""),
                                            skipScan_p(-1), skipSpw_p(-1), skipField_p(-1),
                                            antenna1_p(""),antenna2_p(""), input_received(false) { }

::grpc::Status grpcFlagAgentResponse::button( ::grpc::ServerContext *context,
                                              const ::rpc::gui::ButtonEvent *req,
                                              ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver '" << req->name( ) << "' button event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    // Manage buttons from: Data Plot Window
    if ( req->name( ) == "NextBaseline" || req->name( ) == "PrevBaseline" ||
         req->name( ) == "NextScan" || req->name( ) == "NextField" ||
         req->name( ) == "NextSpw" || req->name( ) == "StopDisplay" || req->name( ) == "Quit") {
        std::lock_guard<std::mutex> lock(state->set_values);
        state->userChoice_p = req->name( );		// set input
        state->input_received = true;			// set whenever state object is modified
        if ( state->input_needed ) {
            state->input_needed = false;		// prevent setting future twice
            state->output.set_value(true);		// signal controlling thread that wait is over
        }

    // Manage buttons from: Report Plot Window
    } else if ( req->name( ) == "Next" || req->name( ) == "Prev" || req->name( ) == "Quit") {
        std::lock_guard<std::mutex> lock(state->set_values);
        state->userChoice_p = req->name( );		// set input
        state->input_received = true;			// set whenever state object is modified
        if ( state->input_needed ) {
            state->input_needed = false;		// prevent setting future twice
            state->output.set_value(true);		// signal controlling thread that wait is over
        }
    }

    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::check( ::grpc::ServerContext *context,
                                             const ::rpc::gui::CheckEvent *req,
                                             ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver " << req->name( ) <<
            " [" << req->state( ) << "] check event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    // Manage check boxes from: Data Plot Window
    if ( req->name( ) == "FixAntenna1" ) {
        std::lock_guard<std::mutex> lock(state->set_values);
        state->userFixA1_p = (req->state( ) == 0) ? "" : state->antenna1_p;
    } else if ( req->name( ) == "FixAntenna2" ) {
        std::lock_guard<std::mutex> lock(state->set_values);
        state->userFixA2_p = (req->state( ) == 0 ) ? "" : state->antenna2_p;
    }
    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::radio( ::grpc::ServerContext *context,
                                             const ::rpc::gui::RadioEvent *req,
                                             ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver " << req->name( ) <<
            " [" << req->state( ) << "] radio event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::linetext( ::grpc::ServerContext *context,
                                                const ::rpc::gui::LineTextEvent *req,
                                                ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver " << req->name( ) <<
            " [" << req->text( ) << "] linetext event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::slidevalue( ::grpc::ServerContext *context,
                                                  const ::rpc::gui::SlideValueEvent *req,
                                                  ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver " << req->name( ) <<
            "[" << req->value( ) << "] slidevalue event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::exiting( ::grpc::ServerContext *context,
                                               const ::google::protobuf::Empty*,
                                               ::google::protobuf::Empty* ) {
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver exiting event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    return grpc::Status::OK;
}
::grpc::Status grpcFlagAgentResponse::closing( ::grpc::ServerContext *context,
                                               const ::rpc::gui::ClosingEvent *req,
                                               ::google::protobuf::Empty* ){
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "plotserver closing event received " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    std::lock_guard<std::mutex> lock(state->set_values);
    state->userChoice_p = "Quit";			// user stopped GUI
    state->input_received = true;			// set whenever state object is modified
    if ( state->input_needed ) {
        state->input_needed = false;		// prevent setting future twice
        state->output.set_value(true);		// signal controlling thread that wait is over
    }
    return grpc::Status::OK;
}

std::string FlagAgentDisplay::plotter_t::get_casaplotserver_path( ) const {
    static std::string python_path = casatools::get_state( ).pythonPath( );
    //*** python3 -m casaplotserver --app-path
    char python_cmd[python_path.size( ) + 35];
    sprintf( python_cmd, "%s -m casaplotserver --app-path", python_path.c_str( ) );
    std::array<char, 512> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(python_cmd, "r"), pclose);
    if ( ! pipe ) return std::string( );        //*** failed to start python
    while ( fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr ) {
        result += buffer.data();
    }
    trim(result);
    if ( result.size( ) == 0 ) return std::string( );
    return result;
}

std::string FlagAgentDisplay::plotter_t::get_fifo( ) const {
    static const char *env_tmpdir = getenv("TMPDIR");
    static std::string fifo_template = trim_trailing_slash(env_tmpdir && isdir(env_tmpdir) ? env_tmpdir : P_tmpdir) + "/cps-XXXXXXXXXX";
    static int fifo_template_size = fifo_template.size( );
    char fifo_path[fifo_template_size+1];
    strncpy( fifo_path, fifo_template.c_str( ), fifo_template_size );
    fifo_path[fifo_template_size] = '\0';
    int fd = mkstemp(fifo_path);
    if ( fd == -1 ) throw std::runtime_error("mkstemp failed...");
    close( fd );
    unlink(fifo_path);
    mkfifo( fifo_path, 0666 );
    return fifo_path;
}

bool FlagAgentDisplay::plotter_t::start_response_manager( ) {
    static const auto debug = getenv("GRPC_DEBUG");
    //***
    //*** set up a default address (grpc picks port) and address buffers
    //***
    char address_buf[100];
    constexpr char address_template[] = "0.0.0.0:%d";
    snprintf(address_buf,sizeof(address_buf),address_template,0);
    std::string server_address(address_buf);
    int selected_port = 0;

    //***
    //*** build grpc service
    //***
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &selected_port);
    // Register "service" as the instance through which we'll receive from
    // the plot server client.
    builder.RegisterService(response_svc.get( ));

    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    // Launch server...
    response_server = builder.BuildAndStart( );
    if ( selected_port > 0 ) {
        // if an available port can be found, selected_port is set to a value greater than zero
        snprintf(address_buf,sizeof(address_buf),address_template,selected_port);
        response_uri = address_buf;
        if ( debug ) {
            std::cerr << "flagagentdisplay response service available at " << response_uri << std::endl;
            fflush(stdout);
        }
        return true;
    }
    return false;
}

bool FlagAgentDisplay::plotter_t::launch(std::string plotserver_path) {
    static const auto debug = getenv("GRPC_DEBUG");

    std::string fifo = get_fifo( );
    if ( fifo.size( ) == 0 ) return false;

    // plot server will generate events (formerly DBus signals)
    // in response to GUI operations...
    if ( start_response_manager( ) == false ) return false;

    // here we start the viewer in a very basic manner... we do not bother
    // with all of the theatrics needed to daemonize the launched process
    // (see https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux)
    // it could be that this should be done in the future, but for now we
    // will adopt the simple...

    // casaplotserver --server=<FIFO-or-GRPC> --event-uri=<GRPC> [ --datapath=<PATH> --casalog=<PATH> ]

    int argc = 3;
    int logarg = argc;    // if a log file is specfied it comes last...
    std::string log_path = casatools::get_state( ).logPath( );
    if ( log_path.size( ) > 0 ) ++argc;
    char **arguments = (char**) malloc(sizeof(char*) * (argc + 1));

    arguments[argc] = 0;
    arguments[0] = strdup(plotserver_path.c_str( ));
    arguments[1] = (char*) malloc(sizeof(char) * (fifo.size( ) + 12));
    sprintf( arguments[1], "--server=%s", fifo.c_str( ) );
    arguments[2] = (char*) malloc(sizeof(char) * (response_uri.size( ) + 18));
    sprintf( arguments[2], "--event-uri=%s", response_uri.c_str( ) );
    if ( log_path.size( ) > 0 ) {
        arguments[logarg] = (char*) malloc(sizeof(char) * (log_path.size( ) + 17));
        sprintf( arguments[logarg], "--logfile=%s", log_path.c_str( ) );
    }

    if ( debug ) {
        std::cerr << "forking plotserver process: ";
        for (int i=0; i < argc; ++i) std::cerr << arguments[i] << " ";
        std::cerr << " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    pid_t newpid = fork( );

    if ( newpid == 0 ) {
        if ( debug ) {
            std::cerr << "execing plotserver process: ";
            for (int i=0; i < argc; ++i) std::cerr << arguments[i] << " ";
            std::cerr << " (process " << getpid( ) << ", thread " << 
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        execvp( arguments[0], (char* const*) arguments );
        perror( "FlagAgentDisplay::plotter_t::launch(...) child process exec failed" );
        exit(1);
    }

    for ( int i=0; i < argc; ++i ) free(arguments[i]);
    free(arguments);

    if ( newpid == -1 ) {
        perror( "FlagAgentDisplay::plotter_t::launch(...) child process fork failed" );
        return false;
    }

    // perform a health check, after a delay...
    int status;
    sleep(2);
    pid_t w = waitpid( newpid, &status, WUNTRACED | WCONTINUED | WNOHANG );
    if ( w == -1 ){
        if ( debug ) {
            std::cerr << "plotserver process failed " <<
                " (process " << getpid( ) << ", thread " << 
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        // waitpid failed
        return false;
    } else if ( w != 0 ) {
        if ( debug ) {
            std::cerr << "plotserver process died " <<
                " (process " << getpid( ) << ", thread " << 
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        // process exited
        if ( WIFEXITED(status) ) {
            printf("exited, status=%d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("killed by signal %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("stopped by signal %d\n", WSTOPSIG(status));
        }
        return false;
    }

    if ( debug ) {
        std::cerr << "fetching casaplotserver uri from " << fifo <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    char buffer[512];
    std::string uri_buffer;
    FILE *fp = fopen(fifo.c_str( ), "r");
    while ( fgets( buffer, sizeof(buffer), fp ) ) { uri_buffer = uri_buffer + buffer; }
    fclose(fp);
    trim(uri_buffer);

    // validate viewer uri...
    if ( ! std::regex_match( uri_buffer, std::regex("^([0-9]+\\.){3}[0-9]+:[0-9]+$") ) ) {
        //rework of regex required for IPv6...
        if ( debug ) {
            std::cerr << "bad casaplotserver uri " << uri_buffer <<
                " (process " << getpid( ) << ", thread " << 
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        return false;
    }

    if ( debug ) {
        std::cerr << "received casaplotserver uri: " << uri_buffer <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    plot_uri = uri_buffer;
    pid = newpid;
    if ( debug ) {
        std::cerr << "creating plotserver stub: " << plot_uri <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    plot = rpc::gui::plotserver::NewStub( grpc::CreateChannel( plot_uri,
                                                               grpc::InsecureChannelCredentials( ) ) );

    grpc::ClientContext context;
    ::google::protobuf::Empty resp;
    ::google::protobuf::Empty msg;
    auto ping = casatools::rpc::Ping::NewStub( grpc::CreateChannel( plot_uri, grpc::InsecureChannelCredentials( ) ) );
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    ::grpc::Status st = ping->now( &context, msg, &resp );
    bool ping_result = st.ok( );
    if ( debug ) {
        std::cerr << "ping result: " << (ping_result ? "OK" : "FAIL")<< 
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    
    plot_started_ = true;

    return true;
}


FlagAgentDisplay::plotter_t::plotter_t(std::shared_ptr<grpcFlagAgentState> state) :
        active_(false), response_svc(new grpcFlagAgentResponse(state)), plot_started_(false) {
    static const auto debug = getenv("GRPC_DEBUG");

    if ( debug ) {
        std::cerr << "attempting to start plotserver process " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    std::string ps_path = get_casaplotserver_path( );
    if ( ps_path.size() > 0 ) {
        // sanity check on casaplotserver path...
        struct stat statbuf;
        if ( stat( ps_path.c_str( ), &statbuf ) < 0 ) {
            // file (or dir) does not exist... e.g.
            //   >>>>>>registry available at 0.0.0.0:40939
            //   stopping registry<<<<<<
            if ( debug ) {
                std::cerr << "could not find casaplotserver executable " <<
                    " (process " << getpid( ) << ", thread " << 
                    std::this_thread::get_id() << ")" << std::endl;
                fflush(stderr);
            }
        } else {
            if ( launch(ps_path) ) {
                active_ = true;
            } else if ( debug ) {
                std::cerr << "could not find casaplotserver executable " <<
                    " (process " << getpid( ) << ", thread " << 
                    std::this_thread::get_id() << ")" << std::endl;
                fflush(stderr);
            }
        }        
    } else if ( debug ) {
        std::cerr << "could not find casaplotserver executable " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

}


FlagAgentDisplay::FlagAgentDisplay(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube):
        		FlagAgentBase(dh,config,ANTENNA_PAIRS_INTERACTIVE,writePrivateFlagCube),
            	gui_state(new grpcFlagAgentState),
        		dataplotter_p(NULL),reportplotter_p(NULL), pause_p(false),
        		fieldId_p(-1), fieldName_p(""), scanStart_p(-1), scanEnd_p(-1), spwId_p(-1),
        		nPolarizations_p(1), freqList_p(Vector<Double>()),
        		dataDisplay_p(false), reportDisplay_p(false),reportFormat_p("screen"),
        		stopAndExit_p(false),/* reportReturn_p(false),*/ showBandpass_p(false)
{
	// Parse parameters and set base variables.
	setAgentParameters(config);
	// Request loading polarization map to FlagDataHandler
	flagDataHandler_p->setMapPolarizations(true);
	// Make the list of colours (these are almost all predefined ones for Qt.
	// Can add more later, based on RGB values.
	plotColours_p.resize(13);
	plotColours_p[0]="blue";
	plotColours_p[1]="red";
	plotColours_p[2]="green";
	plotColours_p[3]="cyan";
	plotColours_p[4]="darkGray";
	plotColours_p[5]="magenta";
	plotColours_p[6]="yellow";
	plotColours_p[7]="darkBlue";
	plotColours_p[8]="darkRed";
	plotColours_p[9]="darkGreen";
	plotColours_p[10]="darkCyan";
	plotColours_p[11]="black";
	plotColours_p[12]="darkMagenta";

}

bool FlagAgentDisplay::done( std::shared_ptr<FlagAgentDisplay::plotter_t> plotter ) {
    // send shutdown message to plotserver...
    static const auto debug = getenv("GRPC_DEBUG");

    if ( plotter && plotter->active( ) && plotter->plot_uri.size( ) > 0) {
        grpc::ClientContext context;
        ::google::protobuf::Empty req;
        ::google::protobuf::Empty resp;
        if ( debug ) {
            std::cerr << "attempting to shutdown plotserver [" <<
                plotter->plot_uri << "] " << 
                " (process " << getpid( ) << ", thread " << 
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        auto shutdown = casatools::rpc::Shutdown::NewStub( grpc::CreateChannel( plotter->plot_uri, grpc::InsecureChannelCredentials( ) ) );
        auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
        context.set_deadline(deadline);
        shutdown->now( &context, req, &resp );

        // wait on plotserver to exit...
        int status;
        pid_t w = waitpid( plotter->pid, &status, WUNTRACED | WCONTINUED );
        if ( w == -1 ){
            if ( debug ) {
                std::cerr << "plotserver waitpid failed " <<
                    " (process " << getpid( ) << ", thread " << 
                    std::this_thread::get_id() << ")" << std::endl;
                fflush(stderr);
            }
            // waitpid failed
            return false;
        } else if ( w == 0 ) {
            if ( debug ) {
                std::cerr << "plotserver process not found " <<
                    " (process " << getpid( ) << ", thread " << 
                    std::this_thread::get_id() << ")" << std::endl;
                fflush(stderr);
            }
            return false;
        } else {
            if ( debug ) {
                std::cerr << "plotserver process (" << w << ") exited, status fetched " <<
                    " (process " << getpid( ) << ", thread " << 
                    std::this_thread::get_id() << ")" << std::endl;
                fflush(stderr);
            }
            plotter->pid = 0;
            return true;
        }
    }

    plotter.reset( );
    return true;
}

int FlagAgentDisplay::create_panel( std::shared_ptr<plotter_t> plot, int parent, bool new_row ) {
    static const auto debug = getenv("GRPC_DEBUG");
	string zoomloc="";
	string legendloc="bottom";
    grpc::ClientContext context;
    ::rpc::gui::Id result;
    ::rpc::gui::NewPanel panel;
    panel.set_title("");
    panel.set_xlabel("");
    panel.set_ylabel("");
    panel.set_window_title("");
    *panel.mutable_size( ) = { };
    panel.set_legend(legendloc);
    panel.set_zoom(zoomloc);
    panel.set_with_panel(parent);
    panel.set_new_row(new_row);
    panel.set_hidden(false);
    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay creating panel " : "FlagAgentDisplay create ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return -1;

    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    ::grpc::Status st = plot->plot->panel(&context,panel,&result);
    bool stat = st.ok( );
    if ( debug ) {
        std::cerr << "panel creation " << (stat ? "SUCCEEDED " : "FAILED ") << (stat ? result.id( ) : -1) <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }
    return stat ? result.id( ) : -1;
}

void FlagAgentDisplay::erase( std::shared_ptr<plotter_t> plot, int panel ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::google::protobuf::Empty resp;
    ::rpc::gui::Id id;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay erase " : "FlagAgentDisplay erase ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return;

    id.set_id(panel);
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->erase(&context,id,&resp);
}
    
void FlagAgentDisplay::setlabel( std::shared_ptr<plotter_t> plot, int panel,
                                 std::string xlabel, std::string ylabel, std::string title ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::google::protobuf::Empty resp;
    ::rpc::gui::Label label;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay setlabel " : "FlagAgentDisplay setlabel ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return;

    label.mutable_panel( )->set_id(panel);
    label.set_xlabel(xlabel);
    label.set_ylabel(ylabel);
    label.set_title(title);
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->setlabel(&context,label,&resp);
}

int FlagAgentDisplay::create_dock( std::shared_ptr<plotter_t> plot, int panel, std::string xml ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::rpc::gui::DockSpec spec;
    ::rpc::gui::Id result;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay create_dock " : "FlagAgentDisplay create_dock ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return -1;

    spec.set_file_or_xml(xml);
    spec.set_loc("bottom");
    spec.add_dockable("top");
    spec.mutable_panel( )->set_id(panel);
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->loaddock(&context,spec,&result);
    return result.id( );
}

int FlagAgentDisplay::raster( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &data, ssize_t sizex, ssize_t sizey ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::rpc::gui::Id result;
    ::rpc::gui::NewRaster raster;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay raster " : "FlagAgentDisplay raster ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return -1;

    raster.mutable_panel( )->set_id(panel);
    *raster.mutable_matrix( ) = { data.begin( ), data.end( ) };
    raster.set_sizex(sizex);
    raster.set_sizey(sizey);
    raster.set_colormap("Hot Metal 1");

    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->raster(&context,raster,&result);
    return result.id( );
}    

int FlagAgentDisplay::line( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &xdata, const std::vector<float> &ydata, std::string color, std::string label ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::rpc::gui::Id result;
    ::rpc::gui::NewLine line;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay line " : "FlagAgentDisplay line ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return -1;

    line.mutable_panel( )->set_id(panel);
    *line.mutable_x( ) = { xdata.begin( ), xdata.end( ) };
    *line.mutable_y( ) = { ydata.begin( ), ydata.end( ) };
    line.set_color(color);
    line.set_label(label);
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->line(&context,line,&result);
    return result.id( );
}

int FlagAgentDisplay::scatter( std::shared_ptr<plotter_t> plot, int panel, const std::vector<float> &xdata,
                               const std::vector<float> &ydata, std::string color, std::string label,
                               std::string symbol, int symbol_size, int dot_size ) {
    static const auto debug = getenv("GRPC_DEBUG");
    grpc::ClientContext context;
    ::rpc::gui::Id result;
    ::rpc::gui::NewScatter scatter;

    if ( debug ) {
        std::cerr << (plot->active( ) ? "FlagAgentDisplay scatter " : "FlagAgentDisplay scatter ERROR plot not active ") <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! plot->active( ) ) return -1;

    scatter.mutable_panel( )->set_id(panel);
    *scatter.mutable_x( ) = { xdata.begin( ), xdata.end( ) };
    *scatter.mutable_y( ) = { ydata.begin( ), ydata.end( ) };
    scatter.set_color(color);
    scatter.set_label(label);
    scatter.set_symbol(symbol);
    scatter.set_symbol_size(symbol_size);
    scatter.set_dot_size(dot_size);
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(TIMEOUT);
    context.set_deadline(deadline);
    plot->plot->scatter(&context,scatter,&result);
    return result.id( );
}

FlagAgentDisplay::~FlagAgentDisplay()
{
    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << "FlagAgentDisplay dtor " <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
	if ( dataplotter_p != NULL ) {
        done(dataplotter_p);
        dataplotter_p=NULL;
    }
	if ( reportplotter_p != NULL ) {
        done(reportplotter_p);
        reportplotter_p=NULL;
    }
}

void FlagAgentDisplay::setAgentParameters(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	int exists;

	exists = config.fieldNumber ("pause");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'pause' must be of type 'bool'" ) );
		}

		pause_p = config.asBool("pause");
	}
	else
	{
		pause_p = true;
	}

	*logger_p << LogIO::NORMAL << " pause is " << pause_p << LogIO::POST;

	exists = config.fieldNumber ("datadisplay");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'datadisplay' must be of type 'bool'" ) );
		}

		dataDisplay_p = config.asBool("datadisplay");
	}
	else
	{
		dataDisplay_p = false;
	}

	*logger_p << LogIO::NORMAL << " datadisplay is " << dataDisplay_p << LogIO::POST;

	exists = config.fieldNumber ("reportdisplay");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'reportdisplay' must be of type 'bool'" ) );
		}

		reportDisplay_p = config.asBool("reportdisplay");
	}
	else
	{
		reportDisplay_p = false;
	}

	*logger_p << LogIO::NORMAL << " reportdisplay is " << reportDisplay_p << LogIO::POST;


	exists = config.fieldNumber ("format");
	if (exists >= 0)
	{
		if( config.type(exists) != TpString )
		{
			throw( AipsError ( "Parameter 'format' must be of type 'bool'" ) );
		}

		reportFormat_p = config.asString("format");
		if( reportFormat_p != "screen" && reportFormat_p != "file")
		{
			throw( AipsError( "Unsupported report format : " + reportFormat_p + ". Supported formats are 'screen' and 'file'") );
		}
	}
	else
	{
		reportFormat_p = String("screen");
	}

	*logger_p << LogIO::NORMAL << " format is " << reportFormat_p << LogIO::POST;

}


void
FlagAgentDisplay::preProcessBuffer(const vi::VisBuffer2 &visBuffer)
{

	getChunkInfo(visBuffer);

	return;
}


void
FlagAgentDisplay::iterateAntennaPairsInteractive(antennaPairMap *antennaPairMap_ptr)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	// Check if the visibility expression is suitable for this spw
	if (!checkVisExpression(flagDataHandler_p->getPolarizationMap())) return;

	// Iterate through antenna pair map
	std::pair<Int,Int> antennaPair;
	antennaPairMapIterator myAntennaPairMapIterator;
	bool stepback=false;
	for (myAntennaPairMapIterator=antennaPairMap_ptr->begin(); myAntennaPairMapIterator != antennaPairMap_ptr->end(); ++myAntennaPairMapIterator)
	{

		// Check whether to skip the rest of this chunk or not
		if(gui_state->skipSpw_p != -1)
		{
			if(gui_state->skipSpw_p == spwId_p) {dataDisplay_p=false;} // Skip the rest of this SPW
			else {
                std::lock_guard<std::mutex> lock(gui_state->set_values);
                gui_state->skipSpw_p = -1;
                dataDisplay_p=true;
            } // Reached next SPW. Reset state
		}
		if(gui_state->skipField_p != -1)
		{
			if(gui_state->skipField_p == fieldId_p) {dataDisplay_p=false;} // Skip the rest of this Field
			else {
                std::lock_guard<std::mutex> lock(gui_state->set_values);
                gui_state->skipField_p = -1;
                dataDisplay_p=true;
            } // Reached next Field. Reset state
		}
		if(gui_state->skipScan_p != -1)
		{
			if(gui_state->skipScan_p == scanEnd_p) {dataDisplay_p=false;} // Skip the rest of this Scan
			else {
                std::lock_guard<std::mutex> lock(gui_state->set_values);
                gui_state->skipScan_p = -1;
                dataDisplay_p=true;
            } // Reached next Scan. Reset state
		}


		// Display this baseline
		if(dataDisplay_p)
		{

			// If choice from previous plot was to go backwards in baseline.
			if(stepback)
			{
				// Go to previous baseline (decrement by 2)
				if( myAntennaPairMapIterator != antennaPairMap_ptr->begin() )
					-- myAntennaPairMapIterator;
				if( myAntennaPairMapIterator != antennaPairMap_ptr->begin() )
					-- myAntennaPairMapIterator;

				// If antenna constraints exist, keep going back until first match is found.
				// If not found, stay on current baseline (continue)
				if( gui_state->userFixA1_p != "" || gui_state->userFixA2_p != "" )
				{
					antennaPairMapIterator tempIterator;
					bool found=false;
					for(tempIterator = myAntennaPairMapIterator; tempIterator != antennaPairMap_ptr->begin() ; --tempIterator )
					{
						if( ! skipBaseline(tempIterator->first) ) {found=true; break;}
					}
					if(found) // Jump to this antenna pair
					{
						myAntennaPairMapIterator = tempIterator;
					}
					else
					{
						*logger_p << "No Previous baseline in this chunk with Ant1 : "
								<< ( (gui_state->userFixA1_p != "") ? gui_state->userFixA1_p : "any" )
								<< "  and Ant2 : "
								<< ( (gui_state->userFixA2_p != "") ? gui_state->userFixA2_p : "any" )
								<< LogIO::POST;

						// Stay on current baseline
						if( myAntennaPairMapIterator != antennaPairMap_ptr->end() )
							++myAntennaPairMapIterator;
					}
				}

				// Reset state
				stepback=false;
			}

			// Get antenna pair from map
			antennaPair = myAntennaPairMapIterator->first;

			// Check whether or not to display this baseline (for going in the forward direction)
			if( skipBaseline(antennaPair) ) continue;

			// Process antenna pair
			processAntennaPair(antennaPair.first,antennaPair.second);

			// If Plot window is visible, and, if asked for, get and react to user-choices.
			if(pause_p==true)
			{

				// Wait for User Input
				getUserInput( ); // Fills in userChoice_p. userfix

				// React to user-input
				if(gui_state->userChoice_p=="Quit")
				{
					dataDisplay_p = false;
					stopAndExit_p = true;
					*logger_p << "Exiting flagger" << LogIO::POST;
					if ( dataplotter_p != NULL ) {
                        done(dataplotter_p);
                        dataplotter_p=NULL;
                    }
					flagDataHandler_p->stopIteration();
					return ;
				}
				else if(gui_state->userChoice_p=="StopDisplay")
				{
					dataDisplay_p = false;
					*logger_p << "Stopping display. Continuing flagging." << LogIO::POST;
					if ( dataplotter_p != NULL ) {
                        done(dataplotter_p);
                        dataplotter_p=NULL;
                    }
				}
				else if(gui_state->userChoice_p=="PrevBaseline")
				{
					if( myAntennaPairMapIterator==antennaPairMap_ptr->begin() )
						*logger_p << "Already on first baseline..." << LogIO::POST;
					stepback=true;
				}
				else if(gui_state->userChoice_p=="NextScan")
				{
					//*logger_p << "Next Scan " << LogIO::POST;
                    std::lock_guard<std::mutex> lock(gui_state->set_values);
					gui_state->skipScan_p = scanEnd_p;
				}
				else if(gui_state->userChoice_p=="NextSpw")
				{
					//*logger_p << "Next SPW " << LogIO::POST;
                    std::lock_guard<std::mutex> lock(gui_state->set_values);
					gui_state->skipSpw_p = spwId_p;
				}
				else if(gui_state->userChoice_p=="NextField")
				{
					//*logger_p << "Next Field " << LogIO::POST;
                    std::lock_guard<std::mutex> lock(gui_state->set_values);
					gui_state->skipField_p = fieldId_p;
				}
				else if(gui_state->userChoice_p=="Continue")
				{
					//*logger_p << "Next chunk " << LogIO::POST; // Right now, a chunk is one baseline !
					return;
				}

			}// end if pause=true

		}// if dataDisplay_p

	}// end antennaMapIterator

	return;
}// end iterateAntennaPairsInteractive


Bool
FlagAgentDisplay::skipBaseline(std::pair<Int,Int> antennaPair)
{
	std::string antenna1Name = flagDataHandler_p->antennaNames_p->operator()(antennaPair.first);
	std::string antenna2Name = flagDataHandler_p->antennaNames_p->operator()(antennaPair.second);
	//	    if(userFixA2_p != "") cout << "*********** userfixa2 : " << userFixA2_p << "   thisant : " << antenna1Name << " && " << antenna2Name << LogIO::POST;
	return  (  (gui_state->userFixA1_p != ""  && gui_state->userFixA1_p != antenna1Name)  ||
               (gui_state->userFixA2_p != ""  && gui_state->userFixA2_p != antenna2Name) ) ;
}

bool
FlagAgentDisplay::computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer,
		VisMapper &visibilities,FlagMapper &flags,Int antenna1,Int antenna2,
		vector<uInt> &/*rows*/)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	// Gather shapes
	IPosition flagCubeShape = visibilities.shape();
	uInt nChannels = flagCubeShape(0);
	uInt nTimes = flagCubeShape(1);

	// Read antenna names for the current baseline
	String antenna1Name = flagDataHandler_p->antennaNames_p->operator()(antenna1);
	String antenna2Name = flagDataHandler_p->antennaNames_p->operator()(antenna2);
	String baselineName = antenna1Name + "&&" + antenna2Name;
    {
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->antenna1_p = antenna1Name;
        gui_state->antenna2_p = antenna2Name;
    }

	String scanRange = (scanStart_p!=scanEnd_p)?String::toString(scanStart_p)+"~"+String::toString(scanEnd_p) : String::toString(scanStart_p);
	String spwName = String::toString(visBuffer.spectralWindows()(0));

	// Get Frequency List
	freqList_p.resize(nChannels);
	for(uInt ch=0;ch<nChannels;ch++) freqList_p[ch]=(Double)ch;


	/*
    // Read current Field name, SPW id, and correlation string from visBuffer Info.
    uInt fieldId_p = visBuffer.fieldId();
    String fieldName = visBuffer.msColumns().field().name().getColumn()[fieldId_p];
    String spwName = String::toString(visBuffer.spectralWindow());
    Int scanstart = visBuffer.scan()[0];
    int scanend = visBuffer.scan()[ (visBuffer.scan().nelements())-1 ];
    String scanRange = (scanstart!=scanend)?String::toString(scanstart)+"~"+String::toString(scanend) : String::toString(scanstart);
	 */

	// Get Polarization Maps
	/*
    Vector<uInt> polarizations = flags.getSelectedCorrelations();
    nPolarizations_p = polarizations.size();
    polarizationIndexMap *polMap = flagDataHandler_p->getPolarizationIndexMap();
    Vector<String> corrTypes(nPolarizations_p);
    for(uInt pol=0;pol<nPolarizations_p;pol++)
      corrTypes[pol] = (*polMap)[polarizations[pol]];
	 */

	// Get Polarization Maps
	// jagonzal: Migrated to new implementation of multiple expressions handling
	vector<string> corrTypes = visibilities.getSelectedCorrelationStrings();
	nPolarizations_p = corrTypes.size();

	///cout << "Selected Correlations : " << polarizations << LogIO::POST;

	// Print where we are...
	//    *logger_p << LogIO::NORMAL  << " Baseline : " << baselineName << " Field : " << fieldName_p << " Spw : " << spwName << "  nChan : " << nChannels << " nPol : " << nPolarizations_p << " nTime : " << nTimes << LogIO::POST;

	// Build the Plot Window for the first time
	if(dataDisplay_p && dataplotter_p==NULL) buildDataPlotWindow();

	// Adding this block of code as a fix for CAS-4052.
	// J.G. : If you find a better way to do this, please change this...
	if(dataDisplay_p==true && dataplotter_p!=NULL)
	{
		Int nrows;
		if(showBandpass_p==true) nrows=3;
		else nrows=2;

		if(panels_p.size() != nPolarizations_p*nrows)
		{
			//cout << "Wrong number of panels !" << endl;
			done(dataplotter_p);
			dataplotter_p = NULL;
			buildDataPlotWindow();
		}
	}

	// Initialize Plot Arrays and other vars
	Float runningsum=0, runningflag=0,runningpreflag=0;
	Vector<Float> vecflagdat(0), vecdispdat(0);
	Vector<Float> origspectrum(0), flagspectrum(0), precountspec(0), countspec(0);
	if(dataDisplay_p)
	{
		vecflagdat.resize(nChannels * nTimes); vecdispdat.resize(nChannels * nTimes);
		origspectrum.resize(nChannels); flagspectrum.resize(nChannels);
		precountspec.resize(nChannels); countspec.resize(nChannels);
	}

	if(dataDisplay_p)
	{
		// Make and send plots for each polarization
		for(int pl=0;pl<(int) nPolarizations_p;pl++)  // Start Correlation Loop
		{
			runningsum=0; runningflag=0;  runningpreflag=0;
			origspectrum=0.0; flagspectrum=0.0; precountspec=0.0; countspec=0.0;
			for(int ch=0;ch<(int) nChannels;ch++)  // Start Channel Loop
			{
				for(uInt tm=0;tm<nTimes;tm++)  // Start Time Loop
				{
					// UUU FOR TEST ONLY -- Later, enable additional ManualFlagAgent in the tFlagAgentDisplay
					/////if(ch>10 && ch<20) flags.applyFlag(ch,tm);

					vecdispdat( ch*nTimes + tm ) = visibilities(pl,ch,tm) * ( ! flags.getOriginalFlags(pl,ch,tm) );
					vecflagdat( ch*nTimes + tm ) = visibilities(pl,ch,tm) * ( ! flags.getModifiedFlags(pl,ch,tm) );

					origspectrum[ch] += visibilities(pl,ch,tm) * ( ! flags.getOriginalFlags(pl,ch,tm) );
					flagspectrum[ch] += visibilities(pl,ch,tm) * ( ! flags.getModifiedFlags(pl,ch,tm) );

					precountspec[ch] += ( ! flags.getOriginalFlags(pl,ch,tm) );
					countspec[ch] += ( ! flags.getModifiedFlags(pl,ch,tm) );

					runningsum += visibilities(pl,ch,tm);
					runningflag += (Float)(flags.getModifiedFlags(pl,ch,tm));
					runningpreflag += (Float)(flags.getOriginalFlags(pl,ch,tm));

				}// End Time Loop
			}//End Channel Loop

			// Make the Labels
			stringstream ostr1,ostr2;
			ostr1 << "(" << fieldId_p << ") " << fieldName_p << "  [scan:" << scanRange << "]\n[spw:" << spwName << "] " << baselineName << "  ( " << corrTypes[pl] << " )";
			ostr2 << fixed;
			ostr2.precision(1);
			ostr2 << " flag:" << 100 * runningflag/(nChannels*nTimes) << "% (pre-flag:" << 100 * runningpreflag/(nChannels*nTimes) << "%)";

			//*logger_p << "[" << corrTypes[pl] << "]:" << 100 * runningflag/(nChannels*nTimes) << "%(" << 100 * runningpreflag/(nChannels*nTimes) << "%) ";

			// Make the Before/After Raster Plots
			DisplayRaster(nChannels,nTimes,vecdispdat,panels_p[pl]);
            setlabel( dataplotter_p, panels_p[pl], " ", pl?" ":"Time", ostr1.str() );
			DisplayRaster(nChannels,nTimes,vecflagdat,panels_p[pl+nPolarizations_p]);
            setlabel( dataplotter_p, panels_p[pl+nPolarizations_p], "Frequency", pl?" ":"Time", ostr2.str() );

			if(showBandpass_p==true)
			{

				// Make the Before/After bandpass plots
				for(uInt ch=0;ch<nChannels;ch++)
				{
					if(precountspec[ch]==0) {origspectrum[ch]=0.0; precountspec[ch]=1.0;}
					if(countspec[ch]==0) {flagspectrum[ch]=0.0; countspec[ch]=1.0;}
				}

				origspectrum = (origspectrum/precountspec);
				flagspectrum = (flagspectrum/countspec);
				AlwaysAssert( freqList_p.nelements()==nChannels , AipsError);

				DisplayLine( nChannels, freqList_p, origspectrum, String("before:")+String(corrTypes[pl]),
                             String("red"), false, panels_p[pl+(2*nPolarizations_p)] );
				DisplayScatter( nChannels, freqList_p, flagspectrum, String("after:")+String(corrTypes[pl]),
                                String("blue"), true, panels_p[pl+(2*nPolarizations_p)] );

				//// TODO : Can I query the tfcrop agent for a "view" to overlay here.
				// If available, get a plot from the agents
				/*
		  for (uInt fmeth=0; fmeth<flagmethods.nelements(); fmeth++)
		  {
		  if(flagmethods[fmeth]->getMonitorSpectrum(flagspectrum,pl,bs))
		  {
		  //		    flagspectrum = log10(flagspectrum);
		  DisplayLine(nChannels, freqlist_p, flagspectrum, flagmethods[fmeth]->methodName(), 
		  String("green"), true, panels_p[pl+(2*nPolarizations_p)].getInt());
		  }
		  }
				 */
			}// end of if (showBandPass_p)

		}//End Correlation Loop

		//*logger_p << LogIO::POST;

	}// end if dataDisplay_p

	return false;
}// end computeAntennaPairFlags

//----------------------------------------------------------------------------------------------------------

void
FlagAgentDisplay::getChunkInfo(const vi::VisBuffer2 &visBuffer)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	// Read current Field name, SPW id, and scan info.
	fieldId_p = visBuffer.fieldId()(0);
	fieldName_p = flagDataHandler_p->fieldNames_p->operator()(fieldId_p);
	spwId_p = visBuffer.spectralWindows()(0);
	scanStart_p = visBuffer.scan()[0];
	scanEnd_p = visBuffer.scan()[ (visBuffer.scan().nelements())-1 ];

	*logger_p << LogIO::NORMAL << "FlagAgentDisplay::" << __FUNCTION__ << " Field : " << fieldId_p << " , " << fieldName_p << " Spw : " << spwId_p << " Scan : " << scanStart_p << " : " << scanEnd_p << LogIO::POST;
}

//----------------------------------------------------------------------------------------------------------

FlagReport
FlagAgentDisplay::getReport()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	//       FlagReport dispRep("plot",agentName_p);

	// Make empty list
	FlagReport dispRep("list");

	/*

       // Make sample arrays/vectors
       Int N=10;
       Array<Float> sample( IPosition(2, N, N) );
       sample = 0.0;
       sample( IPosition(2,N/2,N/2)) = 1.0;
       Vector<Float> xdata( N ), ydata( N ), error ( N );
       for(Int i=0;i<N;i++) {xdata[i]=i;}
       ydata = 1.0;

       // (1) Make a raster plot. Only one set of data is allowed here.
       FlagReport subRep0 = FlagReport("plotraster",agentName_p,"example raster", "xaxis", "yaxis");
       subRep0.addData(sample); // add 2D data

       // Add this raster FlagReport to the list.
       dispRep.addReport( subRep0 ); 

       // (2) Make a line plot. Can give multiple lines to overlay on the same panel.
       FlagReport subRep1 = FlagReport("plotpoints",agentName_p,"example line", "xaxis", "yaxis");
       subRep1.addData("line", xdata,ydata,"",Vector<Float>(),"line 1"); // add first set of line data
       ydata[N/2]=2.0;
       subRep1.addData("scatter", xdata,ydata,"",Vector<Float>(),"scatter 2"); // add second set of line data to overlay

       // Add this line FlagReport to the list
       dispRep.addReport( subRep1 ); 

       // (3) Make an overlay of a line with errorbar, scatter with errorbar, and scatter with circle
       FlagReport subRep2 = FlagReport("plotpoints",agentName_p,"example line", "xaxis", "yaxis");
       for(Int i=0;i<N;i++) {error[i]=i;}
       subRep2.addData("line", xdata,ydata,"bar",error,"line+bar"); // add first set of line data, with errorbars
       for(Int i=0;i<N;i++) {xdata[i] += 0.3; error[i]=i; ydata[i]+=2.0;}
       subRep2.addData("scatter", xdata, ydata,"bar",error,"scatter+bar"); // add second set of scatter data to overlay, with error bars
       for(Int i=0;i<N;i++) {xdata[i] += 0.3; error[i]=i*10; ydata[i]+=2.0;}
       subRep2.addData("scatter", xdata, ydata,"circle",error,"scatter+circle"); // add third set, scatter data with circles.

       // Add this line FlagReport to the list
       dispRep.addReport( subRep2 ); 

       if( ! dispRep.verifyFields() )
	 cout << "Problem ! " << endl;


	 */

	return dispRep;
}// end of getReport()

//----------------------------------------------------------------------------------------------------------
// Go through the list of reports and make plots
Bool
FlagAgentDisplay::displayReports(FlagReport &combinedReport)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	if(reportDisplay_p && !stopAndExit_p)
	{
		Int nReports = combinedReport.nReport();

		if( dataplotter_p != NULL ) {
            done(dataplotter_p);
            dataplotter_p = NULL;
        }
		if(nReports>0 && reportplotter_p==NULL) buildReportPlotWindow();

		Bool stepback=false;

		for (Int reportid=0; reportid<nReports; reportid++)
		{
			String agentName, title, xlabel, ylabel;
			FlagReport oneRep;

			if(stepback==true)
			{
				// Go back until a valid one is found.
				Int previd=-1;
				for( Int rep=reportid-1; rep>=0; --rep)
				{
					Bool valid = combinedReport.accessReport(rep,oneRep);
					if(valid)
					{
						String type = oneRep.reportType();
						if( type=="plotraster" || type=="plotpoints" )
						{
							previd=rep; break;
						}
					}
				}

				if(previd==-1)
				{
					*logger_p << "Already on first plot" << LogIO::POST;
					reportid=0;
				}
				else
				{
					reportid = previd;
				}

				stepback=false;
			}


			Bool valid = combinedReport.accessReport(reportid,oneRep);
			if(valid)
			{
				oneRep.get( RecordFieldId("name") , agentName );
				String type = oneRep.reportType();

				if( type=="plotraster" || type=="plotpoints" )
				{

					*logger_p << reportid << " : " << type << " with " << oneRep.nData() << " layer(s) " << " from " << agentName << LogIO::POST;

					if( type == "plotraster" )
					{
						oneRep.get( RecordFieldId("title") , title );
						oneRep.get( RecordFieldId("xlabel") , xlabel );
						oneRep.get( RecordFieldId("ylabel") , ylabel );

						Array<Float> data;
						oneRep.get( RecordFieldId("data"+String::toString(0)) , data );

                        std::vector<float> flatdata(data.begin(),data.end());

                        erase( reportplotter_p, report_panels_p[0] );
                        raster( reportplotter_p, report_panels_p[0], flatdata, data.shape()[0], data.shape()[1] );
                        setlabel( reportplotter_p, report_panels_p[0], xlabel, ylabel, title );

					}
					else if( type == "plotpoints")
					{
						oneRep.get( RecordFieldId("title") , title );
						oneRep.get( RecordFieldId("xlabel") , xlabel );
						oneRep.get( RecordFieldId("ylabel") , ylabel );

                        erase( reportplotter_p, report_panels_p[0] );

						Int ndata = oneRep.nData();
						for(Int datid=0;datid<ndata;datid++)
						{
							Vector<Float> xdata,ydata,error;
							String legendlabel,plottype="line",errortype="";
							oneRep.get( RecordFieldId("xdata"+String::toString(datid)) , xdata );
							oneRep.get( RecordFieldId("ydata"+String::toString(datid)) , ydata );
							oneRep.get( RecordFieldId("label"+String::toString(datid)) , legendlabel );
							oneRep.get( RecordFieldId("plottype"+String::toString(datid)) , plottype );

							///reportplotter_p->line(dbus::af(xdata), dbus::af(ydata),(datid%2)?String("red"):String("blue"),legendlabel, report_panels_p[0].getInt());

							if ( oneRep.isDefined( "error"+String::toString(datid)  ) )
							{
								oneRep.get( RecordFieldId("error"+String::toString(datid)) , error );
								oneRep.get( RecordFieldId("errortype"+String::toString(datid)) , errortype );
							}

							DisplayLineScatterError(reportplotter_p , plottype, xdata, ydata, errortype, error, legendlabel, plotColours_p[datid%plotColours_p.nelements()], report_panels_p[0] );
							/// (datid%2)?String("red"):String("blue")
						}// end of for datid

                        setlabel( reportplotter_p, report_panels_p[0], xlabel, ylabel, title );

					}// end of plotpoints
					else
					{
						*logger_p << "NO Display for : " << reportid << " : " << agentName << LogIO::POST;
					}

					getReportUserInput();

					// React to user-input
					if(gui_state->userChoice_p=="Quit")
					{
						dataDisplay_p = false;
						stopAndExit_p = true;
						//*logger_p << "Exiting flagger" << LogIO::POST;
						if( reportplotter_p != NULL ) {
                            done(reportplotter_p);
                            reportplotter_p = NULL;
                        }
						return true;
					}
					else if(gui_state->userChoice_p=="Prev")
					{
						//*logger_p << "Prev Plot" << LogIO::POST;
						if( reportid==0 )
							*logger_p << "Already on first plot..." << LogIO::POST;
						else
							--reportid;
						stepback=true;
					}
					else if(gui_state->userChoice_p=="Continue" || gui_state->userChoice_p=="Next")
					{
						//*logger_p << "Next Plot " << LogIO::POST;
						if( reportid==nReports-1 )
						{
							*logger_p << "Already on last plot..." << LogIO::POST;
							--reportid;
						}
					}

				}// if valid plot type
				else
				{
					//*logger_p  <<  "No plot for Report : " << reportid << LogIO::POST;
					*logger_p << reportid << " : No plot for report from " << agentName << LogIO::POST;

				}
			}// if valid plot record.
			else
			{
				*logger_p << LogIO::WARN <<  "Invalid Plot Record for : " << reportid << LogIO::POST;
			}

		}// end of for-report-in-combinedReport

	}// end of reportDisplay_p==true
	else
	{
		*logger_p << "Report Displays are turned OFF " << LogIO::POST;
	}
	return true;
} // end of displayReports()

/***********************************************************************/
/******************     Plot Functions      ******************************/
/***********************************************************************/

// Note : By default, only two rows of plots are created.
// The third row, for bandpass plots (before and after) are turned off
// by the private variable 'showBandPass_p'. This can be later
// enabled when we get per-chunk 'extra info' displays/reports from
// the agents. Right now, it's redundant info, and takes up too much
// space on small laptop screens.
Bool FlagAgentDisplay::buildDataPlotWindow()
{

	setDataLayout();
	AlwaysAssert( dock_xml_p != NULL , AipsError );

	dataplotter_p.reset(new plotter_t(gui_state));

	Int nrows;
	if(showBandpass_p==true) nrows=3;
	else nrows=2;
	panels_p.resize(nPolarizations_p*nrows);
	string zoomloc="";
	string legendloc="bottom";

    // First row : Data with no flags
    panels_p[0] = create_panel(dataplotter_p,0,false);

	if(nPolarizations_p>1)
	{
		for(Int i=1;i<(int) nPolarizations_p;i++)
		{
            panels_p[i] = create_panel(dataplotter_p,panels_p[i-1],false);
		}
	}


	// Second row : Data with flags
	panels_p[nPolarizations_p] = create_panel(dataplotter_p,panels_p[0],true);
	if(nPolarizations_p>1)
	{
		for(int i=static_cast<int>(nPolarizations_p)+1;i<2*static_cast<int>(nPolarizations_p);i++)
		{
			panels_p[i] = create_panel(dataplotter_p,panels_p[i-1],false);
		}
	}

	// Third row : Average Bandpass
	if(showBandpass_p==true)
	{
		panels_p[2*nPolarizations_p] = create_panel(dataplotter_p,panels_p[0],true);

		if(nPolarizations_p>1)
		{
			for(int i=2* static_cast<int>(nPolarizations_p)+1;i<3*static_cast<int>(nPolarizations_p);i++)
			{
				panels_p[i] = create_panel(dataplotter_p,panels_p[i-1],false);
			}
		}
	}// if showbandpass

	/*
    // Misc panel
    panels_p[8] = dataplotter_p->panel( "BandPass", "Frequency", "Amp", "",
    std::vector<int>( ),legendloc,zoomloc, panels_p[3].getInt(),false,false);

    // Dummy panel
    panels_p[9] = dataplotter_p->panel( "---", "----", "---", "",
    std::vector<int>( ),legendloc,zoomloc, panels_p[7].getInt(),false,false);
	 */

    create_dock(dataplotter_p, panels_p[0],dock_xml_p);
	return true;

}// end buildDataPlotWindow


Bool FlagAgentDisplay::buildReportPlotWindow()
{

	setReportLayout();
	AlwaysAssert( report_dock_xml_p != NULL , AipsError );

	reportplotter_p.reset(new plotter_t(gui_state));

	report_panels_p.resize(1);
	string zoomloc="";
	string legendloc="bottom";
    report_panels_p[0] = create_panel(reportplotter_p,0,false);

    create_dock( reportplotter_p, report_panels_p[0], report_dock_xml_p );
	return true;

}// end buildReportPlotWindow



void FlagAgentDisplay::getUserInput( ) {

    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << ( active( ) ? 
                       "FlagAgentDisplay getUserInput " : 
                       "FlagAgentDisplay getUserInput ERROR no active GUIs " ) <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! active( ) ) return;

    if ( gui_state->input_received ) {
        // GUI input received while this thread was preoccupied...
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->input_received = false;
        return;
    } else  {
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->userChoice_p = "Continue";
        gui_state->output = std::promise<bool>( );
        gui_state->input_needed = true;
    }

    auto fut = gui_state->output.get_future();
    fut.get( );
    {
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->input_received = false;
    }
}



void FlagAgentDisplay :: getReportUserInput() {

    static const auto debug = getenv("GRPC_DEBUG");
    if ( debug ) {
        std::cerr << ( active( ) ? 
                       "FlagAgentDisplay getReportUserInput " : 
                       "FlagAgentDisplay getReportUserInput ERROR no active GUIs " ) <<
            " (process " << getpid( ) << ", thread " << 
            std::this_thread::get_id() << ")" << std::endl;
        fflush(stderr);
    }

    if ( ! active( ) ) return;

    if ( gui_state->input_received ) {
        // GUI input received while this thread was preoccupied...
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->input_received = false;
        return;
    } else  {
        std::lock_guard<std::mutex> lock(gui_state->set_values);
        gui_state->userChoice_p = "Continue";
        gui_state->output = std::promise<bool>( );
        gui_state->input_needed = true;
    }

    auto fut = gui_state->output.get_future();
    fut.get( );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////   Plot functions
//////////////////////////////////////////////////////////////////////////////////////////////////////

void FlagAgentDisplay::DisplayRaster(Int xdim, Int ydim, Vector<Float> &data, int frame)
{
	if(data.nelements() != (uInt) xdim*ydim)
	{
		logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
		*logger_p << LogIO::WARN << "Error in data XY dimensions. Not plotting" << LogIO::POST;
		return;
	}
	//    cout << "panel id : " << frame << endl;

	// dataplotter_p->release( panel_p.getInt( ) );
    erase( dataplotter_p, frame );

	//	dataplotter_p->line(x, y, "blue", "time", panel_p.getInt() );
	raster( dataplotter_p, frame, std::vector<float>(data.begin(),data.end()), xdim, ydim );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void FlagAgentDisplay::DisplayLine(Int /*xdim*/, Vector<Double> &xdata, Vector<Float> &ydata, String label, String color, Bool hold, int frame)
{
	if(xdata.nelements() != ydata.nelements())
	{
		logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
		*logger_p << LogIO::WARN << "X and Y plot data have different sizes. Not plotting " << LogIO::POST;
		return;
	}
	if( hold==false ) erase( dataplotter_p, frame );
	line( dataplotter_p, frame, std::vector<float>(xdata.begin(),xdata.end()), std::vector<float>(ydata.begin(),ydata.end()), color, label );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

void FlagAgentDisplay::DisplayScatter(Int /*xdim*/, Vector<Double> &xdata, Vector<Float> &ydata, String label, String color, Bool hold, int frame)
{
	if(xdata.nelements() != ydata.nelements())
	{
		logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
		*logger_p << LogIO::WARN << "X and Y plot data have different sizes. Not plotting " << LogIO::POST;
		return;
	}
	if( hold==false ) erase( dataplotter_p, frame );
	scatter( dataplotter_p, frame, std::vector<float>(xdata.begin(),xdata.end()),
             std::vector<float>(ydata.begin(),ydata.end()), color, label, "dot", 1, 4 );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

// Display scatter dots, with optional vertical error-bars, or circles or text labels, or all together.
// Send in vectors of size 0 for radii and pointlabels and error to disable them.
void FlagAgentDisplay::DisplayLineScatterError(std::shared_ptr<FlagAgentDisplay::plotter_t> plotter, String &plottype, Vector<Float> &xdata, Vector<Float> &ydata, String &errortype, Vector<Float> &error, String label, String color, int frame)
{
	// Check X and Y shapes
	if(xdata.nelements() != ydata.nelements())
	{
		logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
		*logger_p << LogIO::WARN << "X, Y plot data have different sizes. Not plotting." << LogIO::POST;
		return;
	}

	if(plottype==String("scatter"))
	{
		// Scatter-plot of X and Y
		scatter( plotter, frame, std::vector<float>(xdata.begin(),xdata.end()),
                 std::vector<float>(ydata.begin(),ydata.end()),color,label,"dot",1,4 );
	}
	else if (plottype==String("line"))
	{
		// Line plot of X and Y
		line( plotter, frame, std::vector<float>(xdata.begin(),xdata.end()),
              std::vector<float>(ydata.begin(),ydata.end()),color,label );
	}
	else
	{
		return;
	}

	// Check Error shape ( if not zero, must match xdata,ydata)
	if(error.nelements()>0)
	{
		if(error.nelements()!=xdata.nelements())
		{
			logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
			*logger_p << LogIO::WARN << "Number of Error elements is different from data. Not plotting." << LogIO::POST;
			return;
		}

		if(errortype == "bar")
		{
			Vector<Double>pointerror(2);
			Vector<Double>xval(2);
			for ( Int onepoint=0; onepoint<(Int) error.nelements(); onepoint++)
			{
				xval[0]=xdata[onepoint];
				xval[1]=xdata[onepoint];
				pointerror[0]=ydata[onepoint] + error[onepoint];
				pointerror[1]=ydata[onepoint] - error[onepoint];
				line( plotter, frame, std::vector<float>(xval.begin(),xval.end()),
                      std::vector<float>(pointerror.begin(),pointerror.end()), color,"" );
			}
		}
		else if(errortype == "separator")
		{
			Vector<Double>pointerror(2);
			Vector<Double>xval(2);
			for ( Int onepoint=0; onepoint<(Int) error.nelements(); onepoint++)
			{
				xval[0]=xdata[onepoint];
				xval[1]=xdata[onepoint];
				pointerror[0]=ydata[onepoint] + error[onepoint];
				pointerror[1]=0;
				line( plotter, frame, std::vector<float>(xval.begin(),xval.end()),
                      std::vector<float>(pointerror.begin(),pointerror.end()),"black","" );
			}
		}
		else if(errortype == "circle")
		{
			// Check Circle shape ( if not zero, must match xdata,ydata)
			Vector<Double>xval(1),yval(1);
			for ( Int onepoint=0; onepoint<(Int) error.nelements(); onepoint++)
			{
				xval[0]=xdata[onepoint];
				yval[0]=ydata[onepoint];
				// function signature should allow a float for the error bar
				scatter( plotter, frame, std::vector<float>(xval.begin(),xval.end()),
                         std::vector<float>(yval.begin(),yval.end()), color, "", "ellipse",
                         static_cast<Int>(error[onepoint]), 4 );
				// Note : I think this plots the centre point too. If so, it's a repeat....
			}
		}
		else
		{
			logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
			*logger_p << LogIO::WARN << "Unknown error-plot type. Not plotting." << LogIO::POST;
		}
	}// if error

	// Do a similar thing for labels.
	// NOTE : Cannot plot point-labels or text strings yet.

}
//////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////     GUI Layout Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////


Bool FlagAgentDisplay :: setDataLayout()
{

	dock_xml_p = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<ui version=\"4.0\">\
 <class>dock01</class>\
 <widget class=\"QDockWidget\" name=\"dock01\">\
  <property name=\"geometry\">\
   <rect>\
    <x>0</x>\
    <y>0</y>\
    <width>770</width>\
    <height>120</height>\
   </rect>\
  </property>\
  <property name=\"sizePolicy\">\
   <sizepolicy hsizetype=\"Preferred\" vsizetype=\"Preferred\">\
    <horstretch>0</horstretch>\
    <verstretch>0</verstretch>\
   </sizepolicy>\
  </property>\
  <property name=\"minimumSize\">\
   <size>\
    <width>770</width>\
    <height>87</height>\
   </size>\
  </property>\
  <property name=\"windowTitle\">\
   <string/>\
  </property>\
  <widget class=\"QWidget\" name=\"dockWidgetContents\">\
   <layout class=\"QGridLayout\" name=\"gridLayout_2\">\
    <item row=\"0\" column=\"0\">\
     <layout class=\"QGridLayout\" name=\"gridLayout\">\
      <item row=\"0\" column=\"0\">\
       <widget class=\"QPushButton\" name=\"PrevBaseline\">\
        <property name=\"text\">\
         <string>Prev Baseline</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"1\">\
       <widget class=\"QPushButton\" name=\"NextBaseline\">\
        <property name=\"text\">\
         <string>Next Baseline</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"2\">\
       <layout class=\"QVBoxLayout\" name=\"verticalLayout\">\
        <property name=\"spacing\">\
         <number>0</number>\
        </property>\
        <property name=\"sizeConstraint\">\
         <enum>QLayout::SetMinimumSize</enum>\
        </property>\
        <item>\
         <widget class=\"QCheckBox\" name=\"FixAntenna1\">\
          <property name=\"text\">\
           <string>Fix Antenna1</string>\
          </property>\
         </widget>\
        </item>\
        <item>\
         <widget class=\"QCheckBox\" name=\"FixAntenna2\">\
          <property name=\"text\">\
           <string>Fix Antenna2</string>\
          </property>\
         </widget>\
        </item>\
       </layout>\
      </item>\
      <item row=\"0\" column=\"3\">\
       <widget class=\"QPushButton\" name=\"NextSpw\">\
        <property name=\"text\">\
         <string>Next SPW</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"4\">\
       <widget class=\"QPushButton\" name=\"NextScan\">\
        <property name=\"text\">\
         <string>Next Scan</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"5\">\
       <widget class=\"QPushButton\" name=\"NextField\">\
        <property name=\"text\">\
         <string>Next Field</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"6\">\
       <widget class=\"QPushButton\" name=\"StopDisplay\">\
        <property name=\"text\">\
         <string>Stop Display</string>\
        </property>\
       </widget>\
      </item>\
      <item row=\"0\" column=\"7\">\
       <widget class=\"QPushButton\" name=\"Quit\">\
        <property name=\"text\">\
         <string>Quit</string>\
        </property>\
       </widget>\
      </item>\
     </layout>\
    </item>\
   </layout>\
  </widget>\
 </widget>\
 <resources/>\
 <connections/>\
</ui>\
";

	return true;

}// end of SetLayout


Bool FlagAgentDisplay :: setReportLayout()
{

	report_dock_xml_p = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<ui version=\"4.0\">\
 <class>dock01</class>\
 <widget class=\"QDockWidget\" name=\"dock01\">\
  <property name=\"geometry\">\
   <rect>\
    <x>0</x>\
    <y>0</y>\
    <width>320</width>\
    <height>80</height>\
   </rect>\
  </property>\
  <property name=\"sizePolicy\">\
   <sizepolicy hsizetype=\"Preferred\" vsizetype=\"Preferred\">\
    <horstretch>0</horstretch>\
    <verstretch>0</verstretch>\
   </sizepolicy>\
  </property>\
  <property name=\"minimumSize\">\
   <size>\
    <width>320</width>\
    <height>80</height>\
   </size>\
  </property>\
  <property name=\"windowTitle\">\
   <string/>\
  </property>\
  <widget class=\"QWidget\" name=\"dockWidgetContents\">\
   <property name=\"enabled\">\
    <bool>true</bool>\
   </property>\
   <property name=\"sizePolicy\">\
    <sizepolicy hsizetype=\"Preferred\" vsizetype=\"Preferred\">\
     <horstretch>0</horstretch>\
     <verstretch>0</verstretch>\
    </sizepolicy>\
   </property>\
   <layout class=\"QGridLayout\" name=\"gridLayout\">\
    <item row=\"0\" column=\"0\">\
     <layout class=\"QHBoxLayout\" name=\"horizontalLayout\">\
      <item>\
       <widget class=\"QPushButton\" name=\"Prev\">\
        <property name=\"text\">\
         <string>Prev</string>\
        </property>\
       </widget>\
      </item>\
      <item>\
       <widget class=\"QPushButton\" name=\"Next\">\
        <property name=\"text\">\
         <string>Next</string>\
        </property>\
       </widget>\
      </item>\
      <item>\
       <widget class=\"QPushButton\" name=\"Quit\">\
        <property name=\"text\">\
         <string>Quit</string>\
        </property>\
       </widget>\
      </item>\
     </layout>\
    </item>\
   </layout>\
  </widget>\
 </widget>\
 <resources/>\
 <connections/>\
</ui>\
";

	return true;

}


} //# NAMESPACE CASA - END


