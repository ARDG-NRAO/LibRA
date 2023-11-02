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
*     $Id: fwproj.f 17791 2004-08-25 02:28:46Z cvsmgr $
*-----------------------------------------------------------------------
C
C Grid a number of visibility records
C
      subroutine gwgrid (uvw, dphase, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, rownum,
     $     scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $     support, convsize, sampling, wconvsize, convfunc, 
     $     chanmap, polmap,
     $     sumwt)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      double complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(3),
     $     offset(3)
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      real weight(nvischan, nrow), phase
      double precision sumwt(npol, nchan)
      integer rownum
      integer support(*), rsupport
      integer chanmap(nchan), polmap(npol)
      integer dopsf

      double complex nvalue

      integer convsize, sampling, wconvsize
      complex convfunc(convsize/2-1, convsize/2-1, wconvsize),
     $     cwt

      real norm
      real wt

      logical owp

      double precision pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      double precision pi
      data pi/3.14159265358979323846/
      
      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).ne.0.0)) then
               call swp(uvw(1,irow), dphase(irow), freq(ichan), c, 
     $              scale, offset, sampling, pos, loc, off, phasor)
               iloc(3)=max(1, min(wconvsize, loc(3)))
               rsupport=support(iloc(3))
               if (owp(nx, ny, wconvsize, loc, rsupport)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
C If we are making a PSF then we don't want to phase
C rotate but we do want to reproject uvw
                        if(dopsf.eq.1) then
                           nvalue=cmplx(weight(ichan,irow))
                        else
                           nvalue=weight(ichan,irow)*
     $                          (values(ipol,ichan,irow)*phasor)
                        end if
C norm will be the value we would get for the peak
C at the phase center. We will want to normalize 
C the final image by this term.
                        norm=0.0
                        do iy=-rsupport,rsupport
                           iloc(2)=1+abs(iy*sampling+off(2))
C                         !$OMP PARALLEL DEFAULT(SHARED) PRIVATE(ix)
C                           !SOMP DO
                           do ix=-rsupport,rsupport
                              iloc(1)=1+abs(ix*sampling+off(1))
                              if(uvw(3,irow).gt.0.0) then
                                 cwt=conjg(convfunc(iloc(1),
     $                                iloc(2), iloc(3)))
                              else
                                 cwt=convfunc(iloc(1),
     $                                iloc(2), iloc(3))
                              end if
                              grid(loc(1)+ix,
     $                             loc(2)+iy,apol,achan)=
     $                             grid(loc(1)+ix,
     $                             loc(2)+iy,apol,achan)+
     $                             nvalue*cwt
                              norm=norm+real(cwt)
                           end do
C                         !$OMP END DO
C                         !$OMP END PARALLEL
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               else
C                  write(*,*) uvw(3,irow), pos(1), pos(2), pos(3),
C     $                 loc(1), loc(2), loc(3)
               end if
            end if
         end do
         end if
      end do
      return
      end


      subroutine sectgwgridd (uvw, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, 
     $     grid, nx, ny, npol, nchan, 
     $     support, convsize, sampling, wconvsize, convfunc, 
     $     chanmap, polmap,
     $     sumwt, x0,
     $    y0, nxsub, nysub, rbeg, rend, loc, off, phasor)

      implicit none
      
      integer, intent(in) :: nx,ny, npol, nchan, nvispol, nvischan, nrow
      double precision, intent(in) :: uvw(3, nrow)
      complex, intent(in) ::  values(nvispol, nvischan, nrow)
      double complex, intent(inout) ::  grid(nx, ny, npol, nchan)
      integer, intent(in) :: x0, y0, nxsub, nysub
      complex, intent(in) ::  phasor(nvischan, nrow) 
      integer, intent(in) ::  flag(nvispol, nvischan, nrow)
      integer, intent(in) ::  rflag(nrow)
      real, intent(in) :: weight(nvischan, nrow)
      double precision, intent(inout) :: sumwt(npol, nchan)
      integer, intent(in) ::  support(*)
      integer :: rsupport
      integer, intent(in) ::  chanmap(nchan), polmap(npol)
      integer, intent(in) :: dopsf

      double complex :: nvalue

      integer, intent(in) :: convsize, sampling, wconvsize
      complex, intent(in) :: convfunc(convsize/2-1,convsize/2-1,
     $  wconvsize)
      complex :: cwt

      real :: norm
      real :: wt

      logical :: onmywgrid

      integer, intent(in) ::  loc(3, nvischan, nrow)
      integer, intent(in) :: off(3, nvischan, nrow)
      integer :: iloc(3)
      integer, intent(in) :: rbeg, rend
      integer :: ix, iy, ipol, ichan
      integer :: apol, achan, irow
      integer :: posx, posy, msupportx, msupporty, psupportx, psupporty
C      complex :: cfunc(convsize/2-1, convsize/2-1)
C      integer :: npoint

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).ne.0.0)) then
               iloc(3)=max(1, min(wconvsize, loc(3, ichan, irow)))
               rsupport=support(iloc(3))
C               write(*,*) irow, ichan, iloc(3), rsupport 
               if (onmywgrid(loc(1, ichan, irow), nx, ny, wconvsize, 
     $              x0, y0, nxsub, nysub, rsupport, msupportx, msupporty
     $         , psupportx, psupporty)) then
CCC I thought removing the if in the inner loop will be faster
CCC but not really as i have to calculate more conjg at this stage
C                  npoint=rsupport*sampling+1
C                  if(uvw(3,irow).gt.0.0) then
C                     cfunc(1:npoint, 1:npoint)=conjg(
C     $                    convfunc(1:npoint,1:npoint,iloc(3)))
C                  else
C                     cfunc(1:npoint, 1:npoint)=
C     $                    convfunc(1:npoint,1:npoint,iloc(3))
C                  end if
CCCC
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
C If we are making a PSF then we don't want to phase
C rotate but we do want to reproject uvw
                        if(dopsf.eq.1) then
                           nvalue=cmplx(weight(ichan,irow))
                        else
                           nvalue=weight(ichan,irow)*
     $                      (values(ipol,ichan,irow)*phasor(ichan,irow))
                        end if
C norm will be the value we would get for the peak
C at the phase center. We will want to normalize 
C the final image by this term.
                        norm=0.0
C                        msupporty=-rsupport
C                        psupporty=rsupport
C                        psupportx=rsupport
C                        msupportx=-rsupport
C                        if((loc(1, ichan, irow)-rsupport) .lt. x0) 
C     $                       msupportx=  -(loc(1, ichan, irow)-x0)
C                        if((loc(1, ichan, irow)+rsupport).ge.(x0+nxsub)) 
C     $                       psupportx=  x0+nxsub-loc(1, ichan, irow)-1   
C                        if((loc(2, ichan, irow)-rsupport) .lt. y0) 
C     $                       msupporty=  -(loc(2, ichan, irow)-y0)
C                        if((loc(2, ichan, irow)+rsupport).ge.(y0+nysub)) 
C     $                       psupporty=  y0+nysub-loc(2, ichan, irow)-1 
                        do iy=msupporty,psupporty
                           posy=loc(2, ichan, irow)+iy
                           iloc(2)=1+abs(iy*sampling+off(2, ichan,irow))
                           do ix=msupportx,psupportx
                              posx=loc(1, ichan, irow)+ix
                              iloc(1)=1+abs(ix*sampling+
     $                             off(1,ichan,irow))
                              cwt=convfunc(iloc(1),
     $                                iloc(2), iloc(3))
                              if(uvw(3,irow).gt.0.0) cwt=conjg(cwt)
C                              else
C                                 cwt=convfunc(iloc(1),
C     $                                iloc(2), iloc(3))
C                              end if
                              grid(posx,posy,apol,achan)=
     $                             grid(posx,posy,apol,achan)+
     $                             nvalue*cwt
                              norm=norm+real(cwt)
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               else
C                  write(*,*) uvw(3,irow), pos(1), pos(2), pos(3),
C     $                 loc(1), loc(2), loc(3)
               end if
            end if
         end do
         end if
      end do
      return
      end
C
C single precision gridder
      subroutine sectgwgrids (uvw, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, 
     $     grid, nx, ny, npol, nchan, 
     $     support, convsize, sampling, wconvsize, convfunc, 
     $     chanmap, polmap,
     $     sumwt, x0,
     $    y0, nxsub, nysub, rbeg, rend, loc, off, phasor)

      implicit none
      
      integer, intent(in) :: nx,ny, npol, nchan, nvispol, nvischan, nrow
      double precision, intent(in) :: uvw(3, nrow)
      complex, intent(in) ::  values(nvispol, nvischan, nrow)
      complex, intent(inout) ::  grid(nx, ny, npol, nchan)
      integer, intent(in) :: x0, y0, nxsub, nysub
      complex, intent(in) ::  phasor(nvischan, nrow) 
      integer, intent(in) ::  flag(nvispol, nvischan, nrow)
      integer, intent(in) ::  rflag(nrow)
      real, intent(in) :: weight(nvischan, nrow)
      double precision, intent(inout) :: sumwt(npol, nchan)
      integer, intent(in) ::  support(*)
      integer :: rsupport
      integer, intent(in) ::  chanmap(nchan), polmap(npol)
      integer, intent(in) :: dopsf

      double complex :: nvalue

      integer, intent(in) :: convsize, sampling, wconvsize
      complex, intent(in) :: convfunc(convsize/2-1,convsize/2-1,
     $  wconvsize)
      complex :: cwt

      real :: norm
      real :: wt

      logical :: onmywgrid

      integer, intent(in) ::  loc(3, nvischan, nrow)
      integer, intent(in) :: off(3, nvischan, nrow)
      integer :: iloc(3)
      integer, intent(in) :: rbeg, rend
      integer :: ix, iy, ipol, ichan
      integer :: apol, achan, irow
      integer :: posx, posy, msupportx, msupporty, psupportx, psupporty
C      complex :: cfunc(convsize/2-1, convsize/2-1)
C      integer :: npoint

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).ne.0.0)) then
               iloc(3)=max(1, min(wconvsize, loc(3, ichan, irow)))
               rsupport=support(iloc(3))
               if (onmywgrid(loc(1, ichan, irow), nx, ny, wconvsize, 
     $              x0, y0, nxsub, nysub, rsupport, msupportx, 
     $ msupporty, psupportx, psupporty)) then
CCC I thought removing the if in the inner loop will be faster
CCC but not really as i have to calculate more conjg at this stage
C                  npoint=rsupport*sampling+1
C                  if(uvw(3,irow).gt.0.0) then
C                     cfunc(1:npoint, 1:npoint)=conjg(
C     $                    convfunc(1:npoint,1:npoint,iloc(3)))
C                  else
C                     cfunc(1:npoint, 1:npoint)=
C     $                    convfunc(1:npoint,1:npoint,iloc(3))
C                  end if
CCCC
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
C If we are making a PSF then we don't want to phase
C rotate but we do want to reproject uvw
                        if(dopsf.eq.1) then
                           nvalue=cmplx(weight(ichan,irow))
                        else
                           nvalue=weight(ichan,irow)*
     $                      (values(ipol,ichan,irow)*phasor(ichan,irow))
                        end if
C norm will be the value we would get for the peak
C at the phase center. We will want to normalize 
C the final image by this term.
                        norm=0.0
C                        msupporty=-rsupport
C                        psupporty=rsupport
C                        psupportx=rsupport
C                        msupportx=-rsupport
C                        if((loc(1, ichan, irow)-rsupport) .lt. x0) 
C     $                       msupportx=  -(loc(1, ichan, irow)-x0)
C                        if((loc(1, ichan, irow)+rsupport).ge.(x0+nxsub)) 
C     $                       psupportx=  x0+nxsub-loc(1, ichan, irow)-1   
C                        if((loc(2, ichan, irow)-rsupport) .lt. y0) 
C     $                       msupporty=  -(loc(2, ichan, irow)-y0)
C                        if((loc(2, ichan, irow)+rsupport).ge.(y0+nysub)) 
C     $                       psupporty=  y0+nysub-loc(2, ichan, irow)-1 
                        do iy=msupporty,psupporty
                           posy=loc(2, ichan, irow)+iy
                           iloc(2)=1+abs(iy*sampling+off(2, ichan,irow))
                           do ix=msupportx,psupportx
                              posx=loc(1, ichan, irow)+ix
                              iloc(1)=1+abs(ix*sampling+
     $                             off(1,ichan,irow))
                              cwt=convfunc(iloc(1),
     $                                iloc(2), iloc(3))
                              if(uvw(3,irow).gt.0.0) cwt=conjg(cwt)
C                              else
C                                 cwt=convfunc(iloc(1),
C     $                                iloc(2), iloc(3))
C                              end if
                              grid(posx,posy,apol,achan)=
     $                             grid(posx,posy,apol,achan)+
     $                             nvalue*cwt
                              norm=norm+real(cwt)
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               else
C                  write(*,*) uvw(3,irow), pos(1), pos(2), pos(3),
C     $                 loc(1), loc(2), loc(3)
               end if
            end if
         end do
         end if
      end do
      return
      end


C
C Degrid a number of visibility records
C
      subroutine dwgrid (uvw, dphase, values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, rownum, scale, offset, grid, nx, ny, npol, nchan, freq,
     $     c, support, convsize, sampling, wconvsize, convfunc,
     $     chanmap, polmap)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(3),
     $     offset(3)
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      integer rownum
      integer support(*), rsupport
      integer chanmap(*), polmap(*)

      complex nvalue

      integer convsize, wconvsize, sampling
      complex convfunc(convsize/2-1, convsize/2-1, wconvsize),
     $     cwt

      real norm, phase

      logical owp

      double precision pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi
      data pi/3.14159265358979323846/

      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if
C

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan)) then
               call swp(uvw(1,irow), dphase(irow), freq(ichan), c,
     $              scale, offset, sampling, pos, loc, off, phasor)
               iloc(3)=max(1, min(wconvsize, loc(3)))
               rsupport=support(iloc(3))
               if (owp(nx, ny, wconvsize, loc, rsupport)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then

                        nvalue=0.0
                        do iy=-rsupport,rsupport
                           iloc(2)=1+abs(iy*sampling+off(2))
                           do ix=-rsupport,rsupport
                              iloc(1)=1+abs(ix*sampling+off(1))
                              if(uvw(3,irow).gt.0.0) then
                                 cwt=conjg(convfunc(iloc(1),
     $                                iloc(2), iloc(3)))
                              else
                                 cwt=convfunc(iloc(1),
     $                                iloc(2), iloc(3))
                              end if
                              nvalue=nvalue+conjg(cwt)*
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=nvalue*conjg(phasor)
                     end if
                  end do
               end if
            end if
         end do
         end if
      end do
      return
      end
C

      subroutine sectdwgrid (uvw, values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, grid, nx, ny, npol, nchan, 
     $     support, convsize, sampling, wconvsize, convfunc,
     $     chanmap, polmap,  rbeg, rend, loc,off,phasor)

      implicit none
      integer, intent(in) ::  nrow,nx,ny,npol, nchan, nvispol, nvischan
      complex, intent(inout) ::  values(nvispol, nvischan, nrow)
      complex, intent(in) :: grid(nx, ny, npol, nchan)
      double precision, intent(in) ::  uvw(3, nrow)
      complex , intent(in) :: phasor(nvischan, nrow) 
      integer, intent(in) ::  flag(nvispol, nvischan, nrow)
      integer, intent(in) :: rflag(nrow)
      integer, intent(in) ::  support(*), sampling
      integer, intent(in) :: chanmap(*), polmap(*)
      integer, intent(in) :: rbeg, rend
      integer, intent(in)  :: loc(3, nvischan, nrow)
      integer, intent(in) :: off(3, nvischan, nrow) 
      complex :: nvalue

      integer, intent(in) :: convsize, wconvsize
      complex, intent(in) :: convfunc(convsize/2-1, convsize/2-1, 
     $     wconvsize)
      complex ::     cwt

      real :: norm, phase

      logical :: owp

      integer :: iloc(3), rsupport
      integer :: ix, iy, ipol, ichan
      integer :: apol, achan, irow
      real :: wt, wtx, wty


      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan)) then
               iloc(3)=max(1, min(wconvsize, loc(3, ichan, irow)))
               rsupport=support(iloc(3))
               if (owp(nx, ny, wconvsize, loc(1,ichan,irow), rsupport))
     $              then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then

                        nvalue=0.0
                        do iy=-rsupport,rsupport
                           iloc(2)=1+abs(iy*sampling+off(2,ichan,irow))
                           do ix=-rsupport,rsupport
                              iloc(1)=1+abs(ix*sampling+
     $                             off(1,ichan,irow))
                              
                              if(uvw(3,irow).gt.0.0) then
                                 cwt=conjg(convfunc(iloc(1),
     $                                iloc(2), iloc(3)))
                              else
                                 cwt=convfunc(iloc(1),
     $                                iloc(2), iloc(3))
                              end if
                              nvalue=nvalue+conjg(cwt)*
     $                           grid(loc(1,ichan,irow)+ix,loc(2,ichan,
     $                           irow)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=nvalue*conjg(
     $                       phasor(ichan, irow))
                     end if
                  end do
               end if
            end if
         end do
         end if
      end do
      return
      end
C



C Calculate gridded coordinates and the phasor needed for
C phase rotation. 
C
      subroutine swp (uvw, dphase, freq, c, scale, offset, 
     $     sampling, pos, loc, off, phasor)
      implicit none
      integer loc(3), off(3), sampling
      double precision uvw(3), freq, c, scale(3), offset(3)
      double precision pos(3)
      double precision dphase, phase
      complex phasor
      integer idim
      double precision pi
      data pi/3.14159265358979323846/

C      pos(3)=(scale(3)*uvw(3)*freq/c)*(scale(3)*uvw(3)*freq/c)
C     $     +offset(3)+1.0;
C      pos(3)=(scale(3)*uvw(3)*freq/c)+offset(3)+1.0;
      pos(3)=sqrt(abs(scale(3)*uvw(3)*freq/c))+offset(3)+1.0
      loc(3)=nint(pos(3))
      off(3)=0

      do idim=1,2
         pos(idim)=scale(idim)*uvw(idim)*freq/c+
     $        (offset(idim)+1.0)
         loc(idim)=nint(pos(idim))
         off(idim)=nint((loc(idim)-pos(idim))*sampling)
      end do

      phase=-2.0D0*pi*dphase*freq/c
      phasor=cmplx(cos(phase), sin(phase))
      return 
      end
      logical function owp (nx, ny, nw, loc, support)
      implicit none
      integer nx, ny, nw, loc(3), support
      owp=(support.gt.0).and.
     $     (loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $     (loc(2)-support.ge.1).and.(loc(2)+support.le.ny).and.
     $     (loc(3).ge.1).and.(loc(3).le.nw)
      return
      end
      logical function onmywgrid(loc, nx, ny, nw, nx0, ny0, nxsub,nysub,
     $     support, msuppx, msuppy, psuppx, psuppy)
      implicit none
      integer, intent(in) :: nx, ny, nw, nx0, ny0, nxsub, nysub, loc(3), 
     $     support
      integer, intent(out) :: msuppx, msuppy, psuppx, psuppy
      integer :: loc1sub, loc1plus, loc2sub, loc2plus
      msuppx=merge(-support, nx0-loc(1), loc(1)-support  >= nx0)
      msuppy=merge(-support, ny0-loc(2), loc(2)-support >= ny0)
      psuppx=merge(support, nx0+nxsub-loc(1)-1 , (loc(1)+support) 
     $ < (nx0+nxsub))
      psuppy=merge(support, ny0+nysub-loc(2)-1 , (loc(2)+support) 
     $ < (ny0+nysub))
      loc1sub=loc(1)-support
      loc1plus=loc(1)+support
      loc2sub=loc(2)-support
      loc2plus=loc(2)+support
      
       onmywgrid=(support.gt.0).and. (loc(3).ge.1) .and. (loc(3).le.nw)
     $     .and. (loc1plus .ge. nx0) .and. (loc1sub 
     $     .le. (nx0+nxsub)) .and.(loc2plus .ge. ny0) .and. 
     $     (loc2sub .le. (ny0+nysub))
       return
       end
