#pragma once

#include <vector>
#include <type_traits>
#include <cmath>

#include "matrix.h"


template <typename T, size_t N>
class VectorTemplate
{
public:
    
    static VectorTemplate<T, N> create()
    {
        return VectorTemplate<T, N>();
    }

    static VectorTemplate<T, N> create(const std::initializer_list<T>& lst)
    {
        VectorTemplate<T, N> ret;
        size_t i = 0;
        for (auto p = lst.begin(); p != lst.end(); ++p)
        {
            ret[i++] = *p;
        }
        return ret;
    }
    
    static VectorTemplate<T, N> ones()
    {
        VectorTemplate<T, N> vec;
        std::fill(vec.mData.begin(), vec.mData.end(), static_cast<T>(1));
        return vec;
    }
    
    static VectorTemplate<T, N> zero()
    {
        VectorTemplate<T, N> vec;
        std::fill(vec.mData.begin(), vec.mData.end(), static_cast<T>(0));
        return vec;
    }

public:
    size_t size() const
    {
        return mSize;
    }

    float length() const
    {
        float v = 0;
        for (size_t i = 0; i < mSize; ++i) {
            v += this->mData[i] * this->mData[i];
        }
        return std::sqrt(v);
    }

    VectorTemplate<T, N> normalize()
    {
        auto N = length();
        if (N == 0) {
            return *this;
        }
        auto n = size();
        for (int i = 0; i < n; ++i) {
            this->mData[i] /= N;
        }
        return *this;
    }

    VectorTemplate<T, N> pow(const T& exp)
    {
        auto n = size();
        for (int i = 0; i < n; ++i) {
            this->mData[i] = std::pow(this->mData[i], exp);
        }
        return *this;
    }

    VectorTemplate<T, N> clone() const
    {
        return *this;
    }

    bool equals(const VectorTemplate<T, N>& other) const
    {
        if (mSize != other.mSize)
        {
            return false;
        }
        for (size_t i = 0; i < mSize; ++i)
        {
            if (mData[i] != other.mData[i])
            {
                return false;
            }
        }
        return true;
    }

public:
    T& operator[] (size_t idx)
    {
        return mData[idx];
    }

    T operator[] (size_t idx) const
    {
        return mData[idx];
    }

    T& operator() (size_t idx)
    {
        return mData[idx];
    }

    T operator() (size_t idx) const
    {
        return mData[idx];
    }

public:
    
    T dot(const VectorTemplate<T, N>& other) const
    {
        assert(this->mSize == other.mSize);
        T val = static_cast<T>(0);
        for (size_t i = 0; i < this->mSize; ++i)
        {
            val += this->mData[i] * other.mData[i];
        }
        return val;
    }
    
    VectorTemplate<T, N> add(const VectorTemplate<T, N>& other) const
    {
        const VectorTemplate<T, N>& left = *this;
        const VectorTemplate<T, N>& right = other;
        assert(left.size() == right.size());
        auto n = left.size();
        VectorTemplate<T, N> vec;
        for (size_t i = 0; i < n; ++i) {
            vec.mData[i] = left.mData[i] + right.mData[i];
        }
        return vec;
    }
    
    VectorTemplate<T, N> minus(const VectorTemplate<T, N>& other) const
    {
        const VectorTemplate<T, N>& left = *this;
        const VectorTemplate<T, N>& right = other;
        assert(left.size() == right.size());
        auto n = left.size();
        VectorTemplate<T, N> vec;
        for (size_t i = 0; i < n; ++i) {
            vec.mData[i] = left.mData[i] - right.mData[i];
        }
        return vec;
    }

    VectorTemplate<T, N> hmul(const VectorTemplate<T, N>& other) const
    {
        const VectorTemplate<T, N>& left = *this;
        const VectorTemplate<T, N>& right = other;
        assert(left.size() == right.size());
        auto n = left.size();
        VectorTemplate<T, N> vec;
        for (size_t i = 0; i < n; ++i) {
            vec.mData[i] = left.mData[i] * right.mData[i];
        }
        return vec;
    }

    VectorTemplate<T, N> mulByScalar(const T& scalar) const
    {
        VectorTemplate<T, N> vec;
        for (size_t i = 0; i < N; ++i)
        {
            vec.mData[i] = this->mData[i] * scalar;
        }
        return vec;
    }

    template<typename TInner, size_t NInner>
    VectorTemplate<T, N>& assign(const VectorTemplate<TInner, NInner>& other, const T& defaultVal = 0)
    {
        auto otherSize = other.size();
        for (size_t i = 0; i < mSize; ++i)
        {
            if (i < otherSize)
            {
                mData[i] = static_cast<T>(other[i]);
            }
            else {
                mData[i] = defaultVal;
            }
        }
        return *this;
    }

public:
    template<typename TInner>
    VectorTemplate<T, N> lMulMatrix(const MatrixTemplate<TInner> matrix) const
    {
        static_assert(std::is_same<T, TInner>::value);
        assert(mSize == matrix.colNum());
        VectorTemplate<T, N> ret;
        for (size_t i = 0; i < mSize; ++i)
        {
            ret[i] = 0;
            for (size_t j = 0; j < matrix.colNum(); ++j)
            {
                ret[i] += matrix(i, j) * mData[j];
            }
        }
        return ret;
    }

    VectorTemplate perspectiveDivide() const
    {
        static_assert(N == 4);
        assert(mData[3] != 0);
        VectorTemplate ret;
        for (size_t i = 0; i < 3; ++i)
        {
            ret[i] = mData[i] / mData[3];
        }
        return ret;
    }

    void perspectiveDivideInPlace()
    {
        static_assert(N == 4);
        assert(mData[3] != 0);
        for (size_t i = 0; i < 3; ++i)
        {
            mData[i] = mData[i] / mData[3];
        }
    }

public:
    ~VectorTemplate() {}
    VectorTemplate() : mData(N)
    {
        this->mSize = N;
    }

protected:
    std::vector<T> mData;
    size_t mSize;

public:
    template<typename TInner, size_t N>
    friend TInner operator* (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right);

    template<typename TInner, size_t N>
    friend VectorTemplate<TInner, N> operator* (const MatrixTemplate<TInner>& left, const VectorTemplate<TInner, N>& right);

    template<typename TInner, size_t N>
    friend VectorTemplate<TInner, N> operator* (const VectorTemplate<TInner, N>& left, const TInner& scalar);

    template<typename TInner, size_t N>
    friend VectorTemplate<TInner, N> operator* (const TInner& scalar, const VectorTemplate<TInner, N>& right);

    template<typename TInner, size_t N>
    friend VectorTemplate<TInner, N> operator+ (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right);

    template<typename TInner, size_t N>
    friend VectorTemplate<TInner, N> operator- (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right);

public:
    template <typename T, typename TOut>
    friend class Vector3Template;

    template <typename T, typename TOut>
    friend class Vector2Template;
};

template<typename TInner, size_t N>
TInner operator* (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right)
{
    return left.dot(right);
}

template<typename TInner, size_t N>
VectorTemplate<TInner, N> operator* (const MatrixTemplate<TInner>& left, const VectorTemplate<TInner, N>& right)
{
    return right.lMulMatrix(left);
}

template<typename TInner, size_t N>
inline VectorTemplate<TInner, N> operator*(const VectorTemplate<TInner, N>& left, const TInner& scalar)
{
    return left.mulByScalar(scalar);
}

template<typename TInner, size_t N>
inline VectorTemplate<TInner, N> operator*(const TInner& scalar, const VectorTemplate<TInner, N>& right)
{
    return right.mulByScalar(scalar);
}

template<typename TInner, size_t N>
VectorTemplate<TInner, N> operator+ (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right)
{
    return left.add(right);
}

template<typename TInner, size_t N>
VectorTemplate<TInner, N> operator- (const VectorTemplate<TInner, N>& left, const VectorTemplate<TInner, N>& right)
{
    return left.minus(right);
}



template <typename T, typename TOut = float>
class Vector3Template : public VectorTemplate<T, 3>
{
public:
    Vector3Template<T, TOut> rCross(const Vector3Template<T, TOut>& other) const
    {
        Vector3Template vec;
        auto x1 = this->mData[0], y1 = this->mData[1], z1 = this->mData[2];
        auto x2 = other.mData[0], y2 = other.mData[1], z2 = other.mData[2];
        vec.mData[0] = y1 * z2 - y2 * z1;
        vec.mData[1] = x2 * z1 - x1 * z2;
        vec.mData[2] = x1 * y2 - x2 * y1;
        return vec;
    }

public:
    ~Vector3Template() {}
    Vector3Template() {}

    template<typename TInner, size_t N>
    Vector3Template(VectorTemplate<TInner, N>&& other)
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
    }

    template<typename TInner, size_t N>
    Vector3Template(const VectorTemplate<TInner, N>& other)
    {
        this->mSize = other.mSize;
        this->mData = other.mData;
    }

    Vector3Template(Vector3Template&& other) noexcept
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
    }

    Vector3Template(const Vector3Template& other) noexcept
    {
        this->mSize = other.mSize;
        this->mData = other.mData;
    }

    Vector3Template& operator= (Vector3Template&& other) noexcept
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
        return *this;
    }

    Vector3Template& operator= (const Vector3Template& other)
    {
        this->mSize = other.mSize;
        this->mData = other.mData;
        return *this;
    }

public:
    template<typename TInner, typename TInnerOut>
    friend Vector3Template<TInner, TInnerOut> operator% (const Vector3Template<TInner, TInnerOut>& left, const Vector3Template<TInner, TInnerOut>& right);

};

template<typename T, typename TOut = float>
Vector3Template<T, TOut> operator% (const Vector3Template<T, TOut>& left, const Vector3Template<T, TOut>& right)
{
    return left.rCross(right);
}


template <typename T, typename TOut = float>
class Vector2Template : public VectorTemplate<T, 2>
{
public:
    TOut rCross(const Vector2Template<T, TOut>& other) const
    {
        return static_cast<TOut>(this->mData[0] * other.mData[1] - other.mData[0] * this->mData[1]);
    }

public:
    ~Vector2Template() {}
    Vector2Template() {}

    template<typename TInner, size_t N>
    Vector2Template(VectorTemplate<TInner, N>&& other)
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
    }

    Vector2Template(Vector2Template&& other) noexcept
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
    }

    Vector2Template& operator= (Vector2Template&& other)
    {
        this->mSize = other.mSize;
        this->mData = std::move(other.mData);
        return *this;
    }

    Vector2Template& operator= (const Vector2Template& other)
    {
        this->mSize = other.mSize;
        this->mData = other.mData;
        return *this;
    }

public:
    template<typename TInner, typename TInnerOut>
    friend TInnerOut operator% (const Vector2Template<TInner, TInnerOut>& left, const Vector2Template<TInner, TInnerOut>& right);

};

template<typename T, typename TOut = float>
TOut operator% (const Vector2Template<T, TOut>& left, const Vector2Template<T, TOut>& right)
{
    return left.rCross(right);
}


using Vector2f = Vector2Template<float>;
using Vector2i = Vector2Template<int>;

using Vector3f = Vector3Template<float>;
using Vector3i = Vector3Template<int>;

using Vector4f = VectorTemplate<float, 4>;
using Vector4i = VectorTemplate<int, 4>;