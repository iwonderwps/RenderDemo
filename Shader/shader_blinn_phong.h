#pragma once

#include <string>
#include "shader_base.h"
#include "vector.h"


class ShaderBlinnPhongVertex : public ShaderBase
{
public:
    ShaderBlinnPhongVertex();
    virtual ~ShaderBlinnPhongVertex();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Vertex() override;
};

class ShaderBlinnPhongFragment : public ShaderBase
{
public:
    ShaderBlinnPhongFragment();
    virtual ~ShaderBlinnPhongFragment();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Fragment() override;
};

class ShaderBlinnPhongSetup : public ShaderBase
{
public:
    ShaderBlinnPhongSetup();
    virtual ~ShaderBlinnPhongSetup();

public:
    virtual void OnAddedToPipeline(ShaderPipeline* shaderPipeline) override;
    virtual void ClaimCache() override;
    virtual ShaderType GetShaderType() const override;
};


class ShaderBlinnPhongVars
{
public:
    static const std::string VertPosView_VEC3;
    static const std::string LightDirView_VEC3;
    static const std::string NormalView_VEC3;
    static const std::string HalfVec_VEC3;
    //
    static const std::string LightPosViewInput_VEC3;
    static const std::string LightKaInput_VEC3;
    static const std::string LightKdInput_VEC3;
    static const std::string LightKsInput_VEC3;
    //
    static const std::string ModelKaInput_VEC3;
    static const std::string ModelKdInput_VEC3;
    static const std::string ModelKsInput_VEC3;
    static const std::string ModelShininessInput_FLOAT;
    //
    static const std::string EnvAmbientInput_VEC3;
};