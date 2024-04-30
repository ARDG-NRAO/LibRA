//#include <mdspan.hpp>
#include <iostream>
#include <imageInterface.h>
//#include <casacore/casa/Arrays/Matrix.h>


using namespace std;
using namespace casacore;
using namespace libracore;
//using namespace Kokkos::mdspan;


int main() {
  std::array d{
    0, 5, 1,
    3, 8, 4,
    2, 7, 6,
  };

  constexpr int n_rows = 2;
  constexpr int n_cols = 5;
  int data_row_major[] = {
       1,   2,   3,  /*|*/  4,   5,  /* X | */
       6,   7,   8,  /*|*/  9,  10   /* X | */
     /*X,   X,   X,    |    X    X      X |
      *------------------------------------ */
  };

  auto m = col_major_mdspan<int>(data_row_major, n_rows, n_cols);

  // works in gcc 12.1
  //Kokkos::mdspan m{d.data(), Kokkos::extents{3,3}};
  //static_assert(m.rank()==2,"Rank is two");
  
  Matrix<int> matrix(n_rows, n_cols, 0);
  mdspan2casamatrix<int>(m, matrix);
  testing();
  for (std::size_t j = 0; j < m.extent(1); ++j)
    for (std::size_t i = 0; i < m.extent(0); ++i)
    #if MDSPAN_USE_BRACKET_OPERATOR
      std::cout << "m[" << i << ", " << j << "] == " << m[i, j] << " " << matrix(i,j) << " Using []\n";
    #else
      std::cout << "m(" << i << ", " << j << ") == " << m(i, j) << " " << matrix(i,j) << " Using ()\n";
    #endif

  
    auto data_b = std::make_unique<int[]>(n_rows * n_cols);
    auto md2 = col_major_mdspan<int>(data_b.get(), n_rows, n_cols);

    casamatrix2mdspan<int>(matrix, md2);
    for (std::size_t j = 0; j < md2.extent(1); ++j)
      for (std::size_t i = 0; i < md2.extent(0); ++i)
      #if MDSPAN_USE_BRACKET_OPERATOR
        std::cout << "md2[" << i << ", " << j << "] == " << md2[i, j] << " " << matrix(i,j)  << " Using []\n";
      #else
        std::cout << "md2(" << i << ", " << j << ") == " << md2(i, j) << " " << matrix(i,j) << " Using ()\n";
      #endif

 
 return 0;
}
