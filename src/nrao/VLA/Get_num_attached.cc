/*
    Get_num_attached.cc: Find the number of attached processes to a shared
    memory segment.
    Copyright (C) 1999
    Associated Universities, Inc. Washington DC, USA.

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.

    Correspondence concerning AIPS++ should be addressed as follows:
           Internet email: aips2-request@nrao.edu.
           Postal address: AIPS++ Project Office
                           National Radio Astronomy Observatory
                           520 Edgemont Road
                           Charlottesville, VA 22903-2475 USA

    $Id$
*/
/* Make a call to the system utility shmctl() to determine the number
 of processes attached to the shared memory segment denoted by the
 parameter shmid */

/* If the return value from shmctl is -1, return 0 if errno is EINVAL
   (shmid is not a valid shared memory id) or -1 other wise. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

extern int errno;

int Get_num_attached(int shmid) {

   struct shmid_ds buf;
   int err;

   if (shmctl(shmid, IPC_STAT, &buf) == 0) {
      return(buf.shm_nattch);
      }
   else {
      err = errno;
      perror("error on calling shmctl()");
      if (err == EINVAL) {
         return(0);
         }
      else {
         return(-1);
         }
      }
   }
