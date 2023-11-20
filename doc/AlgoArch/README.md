# Algorithm Architecture

## Description

This document describes a _stable and scalable_ architecture with a
theoretical basis for the processing required for image reconstruction
from interfermetric radio telescope.  The architecture must scale for
computing load and complexity as well as for continued evolution of
algorithms.  On the one hand this architecture must be rooted firmly
in the theory of interferometry, optics and signal processing to
provide a stable basis for a scalable software in terms of computing,
development costs and inevitable evolution of the algorithms in the
future.  On the other hand, the architecture must be rooted firmly in
modern scientific software design principles for the resulting
software implementation to be scalable and stable over time.  Note that
the useful design principles for scientific software may have
significant differences from principles for, e.g., non-scientific
software development.

For the end-users a _"stable and scalable"_ architecture must
translate to core architecturally significant components such that
their implementation does not change with the complexity of the
applications they are used in, and it should also be possible to
describe these core components theoretical such that the architecture
does not change with each high-level data processing
procedure/algorithm, which may differ from each other in details but
must ultimately satisfy fundamental scientific principles applicable
to the various domains that rely on variants of the general technique
of indirect imaging (e.g. principles of physics & optics, statistics
and signal processing for interferometric imaging in radio astronomy).
The "end-users" from our point of view are both -- researchers who use
these algorithmic components for data processing to directly produce
astronomical results, and also individual and groups who may use these
algorithmic components in their research for new algorithms which
inevitably requires software development.