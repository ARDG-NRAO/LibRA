#include <filesystem>
#include "Asp/asp.h"
#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;


namespace test{
TEST(AspTest, FuncLevel) {
  string specmode="cube";
  float largestscale = -1;
  float fusedthreshold = 0;
  int nterms=2;
  float gain=0.1;
  float threshold=1e-4;
  float nsigma=1.5;
  int cycleniter=10;
  float cyclefactor=1.0;
  float psfwidth = 5.0;
  float nsigmathreshold = 1;

  float psf[2][5] =
    {
        {1,8,12,20,25},
        {5,9,13,24,26}
    };

  float model[2][5] = {0};
  float residual[2][5] = {
        {1,8,12,-20,-25},
        {5,9,-13,24,26}
    };
  float mask[2][5] = {
        {1,8,0,20,25},
        {5,9,13,24,0}
    };

  Asp<float>(model, psf, residual,
       mask,
       psfwidth,
       largestscale, fusedthreshold,
       nterms,
       gain, 
       threshold, nsigmathreshold,
       nsigma,
       cycleniter, cyclefactor,
       specmode
       );
  hello();

  EXPECT_EQ(psf[1][0],5.0);
  EXPECT_EQ(residual[0][3],-20.0);
  

}

};
