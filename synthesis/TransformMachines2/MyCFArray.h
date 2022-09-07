//#include <hpg/hpg.hpp>
#include "/export/home/gpuhost001/mpokorny/casa_hpg_debug/include/hpg/hpg.hpp"

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  struct MyCFArray final: public hpg::CFArray
  {
    std::vector<std::array<unsigned, 4>> m_extent;
    std::vector<std::vector<std::complex<hpg::cf_fp>>> m_values;
    unsigned cf_oversampling=0;
    
    MyCFArray(): m_extent(), m_values(), cf_oversampling(0) {};

    void setSize(const unsigned& nCFs, const unsigned& oversampling)
    {
      cf_oversampling=oversampling;
      m_extent.resize(nCFs);
      m_values.resize(nCFs);

      // m_extent[0] = 2*(cfX+2) * cf_oversampling + 1;
      // m_extent[1] = 2*(cfY+2) * cf_oversampling + 1;
      // m_extent[2] = cfNPol;
      // m_extent[3] = cfNChan;
      // m_values.resize(m_extent[0]*m_extent[1]*m_extent[2]*m_extent[3]);
      // cerr << "CFArray size: " << m_extent[0] << " " << m_extent[1] << " " << m_extent[2] << " " << m_extent[3] << endl;
    }

    void resize(const unsigned& i, const unsigned& nx,const unsigned& ny,const unsigned& sto=2, const unsigned& cube=1)
    {
      // unsigned X=2*(nx+2) * cf_oversampling + 1,
      // 	Y=2*(ny+2) * cf_oversampling + 1;
      m_extent[i][0] = nx;
      m_extent[i][1] = ny;
      m_extent[i][2] = sto;
      m_extent[i][3] = cube;
      m_values[i].resize(nx*ny*sto*cube);
    }
    unsigned num_groups() const override
    {
      return static_cast<unsigned>(m_extent.size());
    }

    void setValues(const casacore::Complex* values, const unsigned igrp,const unsigned nx, const unsigned ny, const unsigned polNdx, const unsigned freqNdx)
    {
      //      unsigned padding=2*oversampling();
      auto& pix = m_values[igrp];
      auto& ext = m_extent[igrp];
      // for (unsinged i=0;i<ext[0]; i++)
      // 	for (unsinged j=0; j<ext[1]; j++)
      //	  pix[((i * ext[1] + j) * ext[2] + polNdx) * ext[3]+ freqNdx]

      // nx and ny are now without padding.  They are
      // cfSize-4*Oversampling.
      //
      // unsigned k=2*oversampling();
      // k skips the 2-pixel oversampled padding in the beginning.
      // The loops below will skip the 2-pixel oversampled padding in
      // the end.
      // for (unsigned i=padding;i<nx-padding; i++)
      // 	for (unsigned j=padding; j<ny-padding; j++)

      //      Complex max=Complex(0.0);
      
      unsigned k=0;
      for (unsigned j=0; j<ny; j++)
      	for (unsigned i=0;i<nx; i++)
      	  {
	    //	    if (abs(max) < abs(values[k])) max=values[k];
      	    pix[((i * ny + j) * ext[2] + polNdx) * ext[3]+ freqNdx]
      	      =values[k++];
      	  }
      //cerr << "Max CF = " << max << endl;
    }

    unsigned oversampling() const override
    {
      return cf_oversampling;
    }

    std::array<unsigned, 4> extents(unsigned i) const override
    {
      return m_extent[i];
    }

    inline std::complex<hpg::cf_fp> operator()(unsigned x, unsigned y, unsigned pol, unsigned freq, unsigned i) const override
    {
      {
	auto& pix = m_values[i];
	auto& ext = m_extent[i];
	return pix[((x * ext[1] + y) * ext[2] + pol) * ext[3]+ freq];
      }
    }
  };
  }
}
