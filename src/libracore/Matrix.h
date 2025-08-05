#ifndef LIBRACORE_MATRIX_H
#define LIBRACORE_MATRIX_H

#include <mdspan/mdspan.hpp>
#include <casacore/casa/Arrays/Matrix.h>
#include <cassert>

#include <libracore/LibracoreTypes.h>
#include <libracore/imageInterface.h>
#include <libracore/ArrayBase.h>

using namespace casacore;

namespace libracore {

    template<typename T>
    class Matrix : public ArrayBase<T, 2>{
    public:
        Matrix() = default;
        // Constructor that initializes from matrix size (nx, ny)
        Matrix(size_t nx, size_t ny);
        // Copy constructor; new memory allocation and deep copy
        Matrix(const Matrix<T>& other);  
        // Move constructor; no new memory allocation
        Matrix(Matrix<T>&& other) noexcept;  
        // Constructor that initializes from casacore::Matrix
        Matrix(const casacore::Matrix<T>& inMatrix);
        // Conversion constructor; no new memory allocation 
        Matrix(casacore::Matrix<T>&& inMatrix) noexcept    
        : ArrayBase<T, 2>({static_cast<size_t>(inMatrix.shape()(0)), static_cast<size_t>(inMatrix.shape()(1))})  
        {
            bool deleteit;
            this->m_data = std::shared_ptr<T[]>(inMatrix.getStorage(deleteit), [](T* ptr) { /* Custom deleter if necessary */ });
            this->m_mdspan = col_major_mdspan<T>(this->m_data.get(), inMatrix.shape()(0), inMatrix.shape()(1));
            // Copy the shape
            this->m_shape[0] = inMatrix.shape()(0);
            this->m_shape[1] = inMatrix.shape()(1);
        }

        Matrix(const ArrayBase<T, 2>& other)
        : ArrayBase<T, 2>(other) {}
 
        // the following is needed b/c C++ rule of five, 
        // if you declare a move constructor or move assignment 
        // operator, the compiler will implicitly delete the copy 
        // assignment operator unless you explicitly declare it.
        Matrix<T>& operator=(const Matrix<T>& other);       // Copy assignment
        Matrix<T>& operator=(Matrix<T>&& other) noexcept;   // Move assignment

        casacore::Matrix<T> toCasacoreMatrix() const;

    private:
        
    };

    // Basic constructor for 2D Matrix
    template<typename T>
    Matrix<T>::Matrix(size_t nx, size_t ny)
        : ArrayBase<T, 2>({nx, ny}) {}

    // Copy constructor for 2D Matrix (deep copy)
    template<typename T>
    Matrix<T>::Matrix(const Matrix<T>& other)
        : ArrayBase<T, 2>(other.m_shape) {

        this->m_shape = other.m_shape;
        this->allocateMemory();
        this->initializeMdspan();

        for (size_t y = 0; y < this->m_shape[1]; ++y) {
            for (size_t x = 0; x < this->m_shape[0]; ++x) {
                this->m_mdspan(x, y) = other.m_mdspan(x, y);
            }
        }
    }

    // Move constructor for 2D Matrix (no new memory allocation)
    template<typename T>
    Matrix<T>::Matrix(Matrix<T>&& other) noexcept
        : ArrayBase<T, 2>(std::move(other)) {}


    template<typename T>
    Matrix<T>::Matrix(const casacore::Matrix<T>& inMatrix) 
    : ArrayBase<T, 2>({static_cast<size_t>(inMatrix.shape()(0)), static_cast<size_t>(inMatrix.shape()(1))})  
    {
        this->m_shape[0] = inMatrix.shape()(0);  
        this->m_shape[1] = inMatrix.shape()(1);  

        this->allocateMemory();
        this->initializeMdspan(); 
        
        for (std::size_t j = 0; j < this->m_shape[1]; ++j) { 
            for (std::size_t i = 0; i < this->m_shape[0]; ++i) { 
                this->m_mdspan(i, j) = inMatrix(i, j);
            }
        }
    }

    template<typename T>
    Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
        if (this != &other) {
            ArrayBase<T, 2>::operator=(other);  
        }
        return *this;
    }

    template<typename T>
    Matrix<T>& Matrix<T>::operator=(Matrix<T>&& other) noexcept {
        if (this != &other) {
            ArrayBase<T, 2>::operator=(std::move(other));
        }
        return *this;
    }

    template<typename T>
    casacore::Matrix<T> Matrix<T>::toCasacoreMatrix() const {
        casacore::IPosition shape(2, this->m_shape[0], this->m_shape[1]);
        casacore::Matrix<T> outMatrix(shape);

        // Deep copy data
        for (size_t j = 0; j < this->m_shape[1]; ++j) {
            for (size_t i = 0; i < this->m_shape[0]; ++i) {
                outMatrix(i, j) = this->m_mdspan(i, j);
            }
        }       

        return outMatrix;
    }

    
} // namespace libracore


#endif // LIBRACORE_MATRIX_H
