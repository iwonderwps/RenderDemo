
#include "shader_test.h"
#include "shader_pipeline.h"


ShaderVertexTest::ShaderVertexTest()
{
    mTestVertexIdToColor[0] = Vector4f::create({ 1.0f, 0, 0, 1.0f });
    mTestVertexIdToColor[1] = Vector4f::create({ 0, 1.0f, 0, 1.0f });
    mTestVertexIdToColor[2] = Vector4f::create({ 0, 0, 1.0f, 1.0f });
    mTestVertexIdToColor[3] = Vector4f::create({ 1.0f, 1.0f, 1.0f, 1.0f });
}

ShaderVertexTest::~ShaderVertexTest()
{
}

ShaderType ShaderVertexTest::GetShaderType() const
{
    return ShaderType::VERTEX;
}

void ShaderVertexTest::Vertex()
{
    auto& shaderMV = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);

    auto& vertexPosInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderTestVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    auto& vertexPosOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
    auto& vertexColorOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderTestVars::VaryColor_VEC4, ShaderCacheDataType::VEC4);

    vertexPosOutput = shaderMVP * vertexPosInput;
    //if (mTestVertexIdToColor.find(mTestIndex) != mTestVertexIdToColor.end())
    //{
    //    vertexColorOutput = mTestVertexIdToColor[mTestIndex];
    //}
    //else
    //{
    //    vertexColorOutput = Vector4f::zero();
    //}
    vertexColorOutput = mTestVertexIdToColor[mTestIndex % 3];

    ++mTestIndex;
}

ShaderFragTest::ShaderFragTest()
{
}

ShaderFragTest::~ShaderFragTest()
{
}

ShaderType ShaderFragTest::GetShaderType() const
{
    return ShaderType::FRAGMENT;
}

void ShaderFragTest::Fragment()
{
    auto& varyColorInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderTestVars::VaryColor_VEC4), ShaderCacheDataType::VEC4);

    auto& fragColor = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);

    fragColor = varyColorInput;
}

ShaderSetupTest::ShaderSetupTest()
{
}

ShaderSetupTest::~ShaderSetupTest()
{
}

void ShaderSetupTest::OnAddedToPipeline(ShaderPipeline* shaderPipeline)
{
    ShaderBase::OnAddedToPipeline(shaderPipeline);
    ClaimCache();
}

void ShaderSetupTest::ClaimCache()
{
    ShaderBase::ClaimCache();

    mShaderPipeline->RegisterCacheData(ShaderTestVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);

    mShaderPipeline->RegisterRasterizeInterpolate(ShaderTestVars::VaryColor_VEC4);
    mShaderPipeline->RegisterCacheData(ShaderTestVars::VaryColor_VEC4, ShaderCacheDataType::VEC4);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderTestVars::VaryColor_VEC4), ShaderCacheDataType::VEC4);
}

ShaderType ShaderSetupTest::GetShaderType() const
{
    return ShaderType::SHADER_SETUP;
}

const std::string ShaderTestVars::VertexPosInput_VEC4 = "vec4_vertex_pos_input";
const std::string ShaderTestVars::VaryColor_VEC4 = "vec4_vary_color";

