# Algorithm Architecture

## Description

This document describes a _stable and scalable_ architecture which
must scale for computing load and complexity as well as for continued
evolution of algorithms.  On the one hand this architecture must be
rooted firmly in the theory of interferometry, optics and signal
processing to provide a stable basis for a scalable software in terms
of computing, development costs and inevitable evolution of the
algorithms in the future.  On the other hand, the architecture must be
rooted firmly in modern scientific software design principles for the
resulting software implementation to be scalable and stable over time.

For the end-users the architecture must provide direct access to core
architecturally significant components, implemented such that they do
not need to change with the complexity of the applications they are
used in.  These core components should also have solid theoretical
basis to exist, making it possible for the architecture to be stable
across a variety of high-level data processing procedures/algorithms
which may differ from each other in details but must ultimately
satisfy fundamental scientific principles applicable to the various
domains that use the technique of indirect imaging (e.g. principles of
physics & optics, statistics and signal processing for interferometric
imaging in radio astronomy).

The "end-users" from our point of view are both -- researchers who use
these algorithmic components for data processing directly to produce
astronomical results, and also individual and groups engaged in
algorithms R&D and may use these core algorithmic components in their
software.  For the latter group, the implementation must provide easy
and direct access to well established algorithms so that those don't
need to be re-implemented/reinvented, and instead their research can
focus on solving real outstanding problems.

## Mathematical Framework

$\int a^2 + 2x +3 dx$
