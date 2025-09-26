#pragma once

#include <vector>
#include <cmath>
#include <cassert>

template <typename T>
class MatrixTemplate 
{
public:
    static MatrixTemplate<T> create(size_t nRow, size_t nCol)
    {
        return MatrixTemplate<T>(nRow, nCol);
    }

    static MatrixTemplate<T> create(const std::initializer_list<std::initializer_list<T>>& lst)
    {
        size_t nRow = lst.size();
        size_t nCol = lst.begin()->size();
        MatrixTemplate<T> ret = create(nRow, nCol);
        size_t i = 0;
        for (auto pRow = lst.begin(); pRow != lst.end(); ++pRow)
        {
            size_t j = 0;
            for (auto pCol = pRow->begin(); pCol != pRow->end(); ++pCol)
            {
                ret(i, j++) = *pCol;
            }
            i++;
        }
        return ret;
    }

    static MatrixTemplate<T> zero(size_t nRow, size_t nCol)
    {
        MatrixTemplate<T> mat(nRow, nCol);
        std::fill(mat.mData.begin(), mat.mData.end(), static_cast<T>(0));
        return mat;
    }

    static MatrixTemplate<T> identity(size_t n)
    {
        MatrixTemplate<T> mat(n, n);
        std::fill(mat.mData.begin(), mat.mData.end(), static_cast<T>(0));
        for (int rowcol = 0; rowcol < n; ++rowcol) {
            mat.mData[rowcol * n + rowcol] = 1;
        }
        return mat;
    }

public:
    static MatrixTemplate<T> scale3d(T x, T y, T z)
    {
        MatrixTemplate<T> rMat(4, 4);
        std::fill(rMat.mData.begin(), rMat.mData.end(), static_cast<T>(0));
        rMat(0, 0) = x;
        rMat(1, 1) = y;
        rMat(2, 2) = z;
        rMat(3, 3) = static_cast<T>(1);
        return rMat;
    }
    
    static MatrixTemplate<T> translation3d(T x, T y, T z)
    {
        MatrixTemplate<T> rMat(4, 4);
        std::fill(rMat.mData.begin(), rMat.mData.end(), static_cast<T>(0));
        rMat(0, 0) = static_cast<T>(1), rMat(0, 3) = x;
        rMat(1, 1) = static_cast<T>(1), rMat(1, 3) = y;
        rMat(2, 2) = static_cast<T>(1), rMat(2, 3) = z;
        rMat(3, 3) = static_cast<T>(1);
        return rMat;
    }
    
    static MatrixTemplate<T> rotationX3d(T rad)
    {
        MatrixTemplate<T> rMat(4, 4);
        std::fill(rMat.mData.begin(), rMat.mData.end(), static_cast<T>(0));
        rMat(0, 0) = static_cast<T>(1);
        rMat(1, 1) = std::cos(rad), rMat(1, 2) = -std::sin(rad);
        rMat(2, 1) = std::sin(rad), rMat(2, 2) = std::cos(rad);
        rMat(3, 3) = static_cast<T>(1);
        return rMat;
    }
    
    static MatrixTemplate<T> rotationY3d(T rad)
    {
        MatrixTemplate<T> rMat(4, 4);
        std::fill(rMat.mData.begin(), rMat.mData.end(), static_cast<T>(0));
        rMat(0, 0) = std::cos(rad), rMat(0, 2) = std::sin(rad);
        rMat(1, 1) = static_cast<T>(1);
        rMat(2, 0) = -std::sin(rad), rMat(2, 2) = std::cos(rad);
        rMat(3, 3) = static_cast<T>(1);
        return rMat;
    }
    
    static MatrixTemplate<T> rotationZ3d(T rad)
    {
        MatrixTemplate<T> rMat(4, 4);
        std::fill(rMat.mData.begin(), rMat.mData.end(), static_cast<T>(0));
        rMat(0, 0) = std::cos(rad), rMat(0, 1) = -std::sin(rad);
        rMat(1, 0) = std::sin(rad), rMat(1, 1) = std::cos(rad);
        rMat(2, 2) = static_cast<T>(1);
        rMat(3, 3) = static_cast<T>(1);
        return rMat;
    }

public:
    size_t colNum() const
    {
        return mCol;
    }

    size_t rowNum() const
    {
        return mRow;
    }
    
    size_t size() const
    {
        return mRow * mCol;
    }

public:
    
    MatrixTemplate<T> rMul(const MatrixTemplate<T>& right) const
    {
        const MatrixTemplate<T>& left = *this;
        assert(left.mCol == right.mRow);
        MatrixTemplate<T> mat(left.mRow, right.mCol);
        for (int lrow = 0; lrow < left.mRow; ++lrow) {
            for (int rcol = 0; rcol < right.mCol; ++rcol) {
                auto pos = lrow * mat.mCol + rcol;
                T value = 0;
                for (int s = 0; s < left.mCol; ++s) {
                    value += left.mData[lrow * left.mCol + s] * right.mData[s * right.mCol + rcol];
                }
                mat.mData[pos] = value;
            }
        }
        return mat;
    }
    
    MatrixTemplate<T> lMul(const MatrixTemplate<T>& left) const
    {
        return left.rMul(*this);
    }
    
    MatrixTemplate<T> add(const MatrixTemplate<T>& other) const
    {
        const MatrixTemplate<T>& left = *this;
        const MatrixTemplate<T>& right = other;
        assert(left.mRow == right.mRow && left.mCol == right.mCol);
        auto rowNum = left.rowNum(), colNum = left.colNum();
        MatrixTemplate<T> mat(rowNum, colNum);
        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                auto pos = row * colNum + col;
                mat.mData[pos] = left.mData[pos] + right.mData[pos];
            }
        }
        return mat;
    }
    
    MatrixTemplate<T> minus(const MatrixTemplate<T>& other) const
    {
        const MatrixTemplate<T>& left = *this;
        const MatrixTemplate<T>& right = other;
        assert(left.mRow == right.mRow && left.mCol == right.mCol);
        auto rowNum = left.rowNum(), colNum = left.colNum();
        MatrixTemplate<T> mat(rowNum, colNum);
        for (int row = 0; row < rowNum; ++row) {
            for (int col = 0; col < colNum; ++col) {
                auto pos = row * colNum + col;
                mat.mData[pos] = left.mData[pos] - right.mData[pos];
            }
        }
        return mat;
    }

    MatrixTemplate<T> mulByScalar(const T& scalar) const
    {
        MatrixTemplate<T> mat(mRow, mCol);
        for (size_t i = 0; i < mData.size(); ++i)
        {
            mat.mData[i] = mData[i] * scalar;
        }
        return mat;
    }

    void fill(const T& value)
    {
        std::fill(mData.begin(), mData.end(), value);
    }

public:
    MatrixTemplate<T> inverse() const
    {
        assert(mRow == 4 && mCol == 4);

        MatrixTemplate<T> result(mRow, mCol);

        float det;
        int i, j;

        result(0, 0) = mData[5] * mData[10] * mData[15] -
            mData[5] * mData[11] * mData[14] -
            mData[9] * mData[6] * mData[15] +
            mData[9] * mData[7] * mData[14] +
            mData[13] * mData[6] * mData[11] -
            mData[13] * mData[7] * mData[10];

        result(1, 0) = -mData[4] * mData[10] * mData[15] +
            mData[4] * mData[11] * mData[14] +
            mData[8] * mData[6] * mData[15] -
            mData[8] * mData[7] * mData[14] -
            mData[12] * mData[6] * mData[11] +
            mData[12] * mData[7] * mData[10];

        result(2, 0) = mData[4] * mData[9] * mData[15] -
            mData[4] * mData[11] * mData[13] -
            mData[8] * mData[5] * mData[15] +
            mData[8] * mData[7] * mData[13] +
            mData[12] * mData[5] * mData[11] -
            mData[12] * mData[7] * mData[9];

        result(3, 0) = -mData[4] * mData[9] * mData[14] +
            mData[4] * mData[10] * mData[13] +
            mData[8] * mData[5] * mData[14] -
            mData[8] * mData[6] * mData[13] -
            mData[12] * mData[5] * mData[10] +
            mData[12] * mData[6] * mData[9];

        result(0, 1) = -mData[1] * mData[10] * mData[15] +
            mData[1] * mData[11] * mData[14] +
            mData[9] * mData[2] * mData[15] -
            mData[9] * mData[3] * mData[14] -
            mData[13] * mData[2] * mData[11] +
            mData[13] * mData[3] * mData[10];

        result(1, 1) = mData[0] * mData[10] * mData[15] -
            mData[0] * mData[11] * mData[14] -
            mData[8] * mData[2] * mData[15] +
            mData[8] * mData[3] * mData[14] +
            mData[12] * mData[2] * mData[11] -
            mData[12] * mData[3] * mData[10];

        result(2, 1) = -mData[0] * mData[9] * mData[15] +
            mData[0] * mData[11] * mData[13] +
            mData[8] * mData[1] * mData[15] -
            mData[8] * mData[3] * mData[13] -
            mData[12] * mData[1] * mData[11] +
            mData[12] * mData[3] * mData[9];

        result(3, 1) = mData[0] * mData[9] * mData[14] -
            mData[0] * mData[10] * mData[13] -
            mData[8] * mData[1] * mData[14] +
            mData[8] * mData[2] * mData[13] +
            mData[12] * mData[1] * mData[10] -
            mData[12] * mData[2] * mData[9];

        result(0, 2) = mData[1] * mData[6] * mData[15] -
            mData[1] * mData[7] * mData[14] -
            mData[5] * mData[2] * mData[15] +
            mData[5] * mData[3] * mData[14] +
            mData[13] * mData[2] * mData[7] -
            mData[13] * mData[3] * mData[6];

        result(1, 2) = -mData[0] * mData[6] * mData[15] +
            mData[0] * mData[7] * mData[14] +
            mData[4] * mData[2] * mData[15] -
            mData[4] * mData[3] * mData[14] -
            mData[12] * mData[2] * mData[7] +
            mData[12] * mData[3] * mData[6];

        result(2, 2) = mData[0] * mData[5] * mData[15] -
            mData[0] * mData[7] * mData[13] -
            mData[4] * mData[1] * mData[15] +
            mData[4] * mData[3] * mData[13] +
            mData[12] * mData[1] * mData[7] -
            mData[12] * mData[3] * mData[5];

        result(3, 2) = -mData[0] * mData[5] * mData[14] +
            mData[0] * mData[6] * mData[13] +
            mData[4] * mData[1] * mData[14] -
            mData[4] * mData[2] * mData[13] -
            mData[12] * mData[1] * mData[6] +
            mData[12] * mData[2] * mData[5];

        result(0, 3) = -mData[1] * mData[6] * mData[11] +
            mData[1] * mData[7] * mData[10] +
            mData[5] * mData[2] * mData[11] -
            mData[5] * mData[3] * mData[10] -
            mData[9] * mData[2] * mData[7] +
            mData[9] * mData[3] * mData[6];

        result(1, 3) = mData[0] * mData[6] * mData[11] -
            mData[0] * mData[7] * mData[10] -
            mData[4] * mData[2] * mData[11] +
            mData[4] * mData[3] * mData[10] +
            mData[8] * mData[2] * mData[7] -
            mData[8] * mData[3] * mData[6];

        result(2, 3) = -mData[0] * mData[5] * mData[11] +
            mData[0] * mData[7] * mData[9] +
            mData[4] * mData[1] * mData[11] -
            mData[4] * mData[3] * mData[9] -
            mData[8] * mData[1] * mData[7] +
            mData[8] * mData[3] * mData[5];

        result(3, 3) = mData[0] * mData[5] * mData[10] -
            mData[0] * mData[6] * mData[9] -
            mData[4] * mData[1] * mData[10] +
            mData[4] * mData[2] * mData[9] +
            mData[8] * mData[1] * mData[6] -
            mData[8] * mData[2] * mData[5];

        det = mData[0] * result(0, 0) + mData[1] * result(1, 0) + mData[2] * result(2, 0) + mData[3] * result(3, 0);

        det = 1.0f / det;

        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                result(i, j) = result(i, j) * det;
            }
        }

        return result;
    }

    MatrixTemplate<T> transpose() const
    {
        MatrixTemplate<T> result(mCol, mRow);
        for (int i = 0; i < mRow; ++i)
        {
            for (int j = 0; j < mCol; ++j)
            {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

public:
    T& operator ()(size_t row, size_t col)
    {
        return mData[row * mCol + col];
    }

    T operator ()(size_t row, size_t col) const
    {
        return mData[row * mCol + col];
    }

public:
    virtual ~MatrixTemplate() {};

    MatrixTemplate(size_t nRow=1, size_t nCol=1) : mData(nRow* nCol)
    {
        mRow = nRow;
        mCol = nCol;
    }

protected:
    std::vector<T> mData;
    size_t mRow;
    size_t mCol;

public:
    template<typename TInner>
    friend MatrixTemplate<TInner> operator* (const MatrixTemplate<TInner>& left, const MatrixTemplate<TInner>& right)
    {
        return left.rMul(right);
    }

    template<typename TInner>
    friend MatrixTemplate<TInner> operator+ (const MatrixTemplate<TInner>& left, const MatrixTemplate<TInner>& right)
    {
        return left.add(right);
    }

    template<typename TInner>
    friend MatrixTemplate<TInner> operator- (const MatrixTemplate<TInner>& left, const MatrixTemplate<TInner>& right)
    {
        return left.minus(right);
    }

    template<typename TInner>
    friend MatrixTemplate<TInner> operator* (const MatrixTemplate<TInner>& left, const TInner& right)
    {
        return left.mulByScalar(right);
    }

    template<typename TInner>
    friend MatrixTemplate<TInner> operator* (const TInner& left, const MatrixTemplate<TInner>& right)
    {
        return right.mulByScalar(left);
    }
};

using Matrix = MatrixTemplate<float>;