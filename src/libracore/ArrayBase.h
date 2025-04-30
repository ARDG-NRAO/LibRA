#ifndef LIBRACORE_ARRAYBASE_H
#define LIBRACORE_ARRAYBASE_H

#include <mdspan.hpp>
#include <cassert>

#include <libracore/LibracoreTypes.h>
#include <libracore/imageInterface.h>

using namespace casacore;

namespace libracore {


template<typename T, size_t N>
class ArrayBase {
public:
	// Default constructor
	ArrayBase() : m_shape{}, m_data(nullptr) {}  

    // Constructor that initializes the array with the specified shape (Nx, Ny, ..., Nz)
    ArrayBase(std::array<size_t, N> shape);

    // copy constructor, new memory and deep copy
    ArrayBase(const ArrayBase<T, N>& other) noexcept;

    // Move constructor, no new memory allocation
    ArrayBase(ArrayBase<T, N>&& other) noexcept;

    // Function to access mdspan 
    auto getMdspan() const { return m_mdspan; } 

    // Operator overloads
    auto operator+(const ArrayBase& other) const;
    auto operator-(const ArrayBase& other) const;
    auto operator*(const ArrayBase& other) const;
    auto operator/(const ArrayBase& other) const;
    auto operator=(const ArrayBase& other); // copy assignment
    auto operator=(const ArrayBase&& other) noexcept; // move assignment
    bool operator==(const ArrayBase& other) const;
    bool operator!=(const ArrayBase& other) const;

    // Accessor for shape 
    const std::array<size_t, N>& getShape() const { return m_shape; }
    void resize(const std::array<size_t, N>& newShape);
    
protected:
    std::array<size_t, N> m_shape;
    std::shared_ptr<T[]> m_data;  // Shared pointer to manage memory
    // column-major 
    Kokkos::mdspan<T, typename Kokkos::dextents<size_t, N>, Kokkos::layout_left> m_mdspan;  

    // initialize mdspan; 
    void initializeMdspan();

    // Constructor helper to allocate memory
    void allocateMemory();

};


// basic constructor
template<typename T, size_t N>
ArrayBase<T, N>::ArrayBase(std::array<size_t, N> shape)
    : m_shape(shape) {
    allocateMemory();
    initializeMdspan();
}

// copy constructor
template<typename T, size_t N>
ArrayBase<T, N>::ArrayBase(const ArrayBase<T, N>& other) noexcept
    : m_shape(other.m_shape) {
    // Calculate totalSize dynamically based on all dimensions of m_shape
    size_t totalSize = 1;
    for (size_t i = 0; i < N; ++i) {
        totalSize *= m_shape[i];  
    }
    //m_data = std::make_shared<T[]>(totalSize);  
    m_data = std::shared_ptr<T[]>(new T[totalSize], [](T* ptr) { delete[] ptr; });
    for (size_t i = 0; i < totalSize; ++i) {
        m_data[i] = other.m_data[i];
    }
    
    initializeMdspan();
}

// move constructor
template<typename T, size_t N>
ArrayBase<T, N>::ArrayBase(ArrayBase<T, N>&& other) noexcept
    : m_shape(other.m_shape), m_data(std::move(other.m_data)) {
    
    initializeMdspan();
}

// Generalized operator+ 
template<typename T, size_t N>
auto ArrayBase<T, N>::operator+(const ArrayBase<T, N>& other) const {
    // Assert that the shapes are the same
    for (size_t i = 0; i < N; ++i) {
        assert(m_shape[i] == other.m_shape[i]);
    }

    auto result = ArrayBase<T, N>(m_shape);
   
    if constexpr (N == 1) {
        // 1D: Vector addition
        for (size_t i = 0; i < m_shape[0]; ++i) {
            result.m_mdspan(i) = m_mdspan(i) + other.m_mdspan(i);
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix addition (column-major layout)
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                result.m_mdspan(i, j) = m_mdspan(i, j) + other.m_mdspan(i, j);
            }
        }
    } else if constexpr (N == 3) {
        // 3D: Cube addition (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    result.m_mdspan(i, j, k) = m_mdspan(i, j, k) + other.m_mdspan(i, j, k);
                }
            }
        }
    }

    return result;
}

// Generalized operator- 
template<typename T, size_t N>
auto ArrayBase<T, N>::operator-(const ArrayBase<T, N>& other) const {
    // Assert that the shapes are the same
    for (size_t i = 0; i < N; ++i) {
        assert(m_shape[i] == other.m_shape[i]);
    }

    auto result = ArrayBase<T, N>(m_shape);

    if constexpr (N == 1) {
        // 1D: Vector 
        for (size_t i = 0; i < m_shape[0]; ++i) {
            result.m_mdspan(i) = m_mdspan(i) - other.m_mdspan(i);
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix (column-major layout)
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                result.m_mdspan(i, j) = m_mdspan(i, j) - other.m_mdspan(i, j);
            }
        }
    } else if constexpr (N == 3) {
        // 3D: Cube (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    result.m_mdspan(i, j, k) = m_mdspan(i, j, k) - other.m_mdspan(i, j, k);
                }
            }
        }
    }

    return result;
}

// Generalized operator* 
template<typename T, size_t N>
auto ArrayBase<T, N>::operator*(const ArrayBase<T, N>& other) const {
    // Assert that the shapes are the same
    for (size_t i = 0; i < N; ++i) {
        assert(m_shape[i] == other.m_shape[i]);
    }

    auto result = ArrayBase<T, N>(m_shape);

    if constexpr (N == 1) {
        // 1D: Vector
        for (size_t i = 0; i < m_shape[0]; ++i) {
            result.m_mdspan(i) = m_mdspan(i) * other.m_mdspan(i);
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix (column-major layout)
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                result.m_mdspan(i, j) = m_mdspan(i, j) * other.m_mdspan(i, j);
            }
        }
    } else if constexpr (N == 3) {
        // 3D: Cube (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    result.m_mdspan(i, j, k) = m_mdspan(i, j, k) * other.m_mdspan(i, j, k);
                }
            }
        }
    }

    return result;
}

// Generalized operator/ 
template<typename T, size_t N>
auto ArrayBase<T, N>::operator/(const ArrayBase<T, N>& other) const {
    // Assert that the shapes are the same
    for (size_t i = 0; i < N; ++i) {
        assert(m_shape[i] == other.m_shape[i]);
    }

    auto result = ArrayBase<T, N>(m_shape);

    if constexpr (N == 1) {
        // 1D: Vector
        for (size_t i = 0; i < m_shape[0]; ++i) {
            result.m_mdspan(i) = std::abs(other.m_mdspan(i)) != 0 ? m_mdspan(i) / other.m_mdspan(i) : 0;
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix (column-major layout)
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                result.m_mdspan(i, j) = std::abs(other.m_mdspan(i, j)) != 0 ? m_mdspan(i, j) / other.m_mdspan(i, j) : 0;
            }
        }
    } else if constexpr (N == 3) {
        // 3D: Cube (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    result.m_mdspan(i, j, k) = std::abs(other.m_mdspan(i, j, k)) != 0 ? m_mdspan(i, j, k) / other.m_mdspan(i, j, k) : 0;
                }
            }
        }
    }

    return result;
}

// Generalized operator== 
template<typename T, size_t N>
bool ArrayBase<T, N>::operator==(const ArrayBase<T, N>& other) const {
    // Assert that the shapes are the same
    for (size_t i = 0; i < N; ++i) {
        if (m_shape[i] != other.m_shape[i])
        	return false;
    }

    if constexpr (N == 1) {
        // 1D: Vector
        for (size_t i = 0; i < m_shape[0]; ++i) {
            if (m_mdspan(i) != other.m_mdspan(i))
            	return false;
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix (column-major layout)
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                if (m_mdspan(i, j) != other.m_mdspan(i, j))
                	return false;
            }
        }
    } else if constexpr (N == 3) {
        // 3D: Cube (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    if (m_mdspan(i, j, k) != other.m_mdspan(i, j, k))
                    	return false;
                }
            }
        }
    }

    return true;
}

template<typename T, size_t N>
auto ArrayBase<T, N>::operator=(const ArrayBase<T, N>& other) {
  if (this != &other)
  {  
    if constexpr (N == 1) {
        // 1D: Vector
        if (m_shape[0] != other.m_shape[0])
        {        	
            // Reallocate memory 
            m_data = std::make_shared<T[]>(other.m_mdspan.extent(0));
            
            initializeMdspan();
        }

        for (size_t i = 0; i < m_shape[0]; ++i) {
        	m_mdspan(i) = other.m_mdspan(i);
        }
    } else if constexpr (N == 2) {
        // 2D: Matrix (column-major layout)
        if ((m_shape[0] != other.m_shape[0]) ||
        	(m_shape[1] != other.m_shape[1]))
        {        	
            // Reallocate memory 
            m_data = std::make_shared<T[]>(other.m_mdspan.extent(0)*other.m_mdspan.extent(1));
            initializeMdspan();
        }
        for (size_t j = 0; j < m_shape[1]; ++j) {
            for (size_t i = 0; i < m_shape[0]; ++i) {
                m_mdspan(i, j) = other.m_mdspan(i, j);
            }
        }
    } else if constexpr (N == 3) {
    	if ((m_shape[0] != other.m_shape[0]) ||
        	(m_shape[1] != other.m_shape[1]) ||
        	(m_shape[2] != other.m_shape[2]))
        {        	
            // Reallocate memory 
            m_data = std::make_shared<T[]>(other.m_mdspan.extent(0)*other.m_mdspan.extent(1)*other.m_mdspan.extent(2));
            initializeMdspan();
        }
        // 3D: Cube (column-major layout)
        for (size_t k = 0; k < m_shape[2]; ++k) {
            for (size_t j = 0; j < m_shape[1]; ++j) {
                for (size_t i = 0; i < m_shape[0]; ++i) {
                    m_mdspan(i, j, k) = other.m_mdspan(i, j, k);
                }
            }
        }
    }
  }

  return *this;
}

template<typename T, size_t N>
auto ArrayBase<T, N>::operator=(const ArrayBase<T, N>&& other) noexcept {
    if (this != &other) {
        // Transfer ownership of data
        m_data = std::move(other.m_data);
        m_shape = other.m_shape;
        initializeMdspan();
    }

    return *this;
}

template<typename T, size_t N>
bool ArrayBase<T, N>::operator!=(const ArrayBase& other) const {
        return !(*this == other);
}

// Resize function
template<typename T, size_t N>
void ArrayBase<T, N>::resize(const std::array<size_t, N>& newShape) {
    m_shape = newShape;
    allocateMemory();
    initializeMdspan();
}

template<typename T, size_t N>
void ArrayBase<T, N>::initializeMdspan() {
    // Initialize the mdspan with the newly allocated memory
     if constexpr (N == 1) {
        m_mdspan = col_major_mdspan_1d<T>(m_data.get(), m_shape[0]);
    } else if constexpr (N == 2) {
        m_mdspan = col_major_mdspan<T>(m_data.get(), m_shape[0], m_shape[1]);
    } else if constexpr (N == 3) {
        m_mdspan = col_major_mdspan_3d<T>(m_data.get(), m_shape[0], m_shape[1], m_shape[2]);
    }
   
}

template<typename T, size_t N>
void ArrayBase<T, N>::allocateMemory() {
    size_t totalSize = 1;
    for (size_t i = 0; i < N; ++i) {
        totalSize *= m_shape[i];
    }
    m_data = std::shared_ptr<T[]>(new T[totalSize], [](T* ptr) { delete[] ptr; });
}

}  // namespace libracore

#endif // LIBRACORE_ARRAYBASE_H

