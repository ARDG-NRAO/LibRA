      subroutine faccumulateFromGrid(nvalue, norm,grid, convFuncV,
     $     wVal, scaledSupport, scaledSampling, 
     $     off, convOrigin, cfShape, loc,
     $     igrdpos,  sinDPA, cosDPA,
     $     finitePointingOffset,
     $     phaseGrad,
     $     phasor,
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
      integer finitePointingOffset
      complex norm
      complex phaseGrad(phNX, phNY),phasor

      integer l_phaseGradOriginX, l_phaseGradOriginY
      integer iloc(4), iCFPos(4)
      complex wt
      integer ix,iy
      integer l_igrdpos(4)

      data iloc/1,1,1,1/, iCFPos/1,1,1,1/
      l_igrdpos(3) = igrdpos(3)+1
      l_igrdpos(4) = igrdpos(4)+1
      l_phaseGradOriginX=phNX/2 + 1
      l_phaseGradOriginY=phNY/2 + 1

      do iy=-scaledSupport(2),scaledSupport(2) 
         iloc(2)=nint(scaledSampling(2)*iy+off(2)-1)
         iCFPos(2)=iloc(2)+convOrigin(2)+1
         l_igrdpos(2) = loc(2)+iy+1
         do ix=-scaledSupport(1),scaledSupport(1)
            iloc(1)=nint(scaledSampling(1)*ix+off(1)-1)
            iCFPos(1) = iloc(1) + convOrigin(1) + 1
            l_igrdpos(1) = loc(1) + ix + 1
C            
C Conjugate the CF to account for the W-term and poln. squint.  This is
C the equivalent of the A^\dag operator in the A-Projection paper (A&A 487,
C 419-429 (2008); http://arxiv.org/abs/0805.0834) for the antenna optics (plus
C geometric effects like the w-term) 
C
            wt = convFuncV(iCFPos(1), iCFPos(2), 
     $           iCFPos(3), iCFPos(4))
            if (wVal .le. 0.0)  wt = conjg(wt)
            
            norm = norm + (wt)
C
C     Apply the conjugate of the phase gradient.  This, along with
C     conjg(phasor) later, is the equivalent of applying the A^\dag
C     operator in the A-Projection paper for mosaic imaging.
C
            if (finitePointingOffset .eq. 1) then
               wt = wt * conjg(phaseGrad(iloc(1) + l_phaseGradOriginX, 
     $              iloc(2) + l_phaseGradOriginY))
            endif

            nvalue = nvalue + wt * grid(l_igrdpos(1), l_igrdpos(2), 
     $           l_igrdpos(3), l_igrdpos(4))

         enddo
      enddo
c     nvalue = nvalue *conjg(phasor)/norm
c     Normalization by norm is done in the Mueller loop in AWVR.
      nvalue = nvalue *conjg(phasor)
      end
