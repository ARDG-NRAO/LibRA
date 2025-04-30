#include <iostream>
#include <Cube.h>
#include <Matrix.h>
#include <Vector.h>

int main() {
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

    std::cout << "Original Cube (inCube):\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                std::cout << inCube(i, j, k) << "-" << inCube2(i, j, k) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    //libracore::testingCube();
    
    libracore::Cube<float> cube(3,3,3);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                cube.getMdspan()(i,j,k) = static_cast<float>(i * 9 + j * 3 + k);
                std::cout << cube.getMdspan()(i, j, k) << " ";
            }
        }
    }
    std::cout << "\n";

    // Wrap the casacore::Cube with libracore::Cube
    libracore::Cube<float> cube1(inCube);
    libracore::Cube<float> cube2(inCube2);  

    // test getMdspan of cube1 directly
    auto mdspan = cube1.getMdspan();
    auto mdspan2 = cube2.getMdspan();

    std::cout << "Direct access to mdspan from inCubes:\n";
    for (int i = 0; i < mdspan2.extent(0); ++i) {
        for (int j = 0; j < mdspan2.extent(1); ++j) {
            for (int k = 0; k < mdspan2.extent(2); ++k) {
                std::cout << mdspan(i, j, k) << "-" << mdspan2(i, j, k) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }


    // test addition
    libracore::Cube<float> sumCube = cube1 + cube2;
    std::cout << "Result of cube1 + cube2:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                std::cout << sumCube.getMdspan()(i, j, k) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // test element-wise multiplication
    libracore::Cube<float> mulCube = cube1 * cube2;
    std::cout << "Result of cube1 * cube2:\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                std::cout << mulCube.getMdspan()(i, j, k) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }


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

    std::cout << "matrix2 after assignment from matrix1:" << std::endl;
    for (int i = 0; i < Mmdspan1.extent(0); ++i) {
        for (int j = 0; j < Mmdspan1.extent(1); ++j) {
            std::cout << Mmdspan1(i, j) << "->" << Mmdspan2(i, j) << " ";
        }
        std::cout << std::endl;
    }


    // Test for libracore::Vector
    casacore::Vector<float> inVec1 = {1.0f, 2.0f, 3.0f};
    casacore::Vector<float> inVec2 = {4.0f, 5.0f, 6.0f};

    libracore::Vector<float> vec1(inVec1);  // Create Vector from casacore::Vector
    libracore::Vector<float> vec2 = vec1;  

    libracore::Vector<float> vec3 = vec1 + vec2;  // Use the addition operator

    if (vec1 == vec2) {
        std::cout << "vec1 and vec2 are equal." << std::endl;
    } else {
        std::cout << "vec1 and vec2 are not equal." << std::endl;
    }

    auto Vmdspan = vec3.getMdspan();
    std::cout << "The sum of the two vectors are:" << std::endl;
    for (int i = 0; i < Vmdspan.extent(0); ++i) {
            std::cout << Vmdspan(i) << " ";
    }
    std::cout << std::endl;


    // temp test
    
    // Assuming the Cube has been initialized with move semantics
    //casacore::Cube<T> inCube;  // A casacore Cube object.
libracore::Cube<float> cube_move(std::move(inCube)); // Move ownership from inCube to cube

// Now modify the data using mdspan
size_t i = 1, j = 1, k = 1;
std::cout << "cube_move " << cube_move.getMdspan()(i,j,k) << std::endl;
cube_move.getMdspan()(i, j, k) = 42; // Change the value at position (i, j, k)
std::cout << "after: cube_move " << cube_move.getMdspan()(i,j,k) << std::endl;


    /*

    // Create a casacore::Cube (example data)
    casacore::Cube<int> inCube(3, 3, 3);
    inCube(0, 0, 0) = 1;
    inCube(1, 1, 1) = 2;
    inCube(2, 2, 2) = 3;

    // Create libracore::Cube using move constructor with shared memory
    libracore::Cube<int> cube_shared(std::move(inCube), true);  // Share ownership of the memory

    // Create libracore::Cube using move constructor with unique memory (no deep copy)
    libracore::Cube<int> cube_unique(std::move(inCube), false);  // Use the memory of inCube directly (without allocation)

    return 0;

    */

    return 0;
}
