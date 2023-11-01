/* -*- mode: c++ -*- */
//# MPIGlue.h: casacore::Smooth out source code differences for MPI vs non-MPI builds
//# Copyright (C) 2016
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
//
// This header file is intended to facilitate writing source modules that use
// MPI, so that they can also be compiled without an MPI compiler and run
// without the MPI libraries. Note, however, that the definitions in this header
// file do not provide a complete replacement of MPI functions with stub
// versions; that task is beyond the scope of a single header file (duplicating
// MPI datatypes, for example, would take much more effort). Instead, this file
// is currently only used by new tclean2 code that uses MPI functions in C++,
// and, therefore, only provides support for those parts of MPI that are
// currently needed in the tclean2 code.
#ifndef MPI_GLUE_H_
#define MPI_GLUE_H_

#if HAVE_MPI

# include <mpi.h>
# define UNUSED_WITHOUT_MPI
# define MPI_BOOL_TYPE unsigned char // TODO: replace with bool under MPI-3
# define MPI_BOOL MPI_UNSIGNED_CHAR // TODO: replace with MPI_CXX_BOOL under MPI-3
# warning "Implementing unsafe MPI-2 workaround for MPI_Comm_group_create"
# define MPI_Comm_group_create(c, g, t, pc) MPI_Comm_create(c, g, pc)

#else // ! HAVE_MPI

# include <cstddef>
# define UNUSED_WITHOUT_MPI __attribute__((unused))
# define MPI_BOOL_TYPE unsigned char
# define MPI_BOOL 0
# define MPI_INT 1
# define MPI_FLOAT 2
# define MPI_DOUBLE 3
namespace casa {
typedef int MPI_Comm;
typedef int MPI_Group;
typedef int MPI_Datatype;
typedef int MPI_Op;
typedef std::ptrdiff_t MPI_Aint;
};
# define MPI_COMM_WORLD 0
# define MPI_COMM_SELF 0
# define MPI_COMM_NULL -1
# define MPI_GROUP_NULL -1
# define MPI_UNDEFINED -1
# define MPI_IN_PLACE nullptr
# define MPI_Comm_size(c, sp)                       \
	do {                                            \
		*(sp) = (((c) != MPI_COMM_NULL) ? 1 : 0);   \
	} while (0)
# define MPI_Comm_rank(c, rp)                       \
	do {                                            \
		*(rp) = (((c) != MPI_COMM_NULL) ? 0 : -1);  \
	} while (0)
# define MPI_Comm_dup(c, cp)                    \
	do {                                        \
		*(cp) = (c);                            \
	} while (0)
# define MPI_Comm_free(c) do {} while (0)
# define MPI_Comm_split(comm, color, key, cp)   \
	do {                                        \
		*(cp) = (((color) != MPI_UNDEFINED)     \
		         ? (comm)                       \
		         : MPI_COMM_NULL);              \
	} while (0)
# define MPI_Comm_group(c, pg) do {             \
		*(pg) = (((c) != MPI_COMM_NULL)         \
		         ? (MPI_GROUP_NULL + 1)         \
		         : MPI_GROUP_NULL);             \
	} while (0)
# define MPI_Group_free(pg) do {} while (0)
# define MPI_Group_incl(g, nr, pr, pg) do {     \
		if ((g) != MPI_GROUP_NULL && (nr) > 0)  \
			*(pg) = MPI_GROUP_NULL + 1;         \
		else                                    \
			*(pg) = MPI_GROUP_NULL;             \
	} while (0)
# define MPI_Comm_group_create(c, g, t, pc) do {            \
		if ((c) != MPI_COMM_NULL && (g) != MPI_GROUP_NULL)  \
			*(pc) = (c);                                    \
		else                                                \
			*(pc) = MPI_COMM_NULL;                          \
	} while (0)
# define MPI_Type_create_struct(a, b, c, d, pd) do { *(pd) = 0; } while (0)
# define MPI_Type_create_resized(a, b, c, pd) do { *(pd) = 0; } while (0)
# define MPI_Type_free(pd) do {} while (0)
# define MPI_Type_commit(pd) do {} while (0)
# define MPI_Op_create(a, b, po) do { *(po) = 0; } while (0)
# define MPI_Op_free(po) do {} while (0)
# define MPI_Barrier(c) do {} while (0)
# define MPI_Bcast(a, b, c, d, e) do {} while (0)
# define MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm) \
	do {                                                            \
		assert((sendbuf) == MPI_IN_PLACE);                          \
	} while (0)
# define MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm) \
	do {                                                                \
		assert((sendbuf) == MPI_IN_PLACE                                \
		       && (sendtype) == (recvtype)                              \
		       && (sendcount) == (recvcount));                          \
	} while (0)
# define MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm) \
	do {                                                                \
		assert((sendbuf) == MPI_IN_PLACE                                \
		       && (sendtype) == (recvtype)                              \
		       && (sendcount) == (recvcounts)[0]);                      \
	} while (0)
#endif // HAVE_MPI

#endif // MPI_GLUE_H_
