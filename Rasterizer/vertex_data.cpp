#include <iostream>
#include "vertex_data.h"

VertexData VertexData::Lerp(const VertexData& a, const VertexData& b, float t)
{
    VertexData result(a);
    float m = 1 - t, n = t;
    for (auto p = a.mVec4Attributes.begin(); p != a.mVec4Attributes.end(); ++p)
    {
        result[p->first] = m * a[p->first] + n * b[p->first];
    }
    
    for (auto p = a.mVec4Additionals.begin(); p != a.mVec4Additionals.end(); ++p)
    {
        RefVertexDataAdditionalAttr(&result, p->first, ShaderCacheDataType::VEC4) = 
            m * RefConstVertexDataAdditionalAttr(&a, p->first, ShaderCacheDataType::VEC4) + n * RefConstVertexDataAdditionalAttr(&b, p->first, ShaderCacheDataType::VEC4);
    }
    for (auto p = a.mVec3Additionals.begin(); p != a.mVec3Additionals.end(); ++p)
    {
        RefVertexDataAdditionalAttr(&result, p->first, ShaderCacheDataType::VEC3) =
            m * RefConstVertexDataAdditionalAttr(&a, p->first, ShaderCacheDataType::VEC3) + n * RefConstVertexDataAdditionalAttr(&b, p->first, ShaderCacheDataType::VEC3);
    }
    for (auto p = a.mVec2Additionals.begin(); p != a.mVec2Additionals.end(); ++p)
    {
        RefVertexDataAdditionalAttr(&result, p->first, ShaderCacheDataType::VEC2) =
            m * RefConstVertexDataAdditionalAttr(&a, p->first, ShaderCacheDataType::VEC2) + n * RefConstVertexDataAdditionalAttr(&b, p->first, ShaderCacheDataType::VEC2);
    }
    for (auto p = a.mFloatAdditionals.begin(); p != a.mFloatAdditionals.end(); ++p)
    {
        RefVertexDataAdditionalAttr(&result, p->first, ShaderCacheDataType::FLOAT) =
            m * RefConstVertexDataAdditionalAttr(&a, p->first, ShaderCacheDataType::FLOAT) + n * RefConstVertexDataAdditionalAttr(&b, p->first, ShaderCacheDataType::FLOAT);
    }

    return result;
}

VertexData VertexData::CreateLike(const VertexData& that)
{
    VertexData result;
    for (auto p = that.mVec4Attributes.begin(); p != that.mVec4Attributes.end(); ++p)
    {
        result.mVec4Attributes[p->first] = Vector4f::create();
    }
    return result;
}

VertexData::VertexData()
{
}

VertexData::VertexData(std::initializer_list<VertexDataAttribute> attrs)
{
    for (auto p = attrs.begin(); p != attrs.end(); ++p)
    {
        mVec4Attributes[*p] = Vector4f::create();
    }
}

VertexData::VertexData(const VertexData& other)
{
    for (auto p = other.mVec4Attributes.begin(); p != other.mVec4Attributes.end(); ++p)
    {
        mVec4Attributes[p->first] = p->second;
    }
    
    for (auto p = other.mVec4Additionals.begin(); p != other.mVec4Additionals.end(); ++p)
    {
        mVec4Additionals[p->first] = p->second;
    }
    for (auto p = other.mVec3Additionals.begin(); p != other.mVec3Additionals.end(); ++p)
    {
        mVec3Additionals[p->first] = p->second;
    }
    for (auto p = other.mVec2Additionals.begin(); p != other.mVec2Additionals.end(); ++p)
    {
        mVec2Additionals[p->first] = p->second;
    }
    for (auto p = other.mFloatAdditionals.begin(); p != other.mFloatAdditionals.end(); ++p)
    {
        mFloatAdditionals[p->first] = p->second;
    }
}

VertexData::VertexData(VertexData&& other) noexcept
{
    mVec4Attributes = std::move(other.mVec4Attributes);

    mVec4Additionals = std::move(other.mVec4Additionals);
    mVec3Additionals = std::move(other.mVec3Additionals);
    mVec2Additionals = std::move(other.mVec2Additionals);
    mFloatAdditionals = std::move(other.mFloatAdditionals);
}

VertexData::~VertexData()
{
    mVec4Attributes.clear();

    mVec4Additionals.clear();
    mVec3Additionals.clear();
    mVec2Additionals.clear();
    mFloatAdditionals.clear();
}

void VertexData::Reset()
{
    for (auto p = mVec4Attributes.begin(); p != mVec4Attributes.end(); ++p)
    {
        p->second = Vector4f::zero();
    }

    for (auto p = mVec4Additionals.begin(); p != mVec4Additionals.end(); ++p)
    {
        p->second = Vector4f::zero();
    }
    for (auto p = mVec3Additionals.begin(); p != mVec3Additionals.end(); ++p)
    {
        p->second = Vector3f::zero();
    }
    for (auto p = mVec2Additionals.begin(); p != mVec2Additionals.end(); ++p)
    {
        p->second = Vector2f::zero();
    }
    for (auto p = mFloatAdditionals.begin(); p != mFloatAdditionals.end(); ++p)
    {
        p->second = 0;
    }
}

VertexData& VertexData::operator=(const VertexData& other)
{
    for (auto p = other.mVec4Attributes.begin(); p != other.mVec4Attributes.end(); ++p)
    {
        mVec4Attributes[p->first] = p->second;
    }
    
    for (auto p = other.mVec4Additionals.begin(); p != other.mVec4Additionals.end(); ++p)
    {
        mVec4Additionals[p->first] = p->second;
    }
    for (auto p = other.mVec3Additionals.begin(); p != other.mVec3Additionals.end(); ++p)
    {
        mVec3Additionals[p->first] = p->second;
    }
    for (auto p = other.mVec2Additionals.begin(); p != other.mVec2Additionals.end(); ++p)
    {
        mVec2Additionals[p->first] = p->second;
    }
    for (auto p = other.mFloatAdditionals.begin(); p != other.mFloatAdditionals.end(); ++p)
    {
        mFloatAdditionals[p->first] = p->second;
    }

    return *this;
}

VertexData& VertexData::operator=(VertexData&& other) noexcept
{
    mVec4Attributes = std::move(other.mVec4Attributes);

    mVec4Additionals = std::move(other.mVec4Additionals);
    mVec3Additionals = std::move(other.mVec3Additionals);
    mVec2Additionals = std::move(other.mVec2Additionals);
    mFloatAdditionals = std::move(other.mFloatAdditionals);

    return *this;
}

std::unordered_map<VertexDataAttribute, Vector4f> VertexData::AllVec4Attributes() const
{
    return mVec4Attributes;
}

const Vector4f& VertexData::operator[](const VertexDataAttribute& attr) const
{
    return mVec4Attributes.at(attr);
}

Vector4f& VertexData::operator[](const VertexDataAttribute& attr)
{
    return mVec4Attributes[attr];
}

void VertexData::RegisterAdditionalAttrs(const std::string& name, ShaderCacheDataType dataType)
{
    bool bSuccess = false;
    switch (dataType)
    {
    case ShaderCacheDataType::VEC4:
        if (mVec4Additionals.find(name) == mVec4Additionals.end())
        {
            mVec4Additionals[name] = Vector4f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::VEC3:
        if (mVec3Additionals.find(name) == mVec3Additionals.end())
        {
            mVec3Additionals[name] = Vector3f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::VEC2:
        if (mVec2Additionals.find(name) == mVec2Additionals.end())
        {
            mVec2Additionals[name] = Vector2f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::FLOAT:
        if (mFloatAdditionals.find(name) == mFloatAdditionals.end())
        {
            mFloatAdditionals[name] = 0;
            bSuccess = true;
        }
        break;
    default:
        break;
    }

    if (!bSuccess)
    {
        std::cout << "vertex data failed register shader cache, name: " << name << ", type: " << static_cast<int>(dataType) << std::endl;
    }
}

void* VertexData::AdditionalAttrs(const std::string& name, ShaderCacheDataType dataType)
{
    switch (dataType)
    {
    case ShaderCacheDataType::VEC4:
        if (mVec4Additionals.find(name) != mVec4Additionals.end())
        {
            return &mVec4Additionals[name];
        }
        break;
    case ShaderCacheDataType::VEC3:
        if (mVec3Additionals.find(name) != mVec3Additionals.end())
        {
            return &mVec3Additionals[name];
        }
        break;
    case ShaderCacheDataType::VEC2:
        if (mVec2Additionals.find(name) != mVec2Additionals.end())
        {
            return &mVec2Additionals[name];
        }
        break;
    case ShaderCacheDataType::FLOAT:
        if (mFloatAdditionals.find(name) != mFloatAdditionals.end())
        {
            return &mFloatAdditionals[name];
        }
        break;
    default:
        break;
    }

    std::cout << "vertex data failed get shader cache, name: " << name << ", type: " << static_cast<int>(dataType) << std::endl;
    return nullptr;
}

const void* VertexData::AdditionalAttrs(const std::string& name, ShaderCacheDataType dataType) const
{
    switch (dataType)
    {
    case ShaderCacheDataType::VEC4:
        if (mVec4Additionals.find(name) != mVec4Additionals.end())
        {
            return &mVec4Additionals.at(name);
        }
        break;
    case ShaderCacheDataType::VEC3:
        if (mVec3Additionals.find(name) != mVec3Additionals.end())
        {
            return &mVec3Additionals.at(name);
        }
        break;
    case ShaderCacheDataType::VEC2:
        if (mVec2Additionals.find(name) != mVec2Additionals.end())
        {
            return &mVec2Additionals.at(name);
        }
        break;
    case ShaderCacheDataType::FLOAT:
        if (mFloatAdditionals.find(name) != mFloatAdditionals.end())
        {
            return &mFloatAdditionals.at(name);
        }
        break;
    default:
        break;
    }

    std::cout << "vertex data failed get const shader cache, name: " << name << ", type: " << static_cast<int>(dataType) << std::endl;
    return nullptr;
}

