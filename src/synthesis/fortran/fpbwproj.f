*=======================================================================
* -*- FORTRAN -*-
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
*     $Id: fpbwproj.f,v 1.13 2006/07/20 00:24:20 sbhatnag Exp $
*-----------------------------------------------------------------------
C
C Grid a number of visibility records
C
      subroutine gpbwproj (uvw, dphase, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, rownum,
     $     scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $     support, convsize, sampling, wconvsize, convfunc, 
     $     chanmap, polmap,polused,sumwt,
     $     ant1, ant2, nant, scanno, sigma,raoff, decoff,area,
     $     dograd,dopointingcorrection,npa,paindex,cfmap,conjcfmap,
     $     currentCFPA,actualPA,cfRefFreq)


      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow,polused
      integer npa,paindex
      integer convsize, sampling, wconvsize,dograd,dopointingcorrection
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(3),
     $     offset(3),currentCFPA,actualPA,cfRefFreq
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow)
      real weight(nvischan, nrow), phase
      double precision sumwt(npol, nchan)
      integer rownum
      integer support(wconvsize,polused,npa), rsupport
      integer chanmap(nchan), polmap(npol),cfmap(npol),conjcfmap(npol)
      integer dopsf

      complex nvalue

C      complex convfunc(convsize/2-1, convsize/2-1, wconvsize, polused),
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt

      real norm
      complex cnorm,tcnorm
      real wt

      logical opbwproj,reindex
      external nwcppeij

      real pos(3)
      integer loc(3), off(3), iloc(3),iu,iv
      integer rbeg, rend
      integer ix, iy, ipol, ichan
      integer apol, achan, irow, PolnPlane, ConjPlane
      double precision pi,tc,ts,cfscale,ixr,iyr
      data pi/3.14159265358979323846/

      double precision griduvw(2),dPA, cDPA,sDPA
      double precision mysigma, ra1,ra2,dec1,dec2
      double precision sigma,area,lambda
      complex pcwt, pdcwt1, pdcwt2
      integer nant, scanno, ant1(nrow), ant2(nrow)
      integer convOrigin
      real raoff(nant), decoff(nant)


      integer ii,jj,kk,imax,jmax
      real tmp

      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if
      cfscale=1.0
      dPA = -(currentCFPA - actualPA)
c      dPA = 0
      cDPA = cos(dPA)
      sDPA = sin(dPA)

      convOrigin = (convsize-1)/2
c      convOrigin = (convsize+1)/2
c$$$      write(*,*) convOrigin, convsize, (convsize/2),
c$$$     $     convfunc(convOrigin,convOrigin,1,1)
      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
            do ichan=1, nvischan
               achan=chanmap(ichan)+1

               lambda = c/freq(ichan)

               if((achan.ge.1).and.(achan.le.nchan).and.
     $              (weight(ichan,irow).ne.0.0)) then
                  call spbwproj(uvw(1,irow), dphase(irow), 
     $                 freq(ichan), c, scale, offset, sampling, 
     $                 pos, loc, off, phasor)
                  iloc(3)=max(1, min(wconvsize, loc(3)))
                  rsupport=support(iloc(3),1,paindex)
                  if (opbwproj(nx, ny, wconvsize, loc, rsupport)) then
                     PolnPlane=polused+1

                     do ipol=1, nvispol
                        apol=polmap(ipol)+1
                     
                        if((flag(ipol,ichan,irow).ne.1).and.
     $                       (apol.ge.1).and.(apol.le.npol)) then

c$$$                           ConjPlane = conjcfmap(ipol)+1
c$$$                           PolnPlane = cfmap(ipol)+1
C
C The following after feed_x -> -feed_x and PA -> PA + PI/2
                           ConjPlane = cfmap(ipol)+1
                           PolnPlane = conjcfmap(ipol)+1
C
C If we are making a PSF then we don't want to phase rotate but we do want 
C to reproject uvw
C
                           if(dopsf.eq.1) then
                              nvalue=cmplx(weight(ichan,irow))
                           else
                              nvalue=weight(ichan,irow)*
     $                             (values(ipol,ichan,irow)*phasor)
                           end if
C
C norm will be the value we would get for the peak at the phase center. 
C We will want to normalize the final image by this term.
C
                           norm=0.0
                           cnorm=cmplx(1.0,0.0)
                           cnorm=0.0
			   tcnorm=0.0
                           do iy=-rsupport,rsupport
c$$$                              iloc(2)=1+(iy*sampling+off(2))
c$$$     $                             +convOrigin
c$$$                              ii=iloc(2)
                              iloc(2)=(iy*sampling+off(2))
                              iv = iloc(2)
                              do ix=-rsupport,rsupport
c$$$                                 iloc(1)=1+(ix*sampling+off(1))
c$$$     $                                +convOrigin
c$$$                                 jj=iloc(1)
                                 iloc(1)=(ix*sampling+off(1))
                                 
                                 iu = iloc(1)

c$$$                                 ixr = ix*sampling*cfscale
c$$$                                 iyr = iy*sampling*cfscale
c$$$                                 iu = nint(cDPA*ixr + sDPA*iyr)
c$$$                                 iv = nint(-sDPA*ixr + cDPA*iyr)
c$$$                                 iu=iu+off(1)*cfscale
c$$$                                 iv=iv+off(2)*cfscale

                                 ixr = (ix*sampling+off(1))*cfscale
                                 iyr = (iy*sampling+off(2))*cfscale
                                 iu = nint(cDPA*ixr + sDPA*iyr)
                                 iv = nint(-sDPA*ixr + cDPA*iyr)

                                 ts=0.0
                                 tc=1.0
                                 if (reindex(iu,iv,iloc(1),iloc(2),
     $                                ts,tc,
     $                                convOrigin, convSize)) then
                                    if (dopointingcorrection .eq. 1) 
     $                                   then
c$$$                                       griduvw(1) = (loc(1)-offset(1)+
c$$$     $                                      ix-1)
c$$$     $                                      /scale(1)-uvw(1,irow)/lambda
c$$$                                       griduvw(2) = (loc(2)-offset(2)+
c$$$     $                                      iy-1)
c$$$     $                                      /scale(2)-uvw(2,irow)/lambda
                                       iu=iu-off(1)*cfscale
                                       iv=iv-off(2)*cfscale
                                     griduvw(1)=(iu)/(scale(1)*sampling)
                                     griduvw(2)=(iv)/(scale(2)*sampling)
                                       ra1 = raoff(ant1(irow)+1)
                                       ra2 = raoff(ant2(irow)+1)
                                       dec1= decoff(ant1(irow)+1)
                                       dec2= decoff(ant2(irow)+1)
                                       call nwcppeij(griduvw,area,
     $                                      ra1,dec1,ra2,dec2,
     $                                      dograd,pcwt,pdcwt1,pdcwt2,
     $                                      currentCFPA)
                                    else 
                                       pcwt=cmplx(1.0,0.0)
                                    endif
c$$$                                    if(uvw(3,irow).gt.0.0) then
c$$$                                       cwt=conjg(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),
c$$$     $                                      ConjPlane))
c$$$                                       pcwt=conjg(pcwt)
c$$$                                    else
c$$$                                       cwt=(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),
c$$$     $                                      PolnPlane))
c$$$c                                       pcwt=conjg(pcwt)
c$$$                                    end if
                                    
                                    cwt=(convfunc(iloc(1),
     $                                   iloc(2), iloc(3),
     $                                   PolnPlane))
                                    cwt = cwt * pcwt
                                    if (dopsf .eq. 1) then
C                                       cnorm=cnorm+real(cwt)
                                       cnorm=cnorm+cwt
                                    else
                                       cnorm=cnorm+cwt
                                    endif
                                 endif
                              enddo
                           enddo
C                           stop
c$$$                        else
c$$$                           cnorm=cmplx(1.0,0.0)
c$$$                        endif
                        
                        
                           do iy=-rsupport,rsupport
                              iloc(2)=(iy*sampling+off(2))
                              iv = iloc(2)
                              do ix=-rsupport,rsupport
                                 iloc(1)=(ix*sampling+off(1))
                                 iu = iloc(1)

c$$$                                 ixr = ix*sampling*cfscale
c$$$                                 iyr = iy*sampling*cfscale
c$$$                                 iu = nint(cDPA*ixr + sDPA*iyr)
c$$$                                 iv = nint(-sDPA*ixr + cDPA*iyr)
c$$$                                 iu=iu+off(1)*cfscale
c$$$                                 iv=iv+off(2)*cfscale

                                 ixr = (ix*sampling+off(1))*cfscale
                                 iyr = (iy*sampling+off(2))*cfscale
                                 iu = nint(cDPA*ixr + sDPA*iyr)
                                 iv = nint(-sDPA*ixr + cDPA*iyr)

                                 ts=0.0
                                 tc=1.0
                                 if (reindex(iu,iv,iloc(1),iloc(2),
     $                                ts,tc,
     $                                convOrigin,convSize)) then
C
C Compute the pointing offset term
C
                                    if (dopointingcorrection .eq. 1) 
     $                                   then
c$$$                                       iu=iu-off(1)*cfscale
c$$$                                       iv=iv-off(2)*cfscale
C
C     Use the rotated CF co-oridinates to compute the phase grad.  This
C     effectively rotates the pointing vector with the PA
C
                                       iu = iloc(1)-convOrigin
                                       iv = iloc(2)-convOrigin
                                       griduvw(1)=(iu)/
     $                                      (scale(1)*sampling)
                                       griduvw(2)=(iv)/
     $                                      (scale(2)*sampling)
                                       ra1 = raoff(ant1(irow)+1)
                                       ra2 = raoff(ant2(irow)+1)
                                       dec1= decoff(ant1(irow)+1)
                                       dec2= decoff(ant2(irow)+1)
                                       call nwcppeij(griduvw,area,
     $                                      ra1,dec1,ra2,dec2,
     $                                      dograd,pcwt,pdcwt1,pdcwt2,
     $                                      currentCFPA)
                                    else 
                                       pcwt=cmplx(1.0,0.0)
                                    endif

c$$$                                    if(uvw(3,irow).gt.0.0) then
c$$$                                       cwt=conjg(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),
c$$$     $                                      ConjPlane))
c$$$                                       pcwt=conjg(pcwt)
c$$$                                    else
c$$$                                       cwt=(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),
c$$$     $                                      PolnPlane))
c$$$                                       pcwt=(pcwt)
c$$$                                    end if
                                    cwt=(convfunc(iloc(1),
     $                                   iloc(2), iloc(3),
     $                                   PolnPlane))
                                    cwt = cwt * pcwt
c$$$                                    if (dopsf .eq. 1) then
c$$$                                       cwt = real(cwt)
c$$$                                    endif
                                    grid(loc(1)+ix,
     $                                   loc(2)+iy,apol,achan)=
     $                                   grid(loc(1)+ix,
     $                                   loc(2)+iy,apol,achan)+
     $                                   nvalue*cwt
     $                                   /cnorm
c                                    norm=norm+real(cwt/cnorm)
c$$$                                    write(*,*) 'G ',abs(cwt),
c$$$     $                                   ix,iy,iloc(1),iloc(2)
                                    tcnorm=tcnorm+(cwt/cnorm)
c                              cnorm=cnorm+cwt
                                 endif
                              end do
c$$$                              write(*,*)
                           end do
c$$$                           stop
                           norm=real(tcnorm)
                           sumwt(apol,achan)=sumwt(apol,achan)+
     $                          weight(ichan,irow)*norm
                        end if
                     end do
c               else
                  end if
               end if
            end do
         end if
      end do
c      write(*,*)'Gridded ',irow,' visibilities'
c$$$      close(11)
c$$$      close(12)
      return
      end
C
C Degrid a number of visibility records
C
      subroutine dpbwproj (uvw, dphase, values, nvispol, nvischan,
     $     flag, rflag,
     $     nrow, rownum, scale, offset, grid, nx, ny, npol, nchan, freq,
     $     c, support, convsize, sampling, wconvsize, convfunc,
     $     chanmap, polmap,polused, ant1, ant2, nant, 
     $     scanno, sigma, raoff, decoff,area,dograd,
     $     dopointingcorrection,npa,paindex,cfmap,conjcfmap,
     $     currentCFPA,actualPA,cfRefFreq)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow,polused
      integer npa,paindex
      integer convsize, wconvsize, sampling
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(3),
     $     offset(3),currentCFPA,actualPA, dPA, sDPA, cDPA,cfRefFreq
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow),cfmap(npol),conjcfmap(npol)
      integer rownum
      integer support(wconvsize,polused,npa), rsupport
      integer chanmap(*), polmap(*)

      integer nant, scanno, ant1(nrow), ant2(nrow),dograd,
     $     dopointingcorrection
      real raoff(nant), decoff(nant)
      double precision sigma,area,lambda

      complex nvalue

C      complex convfunc(convsize/2-1, convsize/2-1, wconvsize, polused),
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt, pcwt,pdcwt1,pdcwt2
      double precision griduvw(2)
      double precision mysigma, ra1,ra2,dec1,dec2

      complex norm(4)

      logical opbwproj,reindex
      external nwcppEij

      real pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, PolnPlane, ConjPlane
      integer convOrigin
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi,ts,tc,cfscale,ixr,iyr
      data pi/3.14159265358979323846/
      integer ii,jj,iu,iv

      complex tmp

      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if
C

      cfscale=1.0
      dPA = -(currentCFPA - actualPA)
c      dPA=0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
      convOrigin = (convsize+1)/2
      convOrigin = (convsize-1)/2
      convOrigin = (convsize)/2

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1

            lambda = c/freq(ichan)

            if((achan.ge.1).and.(achan.le.nchan)) then
               call spbwproj(uvw(1,irow), dphase(irow), freq(ichan), c,
     $              scale, offset, sampling, pos, loc, off, phasor)
               iloc(3)=max(1, min(wconvsize, loc(3)))
               rsupport=support(iloc(3),1,paindex)
c$$$               off(1)=0
c$$$               off(2)=0
               if (opbwproj(nx, ny, wconvsize, loc, rsupport)) then
                  PolnPlane=0
                  if (ant1(irow).eq. 1 .and.
     $                 ant2(irow).eq. 2) then
c$$$                     write(*,*) ichan,off(1), off(2),loc(1),loc(2),
c$$$     $                    sampling,scale(1),uvw(1,irow),uvw(2,irow)
                  endif

                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then

                        ConjPlane = cfmap(ipol)+1
                        PolnPlane = conjcfmap(ipol)+1

                        nvalue=0.0
                        norm(apol)=cmplx(0.0,0.0)
                        pcwt=cmplx(1.0,0.0)
			ii=0
                        do iy=-rsupport,rsupport
                           iloc(2)=1+(iy*sampling+off(2))+convOrigin
                           iv = (iy*sampling+off(2))
                           do ix=-rsupport,rsupport
                              iloc(1)=1+(ix*sampling+off(1))+convOrigin
                              iu = (ix*sampling+off(1))

c$$$                              ixr = ix*sampling*cfscale
c$$$                              iyr = iy*sampling*cfscale
c$$$                              iu = nint(cDPA*ixr + sDPA*iyr)
c$$$                              iv = nint(-sDPA*ixr + cDPA*iyr)
c$$$                              iu=iu+off(1)*cfscale
c$$$                              iv=iv+off(2)*cfscale

                              ixr = (ix*sampling+off(1))*cfscale
                              iyr = (iy*sampling+off(2))*cfscale
c$$$                              iu = nint(cDPA*ixr + sDPA*iyr)
c$$$                              iv = nint(-sDPA*ixr + cDPA*iyr)
                              iu=ixr
                              iv=iyr
                              ts=0.0
                              tc=1.0
                              ts=sDPA
                              tc=cDPA
                              if(reindex(iu,iv,iloc(1),iloc(2),
     $                             ts,tc, convOrigin, convSize)) 
     $                             then

                                 if (dopointingcorrection .eq. 1) then
c$$$                                    iu = iu - off(1)*cfscale
c$$$                                    iv = iv - off(2)*cfscale
C
C     Use the rotated CF co-oridinates to compute the phase grad.  This
C     effectively rotates the pointing vector with the PA
C
                                    iu = iloc(1)-convOrigin
                                    iv = iloc(2)-convOrigin
                                    griduvw(1)=(iu)/(scale(1)*sampling)
                                    griduvw(2)=(iv)/(scale(2)*sampling)
                                    ra1 = raoff(ant1(irow)+1)
                                    ra2 = raoff(ant2(irow)+1)
                                    dec1= decoff(ant1(irow)+1)
                                    dec2= decoff(ant2(irow)+1)
                                    call nwcppEij(griduvw,area,
     $                                   ra1,dec1,ra2,dec2,
     $                                   dograd,pcwt,pdcwt1,pdcwt2,
     $                                   currentCFPA)
                                 endif

c$$$                                 if(uvw(3,irow).gt.0.0) then
c$$$                                    cwt=conjg(convfunc(iloc(1),
c$$$     $                                   iloc(2), iloc(3),ConjPlane))
c$$$                                    pcwt = conjg(pcwt)


c$$$                                 else
c$$$                                    cwt=(convfunc(iloc(1),
c$$$     $                                   iloc(2), iloc(3),PolnPlane))
c$$$c                                    pcwt = conjg(pcwt)
c$$$                                 endif
                                 cwt=(convfunc(iloc(1),
     $                                iloc(2), iloc(3),PolnPlane))

                                 nvalue=nvalue+(cwt)*(pcwt)*
     $                              grid(loc(1)+ix,loc(2)+iy,apol,achan)
                                 norm(apol)=norm(apol)+(pcwt*cwt)
c 				ii=ii+1
c                                 if (irow .eq. 2) then
c$$$                                 tmp=grid(loc(1)+ix,loc(2)+iy,
c$$$     $                                apol,achan)
c$$$                                 if (ant1(irow) .eq. 1 .and.
c$$$     $                                ant2(irow) .eq. 2) then
c$$$                                    write(*,*)abs(nvalue),
c$$$     $                                   atan2(imag(nvalue),
c$$$     $                                   real(nvalue)),
c$$$     $                                   abs(tmp),
c$$$     $                                   atan2(imag(tmp),real(tmp)),
c$$$     $                                   ix,iy,
c$$$     $                                   iloc(1), iloc(2),
c$$$     $                                   abs(cwt),
c$$$     $                                   norm(apol),
c$$$     $                                   ant1(irow),ant2(irow),
c$$$     $                                   irow,achan,apol,
c$$$     $                                   off(1), off(2)
c$$$                                 endif
                              endif
                           end do
c$$$                           write(*,*)
                        end do
c                        norm(apol) = norm(apol)/abs(norm(apol))
c$$$                        write (*,*) nvalue, norm(apol)
                        values(ipol,ichan,irow)=
     $                       nvalue*conjg(phasor)
     $                       /norm(apol)
c$$$			if ((ant1(irow).eq.1).and.
c$$$     $                       (ant2(irow).eq.2)) then
c$$$                           write(*,*) irow,apol,ipol,
c$$$     $                          abs(values(ipol,ichan,irow)),
c$$$     $                          atan2(imag(values(ipol,ichan,irow)),
c$$$     $                          real(values(ipol,ichan,irow)))*57.2956,
c$$$     $                          real(norm(apol)),imag(norm(apol)),ii
c$$$                        endif
                     end if
                  end do
c$$$                  write(*,*) "====================================="
c$$$                  stop
               end if
            end if
         end do
         end if
      end do
      return
      end
C
C Degrid a number of visibility records along with the grad. computations
C
      subroutine dpbwgrad (uvw, dphase, values, nvispol, nvischan,
     $     gazvalues, gelvalues, doconj,
     $     flag, rflag, nrow, rownum, scale, offset, grid, 
     $     nx, ny, npol, nchan, freq, c, support, convsize, sampling, 
     $     wconvsize, convfunc, chanmap, polmap,polused,ant1,ant2,nant, 
     $     scanno, sigma, raoff, decoff,area,dograd,
     $     dopointingcorrection,npa,paindex,cfmap,conjcfmap,
     $     currentCFPA,actualPA,cfRefFreq)

      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow,polused
      integer npa,paindex,doconj
      integer convsize, wconvsize, sampling
      complex values(nvispol, nvischan, nrow)
      complex gazvalues(nvispol, nvischan, nrow)
      complex gelvalues(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan)
      double precision uvw(3, nrow), freq(nvischan), c, scale(3),
     $     offset(3),currentCFPA,actualPA, dPA, sDPA, cDPA,cfRefFreq
      double precision dphase(nrow), uvdist
      complex phasor
      integer flag(nvispol, nvischan, nrow)
      integer rflag(nrow),cfmap(npol),conjcfmap(npol)
      integer rownum
      integer support(wconvsize,polused,npa), rsupport
      integer chanmap(*), polmap(*)

      integer nant, scanno, ant1(nrow), ant2(nrow),dograd,
     $     dopointingcorrection
      real raoff(2,1,nant), decoff(2,1,nant)
      double precision sigma,area,lambda

      complex nvalue,ngazvalue,ngelvalue

C      complex convfunc(convsize/2-1, convsize/2-1, wconvsize, polused),
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt, pcwt,pdcwt1,pdcwt2
      double precision griduvw(2)
      double precision mysigma, ra1,ra2,dec1,dec2

      complex norm(4),gradaznorm(4),gradelnorm(4)

      logical opbwproj,reindex
      external nwcppEij

      real pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, PolnPlane, ConjPlane
      integer convOrigin
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi,ts,tc,cfscale,ixr,iyr
      data pi/3.14159265358979323846/
      integer ii,iu,iv,tt

      complex tmp
      tt=0
      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if
C

      cfscale=1.0
      dPA = -(currentCFPA - actualPA)
c      dPA = 0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
c      convOrigin = (convsize+1)/2
      convOrigin = (convsize-1)/2
      convOrigin = (convsize)/2

      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
         do ichan=1, nvischan
            achan=chanmap(ichan)+1

            lambda = c/freq(ichan)

            if((achan.ge.1).and.(achan.le.nchan)) then
               call spbwproj(uvw(1,irow), dphase(irow), freq(ichan), c,
     $              scale, offset, sampling, pos, loc, off, phasor)
               iloc(3)=max(1, min(wconvsize, loc(3)))
               rsupport=support(iloc(3),1,paindex)
               if (opbwproj(nx, ny, wconvsize, loc, rsupport)) then

                  do ipol=1, nvispol
                     apol=polmap(ipol)+1
                     if((flag(ipol,ichan,irow).ne.1).and.
     $                    (apol.ge.1).and.(apol.le.npol)) then
                        ConjPlane = conjcfmap(ipol)+1
                        PolnPlane = cfmap(ipol)+1
                        ConjPlane = cfmap(ipol)+1
                        PolnPlane = conjcfmap(ipol)+1
                        nvalue=0.0
                        ngazvalue = 0.0
                        ngelvalue = 0.0

                        norm(apol)=cmplx(0.0,0.0)
                        gradaznorm(apol)=cmplx(0.0,0.0)
                        gradelnorm(apol)=cmplx(0.0,0.0)
                        pcwt=cmplx(1.0,0.0)

                        do iy=-rsupport,rsupport
                           iloc(2)=1+(iy*sampling+off(2))+convOrigin
                           iv=(iy*sampling+off(2))
                           do ix=-rsupport,rsupport
                              iloc(1)=1+(ix*sampling+off(1))+convOrigin
                              iu=(ix*sampling+off(1))
                              
                              ixr = (ix*sampling+off(1))*cfscale
                              iyr = (iy*sampling+off(2))*cfscale
                              ts=sDPA
                              tc=cDPA
                              iu=ixr
                              iv=iyr
                              if(reindex(iu,iv,iloc(1),iloc(2),
     $                             ts,tc, convOrigin, convSize)) 
c$$$                              if(reindex(ixr,iyr,iloc(1),iloc(2),
c$$$     $                             ts,tc, convOrigin, convSize)) 
     $                             then
                                 if (dopointingcorrection .eq. 1) then
c$$$                                    iu=iu-off(1)*cfscale
c$$$                                    iv=iv-off(2)*cfscale
C
C     Use the rotated CF co-oridinates to compute the phase grad.  This
C     effectively rotates the pointing vector with the PA
C
                                    iu=iloc(1)-convOrigin
                                    iv=iloc(2)-convOrigin
                                    griduvw(1)=(iu)/(scale(1)*sampling)
                                    griduvw(2)=(iv)/(scale(2)*sampling)
                                    ii = PolnPlane
c                                    ii=apol
                                    ra1 = raoff(ii,1,ant1(irow)+1)
                                    ra2 = raoff(ii,1,ant2(irow)+1)
                                    dec1= decoff(ii,1,ant1(irow)+1)
                                    dec2= decoff(ii,1,ant2(irow)+1)
                                    call nwcppEij(griduvw,area,
     $                                   ra1,dec1,ra2,dec2,
     $                                   dograd,pcwt,pdcwt1,pdcwt2,
     $                                   currentCFPA)
                                 endif
c$$$                                 if(uvw(3,irow).gt.0.0) then
c$$$                                    cwt=conjg(convfunc(iloc(1),
c$$$     $                                   iloc(2), iloc(3),ConjPlane))
c$$$				    pcwt = conjg(pcwt)
c$$$                                    pdcwt1 = conjg(pdcwt1)
c$$$                                    pdcwt2 = conjg(pdcwt2)
c$$$                                 else
c$$$                                    cwt=(convfunc(iloc(1),
c$$$     $                                   iloc(2), iloc(3),PolnPlane))
c$$$                                 endif
                                 cwt=(convfunc(iloc(1),
     $                                iloc(2), iloc(3),PolnPlane))
                                 
                                 cwt = cwt*pcwt
                                 nvalue=nvalue+(cwt)*
     $                                grid(loc(1)+ix,loc(2)+iy,apol,
     $                                achan)
                                 
                                 if ((doconj .eq. 1) .and. 
     $                                (dograd .eq. 1)) then
                                    pdcwt1 = conjg(pdcwt1)
                                    pdcwt2 = conjg(pdcwt2)
                                 endif
                                 if (dograd .eq. 1) then
                                    pdcwt1 = pdcwt1*cwt
                                    pdcwt2 = pdcwt2*cwt
                                    ngazvalue=ngazvalue+(pdcwt1)*
     $                                   (grid(loc(1)+ix,loc(2)+iy,
     $                                   apol,achan))
                                    ngelvalue=ngelvalue+(pdcwt2)*
     $                                   (grid(loc(1)+ix,loc(2)+iy,
     $                                   apol,achan))
                                 endif
                                 
                                 norm(apol)=norm(apol)+(cwt)
                                 gradaznorm(apol)=gradaznorm(apol)+
     $                                pdcwt1
                                 gradelnorm(apol)=gradelnorm(apol)+
     $                                pdcwt1
c$$$                              if (apol .eq. 1) then
c$$$                              write(*,*)ix,iy,
c$$$     $                             abs(cwt),atan2(imag(cwt),real(cwt)),
c$$$     $                             abs(pdcwt1),
c$$$     $                             atan2(imag(pdcwt1),real(pdcwt1)),
c$$$     $                             abs(pdcwt2),
c$$$     $                             atan2(imag(pdcwt2),real(pdcwt2))
c$$$                              endif
                              endif
                           end do
c$$$                              write(*,*)
                        end do
c                        norm(apol) = norm(apol)/abs(norm(apol))
                        values(ipol,ichan,irow)=
     $                       nvalue*conjg(phasor)
     $                       /norm(apol)
c$$$                        if (ant1(irow) .eq. 3 .and.
c$$$     $                       ant2(irow) .eq. 7) then
c$$$                           write(*,*)irow,ant1(irow),
c$$$     $                          ant2(irow),ipol,ichan,
c$$$     $                          values(ipol,ichan,irow)
c$$$                           stop
c$$$                        endif
                        if (dograd .eq. 1) then
                           gazvalues(ipol,ichan,irow)=ngazvalue*
     $                          conjg(phasor)/norm(apol)
C     $                          conjg(phasor)/gradaznorm(apol)
                           gelvalues(ipol,ichan,irow)=ngelvalue*
     $                          conjg(phasor)/norm(apol)
C     $                          conjg(phasor)/gradelnorm(apol)
                        endif
                     end if
                  end do
                  tt=0
c$$$                  stop
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
      subroutine spbwproj (uvw, dphase, freq, c, scale, offset, 
     $     sampling, pos, loc, off, phasor)
      implicit none
      integer loc(3), off(3), sampling
      double precision uvw(3), freq, c, scale(3), offset(3)
      real pos(3)
      double precision dphase, phase
      complex phasor
      integer idim
      double precision pi
      data pi/3.14159265358979323846/

C      pos(3)=(scale(3)*uvw(3)*freq/c)*(scale(3)*uvw(3)*freq/c)
C     $     +offset(3)+1.0
C      pos(3)=(scale(3)*uvw(3)*freq/c)+offset(3)+1.0
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
      logical function opbwproj (nx, ny, nw, loc, support)
      implicit none
      integer nx, ny, nw, loc(3), support
      opbwproj=(support.gt.0).and.
     $     (loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $     (loc(2)-support.ge.1).and.(loc(2)+support.le.ny).and.
     $     (loc(3).ge.1).and.(loc(3).le.nw)
      return
      end

      logical function reindex(inx,iny,outx,outy,sinDPA,cosDPA,
     $     Origin, Size)
      integer inx,iny,outx,outy, Origin, Size
      double precision sinDPA, cosDPA
      integer ix,iy
      
      ix = nint(cosDPA*inx + sinDPA*iny)+1
      iy = nint(-sinDPA*inx + cosDPA*iny)+1
c$$$      ix = nint(cosDPA*inx + sinDPA*iny)
c$$$      iy = nint(-sinDPA*inx + cosDPA*iny)
      outx = ix+Origin
      outy = iy+Origin

c$$$      outx=ix
c$$$      outy=iy
      reindex=(outx .ge. 1 .and. 
     $     outx .le. Size .and.
     $     outy .ge. 1 .and.
     $     outy .le. Size)
      
      return
      end
