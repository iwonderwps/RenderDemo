#pragma once

#include <string>
#include "shader_base.h"
#include "vector.h"


class ShaderPBRVertex : public ShaderBase
{
public:
    ShaderPBRVertex();
    virtual ~ShaderPBRVertex();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Vertex() override;
};


class ShaderPBRFragment : public ShaderBase
{
public:
    ShaderPBRFragment();
    virtual ~ShaderPBRFragment();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Fragment() override;
};


class ShaderPBRSetup : public ShaderBase
{
public:
    ShaderPBRSetup();
    virtual ~ShaderPBRSetup();

public:
    virtual void OnAddedToPipeline(ShaderPipeline* shaderPipeline) override;
    virtual void ClaimCache() override;
    virtual ShaderType GetShaderType() const override;
};


class ShaderPBRVars
{
public:
    static const std::string VertPosView_VEC3;
    static const std::string NormalView_VEC3;
    //
    static const std::string EnvAmbientInput_VEC3;
    //
    static const std::string LightPositionsInput_MATRIX;
    static const std::string LightColorsInput_MATRIX;
};


class ShaderPBRFuncs
{
    // �հṫʽ
public:
    static float DistributionGGX(const Vector3f& normal, const Vector3f& half, const float roughness);
    static float GeometrySchlickGGX(const float nv, const float k);
    static float GeometrySmith(const Vector3f& normal, const Vector3f& half, const Vector3f& light, const float k);
    static Vector3f FresnelSchlick(const float cosT, const Vector3f& F0);
};