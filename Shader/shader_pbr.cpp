#include "shader_pbr.h"
#include "shader_pipeline.h"

static constexpr float PI = 3.1415926F;
static constexpr float PI_INV = 1.0F / PI;
static const Vector3f ONE_VEC3 = Vector3f::create({ 1.0f, 1.0f, 1.0f });

static Vector3f Mix(const Vector3f& x, const Vector3f& y, const float t)
{
    return x * (1 - t) + y * t;
}

ShaderPBRVertex::ShaderPBRVertex()
{
}

ShaderPBRVertex::~ShaderPBRVertex()
{
}

ShaderType ShaderPBRVertex::GetShaderType() const
{
    return ShaderType::VERTEX;
}

void ShaderPBRVertex::Vertex()
{
    auto& shaderMV = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderNormalMat = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::NORMAL_MAT, ShaderCacheDataType::MATRIX);

    auto& vertexPosInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    auto& vertexNormalInput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);

    auto& vertexPosOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);

    auto& vertPosViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderPBRVars::VertPosView_VEC3, ShaderCacheDataType::VEC3);
    auto& normalViewOutput = RefShaderPiplineCachedData(mShaderPipeline, ShaderPBRVars::NormalView_VEC3, ShaderCacheDataType::VEC3);

    vertexPosOutput = shaderMVP * vertexPosInput;

    vertPosViewOutput.assign(shaderMV * vertexPosInput);
    normalViewOutput.assign(shaderNormalMat * vertexNormalInput);
}

ShaderPBRFragment::ShaderPBRFragment()
{
}

ShaderPBRFragment::~ShaderPBRFragment()
{
}

ShaderType ShaderPBRFragment::GetShaderType() const
{
    return ShaderType::FRAGMENT;
}

void ShaderPBRFragment::Fragment()
{
    auto& envAmbient = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderPBRVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);

    auto& lightPositionsMat = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderPBRVars::LightPositionsInput_MATRIX, ShaderCacheDataType::MATRIX);
    auto& lightColrosMat = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderPBRVars::LightColorsInput_MATRIX, ShaderCacheDataType::MATRIX);

    auto& albedoTex = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE);
    auto& metallicTex = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::Texture_02_Texture, ShaderCacheDataType::TEXTURE);
    auto& roughnessTex = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::Texture_03_Texture, ShaderCacheDataType::TEXTURE);
    auto& aoTex = RefShaderPiplineGlobalCachedData(mShaderPipeline, ShaderBaseVars::Texture_04_Texture, ShaderCacheDataType::TEXTURE);

    auto& fragCoord = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);

    auto& fragColor = RefShaderPiplineCachedData(mShaderPipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);

    auto& vertPosViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderPBRVars::VertPosView_VEC3), ShaderCacheDataType::VEC3);
    auto& normalViewInput = RefShaderPiplineCachedData(mShaderPipeline, mShaderPipeline->FragVarNameFromVertexVarName(ShaderPBRVars::NormalView_VEC3), ShaderCacheDataType::VEC3);

    static const Vector3f F0Base = Vector3f::ones() * 0.04f;
    static const float r = 2.2f;
    static const float r_inv = 1.0f / r;

    float u = fragCoord[0], v = fragCoord[1];
    Vector3f albedo = albedoTex.Sample(u, v).pow(r);
    float metallic = metallicTex.Sample(u, v)[0];
    float roughness = roughnessTex.Sample(u, v)[0];
    float ao = aoTex.Sample(u, v)[0];

    size_t numLight = lightPositionsMat.rowNum();
    
    Vector3f N = normalViewInput.normalize();
    Vector3f V = (-1.0f * vertPosViewInput).normalize();

    Vector3f F0 = Mix(F0Base, albedo, metallic);

    Vector3f Lo = Vector3f::zero();
    for (size_t i = 0; i < numLight; ++i)
    {
        Vector3f lightPosViewInput = Vector3f::create({ lightPositionsMat(i, 0), lightPositionsMat(i, 1), lightPositionsMat(i, 2) });
        Vector3f lightColorInput = Vector3f::create({ lightColrosMat(i, 0), lightColrosMat(i, 1), lightColrosMat(i, 2) });

        Vector3f L = (lightPosViewInput - vertPosViewInput).normalize();
        Vector3f H = (V + L).normalize();
        float distance = (lightPosViewInput - vertPosViewInput).length();
        float attenuation = 1.0f / std::powf(distance, 2.0f);
        Vector3f radiance = lightColorInput * attenuation;

        float NDF = ShaderPBRFuncs::DistributionGGX(N, H, roughness);
        float G = ShaderPBRFuncs::GeometrySmith(N, V, L, roughness);
        Vector3f F = ShaderPBRFuncs::FresnelSchlick(std::max(0.0f, H * V), F0);

        Vector3f kS = F;
        Vector3f kD = ONE_VEC3 - kS;
        kD = (1.0f - metallic) * kD;

        Vector3f numerator = NDF * G * F;
        float denominator = 4.0f * std::max(0.0f, N * V) * std::max(0.0f, N * L) + 0.0001f;
        Vector3f specular = numerator * (1.0f / denominator);

        float NdotL = std::max(0.0f, N * L);
        Lo = Lo + (albedo.hmul(kD) * PI_INV + specular).hmul(radiance) * NdotL;
    }

    Vector3f ambient = envAmbient.hmul(albedo) * ao;
    Vector3f color = ambient + Lo;

    for (int i = 0; i < 3; ++i)
    {
        color[i] = color[i] / (color[i] + 1.0f);
        color[i] = std::powf(color[i], r_inv);
    }

    fragColor = Vector4f::zero().assign(color, 1.0f);
}

ShaderPBRSetup::ShaderPBRSetup()
{
}

ShaderPBRSetup::~ShaderPBRSetup()
{
}

void ShaderPBRSetup::OnAddedToPipeline(ShaderPipeline* shaderPipeline)
{
    ShaderBase::OnAddedToPipeline(shaderPipeline);
    ClaimCache();
}

void ShaderPBRSetup::ClaimCache()
{
    ShaderBase::ClaimCache();

    mShaderPipeline->RegisterGlobalCacheData(ShaderPBRVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterGlobalCacheData(ShaderPBRVars::LightPositionsInput_MATRIX, ShaderCacheDataType::MATRIX);
    mShaderPipeline->RegisterGlobalCacheData(ShaderPBRVars::LightColorsInput_MATRIX, ShaderCacheDataType::MATRIX);

    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE); // 此处用于 albedo
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::Texture_02_Texture, ShaderCacheDataType::TEXTURE); // 此处用于 metallic
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::Texture_03_Texture, ShaderCacheDataType::TEXTURE); // 此处用于 roughness
    mShaderPipeline->RegisterGlobalCacheData(ShaderBaseVars::Texture_04_Texture, ShaderCacheDataType::TEXTURE); // 此处用于 ao
    // ---- // 待补充 normal map

    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
    mShaderPipeline->RegisterCacheData(ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);

    mShaderPipeline->RegisterRasterizeInterpolate(ShaderPBRVars::VertPosView_VEC3);
    mShaderPipeline->RegisterRasterizeInterpolate(ShaderPBRVars::NormalView_VEC3);

    mShaderPipeline->RegisterCacheData(ShaderPBRVars::VertPosView_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(ShaderPBRVars::NormalView_VEC3, ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderPBRVars::VertPosView_VEC3), ShaderCacheDataType::VEC3);
    mShaderPipeline->RegisterCacheData(mShaderPipeline->FragVarNameFromVertexVarName(ShaderPBRVars::NormalView_VEC3), ShaderCacheDataType::VEC3);
}

ShaderType ShaderPBRSetup::GetShaderType() const
{
    return ShaderType::SHADER_SETUP;
}

const std::string ShaderPBRVars::VertPosView_VEC3 = "vert_pos_view_vec3";
const std::string ShaderPBRVars::NormalView_VEC3 = "normal_view_vec3";
const std::string ShaderPBRVars::EnvAmbientInput_VEC3 = "env_ambient_input_vec3";
const std::string ShaderPBRVars::LightPositionsInput_MATRIX = "light_positions_input_matrix";
const std::string ShaderPBRVars::LightColorsInput_MATRIX = "light_colors_input_matrix";

float ShaderPBRFuncs::DistributionGGX(const Vector3f& normal, const Vector3f& half, const float roughness)
{
    float a2 = std::powf(roughness, 2.0f);
    float nh2 = std::powf(std::max(0.0f, normal * half), 2.0f);
    return a2 / (PI * std::powf(nh2 * (a2 - 1.0f) + 1.0f, 2.0f));
}

float ShaderPBRFuncs::GeometrySchlickGGX(const float nv, const float k)
{
    return nv / (nv * (1.0f - k) + k);
}

float ShaderPBRFuncs::GeometrySmith(const Vector3f& normal, const Vector3f& view, const Vector3f& light, const float k)
{
    float nv = std::max(0.0f, normal * view);
    float nl = std::max(0.0f, normal * light);
    float ggx1 = GeometrySchlickGGX(nv, k);
    float ggx2 = GeometrySchlickGGX(nl, k);
    return ggx1 * ggx2;
}

Vector3f ShaderPBRFuncs::FresnelSchlick(const float cosT, const Vector3f& F0)
{
    return F0 + (ONE_VEC3 - F0) * std::powf(1.0f - cosT, 5.0f);
}
