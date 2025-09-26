#pragma once

#include <string>
#include <unordered_map>
#include "shader_base.h"
#include "vector.h"


class ShaderVertexTest : public ShaderBase
{
public:
    ShaderVertexTest();
    virtual ~ShaderVertexTest();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Vertex() override;

protected:
    std::unordered_map<int, Vector4f> mTestVertexIdToColor;

protected:
    int mTestIndex;
};


class ShaderFragTest : public ShaderBase
{
public:
    ShaderFragTest();
    virtual ~ShaderFragTest();

public:
    virtual ShaderType GetShaderType() const override;

public:
    virtual void Fragment() override;
};

class ShaderSetupTest : public ShaderBase
{
public:
    ShaderSetupTest();
    virtual ~ShaderSetupTest();

public:
    virtual void OnAddedToPipeline(ShaderPipeline* shaderPipeline) override;
    virtual void ClaimCache() override;
    virtual ShaderType GetShaderType() const override;
};


class ShaderTestVars
{
public:
    static const std::string VertexPosInput_VEC4;
    static const std::string VaryColor_VEC4;
};