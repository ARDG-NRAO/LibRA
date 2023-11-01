/*
 * PThreadUtils.cc
 *
 *  Created on: Feb 29, 2016
 *      Author: nakazato
 */

#include <singledishfiller/Filler/PThreadUtil.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

void create_thread(pthread_t *tid, pthread_attr_t *attr, void *(*func)(void *),
        void *param) {
    int ret = pthread_create(tid, attr, func, param);
    THROW_IF(ret != 0,
            "create_thread: failed to create thread with default creation");
}
void join_thread(pthread_t *tid, void **status) {
    int ret = pthread_join(*tid, status);
    THROW_IF(ret != 0, "join_thread: failed to finalize the thread");
}

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END


