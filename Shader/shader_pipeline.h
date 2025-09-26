#pragma once


#include <vector>
#include <unordered_map>
#include <string>
#include "shader_common_types.h"
#include "shader_cache_type_traits.h"
#include "matrix.h"
#include "vector.h"
#include "texture.h"


#ifndef __SHADER_BASE_H_MACRO__
#define __SHADER_BASE_H_MACRO__

// 需确保容器稳定。 
// 简单起见，不如把所需变量直接声明在 Shader 类中

#define RefShaderPiplineCachedData(_pPipeline, _name, _cacheDataType) \
    (static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::RefType>( *static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::PointerType>((_pPipeline)->CachedData((_name), (_cacheDataType))) ) )

#define RefShaderPiplineGlobalCachedData(_pPipeline, _name, _cacheDataType) \
    (static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::RefType>( *static_cast<typename ShaderCacheDataTypeTrait<static_cast<int>(_cacheDataType)>::PointerType>((_pPipeline)->GlobalCachedData((_name), (_cacheDataType))) ) )

#endif // !__SHADER_BASE_H_MACRO__


class ShaderBase;

class ShaderPipeline
{
public:
    ShaderPipeline();
    virtual ~ShaderPipeline();

public:
    void AddShader(ShaderBase* shader);
    ShaderBase* GetShader(ShaderType shaderType);

public:
    virtual void RegisterCacheData(const std::string& name, ShaderCacheDataType dataType);
    virtual void RegisterGlobalCacheData(const std::string& name, ShaderCacheDataType dataType);

    virtual void* CachedData(const std::string& name, ShaderCacheDataType dataType);
    virtual void* GlobalCachedData(const std::string& name, ShaderCacheDataType dataType);

    virtual ShaderCacheDataType GetCacheDataTypeByName(const std::string& name) const; // 不考虑 Global

public:
    virtual void RegisterRasterizeInterpolate(const std::string& name);

    virtual std::vector<std::string>* RasterizeInterpolateNameList();

    virtual std::string FragVarNameFromVertexVarName(const std::string& name) const;

protected:
    static std::string sGlobalCachePrefix;
    static std::string sFragVaryCachePrefix;

protected:
    std::unordered_map<std::string, Matrix> mMatrixCache;
    std::unordered_map<std::string, Vector4f> mVec4Cache;
    std::unordered_map<std::string, Vector3f> mVec3Cache;
    std::unordered_map<std::string, Vector2f> mVec2Cache;
    std::unordered_map<std::string, float> mFloatCache;

protected:
    std::unordered_map<std::string, Texture> mTextureCache;

protected:
    std::vector<std::string> mRasterizeInterpolateList;

protected:
    std::unordered_map<std::string, ShaderCacheDataType> mVarNameToType;

protected:
    std::unordered_map<ShaderType, ShaderBase*> mShaders;
};

class ShaderBaseVars
{
public:
    // 
    static const std::string MV_MAT;
    static const std::string MVP_MAT;
    static const std::string NORMAL_MAT;
    // 
    static const std::string VertexPos_VEC4;
    static const std::string FragCoord_VEC2;
    static const std::string FragColor_VEC4;
    // ---- ---- ---- ---- ---- ---- ---- ---- 
    // 以下变量 ShaderBase 不再注册，由具体 Shader 注册
    static const std::string VertexPosInput_VEC4;
    static const std::string VertexNormalInput_VEC4;
    //
    static const std::string Texture_01_Texture;
    static const std::string Texture_02_Texture;
    static const std::string Texture_03_Texture;
    static const std::string Texture_04_Texture;
    static const std::string Texture_05_Texture;
};