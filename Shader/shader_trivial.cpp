#include "shader_trivial.h"
#include "shader_pipeline.h"


ShaderVertexTrivial::ShaderVertexTrivial()
{
}

ShaderVertexTrivial::~ShaderVertexTrivial()
{
}

ShaderType ShaderVertexTrivial::GetShaderType() const
{
    return ShaderType::VERTEX;
}

void ShaderVertexTrivial::Vertex()
{
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);

    auto& vertexPosInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    auto& vertexPosOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);

    vertexPosOutput = shaderMVP * vertexPosInput;
}

ShaderFragmentTrivial::ShaderFragmentTrivial()
{
}

ShaderFragmentTrivial::~ShaderFragmentTrivial()
{
}

ShaderType ShaderFragmentTrivial::GetShaderType() const
{
    return ShaderType::FRAGMENT;
}

void ShaderFragmentTrivial::Fragment()
{
    auto& texture = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE);

    auto& fragCoord = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
    auto& fragColor = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);

    Vector3f color = texture.Sample(fragCoord[0], fragCoord[1]);
    fragColor = Vector4f::create({ color[0], color[1], color[2], 1.0f });
    //fragColor = Vector4f::create({ 0, 1, 0, 1 });
}

ShaderSetupTrivial::ShaderSetupTrivial()
{
}

ShaderSetupTrivial::~ShaderSetupTrivial()
{
}

void ShaderSetupTrivial::OnAddedToPipeline(ShaderPipeline* shaderPipeline)
{
    ShaderBase::OnAddedToPipeline(shaderPipeline);
    ClaimCache();
}

void ShaderSetupTrivial::ClaimCache()
{
    ShaderBase::ClaimCache();

    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE);

    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
}

ShaderType ShaderSetupTrivial::GetShaderType() const
{
    return ShaderType::SHADER_SETUP;
}
