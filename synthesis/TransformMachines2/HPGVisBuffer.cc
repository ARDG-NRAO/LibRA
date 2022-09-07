#ifdef USE_HPG
#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines2/VBStore.h>
//#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines2/FTMachine.h>

//#include <hpg/hpg.hpp>
//#include <hpg/hpg_indexing.hpp>
#include "/export/home/gpuhost001/mpokorny/casa_hpg_debug/include/hpg/hpg.hpp"
#include "/export/home/gpuhost001/mpokorny/casa_hpg_debug/include/hpg/hpg_indexing.hpp"
using namespace hpg;
//
// -------------------------------------------------------------------------------
//
template <unsigned N>
std::vector<hpg::VisData<N>>
create_blank_vis_data_vector(const unsigned nPol, const unsigned nChan, const unsigned nRow)
{
  assert(nPol == N);
  std::vector<VisData<N> > hpgVisBuffer;
  //hpgVisBuffer.reserve(nRow*nChan);
  hpgVisBuffer.resize(nRow*nChan);
  return hpgVisBuffer;
}
//
// -------------------------------------------------------------------------------
//
static int spwCache=-1;
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
		 const hpg::CFSimpleIndexer& cfsi
		 )
{
  unsigned nVisChan=endChan - startChan + 1;
  unsigned nVisRow=endRow - startRow + 1;
  unsigned targetIMChan, targetIMPol;
  const casa::VisBuffer2& casaVB = *(casaVBS.vb_p);
  IPosition dataShape = casaVB.visCube().shape();

  unsigned nDataPol=dataShape(0);
  std::vector<hpg::VisData<N>> hpgVB=create_blank_vis_data_vector<N>(nVisPol,nVisChan,nVisRow);
  std::array<std::complex<hpg::visibility_fp>, N> vis;
  std::array<hpg::vis_weight_fp, N> wt;

  
  const casacore::Matrix<double> UVW=casaVBS.uvw_p;

  std::array<hpg::vis_uvw_fp, 3> hpgUVW;
  hpg::vis_phase_fp              dphaseHPG;
  unsigned                       grid_cube=0; // For now, grid on the same plane.
  hpg::cf_phase_gradient_t       cf_phase_gradient;

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
  //
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
      //if(!casaVB.flagRow()(irow))
      if(!casaVBS.rowFlag_p(irow) 
	 //&& ((fabs(UVW(0,irow)-(-1674.1299178716)) < 1e-10) && ((fabs(UVW(1,irow)-(-784.6013851680)) < 1e-10)))
	 )
	{

	  CountedPtr<casa::refim::CFBuffer> cfb = (*vb2CFBMap)[irow];
	  for(unsigned ichan=startChan; ichan< endChan; ichan++)
	    {

	      if (casaVBS.imagingWeight_p(ichan,irow)!=0.0) 
		//if (!casaVBS.flagCube_p(0,ichan,irow))
		{  
		  targetIMChan=chanMap[ichan];

		  if(((targetIMChan>=0) && (targetIMChan<nGridChan))) 
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

unsigned mRow=0;
		      for(unsigned ipol=0; ipol< nDataPol; ipol++) 
			{
			  //			  if(!(casaVB.flagCube())(ipol,ichan,irow))
			    {  
			      targetIMPol=polMap(ipol);
			      
			      if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
				{
				  for (uInt mc=0; mc<visElements[mRow].nelements(); mc++) 
				    { 
				      int visVecElement=visElements[mRow][mc];
				      
				      if (casaVBS.vb_p->flagCube()(ipol,ichan,irow)==false)
					{
					  if ((casaVBS.ftmType_p==casa::refim::FTMachine::WEIGHT)||
					      (casaVBS.ftmType_p==casa::refim::FTMachine::PSF))
					    {
					      vis[visVecElement] = std::complex<double>(1.0,0.0);
					    }
					  else
					    {
					      vis[visVecElement] = casaVBS.visCube_p(ipol,ichan,irow);
					    }

					  wt[visVecElement]=casaVBS.imagingWeight_p(ichan,irow);
					}
				      else
					{
					  vis[visVecElement] = 0.0;
					  wt[visVecElement]  = 0.0;
					}
				    }
				  mRow++;
				  sumwt(targetIMPol,targetIMChan) += casaVB.imagingWeight()(ichan, irow);//*abs(norm);
				}
			    }
			} // End poln-loop

		      hpg::vis_frequency_fp frequency=freq[ichan];
		      cf_phase_gradient={(hpg::cf_phase_gradient_fp)pointingOffsets[0][0],
					 (hpg::cf_phase_gradient_fp)pointingOffsets[0][1]};

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
		      unsigned dummyPolNdx=1;
		      hpg::CFCellIndex cfCellNdx(0,0, cfWNdx, cfFreqNdx, dummyPolNdx);
		      // full_hpgcf_index[0] ==> pol
		      // full_hpgcf_index[1] ==> cube : should be 0
		      // full_hpgcf_index[2] ==> group : linearized index for the rest of the coordinates (BLType, PA, W, SPW).
		      std::array<unsigned, 3> full_hpgcf_index=cfsi.cf_index(cfCellNdx);
		      auto& cf_cube_index=full_hpgcf_index[1];
		      auto& cf_group_index=full_hpgcf_index[2];
		      
		      cf_index = {cf_cube_index,cf_group_index};

		      //-------------------------------------------------------------------------------
		      // SB: Debugging code.  Can be removed if deemed unnecessary.
		      //
		      // cerr.precision(17);
		      // if (vbSpw != spwCache)
		      // 	{
		      // 	  spwCache=vbSpw;
		      // 	  cerr << "CFC index(w,f,p), spw: " << cfWNdx << " " << cfFreqNdx << " " << dummyPolNdx << " " << vbSpw << endl;
		      // 	  cerr << "cf_index "; for(auto v:cf_index) cerr << v << " ";cerr << endl;
		      // 	  cerr << "Vis: ";     for(auto v:vis) cerr << v << " "; cerr << endl;
		      // 	  cerr << "full_hpgcf_index(p?,c,g):  "; for(auto v:full_hpgcf_index) cerr << v << " "; cerr << endl;
		      // 	}

		      // cerr << "UVW: " << irow << " " << (double)UVW(0,irow) << " " << UVW(1,irow) << " " << UVW(2,irow) << " " << cfWNdx << " " << cfFreqNdx << " " << vbSpw << " " 
		      // 	   << vis[0]*std::complex<float>(cos(dphaseHPG),sin(dphaseHPG)) << " " << vis[1]*std::complex<float>(cos(dphaseHPG),sin(dphaseHPG)) << " " << wt[1] << " " << cfWNdx << " " << cfFreqNdx << " " << vbSpw << " " << dphaseHPG << " ";
		      // for(auto w:wt) cerr << w << " ";
		      // cerr << endl;
		      //-------------------------------------------------------------------------------
		      
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
//
// template <unsigned N>
// unsigned long nVis(const std::vector<hpg::VisData<N>>& hpgVB)
// {
//   unsigned long nd=0;
//   for(unsigned i=0;i<hpgVB.size();i++)
//     {
//       nd += hpgVB[i].m_visibilities.size()*N;
//     }
//   return nd;
// }
//
// -------------------------------------------------------------------------------
//

// MP: here's the good stuff
// template <unsigned N>
// std::vector<hpg::VisData<N> >
// create_vis_data_vector(const casa::VisBuffer2& casaVB,
// 		       const double& dphase,
// 		       const casacore::Vector<casacore::Vector<casacore::Double> >& pointingOffsets,
// 		       const std::array<unsigned, 2>& vis_cf_index)
// {
//   unsigned nRow,nPol,nChan;
//   IPosition dataShape = casaVB.visCube().shape();
//   nRow=dataShape(0);
//   nChan=dataShape(1);
//   nPol=dataShape(2);
//   if (nPol != N) abort();
  
//   std::vector<VisData<N> > result;
//   result.reserve(nRow*nChan);
  
//   for (unsigned r=0;r<nRow;r++)
//     {
//       if (!(casaVB.flagRow())(r))
// 	{
// 	  for (unsigned ch=0;ch<nChan;ch++)
// 	    {
// 	      {
// 		std::array<std::complex<hpg::visibility_fp>, N> vis;
// 		std::array<hpg::vis_weight_fp, N> wt;

// 		for (unsigned p=0; p<N;p++)
// 		  {
// 		    if (!(casaVB.flagCube())(p,ch,r))
// 		      {
// 			vis[p]=casaVB.visCube()(p,ch,r);
// 			wt[p]=casaVB.imagingWeight()(r,ch);
// 		      }
// 		    else
// 		      {
// 			vis[p]=0.0;
// 			wt[p]=0.0;
// 		      }
// 		  }
		
// 		hpg::vis_frequency_fp    frequency=casaVB.getFrequencies(r)(ch);
// 		hpg::vis_phase_fp        phase=dphase;
// 		hpg::vis_uvw_t           uvw = {(hpg::vis_uvw_fp)casaVB.uvw()(0,r),
// 						(hpg::vis_uvw_fp)casaVB.uvw()(1,r),
// 						(hpg::vis_uvw_fp)casaVB.uvw()(2,r)};
// 		unsigned                 grid_cube=0; // For now, grid on the same plane.
// 		std::array<unsigned, 2>  cf_index=vis_cf_index;
// 		hpg::cf_phase_gradient_t cf_phase_gradient={(hpg::cf_phase_gradient_fp)pointingOffsets[0][0],
// 							    (hpg::cf_phase_gradient_fp)pointingOffsets[0][1]};
		
// 		result.push_back(
// 				 hpg::VisData<N>(vis,wt,frequency,phase,uvw,grid_cube,cf_index,cf_phase_gradient)
// 				 );
// 	      }
// 	    }
// 	}
//     }
//   return result;
// }

// hpg::VisDataVector HPGVisBuffer(casa::VisBuffer2& casaVB,
// 				const double& dphase,
// 				const casacore::Vector<casacore::Vector<casacore::Double> >& pointingOffsets,
// 				const std::array<unsigned, 2>& vis_cf_index)
// {
//   // MP: the following switch statement may be all that you need in
//   // this function, depending on what you're intending for it to
//   // do. It works in combination with the template function
//   // create_vis_data_vector<N>() above
  
//   unsigned dataNPol = casaVB.visCube().shape()[2];
//   switch (dataNPol)
//     {
//     case 1:
//       // MP: since the HPGVisBuffer function has no template
//       // parameter, you must use the hpg::VisDataVector wrapper type
//       // around the values returned from
//       // create_vis_data_vector(). If you don't want that, you
//       // could call the create_vis_data_vector() function directly.
//       return hpg::VisDataVector(create_vis_data_vector<1>(casaVB,dphase, pointingOffsets,vis_cf_index));
//       break;
//     case 2:
//       return hpg::VisDataVector(create_vis_data_vector<2>(casaVB,dphase, pointingOffsets,vis_cf_index));
//       break;
//     case 3:
//       return hpg::VisDataVector(create_vis_data_vector<3>(casaVB,dphase, pointingOffsets,vis_cf_index));
//       break;
//     case 4:
//       return hpg::VisDataVector(create_vis_data_vector<4>(casaVB,dphase, pointingOffsets,vis_cf_index));
//       break;
//     default:
//       throw(AipsError("HPGVisBuffer.cc: Incorrect value of dataNPol in HOGVisBuffer()"));
//     }
// }

#endif //USE_HPG
