*=======================================================================
*     Copyright (C) 1999-2012
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
*     $Id: fgridft.f 19685 2006-10-05 20:57:29Z rurvashi $
*-----------------------------------------------------------------------
C
C Grid a number of visibility records
C
      subroutine ggrid (uvw, dphase, values, nvispol, nvischan,
     $   dopsf, flag, rflag, weight, nrow, rownum,
     $   scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $   support, sampling, convFunc, chanmap, polmap, sumwt)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      double complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(2),
     $     offset(2)
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      real weight(nvischan, nrow)
      double precision sumwt(npol, nchan)
      integer rownum
      integer support, sampling
      integer chanmap(nchan), polmap(npol)
      integer dopsf

      double complex nvalue

      double precision convFunc(*)
      double precision norm
      double precision wt, wtx, wty

      logical ogrid

      double precision pos(2)
      integer loc(2), off(2), iloc(2)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      
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
               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c, 
     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogrid(nx, ny, loc, support)) then
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
     $                        (values(ipol,ichan,irow)*phasor)
                        end if
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2))+1
                           wty=convFunc(iloc(2))
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              grid(loc(1)+ix,loc(2)+iy,apol,achan)=
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)+
     $                             nvalue*wt
                              norm=norm+wt
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
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
C     Single precision gridder
      subroutine ggrids (uvw, dphase, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, rownum,
     $     scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $     support, sampling, convFunc, chanmap, polmap, sumwt)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(2),
     $     offset(2)
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      real weight(nvischan, nrow)
      double precision sumwt(npol, nchan)
      integer rownum
      integer support, sampling
      integer chanmap(nchan), polmap(npol)      
      integer dopsf

      double complex nvalue

      double precision convFunc(*)
      double precision norm
      double precision wt, wtx, wty

      logical ogrid

      double precision pos(2)
      integer loc(2), off(2), iloc(2)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      
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
               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c, 
     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogrid(nx, ny, loc, support)) then
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
     $                        (values(ipol,ichan,irow)*phasor)
                        end if
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2))+1
                           wty=convFunc(iloc(2))
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              grid(loc(1)+ix,loc(2)+iy,apol,achan)=
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)+
     $                             nvalue*wt
                              norm=norm+wt
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               end if
            end if
         end do
         end if
      end do
      return
      end


      subroutine sectggridd(values, nvispol, nvischan,
     $   dopsf, flag, rflag, weight, nrow, 
     $   grid, nx, ny, npol, nchan, 
     $   support, sampling, convFunc, chanmap, polmap, sumwt, x0,
     $    y0, nxsub, nysub, rbeg, rend, loc, off, phasor)
      implicit none

      integer, intent(in) :: nx, ny, npol, nchan, nvispol, nvischan,nrow
      complex, intent(in) :: values(nvispol, nvischan, nrow)
      double complex, intent(inout) :: grid(nx, ny, npol, nchan)
      integer, intent(in) :: x0, y0, nxsub, nysub
      double precision, intent(in) :: convFunc(*)
      integer, intent(in) :: chanmap(nvischan), polmap(nvispol)
      integer, intent(in) ::  flag(nvispol, nvischan, nrow)
      integer, intent(in) ::  rflag(nrow)
      real, intent(in) :: weight(nvischan, nrow)
      double precision, intent(inout) :: sumwt(npol, nchan)
      integer, intent(in) :: support, sampling
      integer, intent(in) ::  dopsf, rbeg, rend


      integer, intent(in)  :: loc(2, nvischan, nrow)
      integer, intent(in) :: off(2, nvischan, nrow) 
      integer :: iloc(2)
      complex, intent(in) :: phasor(nvischan, nrow) 
      double complex :: nvalue

      double precision :: norm
      double precision :: wt, wtx, wty

      logical :: onmygrid

      double precision :: pos(2)
      integer :: ix, iy, ipol, ichan
      integer :: apol, achan, irow
      integer :: posx, posy
      integer :: msupporty, psupporty
      integer :: msupportx, psupportx
      


      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).ne.0.0)) then
C               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c, 
C     $              scale, offset, sampling, pos, loc, off, phasor)
C      write(*,*) loc(1, ichan, irow), loc(2, ichan, irow), irow,ichan
               if (onmygrid(loc(1,ichan, irow), nx, ny, x0, y0, nxsub, 
     $          nysub, support)) then
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
     $                    (values(ipol,ichan,irow)*phasor(ichan, irow))
                        end if
                        norm=0.0
                        msupporty=-support
                        psupporty=support
                        psupportx=support
                        msupportx=-support
                        if((loc(1, ichan, irow)-support) .lt. x0) 
     $                       msupportx=  -(loc(1, ichan, irow)-x0)
                        if((loc(1, ichan, irow)+support).ge.(x0+nxsub)) 
     $                       psupportx=  x0+nxsub-loc(1, ichan, irow)-1   
                        if((loc(2, ichan, irow)-support) .lt. y0) 
     $                       msupporty=  -(loc(2, ichan, irow)-y0)
                        if((loc(2, ichan, irow)+support).ge.(y0+nysub)) 
     $                       psupporty=  y0+nysub-loc(2, ichan, irow)-1   
C         write(*,*) msupportx, psupportx, msupporty, psupporty            
                        do iy=msupporty,psupporty
                           posy=loc(2, ichan, irow)+iy
C           if( (posy .lt. (y0+nysub)) .and. 
C     $        (posy.ge. y0)) then
                            iloc(2)=abs(sampling*iy+off(2,ichan,irow))+1
                            wty=convFunc(iloc(2))
                            do ix=msupportx,psupportx
                               posx=loc(1, ichan, irow)+ix
C           if( (posx .lt. (x0+nxsub)) .and. 
C     $       (posx .ge. x0)) then
C            write(*,*) posx, posy, loc(1), loc(2), x0, y0, nxsub, nysub
                                iloc(1)=abs(sampling*ix+off(1,ichan, 
     $                                 irow))+1
                                  wtx=convFunc(iloc(1))
                                  wt=wtx*wty
                                  grid(posx,posy,apol,achan)=
     $                             grid(posx, posy,apol,achan)+
     $                                   nvalue*wt
                                    norm=norm+wt
C              write(*,*) iloc(1), iloc(2), posx, posy
C               end if
                              end do
C               end if
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
                     end if
                  end do
               end if
C          if onmygrid
            end if
         end do
         end if
      end do
      return 
      end

C      subroutine sectggrids(uvw, dphase, values, nvispol, nvischan,
C     $     dopsf, flag, rflag, weight, nrow, 
C     $     scale, offset, grid, nx, ny, npol, nchan, freq, c,
C     $     support, sampling, convFunc, chanmap, polmap, sumwt, x0,
C     $     y0, nxsub, nysub, rbeg, rend)

       subroutine sectggrids(values, nvispol, nvischan,
     $   dopsf, flag, rflag, weight, nrow, 
     $   grid, nx, ny, npol, nchan, 
     $   support, sampling, convFunc, chanmap, polmap, sumwt, x0,
     $    y0, nxsub, nysub, rbeg, rend, loc, off, phasor)



      implicit none
      
      integer, intent(in) ::  nx,ny,npol,nchan, nvispol, nvischan, nrow
      complex, intent(in) :: values(nvispol, nvischan, nrow)
      complex, intent(inout) :: grid(nx, ny, npol, nchan)
      integer, intent(in) ::  x0, y0, nxsub, nysub
      double precision, intent(in) ::  convFunc(*)
      integer, intent(in) :: chanmap(nvischan), polmap(nvispol)
      integer, intent(in) ::  flag(nvispol, nvischan, nrow)
      integer, intent(in) ::  rflag(nrow)
      real, intent(in) ::  weight(nvischan, nrow)
      double precision, intent(inout) ::  sumwt(npol, nchan)
      integer, intent(in) :: support, sampling
      integer, intent(in) ::  dopsf, rbeg, rend

       integer, intent(in)  :: loc(2, nvischan, nrow)
      integer, intent(in) :: off(2, nvischan, nrow) 
      integer :: iloc(2)
      complex, intent(in) :: phasor(nvischan, nrow) 
      double complex :: nvalue

      double precision :: norm
      double precision :: wt, wtx, wty

      logical :: onmygrid

      double precision :: pos(2)
      integer :: ix, iy, ipol, ichan
      integer :: apol, achan, irow
      integer :: posx, posy
      integer :: msupporty, psupporty
      integer :: msupportx, psupportx
        


      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan).and.
     $           (weight(ichan,irow).ne.0.0)) then
C               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c, 
C     $              scale, offset, sampling, pos, loc, off, phasor)
               if (onmygrid(loc(1,ichan, irow), nx, ny, x0, y0, nxsub, 
     $          nysub, support)) then
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
     $                    (values(ipol,ichan,irow)*phasor(ichan, irow))
                        end if
                        norm=0.0
                        msupporty=-support
                        psupporty=support
                        psupportx=support
                        msupportx=-support
                        if((loc(1, ichan, irow)-support) .lt. x0) 
     $                       msupportx=  -(loc(1, ichan, irow)-x0)
                        if((loc(1, ichan, irow)+support).ge.(x0+nxsub)) 
     $                       psupportx=  x0+nxsub-loc(1, ichan, irow)-1   
                        if((loc(2, ichan, irow)-support) .lt. y0) 
     $                       msupporty=  -(loc(2, ichan, irow)-y0)
                        if((loc(2, ichan, irow)+support).ge.(y0+nysub)) 
     $                       psupporty=  y0+nysub-loc(2, ichan, irow)-1   
                        do iy=msupporty,psupporty
                           posy=loc(2, ichan, irow)+iy
                           iloc(2)=abs(sampling*iy+off(2,ichan,irow))+1
                           wty=convFunc(iloc(2))
                           do ix=msupportx,psupportx
                              posx=loc(1, ichan, irow)+ix
                              iloc(1)=abs(sampling*ix+off(1,ichan, 
     $                             irow))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              grid(posx,posy,apol,achan)=
     $                             grid(posx, posy,apol,achan)+
     $                             nvalue*wt
                              norm=norm+wt
                           end do
                        end do
                        sumwt(apol,achan)=sumwt(apol,achan)+
     $                       weight(ichan,irow)*norm
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
C Degrid a number of visibility records
C
      subroutine dgrid (uvw, dphase, values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, rownum, scale, offset, grid, nx, ny, npol, nchan, freq,
     $     c, support, sampling, convFunc, chanmap, polmap)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(2),
     $     offset(2)
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      integer rownum
      integer support, sampling
      integer chanmap(*), polmap(*)

      complex nvalue

      double precision convFunc(*)
      real norm

      logical ogrid

      double precision pos(2)
      integer loc(2), off(2), iloc(2)
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      real wt, wtx, wty

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
            if((achan.ge.1).and.(achan.le.nchan)) then
               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c,
     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogrid(nx, ny, loc, support)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        nvalue=0.0
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2))+1
                           wty=convFunc(iloc(2))
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              norm=norm+wt
                              nvalue=nvalue+wt*
     $                             grid(loc(1)+ix,loc(2)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=(nvalue*conjg(phasor))
     $                       /norm
                     end if
                  end do
               end if
            end if
         end do
         end if
      end do
      return
      end


      subroutine sectdgrid (values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, grid, nx, ny, npol, nchan, 
     $     support, sampling, convFunc, chanmap, polmap, rbeg, rend, 
     $     loc,off,phasor)

      implicit none
      integer, intent(in) :: nx,ny,npol,nchan,nvispol,nvischan,nrow
      complex, intent(inout) :: values(nvispol, nvischan, nrow)
      complex, intent(in) :: grid(nx, ny, npol, nchan)
      integer, intent(in) :: flag(nvispol, nvischan, nrow)
      integer, intent(in) :: rflag(nrow)
      integer, intent(in) :: support, sampling
      integer, intent(in) ::  chanmap(*), polmap(*)

      complex :: nvalue

      double precision, intent(in) :: convFunc(*)
      real norm

      logical ogrid
      integer, intent(in)  :: loc(2, nvischan, nrow)
      integer, intent(in) :: off(2, nvischan, nrow) 
      complex, intent(in) :: phasor(nvischan, nrow) 
      integer :: iloc(2)
      integer, intent(in) :: rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      real wt, wtx, wty


      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan)) then
C               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c,
C     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogrid(nx, ny, loc(1,ichan, irow) , support)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        nvalue=0.0
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2,ichan, 
     $                                 irow))+1
                           wty=convFunc(iloc(2))
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1,ichan,
     $                             irow))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              norm=norm+wt
                              nvalue=nvalue+wt*
     $                             grid(loc(1, ichan, irow)+ix,
     $                             loc(2, ichan, irow)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=(nvalue*conjg(
     $                       phasor(ichan, irow)))
     $                       /norm
                     end if
                  end do
               end if
            end if
         end do
         end if
      end do
      return
      end

      subroutine sectdgridjy (values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, grid, nx, ny, npol, nchan, 
     $     support, sampling, convFunc, chanmap, polmap, rbeg, rend, 
     $     loc,off,phasor, scalechan)

      implicit none
      integer, intent(in) :: nx,ny,npol,nchan,nvispol,nvischan,nrow
      complex, intent(inout) :: values(nvispol, nvischan, nrow)
      complex, intent(in) :: grid(nx, ny, npol, nchan)
      integer, intent(in) :: flag(nvispol, nvischan, nrow)
      integer, intent(in) :: rflag(nrow)
      integer, intent(in) :: support, sampling
      integer, intent(in) ::  chanmap(*), polmap(*)
      complex :: nvalue

      double precision, intent(in) :: convFunc(*), scalechan(*)
      real norm

      logical ogrid
      integer, intent(in)  :: loc(2, nvischan, nrow)
      integer, intent(in) :: off(2, nvischan, nrow) 
      complex, intent(in) :: phasor(nvischan, nrow) 
      integer :: iloc(2)
      integer, intent(in) :: rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow
      real wt, wtx, wty


      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1
            if((achan.ge.1).and.(achan.le.nchan)) then
C               call sgrid(uvw(1,irow), dphase(irow), freq(ichan), c,
C     $              scale, offset, sampling, pos, loc, off, phasor)
               if (ogrid(nx, ny, loc(1,ichan, irow) , support)) then
                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        nvalue=0.0
                        norm=0.0
                        do iy=-support,support
                           iloc(2)=abs(sampling*iy+off(2,ichan, 
     $                                 irow))+1
                           wty=convFunc(iloc(2))
                           do ix=-support,support
                              iloc(1)=abs(sampling*ix+off(1,ichan,
     $                             irow))+1
                              wtx=convFunc(iloc(1))
                              wt=wtx*wty
                              norm=norm+wt
                              nvalue=nvalue+wt*scalechan(ichan)*
     $                             grid(loc(1, ichan, irow)+ix,
     $                             loc(2, ichan, irow)+iy,apol,achan)
                           end do
                        end do
                        values(ipol,ichan,irow)=(nvalue*conjg(
     $                       phasor(ichan, irow)))
     $                       /norm
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
      subroutine sgrid (uvw, dphase, freq, c, scale, offset, sampling, 
     $     pos, loc, off, phasor)
      implicit none
      integer sampling
      integer loc(2), off(2)
      double precision uvw(3), freq, c, scale(2), offset(2)
      double precision pos(2)
      double precision dphase, phase
      complex phasor
      integer idim
      double precision pi
      data pi/3.14159265358979323846/

      do idim=1,2
         pos(idim)=scale(idim)*uvw(idim)*freq/c+(offset(idim)+1.0)
         loc(idim)=nint(pos(idim))
         off(idim)=nint((loc(idim)-pos(idim))*sampling)
      end do
      phase=-2.0D0*pi*dphase*freq/c
      phasor=cmplx(cos(phase), sin(phase))
      return 
      end
C
C Is this on the grid?
C
      logical function ogrid (nx, ny, loc, support)
      implicit none
      integer nx, ny, loc(2), support
      ogrid=(loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $        (loc(2)-support.ge.1).and.(loc(2)+support.le.ny)
      return
      end

      logical function onmygrid(loc, nx, ny, nx0, ny0, nxsub, nysub, 
     $     support)
      implicit none
      integer nx, ny, nx0, ny0, nxsub, nysub, loc(2), support
      logical ogrid
C      logical onmygrid
C      onmygrid=ogrid(nx, ny, loc, support)
C      if(.not. onmygrid) then
C         return 
C      end if
C      onmygrid=(loc(1)-nx0 .ge. (-support)) .and. ((loc(1)-nx0-nxsub) 
C     $     .le. (support)) .and.((loc(2)-ny0) .ge. (-support)) .and. 
C     $ ((loc(2)-ny0-nysub) .le. (support))  
C----------------------------
      integer loc1sub, loc1plus, loc2sub, loc2plus
      loc1sub=loc(1)-support
      loc1plus=loc(1)+support
      loc2sub=loc(2)-support
      loc2plus=loc(2)+support
C----------------
C      onmygrid=(loc1plus .ge. nx0) .and. (loc1sub 
C     $     .le. (nx0+nxsub)) .and.(loc2plus .ge. ny0) .and. 
C     $     (loc2sub .le. (ny0+nysub))
C--------------
      onmygrid=(loc1plus .ge. nx0) .and. (loc1sub 
     $     .le. (nx0+nxsub)) .and.(loc2plus .ge. ny0) .and. 
     $     (loc2sub .le. (ny0+nysub)) .and. (loc1sub.ge.1)
     $     .and.(loc1plus.le.nx).and.
     $     (loc2sub.ge.1).and.(loc2plus.le.ny)
     
      return 
      end
      subroutine locuvw(uvw, dphase, freq, nvchan, scale, offset,
     $     sampling, loc, off, phasor, irow, doW, Cinv)
      implicit none
      double precision,  intent(in) :: uvw(3, *), dphase(*), freq(*)
      integer, intent(in) :: nvchan, sampling, irow, doW
      double precision, intent(in) :: scale(3), offset(3), Cinv
      integer, intent(inout) :: loc(2+doW, nvchan, *), off(2+doW,
     $     nvchan, *)
      complex, intent(inout) :: phasor(nvchan, *)
      integer :: f, k, row
      double precision :: phase, pos
      double precision :: pi
      data pi/3.14159265358979323846/

      row=irow+1

      do f=1, nvchan
         do k=1,2
            pos=scale(k)*uvw(k,row)*freq(f)*Cinv+(offset(k)+1.0)
            loc(k, f, row)=nint(pos)
            off(k, f, row)=nint((loc(k, f, row)-pos)*sampling)
         end do
         phase=-2.0D0*pi*dphase(row)*freq(f)*Cinv
         phasor(f,row)=cmplx(cos(phase), sin(phase))
         if(doW .eq. 1) then
            pos=sqrt(abs(scale(3)*uvw(3, row)*freq(f)*Cinv))+offset(3)
     $           +1.0
            loc(3,f, row)=nint(pos)
            off(3, f, row)=0
         end if
      end do
      return
      end
