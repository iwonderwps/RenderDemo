#pragma once


class Screen;

class Rasterizer;
struct RasterizerResultTriangle;

class Clipper;

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();

public:
    virtual void Init();
    virtual void Destroy();

public:
    virtual void Update();
    virtual void Render();

public:
    virtual void TestRenderMesh();
    virtual void TestShaderRender();
    virtual void TestShaderWithTexture();
    virtual void TestShaderWithLight();
    virtual void TestShaderWithPBR();

protected:
    void virtual DrawRasterizedTriangle(const RasterizerResultTriangle&);

private:
    Screen* mScreen;

private:
    Rasterizer* mRasterizer;
    Clipper* mClipper;
};