*=======================================================================
*     -*- FORTRAN -*-
*     Copyright (C) 1999,2001,2002
*     Associated Universities, Inc. Washington DC, USA.
*     
*     This library is free software; you can redistribute it and/or
*     modify it under the terms of the GNU Library General Public
*     License as published by the Free Software Foundation; either
*     version 2 of the License, or (at your option) any later version.
*     
*     This library is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU Library General Public License for more details.
*     
*     You should have received a copy of the GNU Library General Public
*     License along with this library; if not, write to the Free
*     Software Foundation, Inc., 675 Massachusetts Ave, Cambridge,
*     MA 02139, USA.
*     
*     Correspondence concerning AIPS++ should be addressed as follows:
*     Internet email: aips2-request@nrao.edu.
*     Postal address: AIPS++ Project Office
*     National Radio Astronomy Observatory
*     520 Edgemont Road
*     Charlottesville, VA 22903-2475 USA
*     
*     $Id:$
*-----------------------------------------------------------------------
C     The LeakyAntsol algorithm (A&A, v.375, p.344-350 (2001)).
C
C     Modified calantso.FullArray.f file to include poln leakage terms.
C
C                                S.Bhatnagar   22 Sept. 2000
C
C     Gain is set to a normal value when ByPass is 1 and to a smaller
C     value otherwise.  This implies that for normal antsol, the gain
C     (and hence the search step size) is also normal and for
C     leakyantsol, the gain is much smaller (finer step size).
C   
C                                S.Bhatnagar   30 Oct. 2001
C
      subroutine leakyantso (corgain, corwt, nant, antgain, leakage, 
     $     mode, oresid, nresid, scanwt,refant,stat,ByPass)
C
      integer           maxnant
      character*(*)	routine
      parameter		(routine = 'calantso')
      parameter		(maxnant = 120)

      integer		nant,mode,stat
      complex	corgain(nant,*), antgain(*), leakage(*)
      real	oresid, nresid, scanwt
      real  corwt(nant, *)
C
      integer		iter, niter, iant, jant, ByPass
      integer		refant
C
C     Number of logical antennas that this routine can handle.
C     
      real  eps, antwt, presid, gain
      complex	gtop(maxnant),top(maxnant)
      real	bottom(maxnant)
      data		eps	/1e-10/
      data		niter	/30000/
      data		gain	/0.001/
C=====================================================================
C
C initialise gain only if the gain on entry is zero
C
      oresid = 0.0
      nresid = 0.0
      scanwt = 0.0

C
C set initial estimates of gains
C     
c$$$      do iant=1,nant
c$$$         write(*,*) 'w=',(corwt(iant,jant),jant=1,nant)
c$$$      enddo
      do iant = 1, nant
         antwt = 0.0
         do jant = 1, nant
            antwt = antwt + corwt(iant, jant)
         enddo
         if (antwt.gt.0.0) then
            scanwt = scanwt + antwt
            if (abs(antgain(iant)).eq.0.0) then
               do jant = 1, nant
                  antgain(iant) = antgain(iant) + corgain(iant, jant) *
     1               corwt(iant, jant)
               enddo
               antgain(iant) = antgain(iant) / antwt
            end if
         else
            antgain(iant) = cmplx(0.0,0.0)
         end if
      enddo

C
C Set initial leakage terms
C
      if (ByPass .eq. 1) then
         gain=0.1
         goto 1212
      else
         gain=0.01
      endif
c$$$      do iant = 1, nant
c$$$         antwt=0.0
c$$$         top(1)=0.0
c$$$         top(2)=0.0
c$$$         do jant = 1, nant
c$$$            antwt = antwt + corwt(iant, jant)
c$$$         enddo
c$$$         if (abs(leakage(iant)) .eq. 0.0) then
c$$$            do jant = 1, nant
c$$$               if (iant .ne. jant) then
c$$$                  top(1)=top(1)+corgain(iant,jant)*corwt(iant,jant)
c$$$                  top(2)=top(2)+conjg(antgain(jant))*corwt(iant,jant)
c$$$               endif
c$$$            enddo
c$$$            leakage(iant)=(top(1)-antgain(iant)*top(2))/antwt
c$$$         endif
c$$$      enddo

      do iant = 1, nant
         antwt = 0.0
         do jant = 1, nant
            antwt = antwt + corwt(iant, jant)
         enddo
         if (antwt .gt. 0.0) then
            if (abs(leakage(iant)) .eq. 0.0) then
               do jant = 1, nant
                  if (iant .ne. jant) then
                     leakage(iant) = leakage(iant) + 
     1                    (corgain(iant,jant)-
     1                    antgain(iant)*conjg(antgain(jant)))*
     1                    corwt(iant,jant)
                  endif
               enddo
               leakage(iant)=leakage(iant)/antwt
            endif
         else
            leakage(iant)=cmplx(0.0,0.0)
         endif
      enddo
 1212 continue
c     
c find antgains w.r.t. to the ref. ant.
c
      if (scanwt.eq.0.0) then
         write (0,*)routine,':  no valid data'
      end if
C
C Default mode is Amp-Phase solution.
C
      if (mode.eq.0) then
C
C Phase only solution.  Put antgain.Amp = 1.0
C
         do iant = 1, nant
            if(abs(antgain(iant)).ne.0.0) then
               antgain(iant) = antgain(iant)/abs(antgain(iant))
            end if
            if(abs(leakage(iant)).ne.0.0) then
               leakage(iant) = leakage(iant)/abs(leakage(iant))
            end if
         enddo
      elseif (mode.eq.1) then
C
C Amp only solution.  Put antgain.Phase = 0.0
C
         do iant = 1, nant
            antgain(iant) = abs(antgain(iant))
            leakage(iant) = abs(leakage(iant))
         enddo
      end if
c
c find initial rms
c
      presid = 0.0
      antwt=0.0
      do iant = 1, nant
         do jant = 1, nant
            oresid=oresid+abs(antgain(iant)*conjg(antgain(jant)) +
     1           leakage(iant)*conjg(leakage(jant)) - 
     1           corgain(iant,jant))**2 * corwt(iant,jant)
            antwt = antwt + corwt(iant, jant)
         enddo
      enddo
      oresid=oresid/antwt
c      write(*,*)"Initial RMS=",oresid
c
c      oresid = sqrt(oresid/scanwt)
c      oresid = 0.0001
      if (oresid .lt. eps) goto 110
c
      nresid = 0.0
      do 100 iter = 1, niter
c         if (mod(iter,10) .eq. 0) write(*,*) iter
         
C
C
C----------------------Gain terms--------------------------
C
C
         do iant = 1, nant
            top(iant) = cmplx (0.0, 0.0)
            gtop(iant)=cmplx(0,0)
            bottom(iant) = 0.0
            do jant=1,nant
               if (iant .ne. jant) then
                  top(iant) = top(iant) 
     1                 +corwt(iant, jant) * antgain(jant) * 
     1                 corgain(iant, jant)
                  gtop(iant)=gtop(iant)+conjg(leakage(jant))*
     1                 antgain(jant)*corwt(iant,jant)
                 bottom(iant) = bottom(iant) + abs(antgain(jant))**2 *
     1                 corwt(iant, jant)
               endif
            enddo
            gtop(iant) = gtop(iant)*leakage(iant)
         enddo
c
c find new antenna gain
c
         do iant = 1, nant
            if (bottom(iant).ne.0.0) then
               antgain(iant) = (1.0-gain) * antgain(iant) +
     1            gain * (top(iant)-gtop(iant)) / bottom(iant)
            end if
         enddo
C
C
C----------------------Leakage terms--------------------------
C
C
         if (ByPass .eq. 1) goto 2121
         do iant = 1, nant
            top(iant) = cmplx (0.0, 0.0)
            gtop(iant)=cmplx(0,0)
            bottom(iant) = 0.0
            do jant=1,nant
               if (iant .ne. jant) then
                  top(iant) = top(iant) 
     1                 +corwt(iant, jant) * leakage(jant) * 
     1                 corgain(iant, jant)
                  gtop(iant)=gtop(iant)+conjg(antgain(jant))*
     1                 leakage(jant)*corwt(iant,jant)
                 bottom(iant) = bottom(iant) + abs(leakage(jant))**2 *
     1                 corwt(iant, jant)
               endif
            enddo
            gtop(iant) = gtop(iant)*antgain(iant)
         enddo
c
c find new leakage gain
c
         do 301 iant = 1, nant
            if (bottom(iant).ne.0.0) then
               leakage(iant) = (1.0-gain) * leakage(iant) +
     1            gain * (top(iant)-gtop(iant)) / bottom(iant)
            end if
 301     continue
 2121    continue
C
C
C-------------------------------------------------------------
C
C
c
c is this a phase-only solution?
c
         if (mode.eq.0) then
            do iant = 1, nant
               if(abs(antgain(iant)).ne.0.0) then
                  antgain(iant) = antgain(iant)/abs(antgain(iant))
               end if
               if(abs(leakage(iant)).ne.0.0) then
                  leakage(iant) = leakage(iant)/abs(leakage(iant))
               end if
            enddo
         elseif (mode.eq.1) then
            do iant = 1, nant
               antgain(iant) = abs(antgain(iant))
               leakage(iant) = abs(leakage(iant))
            enddo
         end if
c     
c find residual
c
         presid = nresid
         nresid = 0.0
         antwt = 0.0
         if (mode.eq.1)then
            do jant = 1, nant
               do iant = 1, nant
                  nresid = nresid + corwt(iant, jant) *
     1                 abs(antgain(iant)*antgain(jant) +
     2                 leakage(iant)*leakage(jant) -
     3                 abs(corgain(iant, jant)))**2
                  antwt = antwt + corwt(iant, jant)
               enddo
            enddo
         else
            do jant = 1, nant
               do iant = 1, nant
                  nresid = nresid + corwt(iant, jant) *
     1                 abs(antgain(iant)*conjg(antgain(jant)) +
     2                 leakage(iant)*conjg(leakage(jant)) -
     3                 corgain(iant, jant))**2
                  antwt = antwt + corwt(iant, jant)
               enddo
            enddo
         endif
         nresid = sqrt(nresid/antwt)
c
         if (abs(nresid-presid).le.eps*oresid) goto 110
c
  100 continue
c
  110 continue
      if (iter.ge.niter) then
         stat = -iter
      else
         stat = iter
      endif
c      write(*,*)'NResid-PResid=',nresid,presid,abs(nresid-presid)
C
C Reference the phases with the reference antenna.
C 
      if (abs(antgain(refant)) .ne. 0.0) then
         top(1) = conjg(antgain(refant))/abs(antgain(refant))
      else
         write(0,*) "###Error: Reference antenna is dead for gains!"
         top(1)=cmplx(1,0)
      endif

      if (ByPass .eq. 0) then
         if (abs(leakage(refant)) .ne. 0.0) then
c            top(2) = conjg(leakage(refant))/abs(leakage(refant))
            top(2) = leakage(refant)
         else
          write(0,*) "###Error: Reference antenna is dead for leakages!"
            top(2)=cmplx(1,0)
         endif
      endif

      do iant=1,nant
         antgain(iant) = antgain(iant)*top(1)
         if (ByPass .eq. 0) leakage(iant) = leakage(iant)-top(2)
      enddo
C
C Make Antgain and Leakage orthogonal
C
c$$$      if (ByPass .ne. 1) then
c$$$         top(1)=cmplx(0.0,0.0)
c$$$         do iant = 1, nant
c$$$            top(1)=top(1)+conjg(antgain(iant))*leakage(iant)
c$$$         enddo
c$$$
c$$$         top(2)=(top(1))/abs(top(1))
c$$$         
c$$$         bottom(1)=cmplx(0.0,0.0)
c$$$         bottom(2)=cmplx(0.0,0.0)
c$$$         do iant = 1, nant
c$$$c            bottom(1)=bottom(1)+abs(antgain(iant))**2
c$$$c            bottom(2)=bottom(1)+abs(leakage(iant))**2
c$$$            bottom(1)=bottom(1)+antgain(iant)*conjg(antgain(iant))
c$$$            bottom(2)=bottom(1)+leakage(iant)*conjg(leakage(iant))
c$$$C           antgain(iant)=antgain(iant)*top(2)
c$$$         enddo
c$$$
c$$$         antwt=atan(2.0*abs(top(1))/(bottom(1)-bottom(2)))/2.0
c$$$
c$$$         do iant = 1, nant
c$$$            top(1)= antgain(iant)*cos(antwt)+leakage(iant)*sin(antwt)
c$$$            top(2)=-antgain(iant)*sin(antwt)+leakage(iant)*cos(antwt)
c$$$            antgain(iant)=top(1)
c$$$            leakage(iant)=top(2)
c$$$         enddo
c$$$
c$$$         top(1)=cmplx(0,0)
c$$$         do iant=1,nant
c$$$            top(1)=conjg(antgain(iant))*leakage(iant)
c$$$         enddo
c$$$c$$$         write(*,*)'Gdag.Alpha=',abs(top(1)),
c$$$c$$$     1        57.295*atan2(imagpart(top(1)),realpart(top(1)))        
c$$$      endif
 999  continue
      end
