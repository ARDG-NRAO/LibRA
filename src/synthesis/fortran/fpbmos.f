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
*     $Id: fpbwproj.f,v 1.13 2006/07/20 00:24:20 sbhatnag Exp $
*-----------------------------------------------------------------------

      subroutine gfeij(uvw,area,ra1,dec1,ra2,dec2,dograd,pcwt,pdcwt1,
     $     pdcwt2,pa)
      implicit none
      double precision uvw(2), area, pa,pi,ra1,ra2,dec1,dec2
      integer dograd
      complex pcwt,pdcwt1,pdcwt2
      complex kij
      data pi/3.14159265358979323846/

      kij = cmplx(0,pi*(uvw(1)*(ra1+ra2) + uvw(2)*(dec1+dec2)))
      pcwt=exp(kij)/area
      pdcwt1 = cmplx(1,0)
      pdcwt2 = cmplx(1,0)
      
      if (dograd .gt. 0) then
         pdcwt1 = (cmplx(0,pi*uvw(1)))
         pdcwt2 = (cmplx(0,pi*uvw(2)))
      endif
      return
      end

      complex function accumulate(doavgpb,area, grid,gridorigin,func,
     $     nvalue, cnorm, rsupport,sampling, off,
     $     cfscale,
     $     scale,lambda, sDPA,cDPA,currentCFPA,convOrigin,convSize,
     $     wconvsize, polused, dopointingcorrection, dograd,nant,
     $     raoff,decoff, ConjPlane, PolnPlane, nx,ny,npol,nchan,
     $     apol,achan,ant1,ant2,uvw,nrow,irow)
      

      integer nrow, irow,doavgpb
      integer ant1(nrow),ant2(nrow)
      integer gridorigin(3)
      integer rsupport,sampling, off(3), convOrigin, convSize,wconvsize,
     $     polused
      integer dopointingcorrection, dograd,nant
      integer ix,iy,iloc(3),loc(3),origin(3), 
     $     ConjPlane, PolnPlane,apol,achan
      double precision offset(3),cfscale,lambda,sDPA,cDPA,currentCFPA,
     $     area
      real raoff(2,1,nant), decoff(2,1,nant)
      double precision griduvw(2), scale(3),uvw(3,nrow),
     $     ra1,ra2,dec1,dec2
      complex cwt,pcwt,pdcwt1,pdcwt2,tcnorm
      complex func(convsize, convsize, wconvsize, polused)
      complex grid(nx, ny, npol, nchan),nvalue,cnorm
      integer ixr,iyr
      logical opbmos,mreindex
      external gcppeij
      double precision ts,tc,tmpnorm
      double precision scaledSampling
      integer scaledSupport

      scaledSampling=int(sampling*cfscale)
      scaledSupport=int(rsupport/cfscale+0.5)

      origin(1)=gridorigin(1)
      origin(2)=gridorigin(2)
      origin(3)=gridorigin(3)
      tcnorm=0
      iloc(3)=1

      do iy=-scaledSupport,scaledSupport
c         iloc(2)=(iy*sampling+off(2))* cfscale
         iloc(2)=iy * scaledSampling + off(2)
         iv = iloc(2)
         do ix=-scaledSupport,scaledSupport
c           iloc(1)=(ix*sampling+off(1))* cfscale
            iloc(1)=ix * scaledSampling + off(1)
            iu = iloc(1)
            ts=sDPA
            tc=cDPA
            if (mreindex(iu,iv,iloc(1),iloc(2),
     $           ts,tc,convOrigin,convSize)) then
               if (dopointingcorrection .eq. 1) then
                  griduvw(1)=(iloc(1)-convOrigin)/(scale(1)*sampling)
                  griduvw(2)=(iloc(2)-convOrigin)/(scale(2)*sampling)
                  ra1 = raoff(apol,1,ant1(irow)+1)
                  ra2 = raoff(apol,1,ant2(irow)+1)
                  dec1= decoff(apol,1,ant1(irow)+1)
                  dec2= decoff(apol,1,ant2(irow)+1)
                  call gfeij(griduvw,area,ra1,dec1,ra2,dec2,
     $                 dograd,pcwt,pdcwt1,pdcwt2,currentCFPA)
               else 
                  pcwt=cmplx(1.0,0.0)
               endif
               if(uvw(3,irow).gt.0.0) then
                  cwt=conjg(func(iloc(1),iloc(2), iloc(3),
     $                 ConjPlane))
               else
                  cwt=(func(iloc(1),iloc(2), iloc(3),
     $                 PolnPlane))
               end if
               cwt=(func(iloc(1),iloc(2), iloc(3), PolnPlane))
               cwt = (cwt/cnorm)

c               tcnorm=tcnorm+cwt*nvalue
               grid(origin(1)+ix,origin(2)+iy,apol,achan)=
     $              grid(origin(1)+ix,origin(2)+iy,apol,achan)+
     $              nvalue*cwt*(pcwt)
c$$$               if ((doavgpb .ne. 0)) then
c$$$                  write(*,*) cnorm,tcnorm,nvalue,cwt
c$$$               endif
            endif
         end do
c$$$         write(*,*)
      end do
c$$$      if ((doavgpb .eq. 1)) then
c$$$         write(*,*) tcnorm,nvalue
c$$$      endif
      accumulate = cnorm
c$$$      stop
      end
C     
C     Compute area under the function func
C     
      complex function getarea(doavgpb,func,area,rsupport, sampling, 
     $     off, 
     $     cfscale,scale,lambda,sDPA, cDPA, currentCFPA, convOrigin, 
     $     convSize, wconvsize, polused,dopointingcorrection, dograd, 
     $     nant, raoff,decoff, ConjPlane, PolnPlane,ant1,ant2,uvw,
     $     nrow,irow)
      
      integer nrow,irow,doavgpb
      integer ant1(nrow),ant2(nrow)
      integer rsupport,sampling, off(3), convOrigin, convSize,wconvsize,
     $     polused
      integer dopointingcorrection, dograd,nant
      integer ix,iy,iloc(3),loc(3), ConjPlane, PolnPlane
      real raoff(2,1,nant), decoff(2,1,nant)
      double precision ra1,ra2,dec1,dec2
      double precision offset(3),cfscale,lambda,sDPA,cDPA,currentCFPA,
     $     area
      double precision griduvw(2), scale(3),uvw(3,nrow)
      complex cnorm,cwt,pcwt,pdcwt1,pdcwt2
      complex func(convsize, convsize, wconvsize, polused)
      integer ixr, iyr
      logical opbmos,mreindex
      external gcppeij
      integer OMP_GET_THREAD_NUM,TID
      integer scaledSupport
      double precision ts, tc, scaledSampling
      
      cnorm=0
      iloc(3)=1
      scaledSampling = int(sampling*cfscale);
      scaledSupport = int(rsupport/cfscale + 0.5)

      do iy=-scaledSupport,scaledSupport
c         iloc(2)=(iy*sampling+off(2))*cfscale
         iloc(2)=iy * scaledSampling + off(2)
         iv = iloc(2)
         do ix=-scaledSupport,scaledSupport
c            iloc(1)=(ix*sampling+off(1))*cfscale
            iloc(1)=ix * scaledSampling + off(1)
            iu=iloc(1)
            ts = sDPA
            tc = cDPA
            if (mreindex(iu,iv,iloc(1),iloc(2), ts,tc,
     $           convOrigin, convSize)) then
               if(uvw(3,irow).gt.0.0) then
                  cwt=conjg(func(iloc(1),iloc(2), iloc(3),
     $                 ConjPlane))
               else
                  cwt=(func(iloc(1),iloc(2),iloc(3),PolnPlane))
               end if
               cwt=(func(iloc(1),iloc(2),iloc(3),PolnPlane))
               cnorm=cnorm+cwt
            endif
         enddo
      enddo
c !$OMP ENDDO
c !$OMP END PARALLEL      
      getarea = cnorm
      end
C     
C     Grid a number of visibility records
C     
      subroutine gpbmos (uvw, dphase, values, nvispol, nvischan,
     $     dopsf, flag, rflag, weight, nrow, rownum,
     $     scale, offset, grid, nx, ny, npol, nchan, freq, c,
     $     support, convsize, convwtsize, sampling, wconvsize, convfunc, 
     $     chanmap, polmap,polused,sumwt,
     $     ant1, ant2, nant, scanno, sigma,raoff, decoff,area,
     $     dograd,dopointingcorrection,npa,paindex,cfmap,conjcfmap,
     $     currentCFPA,actualPA,doavgpb,avgpb,cfRefFreq,convWts,
     $     wtsupport)
      
      
      implicit none
      integer nx, ny, npol, nchan, nvispol, nvischan, nrow,polused
      integer npa,paindex
      integer convsize, sampling, wconvsize,dograd,dopointingcorrection
      complex values(nvispol, nvischan, nrow)
      complex grid(nx, ny, npol, nchan),avgpb(nx,ny,npol,nchan)
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
      integer dopsf, doavgpb,nodoavgpb
      integer wtsupport, convWtOrigin, convwtsize
      complex nvalue
      
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt,cwt2,convWts(convwtsize, convwtsize, wconvsize, polused)
      
      real norm
      complex cnorm,tcnorm,cnorm2
      real wt
      
      logical opbmos,mreindex,cfOK, cfwtOK
      external gcppeij
      complex getarea,accumulate
      
      real pos(3)
      integer loc(3), pboff(3), off(3), iloc(3),iu,iv,lloc(3)
      double precision pboffset(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, tx,ty
      integer apol, achan, irow, PolnPlane, ConjPlane
      double precision pi
      data pi/3.14159265358979323846/
      
      double precision griduvw(2),dPA, cDPA,sDPA
      double precision mysigma, ra1,ra2,dec1,dec2
      double precision sigma,area,lambda,cfscale
      complex pcwt, pdcwt1, pdcwt2,tt
      integer nant, scanno, ant1(nrow), ant2(nrow)
      integer convOrigin
      real raoff(nant), decoff(nant)
      integer accumPB, chanPolPB
      complex tmpvalue
      integer OMP_GET_THREAD_NUM,TID
      logical chansDone(nvischan), polsDone(npol)

      chanPolPB=0
      irow=rownum

      if(irow.ge.0) then
         rbeg=irow+1
         rend=irow+1
      else 
         rbeg=1
         rend=nrow
      end if

      dPA = -(currentCFPA - actualPA)
c      dPA = 0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
      
      do ichan=1,nvischan
         chansDone(ichan) = .false.
      enddo
      do ichan=1,npol
         polsDone(ichan) = .false.
      enddo

      convOrigin = (convsize-1)/2
      convWtOrigin = (convwtsize-1)/2
      accumPB=1
      tcnorm=0.0
      do irow=rbeg, rend
         if(rflag(irow).eq.0) then 
            do ichan=1, nvischan
               achan=chanmap(ichan)+1
               
               lambda = c/freq(ichan)
               cfscale = cfRefFreq/freq(ichan)
c$$$               uvw(1,irow)=0
c$$$               uvw(2,irow)=0
c$$$               uvw(3,irow)=0
               if((achan.ge.1).and.(achan.le.nchan).and.
     $              (weight(ichan,irow).ne.0.0)) then
                  call spbmos(uvw(1,irow), dphase(irow), 
     $                 freq(ichan), c, scale, offset, sampling, 
     $                 pos, loc, off, phasor)
                  iloc(3)=max(1, min(wconvsize, loc(3)))
                  rsupport=support(iloc(3),1,paindex)

c                  wtsupport=rsupport

                  cfOK=opbmos(nx, ny, wconvsize, loc, rsupport)
                  cfwtOK=opbmos(nx, ny, wconvsize, loc, wtsupport)
                  if (cfOK .or. cfwtOK) then
                     PolnPlane=polused+1
                     do ipol=1, nvispol
                        apol=polmap(ipol)+1

                        if((flag(ipol,ichan,irow).ne.1).and.
     $                       (apol.ge.1).and.(apol.le.npol)) then
                           ConjPlane = cfmap(ipol)+1
                           PolnPlane = conjcfmap(ipol)+1

                           if(dopsf.eq.1) then
                              nvalue=cmplx(weight(ichan,irow))
                           else
                              nvalue=weight(ichan,irow)*
     $                             (values(ipol,ichan,irow)*phasor)
                           end if
                           
                           norm=0.0
                           cnorm=0.0
                           tcnorm=0.0
			   cnorm2=0.0
c                           accumPB=1
                           if ((doavgpb .gt. 0) .and. (cfwtOK) .and.
     $                          (accumPB .eq. 1)) then
C
C This is the center of the WT image is.  Weight functions are
C accumulated at the origin of the UV-grid.
C
                              lloc(1)=(nx)/2+1
                              lloc(2)=(ny)/2+1
                              lloc(3)=0
                              pboff(1)=0
                              pbOff(2)=0
                              pboff(3)=off(3)
                              convWtOrigin=convwtsize/2+1
c                              tmpvalue=cmplx(1,0)
                              tmpvalue=cmplx(weight(ichan,irow))
                              cnorm2 = getarea(doavgpb,convWts,area,
c                              cnorm2 = getarea(doavgpb,convfunc,area,
     $                             wtsupport, sampling,pboff, 
     $                             cfscale,scale,lambda, sDPA, cDPA,
     $                             currentCFPA, convWtOrigin, 
     $                             convwtsize,
     $                             wconvsize, polused,
     $                             dopointingcorrection,dograd, 
     $                             nant,raoff,decoff,ConjPlane,
     $                             PolnPlane,ant1,ant2,uvw,nrow,irow)
                              cnorm2=accumulate(doavgpb,area,avgpb,lloc,
     $                             convWts,tmpvalue,cnorm2,wtsupport,
c     $                             convfunc,tmpvalue,cnorm2,wtsupport,
     $                             sampling,pboff,
     $                             cfscale,scale,lambda,sDPA,cDPA, 
     $                             currentCFPA,convWtOrigin,convwtsize,
     $                             wconvsize,polused,
     $                             dopointingcorrection,dograd,nant,
     $                             raoff,decoff,ConjPlane,PolnPlane,
     $                             nx,ny,npol,nchan,apol,achan, 
     $                             ant1,ant2,uvw,nrow,irow)
                              chansDone(ichan)=.true.
                              polsDone(PolnPlane) = .true.
                              tcnorm=tcnorm+weight(ichan,irow)
c$$$                              write(*,*)'PB: ',irow,tcnorm,ichan,apol
c$$$                              if (irow .eq. 1) then
c$$$                                 write(*,*) 'PB:',ichan,PolnPlane,lloc,
c$$$     $                                pboff,cnorm2
c$$$                              endif
                          endif
                           if (cfOK) then
                              convOrigin = convSize/2+1
c$$$                              nvalue=cmplx(1,0)
c$$$                              loc(1)=(nx)/2+1
c$$$                              loc(2)=(ny)/2+1
c$$$                              loc(3)=0
c$$$                              off(1)=0
c$$$                              off(2)=0
                              nodoavgpb=0
                              cnorm = getarea(nodoavgpb,convfunc,area,
     $                             rsupport, sampling, 
     $                             off, cfscale,scale,lambda, 
     $                             sDPA, cDPA,currentCFPA, 
     $                             convOrigin, convSize,
     $                             wconvsize, polused,
     $                             dopointingcorrection,
     $                             dograd, nant,raoff,decoff,
     $                             ConjPlane,PolnPlane,ant1,ant2,
     $                             uvw,nrow,irow)
                              cnorm=accumulate(nodoavgpb,area,grid,loc,
     $                             convfunc,nvalue,
     $                             cnorm,rsupport,
     $                             sampling,off,cfscale,
     $                             scale,lambda,
     $                             sDPA,cDPA,currentCFPA,convOrigin,
     $                             convSize,wconvsize,polused,
     $                             dopointingcorrection,dograd,nant,
     $                             raoff,decoff,ConjPlane,PolnPlane,
     $                             nx,ny,npol,nchan,apol,achan,
     $                             ant1,ant2,uvw,nrow,irow)
c$$$                              write(*,*)'V: ',irow,weight(ichan,irow),
c$$$     $                             ichan,apol,nvalue
c$$$                              if (irow .eq. 1) then
c$$$                                 write(*,*)'Vis:',ichan,PolnPlane,loc,
c$$$     $                                off,cnorm
c$$$                              endif
                              sumwt(apol,achan)=sumwt(apol,achan)+
     $                             weight(ichan,irow)
c*real(cnorm)
                           endif
C     
                        end if
C                     stop
                     end do
                  end if
               end if
            end do
            chanPolPB=0
            do ichan=1,npol
               if (polsDone(ichan) .eqv. .true.) chanPolPB=chanPolPB+1
            enddo
            if (chanPolPB .lt. npol) then
               accumPB=1
            else 
               accumPB=0
            endif
         end if
      end do
      return
      end
C     
C     Degrid a number of visibility records
C     
      subroutine dpbmos0 (uvw, dphase, values, nvispol, nvischan,
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
      double precision sigma,area,lambda,cfscale
      
      complex nvalue
      
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt, pcwt,pdcwt1,pdcwt2
      double precision griduvw(2)
      double precision mysigma, ra1,ra2,dec1,dec2
      
      complex norm(4)
      
      logical opbmos,mreindex
      external gcppeij
      
      real pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, PolnPlane, ConjPlane
      integer convOrigin
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi
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
      
      dPA = -(currentCFPA - actualPA)
c      dPA=0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
      convOrigin = (convsize-1)/2
      
      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
            do ichan=1, nvischan
               achan=chanmap(ichan)+1
               
               lambda = c/freq(ichan)
               cfscale = cfRefFreq/freq(ichan)
               
               if((achan.ge.1).and.(achan.le.nchan)) then
                  call spbmos(uvw(1,irow), dphase(irow), freq(ichan), c,
     $                 scale, offset, sampling, pos, loc, off, phasor)
                  iloc(3)=max(1, min(wconvsize, loc(3)))
                  rsupport=support(iloc(3),1,paindex)
		  rsupport = nint( (rsupport / cfscale)+0.5 )
                  if (opbmos(nx, ny, wconvsize, loc, rsupport)) then
                     PolnPlane=0
                     
                     do ipol=1, nvispol
                        apol=polmap(ipol)+1
                        if((flag(ipol,ichan,irow).ne.1).and.
     $                       (apol.ge.1).and.(apol.le.npol)) then
                           ConjPlane = cfmap(ipol)+1
                           PolnPlane = conjcfmap(ipol)+1
                           
                           nvalue=0.0
                           norm(apol)=cmplx(0.0,0.0)
                           pcwt=cmplx(1.0,0.0)
                           
                           do iy=-rsupport,rsupport
                              iloc(2)=1+(iy*sampling+off(2))+convOrigin
			      iloc(2) = iloc(2) * cfscale
                              iv = (iy*sampling+off(2))
                              do ix=-rsupport,rsupport
                                 iloc(1)=1+(ix*sampling+off(1))
     $                                +convOrigin
                                 iloc(1) = iloc(1) * cfscale
                                 iu = (ix*sampling+off(1))
                                 
                                 if(mreindex(iu,iv,iloc(1),iloc(2),
     $                                sDPA,cDPA, convOrigin, convSize)) 
     $                                then
                                    if (dopointingcorrection .eq. 1)then
                                       griduvw(1)=(loc(1)-offset(1)
     $                                      +ix-1)/scale(1)-uvw(1,irow)
     $                                      /lambda
                                       griduvw(2)=(loc(2)-offset(2)
     $                                      +iy-1)/scale(2)-uvw(2,irow)
     $                                      /lambda
                                       ra1 = raoff(ant1(irow)+1)
                                       ra2 = raoff(ant2(irow)+1)
                                       dec1= decoff(ant1(irow)+1)
                                       dec2= decoff(ant2(irow)+1)
                                       call gfeij(griduvw,area,
     $                                      ra1,dec1,ra2,dec2,
     $                                      dograd,pcwt,pdcwt1,pdcwt2,
     $                                      currentCFPA)
c$$$                                       call gcppeij(griduvw,area,
c$$$     $                                      ra1,dec1,ra2,dec2,
c$$$     $                                      dograd,pcwt,pdcwt1,pdcwt2,
c$$$     $                                      currentCFPA)
                                    endif
                                    
                                    if(uvw(3,irow).gt.0.0) then
                                       cwt=conjg(convfunc(iloc(1),
     $                                      iloc(2), iloc(3),ConjPlane))
                                       pcwt = (pcwt)
                                    else
                                       cwt=(convfunc(iloc(1),
     $                                      iloc(2), iloc(3),PolnPlane))
                                       pcwt = (pcwt)
                                    endif
                                    nvalue=nvalue+(cwt)*(pcwt)*
     $                                   grid(loc(1)+ix,loc(2)+iy,apol,
     $                                   achan)
                                    norm(apol)=norm(apol)+(pcwt*cwt)
                                 endif
                              end do
                           end do
                           values(ipol,ichan,irow)=
     $                          nvalue*conjg(phasor)
     $                          /norm(apol)
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
C     Degrid a number of visibility records along with the grad. computations
C     
      subroutine dpbmosgrad (uvw, dphase, values, nvispol, nvischan,
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
      double precision sigma,area,lambda,cfscale
      
      complex nvalue,ngazvalue,ngelvalue
      
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt, pcwt,pdcwt1,pdcwt2
      double precision griduvw(2)
      double precision mysigma, ra1,ra2,dec1,dec2
      
      complex norm(4),gradaznorm(4),gradelnorm(4)
      
      logical opbmos,mreindex
      external gcppeij
      
      real pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, PolnPlane, ConjPlane
      integer convOrigin
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi, scaledSampling
      data pi/3.14159265358979323846/
      integer ii,jj,iu,iv
      integer scaledSupport
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
      dPA = -(currentCFPA - actualPA)
c      dPA = 0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
c      convOrigin = (convsize-1)/2
      convOrigin = (convsize)/2
      
      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
            do ichan=1, nvischan
               achan=chanmap(ichan)+1
               
               lambda = c/freq(ichan)
               cfscale = cfRefFreq/freq(ichan)
               scaledSampling = int(sampling*cfscale)

               if((achan.ge.1).and.(achan.le.nchan)) then
                  call spbmos(uvw(1,irow), dphase(irow), freq(ichan), c,
     $                 scale, offset, sampling, pos, loc, off, phasor)
                  iloc(3)=max(1, min(wconvsize, loc(3)))
                  rsupport=support(iloc(3),1,paindex)
                  scaledSupport = int(rsupport/cfscale+0.5)
c		  rsupport = nint( (rsupport / cfscale)+0.5 )
                  if (opbmos(nx, ny, wconvsize, loc, rsupport)) then
                     do ipol=1, nvispol
                        apol=polmap(ipol)+1
                        if((flag(ipol,ichan,irow).ne.1).and.
     $                       (apol.ge.1).and.(apol.le.npol)) then
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
                           
                           do iy=-scaledSupport,scaledSupport
c			      iloc(2) = (iy*sampling+off(2)) * cfscale
                              iloc(2) = (1+(iy*sampling+off(2)))
     $                             *cfscale + convOrigin
                              iv=iy*scaledSampling+off(2)
                              do ix=-scaledSupport,scaledSupport
c                                 iloc(1) = (ix*sampling+off(1))*cfscale
                                 iloc(1) = (1+(ix*sampling+off(1)))
     $                                *cfscale + convOrigin
                                 iu=ix*scaledSampling+off(1)
                                   
                                 if(mreindex(iu,iv,iloc(1),iloc(2),
     $                                sDPA,cDPA, convOrigin, convSize)) 
     $                                then
                                    if (dopointingcorrection .eq. 1)then
c$$$                                         griduvw(1)=(loc(1)-offset(1)
c$$$     $                                       +ix-1)/scale(1)-uvw(1,irow)
c$$$     $                                        /lambda
c$$$                                         griduvw(2)=(loc(2)-offset(2)
c$$$     $                                       +iy-1)/scale(2)-uvw(2,irow)
c$$$     $                                        /lambda
                                       iu=iloc(1)-convOrigin
                                       iv=iloc(2)-convOrigin
                                       griduvw(1)=iu/(scale(1)*sampling)
                                       griduvw(2)=iv/(scale(2)*sampling)
                                       ii=apol
                                       ra1 = raoff(ii,1,ant1(irow)+1)
c     $                                      *cfscale
                                       ra2 = raoff(ii,1,ant2(irow)+1)
c     $                                      *cfscale
                                       dec1= decoff(ii,1,ant1(irow)+1)
c     $                                      *cfscale
                                       dec2= decoff(ii,1,ant2(irow)+1)
c     $                                      *cfscale
                                       call gfeij(griduvw,area,
     $                                      ra1,dec1,ra2,dec2,
     $                                      dograd,pcwt,pdcwt1,pdcwt2,
     $                                      currentCFPA)
c$$$                                         call gcppeij(griduvw,area,
c$$$     $                                        ra1,dec1,ra2,dec2,
c$$$     $                                        dograd,pcwt,pdcwt1,pdcwt2,
c$$$     $                                        currentCFPA)
                                    endif
c$$$                                    if(uvw(3,irow).gt.0.0) then
c$$$                                       cwt=conjg(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),ConjPlane))
c$$$                                       pcwt = conjg(pcwt)
c$$$                                       pdcwt1 = conjg(pdcwt1)
c$$$                                       pdcwt2 = conjg(pdcwt2)
c$$$                                    else
c$$$                                       cwt=(convfunc(iloc(1),
c$$$     $                                      iloc(2), iloc(3),PolnPlane))
c$$$                                    endif
                                    cwt=(convfunc(iloc(1),
     $                                   iloc(2), iloc(3),PolnPlane))
                                    cwt = cwt*pcwt
                                    nvalue=nvalue+(cwt)*
     $                                   grid(loc(1)+ix,loc(2)+iy,apol,
     $                                   achan)
                                      
                                    if ((doconj .eq. 1) .and. 
     $                                   (dograd .eq. 1)) then
                                       pdcwt1 = conjg(pdcwt1)
                                       pdcwt2 = conjg(pdcwt2)
                                    endif
                                    if (dograd .eq. 1) then
                                       pdcwt1 = pdcwt1*cwt
                                       pdcwt2 = pdcwt2*cwt
                                       ngazvalue=ngazvalue+(pdcwt1)*
     $                                      (grid(loc(1)+ix,loc(2)+iy,
     $                                      apol,achan))
                                       ngelvalue=ngelvalue+(pdcwt2)*
     $                                      (grid(loc(1)+ix,loc(2)+iy,
     $                                      apol,achan))
                                    endif
                                    
                                    norm(apol)=norm(apol)+(cwt)
                                    gradaznorm(apol)=gradaznorm(apol)+
     $                                   pdcwt1
                                    gradelnorm(apol)=gradelnorm(apol)+
     $                                   pdcwt1
                                 endif
                              end do
                           end do
                           values(ipol,ichan,irow)=
     $                          nvalue*conjg(phasor)
     $                          /norm(apol)
                           if (dograd .eq. 1) then
                              gazvalues(ipol,ichan,irow)=ngazvalue*
     $                             conjg(phasor)/norm(apol)
                              gelvalues(ipol,ichan,irow)=ngelvalue*
     $                             conjg(phasor)/norm(apol)
                           endif
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
C     Degrid a number of visibility records
C     
      subroutine dpbmos (uvw, dphase, values, nvispol, nvischan,
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
      real raoff(2,1,nant), decoff(2,1,nant)
      double precision sigma,area,lambda,cfscale
      
      complex nvalue, junk
      
C     complex convfunc(convsize/2-1, convsize/2-1, wconvsize, polused),
      complex convfunc(convsize, convsize, wconvsize, polused),
     $     cwt, pcwt,pdcwt1,pdcwt2
      double precision griduvw(2)
      double precision mysigma, ra1,ra2,dec1,dec2
      
      complex norm(4)
      
      logical opbmos,mreindex
c     external nwcppEij
      external gcppeij
      
      real pos(3)
      integer loc(3), off(3), iloc(3)
      integer rbeg, rend
      integer ix, iy, ipol, ichan, PolnPlane, ConjPlane
      integer convOrigin
      integer apol, achan, irow
      real wt, wtx, wty
      double precision pi, scaledSampling
      data pi/3.14159265358979323846/
      integer ii,jj,iu,iv
      integer scaledSupport
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
      
      dPA = -(currentCFPA - actualPA)
c      dPA = 0
      cDPA = cos(dPA)
      sDPA = sin(dPA)
      convOrigin = (convsize-1)/2
c      convOrigin = (convsize)/2
      
      do irow=rbeg, rend
         if(rflag(irow).eq.0) then
            do ichan=1, nvischan
               achan=chanmap(ichan)+1
               
               lambda = c/freq(ichan)
               cfscale = cfRefFreq/freq(ichan)
               scaledSampling = int(sampling*cfscale)

               if((achan.ge.1).and.(achan.le.nchan)) then
                  call spbmos(uvw(1,irow), dphase(irow), freq(ichan), c,
     $                 scale, offset, sampling, pos, loc, off, phasor)
                  iloc(3)=max(1, min(wconvsize, loc(3)))
                  rsupport=support(iloc(3),1,paindex)
                  scaledSupport = int(rsupport/cfscale+0.5)
c		  rsupport = nint( (rsupport / cfscale)+0.5 )

                  if (opbmos(nx, ny, wconvsize, loc, rsupport)) then
                     PolnPlane=0
                     do ipol=1, nvispol
                        apol=polmap(ipol)+1
                        if((flag(ipol,ichan,irow).ne.1).and.
     $                       (apol.ge.1).and.(apol.le.npol)) then
                           
c$$$  ConjPlane = conjcfmap(ipol)+1
c$$$  PolnPlane = cfmap(ipol)+1
C     The following after feed_x -> -feed_x and PA -> PA + PI/2
                           ConjPlane = cfmap(ipol)+1
                           PolnPlane = conjcfmap(ipol)+1
                           
                           nvalue=0.0
                           norm(apol)=cmplx(0.0,0.0)
                           pcwt=cmplx(1.0,0.0)
                           
                           do iy=-scaledSupport,scaledSupport
c     iloc(2)=1+(iy*sampling+off(2))+convOrigin
                              iv = iy*scaledSampling+off(2)
                              do ix=-scaledSupport,scaledSupport
c     iloc(1)=1+(ix*sampling+off(1))+convOrigin
                                 iu = ix*scaledSampling+off(1)
                                 
                                 if(mreindex(iu,iv,iloc(1),iloc(2),
     $                                sDPA,cDPA, convOrigin, convSize))
     $                                then
                                    if (dopointingcorrection .eq. 1) 
     $                                   then
c$$$                                       griduvw(1) = (loc(1)-offset(1)
c$$$     $                                      +ix-1)
c$$$     $                                      /scale(1)-uvw(1,irow)/lambda
c$$$                                       griduvw(2) = (loc(2)-offset(2)
c$$$     $                                      +iy-1)
c$$$     $                                      /scale(2)-uvw(2,irow)/lambda
C
C     (iu,iv) are the non-PA-rotated co-ords. for the Conv. Func.  Use
C     those to compute the phase gradient for pointing offset(squint).
C     Pointing errors must rotate with PA and for those, iloc must be
C     used to compute the appropriate phase gradient.
C
                                       griduvw(1)=(iloc(1)-convOrigin)/
     $                                      (scale(1)*sampling)
                                       griduvw(2)=(iloc(2)-convOrigin)/
     $                                      (scale(2)*sampling)
                                       ii=apol
                                       ra1 = raoff(ii,1,ant1(irow)+1)
                                       ra2 = raoff(ii,1,ant2(irow)+1)
                                       dec1= decoff(ii,1,ant1(irow)+1)
                                       dec2= decoff(ii,1,ant2(irow)+1)
                                       call gfeij(griduvw,area,
     $                                      ra1,dec1,ra2,dec2,
     $                                      dograd,pcwt,pdcwt1,pdcwt2,
     $                                      currentCFPA)
c$$$                                       call gcppeij(griduvw,area,
c$$$     $                                      ra1,dec1,ra2,dec2,
c$$$     $                                      dograd,pcwt,pdcwt1,pdcwt2,
c$$$     $                                      currentCFPA)
                                    endif
                                    
                                    if(uvw(3,irow).gt.0.0) then
                                       cwt=(convfunc(iloc(1),
     $                                      iloc(2), iloc(3),PolnPlane))
                                    else
                                       cwt=conjg(convfunc(iloc(1),
     $                                      iloc(2), iloc(3),ConjPlane))
                                    endif
C New code start
                                    cwt=(convfunc(iloc(1),
     $                                   iloc(2), iloc(3),PolnPlane))
                                    cwt=cwt*pcwt;
                                    nvalue=nvalue+(cwt)
     $                                   *grid(loc(1)+ix,loc(2)+iy,
     $                                   apol,achan)
C New code end

c                                    norm(apol)=norm(apol)+(pcwt*cwt)
                                    norm(apol)=norm(apol)+cwt

c$$$                                    junk=grid(loc(1)+ix,loc(2)+iy,
c$$$     $                                   apol,achan)
c$$$                                       write(*,*)nvalue,
c$$$     $                                   abs(junk),
c$$$     $                                   atan2(imag(junk),
c$$$     $                                   real(junk))*57.29563,
c$$$     $                                   ix,iy,apol,loc(1),loc(2),
c$$$     $                                   iloc(1), iloc(2),
c$$$     $                                   (cwt),iu,iv,
c$$$     $                                   norm(apol)

c$$$  write(*,*)abs(grid(loc(1)+ix,loc(2)+iy
c$$$  $                                ,apol,achan)),ix,iy,apol,abs(cwt),
c$$$  $                                (pcwt),dopointingcorrection
                                 endif
                              end do
c     write(*,*)
                           end do
c                           stop
c     norm(apol) = norm(apol)/abs(norm(apol))
                           values(ipol,ichan,irow)=
     $                          nvalue*conjg(phasor)
     $                          /norm(apol)
c$$$                           if ((ant1(irow)+1 .eq. 2) .and.
c$$$     $                          (ant2(irow)+1 .eq. 4)) then
c$$$                              write (*,*) ant1(irow)+1, ant2(irow)+1,
c$$$     $                             ipol,ichan,irow,
c$$$     $                             abs(values(ipol,ichan,irow)),
c$$$     $                             atan2(imag(values(ipol,ichan,irow)),
c$$$     $                             real(values(ipol,ichan,irow))),
c$$$     $                             abs(nvalue),
c$$$     $                             atan2(imag(nvalue),real(nvalue)),
c$$$     $                             abs(norm(apol)),
c$$$     $                             atan2(imag(norm(apol)),
c$$$     $                             real(norm(apol)))
c$$$                           endif
c                           stop
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
C     Calculate gridded coordinates and the phasor needed for
C     phase rotation. 
C     
      subroutine spbmos (uvw, dphase, freq, c, scale, offset, 
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
      logical function opbmos (nx, ny, nw, loc, support)
      implicit none
      integer nx, ny, nw, loc(3), support
      opbmos=(support.gt.0).and.
     $     (loc(1)-support.ge.1).and.(loc(1)+support.le.nx).and.
     $     (loc(2)-support.ge.1).and.(loc(2)+support.le.ny).and.
     $     (loc(3).ge.1).and.(loc(3).le.nw)
      return
      end
      
      logical function mreindex(inx,iny,outx,outy,sinDPA,cosDPA,
     $     Origin, Size)
      integer inx,iny,outx,outy, Origin, Size
      double precision sinDPA, cosDPA
      integer ix,iy
      
      if (sinDPA .eq. 0) then
         ix = inx
         iy = iny
      else
         ix = nint(cosDPA*inx + sinDPA*iny)
         iy = nint(-sinDPA*inx + cosDPA*iny)
      endif

      outx = ix+Origin
      outy = iy+Origin
      
      mreindex=(outx .ge. 1 .and. 
     $     outx .le. Size .and.
     $     outy .ge. 1 .and.
     $     outy .le. Size)
      
      return
      end
      
