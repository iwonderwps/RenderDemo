#pragma once


#include "shader_common_types.h"

#include "matrix.h"
#include "vector.h"

class Texture;

#ifndef __SHADER_CACHE_TYPE_TRAITS
#define __SHADER_CACHE_TYPE_TRAITS

template <int EType>
struct ShaderCacheDataTypeTrait
{
    typedef void* PointerType;
    typedef void RefType;

    typedef void* ConstPointerType;
    typedef void ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::MATRIX)>
{
    typedef Matrix* PointerType;
    typedef Matrix& RefType;

    typedef const Matrix* ConstPointerType;
    typedef const Matrix& ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::VEC4)>
{
    typedef Vector4f* PointerType;
    typedef Vector4f& RefType;

    typedef const Vector4f* ConstPointerType;
    typedef const Vector4f& ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::VEC3)>
{
    typedef Vector3f* PointerType;
    typedef Vector3f& RefType;

    typedef const Vector3f* ConstPointerType;
    typedef const Vector3f& ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::VEC2)>
{
    typedef Vector2f* PointerType;
    typedef Vector2f& RefType;

    typedef const Vector2f* ConstPointerType;
    typedef const Vector2f& ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::FLOAT)>
{
    typedef float* PointerType;
    typedef float& RefType;

    typedef const float* ConstPointerType;
    typedef const float& ConstRefType;
};

template <>
struct ShaderCacheDataTypeTrait<static_cast<int>(ShaderCacheDataType::TEXTURE)>
{
    typedef Texture* PointerType;
    typedef Texture& RefType;

    typedef const Texture* ConstPointerType;
    typedef const Texture& ConstRefType;
};

#endif // !__SHADER_CACHE_TYPE_TRAITS
