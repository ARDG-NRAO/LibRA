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

The relationship between the raw data measured by a pair of antennas
denoted by subscripts $i$ and $j$ and the image of the sky is
expressed as

$V(\vec{u}_{ij}) = G_{ij} \int I(\vec{s}) P_{ij}(\vec{s}) e^{-\iota \vec{u}_{ij} . \vec{s}} d\vec{s} + n_{ij}$

where $V$ is a full-polarization vector of length 4 representing the
measurement from two antennas separated by the vector $\vec{u_{ij}}$
(the visibility), $G_{ij}$ is a 4x4 matrix that models the
direction-independent (DI) corruptions (instrumental or atmospheric) and
$P_{ij}$ models the direction-dependent (DD) effects, $I$ representations
the sky brightness distribution and $n_{ij}$ is the additive noise
with Normal probability distribution.  In linear algebra notation this
can be written as

$\vec V = [G] ~ [A] ~ \vec I + \vec n$

where $G$ is the DI corruption operator and $A$ an operator that transforms $I$ to the data domain and includes the DD effects.  The goal of calibration algorithms is to correct for the effects of $G$. The goal of imaging algorithms is to derive $I$ given $V$, $G$ and a statistical description of $n$, which essentially requires computing $=A^{-1}$. However it can be shown that $A$ is singular.  Image reconsturction is therefore fundamentally an ill-posed inverse problem and _requires_ iterative algorithms to find an optimal solution (a model for the sky brightness distribution consistent with $n$).
