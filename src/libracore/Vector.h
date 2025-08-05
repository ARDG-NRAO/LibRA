#ifndef LIBRACORE_VECTOR_H
#define LIBRACORE_VECTOR_H

#include <mdspan/mdspan.hpp>
#include <casacore/casa/Arrays/Vector.h>
#include <cassert>

#include <libracore/LibracoreTypes.h>
#include <libracore/imageInterface.h>
#include <libracore/ArrayBase.h>

using namespace casacore;

namespace libracore {

    template<typename T>
    class Vector : public ArrayBase<T, 1>{
    public:
        Vector() = default;
        // Constructor that initializes from vector size (nx)
        Vector(size_t nx);  
        // Copy constructor; new memory allocation and deep copy
        Vector(const Vector<T>& other);  
        // Move constructor; no new memory allocation
        Vector(Vector<T>&& other) noexcept;  
         // Constructor that initializes from casacore::Vector
        Vector(const casacore::Vector<T>& inVector);
        // Conversion constructor; no new memory allocation 
        Vector(casacore::Vector<T>&& inVector) noexcept    
        : ArrayBase<T, 1>({static_cast<size_t>(inVector.shape()(0))})  
        {
            bool deleteit;
            this->m_data = std::shared_ptr<T[]>(inVector.getStorage(deleteit), [](T* ptr) { /* Custom deleter if necessary */ });
            this->m_mdspan = col_major_mdspan_1d<T>(this->m_data.get(), inVector.shape()(0));
            this->m_shape[0] = inVector.shape()(0);
        }

        Vector(const ArrayBase<T, 1>& other)
        : ArrayBase<T, 1>(other) {}

        Vector<T>& operator=(const Vector<T>& other);       // Copy assignment
        Vector<T>& operator=(Vector<T>&& other) noexcept;   // Move assignment

        casacore::Vector<T> toCasacoreVector() const;

    private:

    };

 
    // Basic constructor for 1D Vector
    template<typename T>
    Vector<T>::Vector(size_t nx)
        : ArrayBase<T, 1>({nx}) {}

    // Copy constructor for 1D Vector (deep copy)
    template<typename T>
    Vector<T>::Vector(const Vector<T>& other)
        : ArrayBase<T, 1>(other.m_shape) {

        this->m_shape = other.m_shape;
        this->allocateMemory();
        this->initializeMdspan();
       
        for (size_t x = 0; x < this->m_shape[0]; ++x) {
            this->m_mdspan(x) = other.m_mdspan(x);
        }
        
    }

    // Move constructor for 1D Vector (no new memory allocation)
    template<typename T>
    Vector<T>::Vector(Vector<T>&& other) noexcept
        : ArrayBase<T, 1>(std::move(other)) {}


    template<typename T>
    Vector<T>::Vector(const casacore::Vector<T>& inVector) 
    : ArrayBase<T, 1>({static_cast<size_t>(inVector.shape()(0))}) 
    { 
        this->m_shape[0] = inVector.shape()(0);   

        this->allocateMemory();
        this->initializeMdspan(); 
        
        for (std::size_t i = 0; i < this->m_shape[0]; ++i) {
            this->m_mdspan(i) = inVector[i];
        }
    }

    template<typename T>
    Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
        if (this != &other) {
            ArrayBase<T, 1>::operator=(other);  
        }
        return *this;
    }

    template<typename T>
    Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept {
        if (this != &other) {
            ArrayBase<T, 1>::operator=(std::move(other));
        }
        return *this;
    }

    template<typename T>
    casacore::Vector<T> Vector<T>::toCasacoreVector() const {
        casacore::IPosition shape(1, this->m_shape[0]);
        casacore::Vector<T> outVector(shape);

        // Deep copy data
        for (size_t i = 0; i < this->m_shape[0]; ++i) {
            outVector(i) = this->m_mdspan(i);
        }

        return outVector;
    }

   
}  // namespace libracore

#endif // LIBRACORE_VECTOR_H
