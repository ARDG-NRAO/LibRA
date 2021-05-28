
#include <alma/ASDM/Misc.h>

#include <sys/stat.h>
#include <unistd.h>
#include <casacore/casa/System/AppState.h>
#include <casa/OS/Path.h>

using namespace std;

namespace asdm {
    // CASA needs a unique implementation here, the ALMA implementation is not included in the CASA code tree

    string ASDMUtils::pathToxslTransform( const string& xsltFilename) {
        struct stat path_stat;
        string xslt_path = "alma/asdm/" + xsltFilename;
        const std::list<std::string> &state_path = casacore::AppStateSource::fetch( ).dataPath( );
        if ( state_path.size( ) > 0 ) {
            for ( std::list<std::string>::const_iterator it=state_path.begin(); it != state_path.end(); ++it ) {
                string xslpath = casacore::Path( *it + "/" + xslt_path ).absoluteName( );
                 if ( stat(xslpath.c_str( ), &path_stat ) == 0 && S_ISREG(path_stat.st_mode) ) {
                     return xslpath;
                }
            }
        } else {
            // try using CASAPATH
            char * casaPath_p;
            if ((casaPath_p = getenv("CASAPATH")) != 0) {
                string casaPath(casaPath_p);
                vector<string> casaPathElements;
                strsplit(casaPath, ' ', casaPathElements);
                if (casaPath.size() > 0) {
                    string dataPath = casaPathElements[0];
                    if (dataPath.back()!='/') dataPath += "/";
                    string xslpath = casacore::Path( dataPath + "data/" + xslt_path ).absoluteName( );
                    if ( stat(xslpath.c_str( ), &path_stat ) == 0 && S_ISREG(path_stat.st_mode) ) {
                        return xslpath;
                    }
                }
            }
        }

        if (getenv("ASDM_DEBUG"))
            cout  << "pathToxslTransform returns an empty xsltPath " << endl;

        return "" ;  // An empty string will be interpreted as no file found.
    }
}

