#include <cmath>
#include "shader_blinn_phong.h"
#include "shader_pipeline.h"


ShaderBlinnPhongVertex::ShaderBlinnPhongVertex()
{
}

ShaderBlinnPhongVertex::~ShaderBlinnPhongVertex()
{
}

ShaderType ShaderBlinnPhongVertex::GetShaderType() const
{
    return ShaderType::VERTEX;
}

void ShaderBlinnPhongVertex::Vertex()
{
    auto& shaderMV = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderNormalMat = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::NORMAL_MAT, ShaderCacheDataType::MATRIX);

    auto& lightPosViewInput = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::LightPosViewInput_VEC3, ShaderCacheDataType::VEC3);

    auto& vertexPosInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    auto& vertexNormalInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);

    auto& vertexPosOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);

    auto& vertPosViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBlinnPhongVars::VertPosView_VEC3, ShaderCacheDataType::VEC3);
    auto& lightDirViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBlinnPhongVars::LightDirView_VEC3, ShaderCacheDataType::VEC3);
    auto& normalViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBlinnPhongVars::NormalView_VEC3, ShaderCacheDataType::VEC3);
    auto& halfVecViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBlinnPhongVars::HalfVec_VEC3, ShaderCacheDataType::VEC3);

    vertexPosOutput = shaderMVP * vertexPosInput;

    vertPosViewOutput.assign(shaderMV * vertexPosInput).normalize();
    lightDirViewOutput = (lightPosViewInput - vertPosViewOutput).normalize();
    normalViewOutput.assign(shaderNormalMat * vertexNormalInput);
    halfVecViewOutput = lightDirViewOutput - vertPosViewOutput;
}

ShaderBlinnPhongFragment::ShaderBlinnPhongFragment()
{
}

ShaderBlinnPhongFragment::~ShaderBlinnPhongFragment()
{
}

ShaderType ShaderBlinnPhongFragment::GetShaderType() const
{
    return ShaderType::FRAGMENT;
}

void ShaderBlinnPhongFragment::Fragment()
{
    auto& envAmbient = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);

    auto& modelKa = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::ModelKaInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelKd = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::ModelKdInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelKs = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::ModelKsInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelShininess = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::ModelShininessInput_FLOAT, ShaderCacheDataType::FLOAT);

    auto& lightKa = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::LightKaInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightKd = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::LightKdInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightKs = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBlinnPhongVars::LightKsInput_VEC3, ShaderCacheDataType::VEC3);

    auto& fragColor = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);

    auto& vertPosViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::VertPosView_VEC3), ShaderCacheDataType::VEC3);
    auto& lightDirViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::LightDirView_VEC3), ShaderCacheDataType::VEC3);
    auto& normalViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::NormalView_VEC3), ShaderCacheDataType::VEC3);
    auto& halfVecViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::HalfVec_VEC3), ShaderCacheDataType::VEC3);

    Vector3f normalDir = normalViewInput.normalize();
    Vector3f halfVec = halfVecViewInput.normalize();
    float cosHN = halfVec.dot(normalDir);

    Vector3f a = modelKa.hmul(envAmbient) + modelKa.hmul(lightKa);
    Vector3f d = modelKd.hmul(lightKd) * std::max(0.0f, cosHN);
    Vector3f s = modelKs.hmul(lightKs) * std::powf(std::max(0.0f, cosHN), modelShininess * 3.0f);

    Vector4f lighting;
    lighting.assign(a + d + s, 1.0f);

    fragColor = lighting;
}

ShaderBlinnPhongSetup::ShaderBlinnPhongSetup()
{
}

ShaderBlinnPhongSetup::~ShaderBlinnPhongSetup()
{
}

void ShaderBlinnPhongSetup::OnAddedToPipeline(ShaderPipeline* shaderPipeline)
{
    ShaderBase::OnAddedToPipeline(shaderPipeline);
    ClaimCache();
}

void ShaderBlinnPhongSetup::ClaimCache()
{
    ShaderBase::ClaimCache();

    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::LightKaInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::LightKdInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::LightKsInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::LightPosViewInput_VEC3, ShaderCacheDataType::VEC3);

    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::ModelKaInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::ModelKdInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::ModelKsInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::ModelShininessInput_FLOAT, ShaderCacheDataType::FLOAT);

    mShaderPipeline->RegisterGlobalCacheData(ShaderBlinnPhongVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);

    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);

    mShaderPipeline->RegisterRasterizeInterpolate(ShaderBlinnPhongVars::VertPosView_VEC3);
    mShaderPipeline->RegisterRasterizeInterpolate(ShaderBlinnPhongVars::LightDirView_VEC3);
    mShaderPipeline->RegisterRasterizeInterpolate(ShaderBlinnPhongVars::NormalView_VEC3);
    mShaderPipeline->RegisterRasterizeInterpolate(ShaderBlinnPhongVars::HalfVec_VEC3);

    mShaderPipeline->RegisterCacheData(ShaderBlinnPhongVars::VertPosView_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(ShaderBlinnPhongVars::LightDirView_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(ShaderBlinnPhongVars::NormalView_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(ShaderBlinnPhongVars::HalfVec_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::VertPosView_VEC3), ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::LightDirView_VEC3), ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::NormalView_VEC3), ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderBlinnPhongVars::HalfVec_VEC3), ShaderCacheDataType::VEC3);
}

ShaderType ShaderBlinnPhongSetup::GetShaderType() const
{
    return ShaderType::SHADER_SETUP;
}

const std::string ShaderBlinnPhongVars::VertPosView_VEC3 = "vert_pos_view_vec3";
const std::string ShaderBlinnPhongVars::LightDirView_VEC3 = "light_dir_view_vec3";
const std::string ShaderBlinnPhongVars::NormalView_VEC3 = "normal_view_vec3";
const std::string ShaderBlinnPhongVars::HalfVec_VEC3 = "half_vec_vec3";
//
const std::string ShaderBlinnPhongVars::LightPosViewInput_VEC3 = "light_pos_view_input_vec3";
const std::string ShaderBlinnPhongVars::LightKaInput_VEC3 = "light_ka_input_vec3";
const std::string ShaderBlinnPhongVars::LightKdInput_VEC3 = "light_kd_input_vec3";
const std::string ShaderBlinnPhongVars::LightKsInput_VEC3 = "light_ks_input_vec3";
//
const std::string ShaderBlinnPhongVars::ModelKaInput_VEC3 = "model_ka_input_vec3";
const std::string ShaderBlinnPhongVars::ModelKdInput_VEC3 = "model_kd_input_vec3";
const std::string ShaderBlinnPhongVars::ModelKsInput_VEC3 = "model_ks_input_vec3";
const std::string ShaderBlinnPhongVars::ModelShininessInput_FLOAT = "model_shininess_input_float";
//
const std::string ShaderBlinnPhongVars::EnvAmbientInput_VEC3 = "env_ambient_input_vec3";
