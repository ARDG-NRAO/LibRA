      subroutine faccumulateToGrid(grid, convFuncV, nvalue, wVal, 
     $     scaledSupport,scaledSampling,
     $     off, convOrigin, cfShape, loc,
     $     igrdpos, sinDPA, cosDPA,
     $     finitePointingOffset,
     $     doPSFOnly,
     $     norm,
     $     phaseGrad,
     $     imNX, imNY, imNP, imNC,
     $     cfNX, cfNY, cfNP, cfNC,
     $     phNX, phNY)

      implicit none
      integer imNX, imNY, imNC, imNP,
     $     vNX, vNY, vNC, vNP,
     $     cfNX, cfNY, cfNP, cfNC,
     $     phNX, phNY

      complex grid(imNX, imNY, imNP, imNC)
      complex convFuncV(cfNX, cfNY, cfNP, cfNC)
      complex nvalue
      double precision wVal
      integer scaledSupport(2)
      real scaledSampling(2)
      double precision off(2)
      integer convOrigin(4), cfShape(4), loc(4), igrdpos(4)
      double precision sinDPA, cosDPA
      integer finitePointingOffset, doPSFOnly
      complex norm
      complex phaseGrad(phNX, phNY)

      integer l_phaseGradOriginX, l_phaseGradOriginY
      integer iloc(4), iCFPos(4)
      complex wt, cfArea

      complex fGetCFArea
      integer ix,iy
      integer l_igrdpos(4)

      data iloc/1,1,1,1/, iCFPos/1,1,1,1/
      l_igrdpos(3) = igrdpos(3)+1
      l_igrdpos(4) = igrdpos(4)+1
c      norm=0.0
      l_phaseGradOriginX=phNX/2 + 1
      l_phaseGradOriginY=phNY/2 + 1

c     Currently returns 1.0
      cfArea = fGetCFArea(convFuncV, wVal, scaledSupport, 
     $     scaledSampling, off, convOrigin, cfShape,
     $     cfNX, cfNY, cfNP, cfNC)

c      cfArea=complex(1.0, imag(cfArea))
c      cfArea=1.0
      do iy=-scaledSupport(2),scaledSupport(2) 
         iloc(2)=nint(scaledSampling(2)*iy+off(2)-1)
         iCFPos(2)=iloc(2)+convOrigin(2)+1
         l_igrdpos(2) = loc(2)+iy+1
         do ix=-scaledSupport(1),scaledSupport(1)
            iloc(1)=nint(scaledSampling(1)*ix+off(1)-1)
            iCFPos(1) = iloc(1) + convOrigin(1) + 1
            l_igrdpos(1) = loc(1) + ix + 1
            
            wt = convFuncV(iCFPos(1), iCFPos(2), 
     $           iCFPos(3), iCFPos(4))/cfArea
            if (wVal > 0.0) wt = conjg(wt)

            norm = norm + (wt)

c$$$            if ((finitePointingOffset .eq. 1) .and.
c$$$     $              (doPSFOnly .eq. 0)) then
            if (finitePointingOffset .eq. 1) then
               wt = wt * phaseGrad(iloc(1) + l_phaseGradOriginX,
     $              iloc(2) + l_phaseGradOriginY)
            endif

            grid(l_igrdpos(1), l_igrdpos(2), l_igrdpos(3), l_igrdpos(4)) 
     $           =grid(l_igrdpos(1), l_igrdpos(2), 
     $           l_igrdpos(3), l_igrdpos(4)) + nvalue * wt
         enddo
      enddo
      end
      
      subroutine dInnerXLoop(grid, 
     $     imNX, imNY, imNP, imNC,
     $     nvalue,
     $     scaledSupport,scaledSampling,
     $     convOrigin, 
     $     iloc, l_igrdpos,iCFPos,loc,off, 
     $     wVal,
     $     cfNX, cfNY,cfNP, cfNC,
     $     convFuncV,
     $     cfArea,
     $     phNX, phNY,
     $     phaseGrad,
     $     finitePointingOffset, doPSFOnly,
     $     norm)
      implicit none
      integer imNX, imNY, imNP, imNC
      double complex grid(imNX, imNY, imNP, imNC)
      complex nvalue
      integer scaledSupport(2), convOrigin(4)
      integer iloc(4), iCFPos(4),loc(4),l_igrdpos(4)
      double precision off(2),wVal
      integer cfNX, cfNY, cfNP, cfNC
      complex convFuncV(cfNX, cfNY, cfNP, cfNC), cfArea
      complex wt,norm
      integer finitePointingOffset, doPSFOnly
      integer phNX, phNY
      complex phaseGrad(phNX, phNY)
      real scaledSampling(2)

      integer l_phaseGradOriginX, l_phaseGradOriginY

      integer ix

      do ix=-scaledSupport(1),scaledSupport(1)
         iloc(1)=nint(scaledSampling(1)*ix+off(1)-1)
         iCFPos(1) = iloc(1) + convOrigin(1) + 1
         l_igrdpos(1) = loc(1) + ix + 1
         
         wt = convFuncV(iCFPos(1), iCFPos(2), 
     $        iCFPos(3), iCFPos(4))/cfArea
         if (wVal > 0.0) wt = conjg(wt)
         
         norm = norm + (wt)
         
c$$$         if ((finitePointingOffset .eq. 1) .and.
c$$$     $        (doPSFOnly .eq. 0)) then
         if (finitePointingOffset .eq. 1) then
            wt = wt * phaseGrad(iloc(1) + l_phaseGradOriginX,
     $           iloc(2) + l_phaseGradOriginY)
         endif
         
         grid(l_igrdpos(1), l_igrdpos(2), l_igrdpos(3), l_igrdpos(4)) 
     $        =grid(l_igrdpos(1), l_igrdpos(2), 
     $        l_igrdpos(3), l_igrdpos(4)) + nvalue * wt
      enddo
      return
      end
      
      subroutine dfaccumulateToGrid(grid, convFuncV, nvalue, wVal, 
     $     scaledSupport,scaledSampling,
     $     off, convOrigin, cfShape, loc,
     $     igrdpos, sinDPA, cosDPA,
     $     finitePointingOffset,
     $     doPSFOnly,
     $     norm,
     $     phaseGrad,
     $     imNX, imNY, imNP, imNC,
     $     cfNX, cfNY, cfNP, cfNC,
     $     phNX, phNY)
      
      implicit none
      integer imNX, imNY, imNC, imNP,
     $     vNX, vNY, vNC, vNP,
     $     cfNX, cfNY, cfNP, cfNC,
     $     phNX, phNY
      
      double complex grid(imNX, imNY, imNP, imNC)
      complex convFuncV(cfNX, cfNY, cfNP, cfNC)
      complex nvalue
      double precision wVal
      integer scaledSupport(2)
      real scaledSampling(2)
      double precision off(2)
      integer convOrigin(4), cfShape(4), loc(4), igrdpos(4)
      double precision sinDPA, cosDPA
      integer finitePointingOffset, doPSFOnly
      complex norm
      complex phaseGrad(phNX, phNY)
      
      integer l_phaseGradOriginX, l_phaseGradOriginY
      integer iloc(4), iCFPos(4)
      complex wt, cfArea
      
      complex fGetCFArea
      integer ix,iy
      integer l_igrdpos(4)
      
      data iloc/1,1,1,1/, iCFPos/1,1,1,1/
      l_igrdpos(3) = igrdpos(3)+1
      l_igrdpos(4) = igrdpos(4)+1
c      norm=0.0
      l_phaseGradOriginX=phNX/2 + 1
      l_phaseGradOriginY=phNY/2 + 1
      
c     Currently return 1.0
      cfArea = fGetCFArea(convFuncV, wVal, scaledSupport, 
     $     scaledSampling, off, convOrigin, cfShape,
     $     cfNX, cfNY, cfNP, cfNC)

c      cfArea=1.0
c$$$!$OMP PARALLEL 
c$$$!$OMP& DEFAULT(NONE) 
c$$$!$OMP& FIRSTPRIVATE(grid)
c$$$!$OMP& FIRSTPRIVATE(iloc,iCFPos,l_igrdpos,convFuncV)
c$$$!$OMP& FIRSTPRIVATE(imNX,imNY,imNP,imNC,nvalue)
c$$$!$OMP& FIRSTPRIVATE(scaledSupport, scaledSampling)
c$$$!$OMP& FIRSTPRIVATE(convOrigin)
c$$$!$OMP& FIRSTPRIVATE(loc,off,wVal,cfNX)
c$$$!$OMP& FIRSTPRIVATE(cfNY, cfNP, cfNC)
c$$$!$OMP& FIRSTPRIVATE(cfArea,phNX, phNY, phaseGrad)
c$$$!$OMP& FIRSTPRIVATE(finitePointingOffset, doPSFOnly)
c$$$!$OMP& FIRSTPRIVATE(norm)
c$$$!$OMP& PRIVATE(iy) NUM_THREADS(3)
c$$$!$OMP DO
      do iy=-scaledSupport(2),scaledSupport(2) 
         iloc(2)=nint(scaledSampling(2)*iy+off(2)-1)
         iCFPos(2)=iloc(2)+convOrigin(2)+1
         l_igrdpos(2) = loc(2)+iy+1
c$$$         call dInnerXLoop(grid, imNX, imNY, imNP, imNC,
c$$$     $        nvalue, scaledSupport, scaledSampling,
c$$$     $        convOrigin, iloc, l_igrdpos, iCFPos, loc,off,
c$$$     $        wVal, cfNX, cfNY, cfNP, cfNC, convFuncV,cfArea,
c$$$     $        phNX, phNY, phaseGrad,
c$$$     $        finitePointingOffset, doPSFOnly,norm)
         do ix=-scaledSupport(1),scaledSupport(1)
            iloc(1)=nint(scaledSampling(1)*ix+off(1)-1)
            iCFPos(1) = iloc(1) + convOrigin(1) + 1
            l_igrdpos(1) = loc(1) + ix + 1
            
            wt = convFuncV(iCFPos(1), iCFPos(2), 
     $           iCFPos(3), iCFPos(4))/cfArea
            if (wVal > 0.0) wt = conjg(wt)

c$$$            if ((iCFPos(1) .gt. cfShape(1)) .or.
c$$$     $           (iCFPos(2) .gt. cfShape(2)) .or.
c$$$     $           (iCFPos(3) .gt. cfShape(3)) .or.
c$$$     $           (iCFPos(4) .gt. cfShape(4))) then
c$$$               write(*,*) 'O: ',iCFPos, cfShape,scaledSupport,
c$$$     $              off,scaledSampling
c$$$            endif
c$$$            if ((iCFPos(1) .le. 0) .or.
c$$$     $           (iCFPos(2) .le. 0) .or.
c$$$     $           (iCFPos(3) .le. 0) .or.
c$$$     $           (iCFPos(4) .le. 0)) then
c$$$               write(*,*) 'U: ',iCFPos, cfShape,scaledSupport,
c$$$     $              off,scaledSampling
c$$$            endif
            
            norm = norm + (wt)

c$$$            if ((finitePointingOffset .eq. 1) .and.
c$$$     $           (doPSFOnly .eq. 0)) then
            if (finitePointingOffset .eq. 1) then
               wt = wt * phaseGrad(iloc(1) + l_phaseGradOriginX,
     $              iloc(2) + l_phaseGradOriginY)
            endif

c$$$            if (isnan(abs(wt))) then
c$$$               write(*,*) iCFPos(1), iCFPos(2), iCFPos(3), iCFPos(4),
c$$$     $              cfArea,norm
c$$$               return
c$$$            endif
            
            
            
            grid(l_igrdpos(1), l_igrdpos(2), l_igrdpos(3), l_igrdpos(4)) 
     $           =grid(l_igrdpos(1), l_igrdpos(2), 
     $           l_igrdpos(3), l_igrdpos(4)) + nvalue * wt

         enddo
      enddo
c$$$!$OMP END DO
c$$$!$OMP END PARALLEL
      end
      
      complex function fGetCFArea(convFuncV, wVal, support, sampling,
     $     off, convOrigin, cfShape,
     $     cfNX, cfNY, cfNP, cfNC)
      
      implicit none
      integer cfNX, cfNY, cfNP, cfNC
      complex convFuncV(cfNX, cfNY, cfNP, cfNC)
      double precision wVal
      integer support(2)
      real sampling(2)
      double precision off(2)
      integer convOrigin(4), cfShape(4)
      
      complex area, wt
      integer iloc(4), iCFPos(4)
      integer ix,iy
      data iloc/1,1,1,1/, iCFPos/1,1,1,1/
      
      fGetCFArea = 1.0
      return
      area=0.0

      do iy=-support(2), support(2)
         iloc(2) = nint((sampling(2)*iy+off(2))-1)
         iCFPos(2) = iloc(2) + convOrigin(2) + 1
         do ix=-support(1), support(2)
            iloc(1) = nint((sampling(1)*ix+off(1))-1)
            iCFPos(1) = iloc(1) + convOrigin(1) + 1
            
            wt = convFuncV(iCFPos(1), iCFPos(2), iCFPos(3), iCFPos(4))
            if (wVal > 0.0) wt = conjg(wt)
            area = area + wt
         enddo
      enddo
      
      fGetCFArea = area
      return
      end
      
