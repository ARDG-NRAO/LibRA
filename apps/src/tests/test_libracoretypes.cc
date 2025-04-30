#include <filesystem>
#include <gtest/gtest.h>
#include <libracore/Cube.h>
#include <libracore/Matrix.h>
#include <libracore/Vector.h>

using namespace std;


namespace test{
  // set up 
  class LibracoreTypesTest : public ::testing::Test {
  protected:
      libracore::Cube<float> cube1;
      libracore::Cube<float> cube2;

      LibracoreTypesTest()
          : cube1(3, 3, 3), cube2(3, 3, 3) {
          SetUpCubeData(cube1);
          SetUpCubeData(cube2);
      }

      void SetUpCubeData(libracore::Cube<float>& cube) {
          for (size_t z = 0; z < 3; ++z) {
              for (size_t y = 0; y < 3; ++y) {
                  for (size_t x = 0; x < 3; ++x) {
                      cube.getMdspan()(x, y, z) = static_cast<float>(x + y + z); // Sample value
                  }
              }
          }
      }

      ~LibracoreTypesTest() override = default;
  };

  TEST_F(LibracoreTypesTest, CubeConstructorFromSize) {
      // Validate the values in cube1
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cube1.getMdspan()(x, y, z), static_cast<float>(x + y + z));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeCopyConstructor) {
      // Create cube3 using the copy constructor
      libracore::Cube<float> cube3(cube1);

      // Validate the copied values in cube3
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cube3.getMdspan()(x, y, z), static_cast<float>(x + y + z));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeMoveConstructor) {
      // Move cube1 into cube3 (cube1 should be empty or invalid after this)
      libracore::Cube<float> cube3(std::move(cube1));

      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cube3.getMdspan()(x, y, z), static_cast<float>(x + y + z));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeConstructorFromArrayBase) {
      // Initialize cube3 from ArrayBase 
      libracore::ArrayBase<float, 3>& baseRef = cube1; 
      libracore::Cube<float> cube3(baseRef);
      
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cube3.getMdspan()(x, y, z), static_cast<float>(x + y + z));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeOpPlus) {
    auto sumCube = cube1 + cube2;

      // Check if the values are correctly summed
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(sumCube.getMdspan()(x, y, z), static_cast<float>((x + y + z) + (x + y + z)));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeOpAssign) {
    libracore::Cube<float> cubeAssign = cube1;

      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cubeAssign.getMdspan()(x, y, z), static_cast<float>((x + y + z)));
              }
          }
      }
  }

  TEST_F(LibracoreTypesTest, CubeConstructorFromCasacoreCube) {
      casacore::Cube<float> inCube(3, 3, 3);
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  inCube(x, y, z) = static_cast<float>(x + y + z);  
              }
          }
      }

      // Initialize Cube from Casacore Cube
      libracore::Cube<float> cubeFromCasacore(inCube);

      // Validate the values
      for (size_t z = 0; z < 3; ++z) {
          for (size_t y = 0; y < 3; ++y) {
              for (size_t x = 0; x < 3; ++x) {
                  EXPECT_EQ(cubeFromCasacore.getMdspan()(x, y, z), static_cast<float>(x + y + z));
              }
          }
      }

      libracore::Cube<float> cube_move(std::move(inCube)); // Move ownership from inCube to cube
      EXPECT_EQ(cube_move.getMdspan()(1, 1, 1), 3);
      cube_move.getMdspan()(1, 1, 1) = 42; 
      EXPECT_EQ(cube_move.getMdspan()(1, 1, 1), 42);

  }

  TEST_F(LibracoreTypesTest, toCasacoreCube) {
    // Convert to casacore cube
    casacore::Cube<float> casaCube = cube1.toCasacoreCube();

    // Check shape
    ASSERT_EQ(casaCube.shape()(0), 3);
    ASSERT_EQ(casaCube.shape()(1), 3);
    ASSERT_EQ(casaCube.shape()(2), 3);

    // Verify data values match
    for (size_t z = 0; z < 3; ++z) {
        for (size_t y = 0; y < 3; ++y) {
            for (size_t x = 0; x < 3; ++x) {
                ASSERT_FLOAT_EQ(casaCube(x, y, z), static_cast<float>(x + y + z));
            }
        }
    }

  }


  TEST_F(LibracoreTypesTest, CubeMoreOps)
{
  casacore::Cube<float> inCube(3, 3, 3);
  casacore::Cube<float> inCube2(3, 3, 3);

  for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
          for (int k = 0; k < 3; ++k) {
              inCube(i, j, k) = static_cast<float>(i * 9 + j * 3 + k);
              inCube2(i, j, k) = static_cast<float>(i * 18 + j * 6 + k);
          }
      }
  }

  // Wrap the casacore::Cube with libracore::Cube
  libracore::Cube<float> cubeA(inCube);
  libracore::Cube<float> cubeB(inCube2);  

  // test addition
  auto minusCube = libracore::Cube<float>(cubeA - cubeB);
  // test element-wise multiplication
  auto mulCube = cubeA * cubeB;

  EXPECT_EQ(minusCube.getMdspan()(1,1,2), -12);
  EXPECT_EQ(mulCube.getMdspan()(1,1,2), 364);
  
}

TEST_F(LibracoreTypesTest, Resize) {
    libracore::Cube<float> cube;
    // Initial resize to 2x3x4
    array<size_t, 3> shape = {2, 3, 4};
    cube.resize(shape);

    // Verify shape is correctly set
    EXPECT_EQ(cube.getShape()[0], 2);
    EXPECT_EQ(cube.getShape()[1], 3);
    EXPECT_EQ(cube.getShape()[2], 4);

    cube.getMdspan()(1, 2, 3) = 42.0f;
    EXPECT_FLOAT_EQ(cube.getMdspan()(1, 2, 3), 42.0f);

    // Resize to new shape
    std::array<size_t, 3> newShape = {4, 2, 2};
    cube.resize(newShape);
    EXPECT_EQ(cube.getShape()[0], 4);
    EXPECT_EQ(cube.getShape()[1], 2);
    EXPECT_EQ(cube.getShape()[2], 2);

    cube.getMdspan()(3, 1, 1) = 3.14f;
    EXPECT_FLOAT_EQ(cube.getMdspan()(3, 1, 1), 3.14f);
}

TEST_F(LibracoreTypesTest, MatrixOps)
{
  // Test libracore::Matrix
    casacore::Matrix<float> inMatrix(3, 3);
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
              inMatrix(i, j) = static_cast<float>(i * 3 + j);
        }
    }

    // Create a libracore::Matrix from the casacore::Matrix
    libracore::Matrix<float> matrix1(inMatrix);
    libracore::Matrix<float> matrix2 = matrix1;

    auto Mmdspan1 = matrix1.getMdspan();
    auto Mmdspan2 = matrix2.getMdspan();

    EXPECT_EQ(Mmdspan1(1,1), 4);
    EXPECT_EQ(Mmdspan2(1,1), 4);
    
}

TEST_F(LibracoreTypesTest, VectorOps)
{
  // Test for libracore::Vector
    casacore::Vector<float> inVec1 = {1.0f, 2.0f, 3.0f};

    libracore::Vector<float> vec1(inVec1);  // Create Vector from casacore::Vector
    libracore::Vector<float> vec2 = vec1;                          

    libracore::Vector<float> vec3 = vec1 + vec2;  // Use the addition operator
    auto Vmdspan = vec3.getMdspan();

    EXPECT_EQ(Vmdspan(1), 4);
    
}



}