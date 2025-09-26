#include <memory>

#include "renderer.h"
#include "camera.h"
#include "mesh_data.h"
#include "vertex_data.h"
#include "rasterizer.h"
#include "image_screen.h"
#include "time_utility.h"
#include "camera.h"
#include "mesh.h"
#include "clipper.h"
#include "printer.h"


#define _DELETE_POINTER(p) \
{ \
    if (p != nullptr) \
    { \
        delete p; \
        p = nullptr; \
    } \
}

Renderer::Renderer():
    mScreen(nullptr), mRasterizer(nullptr), mClipper(nullptr)
{
    
}

Renderer::~Renderer()
{
    _DELETE_POINTER(mScreen);
    _DELETE_POINTER(mRasterizer);
    _DELETE_POINTER(mClipper);
}

void Renderer::Init()
{
    mScreen = new ImageScreen();
    mRasterizer = new Rasterizer();
    mClipper = new Clipper();
}

void Renderer::Destroy()
{
    _DELETE_POINTER(mScreen);
    _DELETE_POINTER(mRasterizer);
    _DELETE_POINTER(mClipper);
}

void Renderer::Update()
{
}

void Renderer::Render()
{
}

void Renderer::DrawRasterizedTriangle(const RasterizerResultTriangle&)
{
}
