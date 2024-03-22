# Algorithm Architecture [!UNDER CONSTRUCTION!]
## Introduction

A _stable and scalable_ architecture must scale for computing load and
complexity as well as for continued evolution of algorithms.  On the
one hand this architecture must be rooted firmly in the theory of
interferometry, optics and signal processing to provide a stable basis
for a scalable software in terms of computing, development costs and
inevitable evolution of the algorithms in the future.  On the other
hand, the architecture must be rooted firmly in modern scientific
software design principles for the resulting software implementation
to be scalable and stable over time.

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
scientific results, and also individuals and groups engaged in
algorithms R&D and may use these core algorithmic components in their
software.  For the latter group, the implementation must provide easy
and direct access to well established algorithms so that those don't
need to be re-implemented/reinvented, and instead their research can
focus on solving real outstanding problems.

## Mathematical Framework

The relationship between the raw data and the image of the sky is
expressed as

$\vec V_{ij} = M^{DI}_ {ij} A_{ij} I( \vec s ) + n_{ij}$
 
where $\vec V_{ij}$ is a full-polarization vector representing the
measurement from a pair of antennas represented by subscripts $i$ and
$j$, $M^{DI}_ {ij}$ is the direction-independent (DI) Mueller matrices
that models the corruptions (instrumental or atmospheric), $I$
is the sky brightness distribution and $n_{ij}$ is the
additive noise with Normal probability distribution.


$A_{ij}$ is an operator that transforms $I$ to the data domain
including the DD effects and is constructed as $A_{ij}=S_{ij} ~ F ~M^{DD}_ {ij} (\vec s)$ where $S_{ij}$ is the data domain sampling
function, $F$ is the Fourier transform operator, and $M^{DD}_ {ij}$ the direction-dependent (DD) Mueller matrix that encodes the DD
mixing of the elements of the poliarization vector respectively.
The equation above can be expanded as:

$\vec V_{ij} = M^{DI}_ {ij} S_{ij} F M^{DD}_ {ij} I( \vec s ) + n_{ij}$

The goal of calibration algorithms is to derive models for $M^{DI}_ {ij}$ and $M^{DD}_ {ij}$,
given $V$, a model for $I$ and statistical
characterization of $n$. The goal of image reconstruction algorithms
is to derive $I$, given $V$ corrected for $M^{DI}_ {ij}$, a model for
$M^{DD}_ {ij}$ and a statistical description of $n$.  This essentially
requires computing $A^{-1}$. However it can be shown that $A$ is
singular.  Image reconstruction is therefore a fundamentally ill-posed
inverse problem that _requires_ iterative algorithms to find a
solution (a model for the sky brightness distribution) consistent with
the noise and the metric for convergence.  Unsurprisingly, numerically
successful and computationally efficient algorithms for both
calibration and imaging have been fundamentally directed-search
algorithms requiring calculation of the derivative of the chosen
objective function, or variants involving derivative calculation (or
directed-search itself) as a critical step.

<!-- fidelity terms, constraints terms -->