#pragma once

#include <string>
#include <unordered_map>

#include "shader_common_types.h"
#include "shader_cache_type_traits.h"
#include "vector.h"

#ifndef __VERTEX_DATA_H_MACRO__
#define __VERTEX_DATA_H_MACRO__

#define RefVertexDataAdditionalAttr(_pVertexData, _name, _cacheDataType) \
    (static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::RefType>( *static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::PointerType>((_pVertexData)->AdditionalAttrs((_name), (_cacheDataType))) ) )

#define RefConstVertexDataAdditionalAttr(_pVertexData, _name, _cacheDataType) \
    (static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::ConstRefType>( *static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::ConstPointerType>((_pVertexData)->AdditionalAttrs((_name), (_cacheDataType))) ) )

#endif

enum VertexDataAttribute
{
    POSITION = 1,
    UV,
    NORMAL,
};

class VertexData
{
public:
    static VertexData Lerp(const VertexData& a, const VertexData& b, float t);
    static VertexData CreateLike(const VertexData& that);

public:
    VertexData();
    VertexData(std::initializer_list<VertexDataAttribute> attrs);
    VertexData(const VertexData& other);
    VertexData(VertexData&& other) noexcept;
    virtual ~VertexData();

public:
    void Reset();

public:
    VertexData& operator= (const VertexData& other);
    VertexData& operator= (VertexData&& other) noexcept;

public:
    Vector4f& operator[] (const enum VertexDataAttribute& attr);
    const Vector4f& operator[] (const enum VertexDataAttribute& attr) const;

public:
    void RegisterAdditionalAttrs(const std::string& name, ShaderCacheDataType dataType);
    void* AdditionalAttrs(const std::string& name, ShaderCacheDataType dataType);
    const void* AdditionalAttrs(const std::string& name, ShaderCacheDataType dataType) const;

public:
    std::unordered_map<VertexDataAttribute, Vector4f> AllVec4Attributes() const;

protected:
    std::unordered_map<VertexDataAttribute, Vector4f> mVec4Attributes;

protected:
    std::unordered_map<std::string, Vector4f> mVec4Additionals;
    std::unordered_map<std::string, Vector3f> mVec3Additionals;
    std::unordered_map<std::string, Vector2f> mVec2Additionals;
    std::unordered_map<std::string, float> mFloatAdditionals;
};