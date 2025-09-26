#pragma once

#include "shader_base.h"


class ShaderVertexTrivial : public ShaderBase
{
public:
    ShaderVertexTrivial();
    virtual ~ShaderVertexTrivial();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Vertex() override;
};

class ShaderFragmentTrivial : public ShaderBase
{
public:
    ShaderFragmentTrivial();
    virtual ~ShaderFragmentTrivial();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Fragment() override;
};

class ShaderSetupTrivial : public ShaderBase
{
public:
    ShaderSetupTrivial();
    virtual ~ShaderSetupTrivial();

public:
    virtual void OnAddedToPipeline(ShaderPipeline* shaderPipeline) override;
    virtual void ClaimCache() override;
    virtual ShaderType GetShaderType() const override;
};