#ifndef LIBRACORE_CUBE_H
#define LIBRACORE_CUBE_H

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Arrays/Cube.h>
#include <cassert>

#include <libracore/LibracoreTypes.h>
#include <libracore/imageInterface.h>
#include <libracore/ArrayBase.h>

using namespace casacore;

namespace libracore {

    template<typename T>
    class Cube : public ArrayBase<T, 3>{
    public:
        Cube() = default;
        // Initializes a Cube from dimensions
        Cube(size_t nx, size_t ny, size_t nz);  
        // Copy constructor; new memory allocation and deep copy
        Cube(const Cube<T>& other);  
        // Move constructor; no new memory allocation
        Cube(Cube<T>&& other) noexcept;  
        // Constructor that initializes from casacore::Cube
        Cube(const casacore::Cube<T>& inCube);
        // Conversion constructor; no new memory allocation 
        Cube(casacore::Cube<T>&& inCube) noexcept    
        : ArrayBase<T, 3>({static_cast<size_t>(inCube.shape()(0)), static_cast<size_t>(inCube.shape()(1)), static_cast<size_t>(inCube.shape()(2))})  
        {
            bool deleteit;
            this->m_data = std::shared_ptr<T[]>(inCube.getStorage(deleteit), [](T* ptr) { /* Custom deleter if necessary */ });
            this->m_mdspan = col_major_mdspan_3d<T>(this->m_data.get(), inCube.shape()(0), inCube.shape()(1), inCube.shape()(2));
            // Copy the shape
            this->m_shape[0] = inCube.shape()(0);
            this->m_shape[1] = inCube.shape()(1);
            this->m_shape[2] = inCube.shape()(2);
        }

        Cube(const ArrayBase<T, 3>& other)
        : ArrayBase<T, 3>(other) {}

        Cube<T>& operator=(const Cube<T>& other);       // Copy assignment
        Cube<T>& operator=(Cube<T>&& other) noexcept;   // Move assignment
 
        casacore::Cube<T> toCasacoreCube() const; 

    private:
       
    };

    
    // basic constructor
    template<typename T>
    Cube<T>::Cube(size_t nx, size_t ny, size_t nz)
        : ArrayBase<T, 3>({nx, ny, nz}) {}

    // copy constructor, deep copy
    template<typename T>
    Cube<T>::Cube(const Cube<T>& other)
        : ArrayBase<T, 3>(other.m_shape) {

        this->m_shape = other.m_shape;
        this->allocateMemory();
        this->initializeMdspan();

        for (size_t z = 0; z < this->m_shape[2]; ++z) {
            for (size_t y = 0; y < this->m_shape[1]; ++y) {
                for (size_t x = 0; x < this->m_shape[0]; ++x) {
                    this->m_mdspan(x, y, z) = other.m_mdspan(x, y, z);  
                }
            }
        }
    }

    // move constructor
    template<typename T>
    Cube<T>::Cube(Cube<T>&& other) noexcept
        : ArrayBase<T, 3>(std::move(other)){}


    template<typename T>
    Cube<T>::Cube(const casacore::Cube<T>& inCube) 
     : ArrayBase<T, 3>({static_cast<size_t>(inCube.shape()(0)), static_cast<size_t>(inCube.shape()(1)), static_cast<size_t>(inCube.shape()(2))})  
    {
        this->m_shape[0] = inCube.shape()(0);
        this->m_shape[1] = inCube.shape()(1);
        this->m_shape[2] = inCube.shape()(2);
            
        this->allocateMemory();
        this->initializeMdspan();
       
        for (std::size_t z = 0; z < this->m_shape[2] ; ++z) {
          for (std::size_t y = 0; y < this->m_shape[1] ; ++y) {
            for (std::size_t x = 0; x < this->m_shape[0] ; ++x) {
                this->m_mdspan(x, y, z) = inCube(x, y, z);
            }
          }
        }
    }

    template<typename T>
    Cube<T>& Cube<T>::operator=(const Cube<T>& other) {
        if (this != &other) {
            ArrayBase<T, 3>::operator=(other);  
        }
        return *this;
    }

    template<typename T>
    Cube<T>& Cube<T>::operator=(Cube<T>&& other) noexcept {
        if (this != &other) {
            ArrayBase<T, 3>::operator=(std::move(other));
        }
        return *this;
    }

    template<typename T>
    casacore::Cube<T> Cube<T>::toCasacoreCube() const {
        casacore::IPosition shape(3, this->m_shape[0], this->m_shape[1], this->m_shape[2]);
        casacore::Cube<T> outCube(shape);

        // Deep copy data
        for (size_t k = 0; k < this->m_shape[2]; ++k) {
            for (size_t j = 0; j < this->m_shape[1]; ++j) {
                for (size_t i = 0; i < this->m_shape[0]; ++i) {
                    outCube(i, j, k) = this->m_mdspan(i, j, k);
                }
            }
        }

        return outCube;
    }



}

#endif // LIBRACORE_CUBE_H
