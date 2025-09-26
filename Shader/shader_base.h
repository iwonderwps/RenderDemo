#pragma once


#include "shader_common_types.h"


class ShaderPipeline;

class ShaderBase
{
public:
    ShaderBase();
    virtual ~ShaderBase();

public:
    virtual void OnAddedToPipeline(ShaderPipeline* shaderPipeline);

public:
    virtual void ClaimCache();

public:
    virtual ShaderType GetShaderType() const;

public:
    virtual void Vertex();
    virtual void Fragment();

protected:
    ShaderPipeline* mShaderPipeline;
};

