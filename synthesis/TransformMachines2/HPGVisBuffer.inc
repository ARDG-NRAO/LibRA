// -*- C++ -*-
#ifndef SYNTHESIS_TRANSFORM2_HPGVISBUFFER_H
#define SYNTHESIS_TRANSFORM2_HPGVISBUFFER_H

#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines2/VBStore.h>
//#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines2/FTMachine.h>

#include <hpg/hpg.hpp>
#include <hpg/hpg_indexing.hpp>
using namespace hpg;
//
// -------------------------------------------------------------------------------
//
template <unsigned N>
std::vector<hpg::VisData<N>>
create_blank_vis_data_vector(const unsigned nPol, const unsigned nChan, const unsigned nRow)
{
  (void)nPol; // Just to remove an annoying compile-time warning ("unused parameter").
  assert(nPol == N);
  std::vector<VisData<N> > hpgVisBuffer;
  //hpgVisBuffer.reserve(nRow*nChan);
  hpgVisBuffer.resize(nRow*nChan);
  return hpgVisBuffer;
}
#define if_flag_ignore_cond(flag,cond) ((flag)?(flag):(cond))
//
// -------------------------------------------------------------------------------
//
template <unsigned N>
std::vector<hpg::VisData<N>>
makeHPGVisBuffer(casacore::Matrix<double>& sumwt,
		 const casa::refim::VBStore& casaVBS,
		 casacore::CountedPtr<casa::refim::VB2CFBMap> vb2CFBMap,
		 const unsigned nGridPol, const unsigned nGridChan,
		 const unsigned nVisPol,
		 unsigned startRow,  unsigned endRow,
		 const unsigned startChan, const unsigned endChan,
		 const casacore::Vector<casacore::Int>& chanMap,
		 const casacore::Vector<casacore::Int>& polMap,
		 const casa::PolMapType& /*muellerNdx*/,
		 const casa::PolMapType& muellerVals, 
		 const casacore::Vector<double>& dphase,
		 const casacore::Vector<casacore::Vector<casacore::Double> >& pointingOffsets,
		 const hpg::CFSimpleIndexer& cfsi,
		 bool useFlaggedData=false
		 )
{
  unsigned nVisChan=endChan - startChan + 1;
  unsigned nVisRow=endRow - startRow + 1;
  unsigned targetIMChan, targetIMPol;
  const casa::VisBuffer2& casaVB = *(casaVBS.vb_p);
  IPosition dataShape = casaVB.visCube().shape();
  //unsigned nDataChan=dataShape(1);
  unsigned nDataPol=dataShape(0);
  std::vector<hpg::VisData<N>> hpgVB=create_blank_vis_data_vector<N>(nVisPol,nVisChan,nVisRow);

  std::array<std::complex<hpg::visibility_fp>, N> vis;
  std::array<hpg::vis_weight_fp, N> wt;
  std::vector<std::map<unsigned, vis_weight_fp>> channel_wt_map(hpgVB.size());
  
  const casacore::Matrix<double> UVW=casaVBS.uvw_p;
  //cerr << "SHAPE UVW " << UVW.shape() << endl;
  std::array<hpg::vis_uvw_fp, 3> hpgUVW;
  hpg::vis_phase_fp              dphaseHPG;
  unsigned                       grid_cube=0; // For now, grid on the same plane.
  hpg::cf_phase_gradient_t       cf_phase_gradient;
  // SB: hpg::vis_cf_index_t vis_cf_index = {0,cfFreqNdx}; // For fndx A-only
  std::array<unsigned, 2>        cf_index;

  assert(muellerVals.nelements() == N);

  casa::PolMapType visElements=muellerVals;
  for(unsigned i=0;i<visElements.nelements();i++)
    for(unsigned j=0;j<visElements[i].nelements();j++)
      visElements[i][j] = ((3-visElements[i][j])*5)%N;

  Bool Dummy;
  const Double *freq//=casaVBS.freq_p.getStorage(Dummy);
    = casaVB.getFrequencies(0).getStorage(Dummy);
  Int vbSpw = casaVBS.vb_p->spectralWindows()(0);
  // CFSimpleIneders is filled in by HPGLoadCF().  It's extents
  // therefore carries the information about CFArray that is currently
  // on the device (GPU).  CFSI.cell_extents() are the extents in the
  // CFBuffer indexing scheme.
  //
  // extents[0:5] correspond to nBaselineType, nTime/PA, nW, nFreq, nPol
  std::array<unsigned, 5> cfsi_extents = cfsi.cell_extents();
  //  cerr << "cfsi: "; for(auto i:cfsi_extents) cerr << i << " "; cerr << endl;

  unsigned hpgIRow=0;
  for(unsigned irow=startRow; irow< endRow; irow++)
    {   
      wt.fill(0.0);
      vis.fill(0.0);

      if(if_flag_ignore_cond(useFlaggedData,
			     !casaVBS.rowFlag_p(irow))
	 )
	{   
	  CountedPtr<casa::refim::CFBuffer> cfb = (*vb2CFBMap)[irow];
	  for(unsigned ichan=startChan; ichan< endChan; ichan++)
	    {
	      if (if_flag_ignore_cond(useFlaggedData,
				      (casaVBS.imagingWeight_p(ichan,irow)!=0.0))
		  )
		{  
		  targetIMChan=chanMap[ichan];

		  if(if_flag_ignore_cond(useFlaggedData,
					 ((targetIMChan>=0) && (targetIMChan<nGridChan)))
		     )
		    {
		      //Double freq = casaVB.getFrequencies(irow)(ichan);
		      Double dataWVal = casaVB.uvw()(2,irow);

		      Int cfWNdx=0, cfFreqNdx=0; // Narrow band AProjection only.

		      if ((cfsi_extents[2] > 1) && (cfsi_extents[3]==1)) // W-only or WB-AWP case. All W-CFs for _a_ SPW only are loaded
			{
			  cfFreqNdx=0;
			  cfWNdx = cfb->nearestWNdx(abs(dataWVal)*freq[ichan]/C::c);
			}
		      else if ((cfsi_extents[2] == 1) && (cfsi_extents[3]>1)) //A-only case.  CFs for all SPWs are loaded
			{
			  cfFreqNdx=cfb->nearestFreqNdx(vbSpw,ichan,casaVBS.conjBeams_p);
			  cfWNdx=0;
			}

//cfFreqNdx=0;
 // if (cfFreqNdx > 0) 
 //   cerr << "vbSpw, wndx, fndx: " << targetIMChan << " " << vbSpw << " " << irow << " " << ichan << " " << cfWNdx << " " << cfFreqNdx << " " << freq[ichan] << endl;

unsigned mRow=0;
		      for(unsigned ipol=0; ipol< nDataPol; ipol++) 
			{ 
			  //			  if(!(casaVB.flagCube())(ipol,ichan,irow))
			    {  
			      targetIMPol=polMap(ipol);
			      
			      //int muellerElement=(3-ipol)*5; // Pick just the diagonal elements of the Mueller matrix
			      
			      if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
				{
				  // ipol determines the targetIMPol.  Each targetIMPol gets a row of CFs (mRow).
				  // visVecElements is gridded using the convFuncV and added to the target grid.
				  //cerr << "conjMNdx: " << ipol << " " << conjMRow << " " << (muellerElement%nDataPol) << endl << endl;
				  for (uInt mc=0; mc<visElements[mRow].nelements(); mc++) 
				    {
				      int visVecElement=visElements[mRow][mc];
				      
				      if (casaVBS.vb_p->flagCube()(ipol,ichan,irow)==false)
					{
					  if ((casaVBS.ftmType_p==casa::refim::FTMachine::WEIGHT)||
					      (casaVBS.ftmType_p==casa::refim::FTMachine::PSF))
					    vis[visVecElement] = std::complex<double>(1.0,0.0);
					  else if ((casaVBS.ftmType_p==casa::refim::FTMachine::SNRPSF))
					    vis[visVecElement] = std::complex<double>(fabs(casaVBS.visCube_p(ipol,ichan,irow)),0.0);
					  else
					    {
					      // int tmpIPol=3;
					      // vis[visVecElement] = casaVBS.visCube_p(tmpIPol,ichan,irow);
					      vis[visVecElement] = casaVBS.visCube_p(ipol,ichan,irow);
					    }
					    //vis[visVecElement] = casaVBS.visCube_p(visVecElement,ichan,irow);

					  wt[visVecElement]=casaVBS.imagingWeight_p(ichan,irow);
					  // cerr << "HPGVB: " << ipol << " " << ichan << " " << irow << " " << visVecElement << " "
					  //      << visElements << " " << vis[visVecElement] << " " << wt[visVecElement] << " "
					  //      << casaVBS.flagCube_p(ipol,ichan,irow) << " " << UVW(0,irow) << " "
					  //      << UVW(1,irow) << " " <<  UVW(2,irow) << endl;
					  //sumwt(targetIMPol,targetIMChan) += wt[visVecElement];
					}
				      else
					{
					  //cerr << "HPGVB0: " << ipol << " " << ichan << " " << irow << " " << visVecElement << " "
					  //     << visElements << " " << vis[visVecElement] << " " << wt[visVecElement] << " "
					  //     << casaVBS.flagCube_p(ipol,ichan,irow) << " ";
					  vis[visVecElement] = 0.0;
					  wt[visVecElement]  = 0.0;
					}
				    }
				  mRow++;
				  if (((targetIMPol>=0) && (targetIMPol<nGridPol)) &&
				      (((targetIMChan>=0) && (targetIMChan<nGridChan))))
				    sumwt(targetIMPol,targetIMChan) += casaVB.imagingWeight()(ichan, irow);//*abs(norm);
				}
			    }
			} // End poln-loop

		      hpg::vis_frequency_fp frequency=freq[ichan];
		      if (casaVBS.ftmType_p==casa::refim::FTMachine::SNRPSF)
			{
			  // Grid just the pure weights
			  hpgUVW={UVW(0,irow),
				  UVW(1,irow),
				  UVW(2,irow)};
			  dphaseHPG=0.0;
			}
		      if (casaVBS.ftmType_p==casa::refim::FTMachine::PSF)
			{
			  // Grid just the pure weights
			  hpgUVW={UVW(0,irow),
				  UVW(1,irow),
				  UVW(2,irow)};
			  dphaseHPG=0.0;
			}
		      else if (casaVBS.ftmType_p==casa::refim::FTMachine::WEIGHT)
			{
			  // Grid CFs at the center of the uv-grid
			  hpgUVW={0.0, 0.0, 0.0};
			  dphaseHPG=0.0;
			}
		      else
			{
			  // Use the full gridding equation
			  hpgUVW={UVW(0,irow),
				  UVW(1,irow),
				  UVW(2,irow)};
			  dphaseHPG=-2.0*C::pi*dphase[irow]*frequency/C::c;
			}
		      //hpg::vis_frequency_fp frequency=casaVB.getFrequencies(irow)(ichan);
		      grid_cube=0; // For now, grid on the same plane.
		      cf_phase_gradient={(hpg::cf_phase_gradient_fp)pointingOffsets[0][0],
					 (hpg::cf_phase_gradient_fp)pointingOffsets[0][1]};
				      
		      // SB: hpg::vis_cf_index_t vis_cf_index = {0,cfFreqNdx}; // For fndx A-only

		      unsigned dummyPolNdx=1;
		      hpg::CFCellIndex cfCellNdx(0,0, cfWNdx, cfFreqNdx, dummyPolNdx);
		      // full_hpgcf_index[0] ==> pol
		      // full_hpgcf_index[1] ==> cube : should always be 0
		      // full_hpgcf_index[2] ==> group : linearized index from (BLType, PA, W, SPW)
		      std::array<unsigned, 3> full_hpgcf_index=cfsi.cf_index(cfCellNdx);
		      auto& cf_cube_index=full_hpgcf_index[1];
		      auto& cf_group_index=full_hpgcf_index[2];
		      
		      cf_index = {cf_cube_index,cf_group_index};

		      hpgVB[hpgIRow++] = hpg::VisData<N>(vis,wt,frequency,dphaseHPG,hpgUVW,grid_cube,cf_index
							 ,cf_phase_gradient
							 );
		    }
		}
	    } // End chan-loop
	}
    } // End row-loop

  hpgVB.resize(hpgIRow);
  return hpgVB;
}
//
// -------------------------------------------------------------------------------
// Copy visibilities from HPG data strcture to CASA data structure.
//
// We don't live in an ideal world (or both data structures could have
// been the same!).
template <unsigned N>
void makeCASAVisBuffer(//const unsigned nVisRow, const unsigned nVisChan, const unsigned nVisPol,
		       const casacore::Vector<casacore::Int>& polMap,
		       casa::refim::VBStore& vbs,
		       std::vector<hpg::VisData<N>>& hpgVis)
{
  IPosition visShp=vbs.vb_p->visCube().shape();
  // Make sure both data structures have the same number of rows (at least!).
  // unsigned totalHPGRows = nVisRow*nVisChan;
  // // cerr << "totaHPGRows, vb.nRows(): " << " " << totalHPGRows << " " << vbs.vb_p->nRows() << " " << nVisRow << " " << nVisChan << endl
  // //      << visShp << endl;

  // //  assert(vbs.vb_p->nRows() == totalHPGRows);
  // assert(nVisPol == N);
  // While visibilities are conceptually a Cube (Pol, Chan, Row), it
  // has been flattended in HPG data strcture.  So any code that
  // access it need to know the order of storage, do the indexing
  // etc. (uncessary and distracting code spread all over, primed for
  // nasty bugs!).
  //
  // The assumption in the code below is that the order of storage in
  // HPG data structure is the following.  Notice the use of
  // operator() and operator[] to access vis from HPG data structure.
  //
  //    for all Row
  //      for all Chan
  //        for all Pol
  //          VB::visCube(Row, Chan, Pol) = HPG.vis(k++)[iPol]
  unsigned hpgRow=0;
  casacore::Cube<casacore::Complex> casaModelVisCube(visShp);
  for (int iRow=0; iRow < visShp(2); iRow++)
    {
      for (int iChan=0; iChan < visShp(1); iChan++)
	{
	  uint hpgPol=0;
	  for (int iPol=0; iPol < visShp(0); iPol++)
	    {
	      if (polMap(iPol) >= 0)
		{
		  casaModelVisCube(iPol,iChan, iRow) = hpgVis[hpgRow].m_visibilities[hpgPol];
		  // cerr << iPol << " " << iChan << " " << iRow << " " << hpgRow << " " << hpgVis.size()
		  //      << " " << hpgVis[hpgRow].m_visibilities[hpgPol]
		  //      << " " << hpgVis[hpgRow].m_weights[hpgPol]
		  //      << endl;
		  hpgPol++;
		}
	    }
	  hpgRow++;
	}
    }
  const_cast<casa::vi::VisBuffer2&>(vbs.vb()).setVisCubeModel(casaModelVisCube);
  return;// casaModelVisCube;
}
//
// -------------------------------------------------------------------------------
//
template <unsigned N>
unsigned long nVis(const std::vector<hpg::VisData<N>>& hpgVB)
{
  unsigned long nd=0;
  for(unsigned i=0;i<hpgVB.size();i++)
    {
      nd += hpgVB[i].m_visibilities.size()*N;
    }
  return nd;
}
//
// -------------------------------------------------------------------------------
//

// MP: here's the good stuff
template <unsigned N>
std::vector<hpg::VisData<N> >
create_vis_data_vector(const casa::VisBuffer2& casaVB,
		       const double& dphase,
		       const casacore::Vector<casacore::Vector<casacore::Double> >& pointingOffsets,
		       const std::array<unsigned, 2>& vis_cf_index)
{
  unsigned nRow,nPol,nChan;
  IPosition dataShape = casaVB.visCube().shape();
  nRow=dataShape(0);
  nChan=dataShape(1);
  nPol=dataShape(2);
  if (nPol != N) abort();
  
  std::vector<VisData<N> > result;
  result.reserve(nRow*nChan);
  
  for (unsigned r=0;r<nRow;r++)
    {
      if (!(casaVB.flagRow())(r))
	{
	  for (unsigned ch=0;ch<nChan;ch++)
	    {
	      // cerr << "casaVB.flag.shape: " << casaVB.flag().shape() << endl;
	      // if (!(casaVB.flag())(r,ch))
	      {
		std::array<std::complex<hpg::visibility_fp>, N> vis;
		std::array<hpg::vis_weight_fp, N> wt;
		//cerr << "casaVB.flag.shape: " << casaVB.flagCube().shape() << endl;
		for (unsigned p=0; p<N;p++)
		  {
		    if (!(casaVB.flagCube())(p,ch,r))
		      {
			vis[p]=casaVB.visCube()(p,ch,r);
			wt[p]=casaVB.imagingWeight()(r,ch);
		      }
		    else
		      {
			vis[p]=0.0;
			wt[p]=0.0;
		      }
		  }
		
		hpg::vis_frequency_fp    frequency=casaVB.getFrequencies(r)(ch);
		hpg::vis_phase_fp        phase=dphase;
		hpg::vis_uvw_t           uvw = {(hpg::vis_uvw_fp)casaVB.uvw()(0,r),
						(hpg::vis_uvw_fp)casaVB.uvw()(1,r),
						(hpg::vis_uvw_fp)casaVB.uvw()(2,r)};
		unsigned                 grid_cube=0; // For now, grid on the same plane.
		std::array<unsigned, 2>  cf_index=vis_cf_index;
		hpg::cf_phase_gradient_t cf_phase_gradient={(hpg::cf_phase_gradient_fp)pointingOffsets[0][0],
							    (hpg::cf_phase_gradient_fp)pointingOffsets[0][1]};
		
		result.push_back(
				 hpg::VisData<N>(vis,wt,frequency,phase,uvw,grid_cube,cf_index,cf_phase_gradient)
				 );
	      }
	    }
	}
    }
  return result;
}

hpg::VisDataVector HPGVisBuffer(casa::VisBuffer2& casaVB,
				const double& dphase,
				const casacore::Vector<casacore::Vector<casacore::Double> >& pointingOffsets,
				const std::array<unsigned, 2>& vis_cf_index)
{
  unsigned dataNPol = casaVB.visCube().shape()[2];
  switch (dataNPol)
    {
    case 1:
      return hpg::VisDataVector(create_vis_data_vector<1>(casaVB,dphase, pointingOffsets,vis_cf_index));
      break;
    case 2:
      return hpg::VisDataVector(create_vis_data_vector<2>(casaVB,dphase, pointingOffsets,vis_cf_index));
      break;
    case 3:
      return hpg::VisDataVector(create_vis_data_vector<3>(casaVB,dphase, pointingOffsets,vis_cf_index));
      break;
    case 4:
      return hpg::VisDataVector(create_vis_data_vector<4>(casaVB,dphase, pointingOffsets,vis_cf_index));
      break;
    default:
      throw(AipsError("HPGVisBuffer.cc: Incorrect value of dataNPol in HOGVisBuffer()"));
    }
}
#endif
