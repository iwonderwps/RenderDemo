#include <iostream>

#include "shader_pipeline.h"
#include "shader_base.h"


std::string ShaderPipeline::sGlobalCachePrefix = "__g_";
std::string ShaderPipeline::sFragVaryCachePrefix = "__f_";

ShaderPipeline::ShaderPipeline()
{
    mMatrixCache.reserve(20);
    mVec4Cache.reserve(40);
    mVec3Cache.reserve(40);
    mVec2Cache.reserve(20);
    mFloatCache.reserve(20);

    mTextureCache.reserve(10);
}

ShaderPipeline::~ShaderPipeline()
{
    for (auto p = mShaders.begin(); p != mShaders.end(); ++p)
    {
        if (p->second != nullptr)
        {
            delete p->second;
        }
    }
    mShaders.clear();

    mMatrixCache.clear();
    mVec4Cache.clear();
    mVec3Cache.clear();
    mVec2Cache.clear();
    mFloatCache.clear();

    mTextureCache.clear();
}

void ShaderPipeline::AddShader(ShaderBase* shader)
{
    auto shaderType = shader->GetShaderType();
    if (mShaders.find(shaderType) == mShaders.end())
    {
        mShaders[shaderType] = shader;
        shader->OnAddedToPipeline(this);
    }
}

ShaderBase* ShaderPipeline::GetShader(ShaderType shaderType)
{
    if (mShaders.find(shaderType) != mShaders.end())
    {
        return mShaders[shaderType];
    }
    return nullptr;
}

void ShaderPipeline::RegisterCacheData(const std::string& name, ShaderCacheDataType dataType)
{
    bool bSuccess = false;
    switch (dataType)
    {
    case ShaderCacheDataType::MATRIX:
        if (mMatrixCache.find(name) == mMatrixCache.end())
        {
            mMatrixCache[name] = Matrix::create(4, 4);
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::VEC4:
        if (mVec4Cache.find(name) == mVec4Cache.end())
        {
            mVec4Cache[name] = Vector4f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::VEC3:
        if (mVec3Cache.find(name) == mVec3Cache.end())
        {
            mVec3Cache[name] = Vector3f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::VEC2:
        if (mVec2Cache.find(name) == mVec2Cache.end())
        {
            mVec2Cache[name] = Vector2f::zero();
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::FLOAT:
        if (mFloatCache.find(name) == mFloatCache.end())
        {
            mFloatCache[name] = 0;
            bSuccess = true;
        }
        break;
    case ShaderCacheDataType::TEXTURE:
        if (mTextureCache.find(name) == mTextureCache.end())
        {
            mTextureCache[name] = Texture();
            bSuccess = true;
        }
        break;
    default:
        break;
    }

    if (bSuccess)
    {
        mVarNameToType[name] = dataType;
    }

    if (!bSuccess)
    {
        std::cout << "failed register shader cache, name: " << name << ", type: " << static_cast<int>(dataType) << std::endl;
    }
}

void ShaderPipeline::RegisterGlobalCacheData(const std::string& name, ShaderCacheDataType dataType)
{
    RegisterCacheData(sGlobalCachePrefix + name, dataType);
}

void* ShaderPipeline::CachedData(const std::string& name, ShaderCacheDataType dataType)
{
    switch (dataType)
    {
    case ShaderCacheDataType::MATRIX:
        if (mMatrixCache.find(name) != mMatrixCache.end())
        {
            return &mMatrixCache[name];
        }
        break;
    case ShaderCacheDataType::VEC4:
        if (mVec4Cache.find(name) != mVec4Cache.end())
        {
            return &mVec4Cache[name];
        }
        break;
    case ShaderCacheDataType::VEC3:
        if (mVec3Cache.find(name) != mVec3Cache.end())
        {
            return &mVec3Cache[name];
        }
        break;
    case ShaderCacheDataType::VEC2:
        if (mVec2Cache.find(name) != mVec2Cache.end())
        {
            return &mVec2Cache[name];
        }
        break;
    case ShaderCacheDataType::FLOAT:
        if (mFloatCache.find(name) != mFloatCache.end())
        {
            return &mFloatCache[name];
        }
        break;
    case ShaderCacheDataType::TEXTURE:
        if (mTextureCache.find(name) != mTextureCache.end())
        {
            return &mTextureCache[name];
        }
        break;
    default:
        break;
    }

    std::cout << "failed get shader cache, name: " << name << ", type: " << static_cast<int>(dataType) << std::endl;
    return nullptr;
}

void* ShaderPipeline::GlobalCachedData(const std::string& name, ShaderCacheDataType dataType)
{
    return CachedData(sGlobalCachePrefix + name, dataType);
}

ShaderCacheDataType ShaderPipeline::GetCacheDataTypeByName(const std::string& name) const
{
    if (mVarNameToType.find(name) != mVarNameToType.end())
    {
        return mVarNameToType.at(name);
    }
    return ShaderCacheDataType::NONE_SHADER_CACHE_DATA_TYPE;
}

void ShaderPipeline::RegisterRasterizeInterpolate(const std::string& name)
{
    mRasterizeInterpolateList.push_back(name);
}

std::vector<std::string>* ShaderPipeline::RasterizeInterpolateNameList()
{
    return &mRasterizeInterpolateList;
}

std::string ShaderPipeline::FragVarNameFromVertexVarName(const std::string& name) const
{
    return sFragVaryCachePrefix + name;
}


const std::string ShaderBaseVars::MV_MAT = "mv_mat";
const std::string ShaderBaseVars::MVP_MAT = "mvp_mat";
const std::string ShaderBaseVars::NORMAL_MAT = "normal_mat";
const std::string ShaderBaseVars::VertexPos_VEC4 = "vertex_pos_vec4";
const std::string ShaderBaseVars::FragCoord_VEC2 = "frag_coord_vec2";
const std::string ShaderBaseVars::FragColor_VEC4 = "frag_color_vec4";
const std::string ShaderBaseVars::VertexPosInput_VEC4 = "vertex_pos_input_vec4";
const std::string ShaderBaseVars::VertexNormalInput_VEC4 = "vertex_normal_input_vec4";
const std::string ShaderBaseVars::Texture_01_Texture = "texture_01_texture";
const std::string ShaderBaseVars::Texture_02_Texture = "texture_02_texture";
const std::string ShaderBaseVars::Texture_03_Texture = "texture_03_texture";
const std::string ShaderBaseVars::Texture_04_Texture = "texture_04_texture";
const std::string ShaderBaseVars::Texture_05_Texture = "texture_05_texture";
