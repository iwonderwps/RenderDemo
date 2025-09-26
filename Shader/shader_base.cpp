#include <stdexcept>
#include "shader_base.h"
#include "shader_pipeline.h"


ShaderBase::ShaderBase():
    mShaderPipeline(nullptr)
{
}

ShaderBase::~ShaderBase()
{
}

void ShaderBase::OnAddedToPipeline(ShaderPipeline* shaderPipeline)
{
    mShaderPipeline = shaderPipeline;
}

void ShaderBase::ClaimCache()
{
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::NORMAL_MAT, ShaderCacheDataType::MATRIX);

    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
    mShaderPipeline->RegisterCacheData(ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
    mShaderPipeline->RegisterCacheData(ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);
}

ShaderType ShaderBase::GetShaderType() const
{
    return ShaderType::NONE_SHADER_TYPE;
}

void ShaderBase::Vertex()
{
    throw std::runtime_error("ShaderBase::Vertex: subclass methods should be called");
}

void ShaderBase::Fragment()
{
    throw std::runtime_error("ShaderBase::Fragment: subclass methods should be called");
}
