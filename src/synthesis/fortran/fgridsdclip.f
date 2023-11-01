*=======================================================================
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
*            Internet email: aips2-request@nrao.edu.
*            Postal address: AIPS++ Project Office
*                            National Radio Astronomy Observatory
*                            520 Edgemont Road
*                            Charlottesville, VA 22903-2475 USA
*
*     $Id$
*-----------------------------------------------------------------------
C
C Calculate gridded coordinates
C
      subroutine sgridsdclip (xy, sampling, pos, loc, off)
      implicit none
      integer sampling
      integer loc(2), off(2)
      double precision xy(2)
      real pos(2)
      integer idim

      do idim=1,2
         pos(idim)=xy(idim)+1.0
         loc(idim)=nint(pos(idim))
         off(idim)=nint((loc(idim)-pos(idim))*sampling)
      end do
      return 
      end
C
C Is this on the grid?
C
      logical function ogridsdclip (nx, ny, loc, support)
      implicit none
      integer nx, ny, loc(2), support
      ogridsdclip=(loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $     (loc(2)-support.ge.1).and.(loc(2)+support.le.ny)
      return
      end
C
C Grid a number of visibility records: single dish gridding
C but with complex images including additional process for
C min/max clipping 
C
C This subroutine assumes specific clipping process after
C all the data are accumulated
C
C Post-accumulation process is as follows. On each pixel,
C
C - if npoints == 1, grid = gmin, wgrid = wmin, sumwt must
C   be updated accordingly
C - if npoints == 2, grid = gmin + gmax, wgrid = wmin + wmax,
C   sumwt must be updated accordingly
C - otherwise, leave grid, wgrid, and sumwt as they are
C
      subroutine ggridsdclip (xy, values, nvispol, nvischan,
     $   dowt, flag, rflag, weight, nrow, irow,
     $   grid, wgrid, 
     $   npoints, gmin, wmin, gmax, wmax,
     $   nx, ny, npol, nchan, 
     $   support, sampling, convFunc, chanmap, polmap, sumwt)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      real wgrid(nx, ny, npol, nchan)
      integer npoints(nx, ny, npol, nchan)
      complex gmin(nx, ny, npol, nchan)
      complex gmax(nx, ny, npol, nchan)
      real wmin(nx, ny, npol, nchan)
      real wmax(nx, ny, npol, nchan)
      double precision xy(2,nrow)
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      real weight(nvischan, nrow)
      double precision sumwt(npol, nchan)
      integer irow
      integer support, sampling
      integer chanmap(nvischan), polmap(nvispol)
      integer dowt

      complex nvalue, thevalue

      real convFunc(*)
      real norm
      real wt, wtx, wty
      real swap, theweight, nweight

      logical ogridsdclip

      real pos(2), rloc(2)
      integer loc(2), off(2)
      integer rbeg, rend
      integer irad((2*support+1)**2)
      integer ix, iy, ipol, ichan
      integer apol, achan
      integer ir
      integer xloc(2*support+1), yloc(2*support+1)
      integer ax, ay

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         call sgridsdclip(xy(1,irow), sampling, pos, loc, off)
C          if (ogridsdclip(nx, ny, loc, support)) then
         if (ogridsdclip(nx, ny, loc, 0)) then
            ir=1
            norm=-(support+1)*sampling+off(1)
            rloc(2)=-(support+1)*sampling+off(2)
            do iy=1,2*support+1
               rloc(2)=rloc(2)+sampling
               rloc(1)=norm
               do ix=1,2*support+1
                  rloc(1)=rloc(1)+sampling
                  irad(ir)=sqrt(rloc(1)**2+rloc(2)**2)+1
                  ir=ir+1
               end do
            end do
            xloc(1)=loc(1)-support
            do ix=2,2*support+1
               xloc(ix)=xloc(ix-1)+1
            end do
            yloc(1)=loc(2)-support
            do iy=2,2*support+1
               yloc(iy)=yloc(iy-1)+1
            end do
            do ichan=1, nvischan
               achan=chanmap(ichan)+1
               if((achan.ge.1).and.(achan.le.nchan).and.
     $              (weight(ichan,irow).gt.0.0)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        if (dowt.eq.1) then
                         thevalue=1.0
                        else
                         thevalue=conjg(values(ipol,ichan,irow))
                        end if
                        theweight=weight(ichan,irow)
                        norm=0.0
                        ir=1
C                        do iy=-support,support
                        do iy=1,2*support+1
                           ay=yloc(iy)
                           if ((ay.ge.1).and.(ay.le.ny)) then
C                            do ix=-support,support
                           do ix=1,2*support+1
                              ax=xloc(ix)
                              if ((ax.ge.1).and.(ax.le.nx)) then
                              ir = (iy-1)*(2*support+1) + ix
                              wt=convFunc(irad(ir))
                              nweight=theweight*wt
                              nvalue=thevalue*nweight
                              if (npoints(ax,ay,apol,achan).eq.0) then
                                gmin(ax,ay,apol,achan)=thevalue
                                wmin(ax,ay,apol,achan)=nweight
                              else if
     $                          (npoints(ax,ay,apol,achan).eq.1) then
                                if (real(gmin(ax,ay,apol,achan)).lt.
     $                            real(thevalue)) then
                                  gmax(ax,ay,apol,achan)=thevalue
                                  wmax(ax,ay,apol,achan)=nweight
                                else
                                  gmax(ax,ay,apol,achan)=
     $                              gmin(ax,ay,apol,achan)
                                  wmax(ax,ay,apol,achan)=
     $                              wmin(ax,ay,apol,achan)
                                  gmin(ax,ay,apol,achan)=thevalue
                                  wmin(ax,ay,apol,achan)=nweight
                                end if
                              else
                              if (real(thevalue).le.
     $                          real(gmin(ax,ay,apol,achan))) then
                                nvalue=wmin(ax,ay,apol,achan)*
     $                            gmin(ax,ay,apol,achan)
                                gmin(ax,ay,apol,achan)=thevalue
                                swap=nweight
                                nweight=wmin(ax,ay,apol,achan)
                                wmin(ax,ay,apol,achan)=swap
                              else if (real(thevalue).ge.
     $                          real(gmax(ax,ay,apol,achan))) then
                                nvalue=wmax(ax,ay,apol,achan)*
     $                            gmax(ax,ay,apol,achan)
                                gmax(ax,ay,apol,achan)=thevalue
                                swap=nweight
                                nweight=wmax(ax,ay,apol,achan)
                                wmax(ax,ay,apol,achan)=swap
                              end if
                              grid(ax,ay,apol,achan)=
     $                             grid(ax,ay,apol,achan)+nvalue
                              wgrid(ax,ay,apol,achan)=
     $                             wgrid(ax,ay,apol,achan)+nweight
                              norm=norm+nweight
                              end if
                              npoints(ax,ay,apol,achan)=
     $                          npoints(ax,ay,apol,achan)+1
                              end if
                           end do
                           end if
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+norm
                     end if
                  end do
               end if
            end do
         end if
         end if
      end do
      return
      end
