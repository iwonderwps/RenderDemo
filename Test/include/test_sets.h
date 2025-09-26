#pragma once

#include <unordered_map>
#include <string>

enum class TestIndex;

class TestSets
{
protected:
    TestSets();
public:
    static TestSets& GetInstance();
    virtual ~TestSets();

public:
    virtual void LaunchAll();
    virtual void LaunchOne(TestIndex idx);

protected:
    std::unordered_map<TestIndex, void (*)(void)> mTestFuncSets;
    std::unordered_map<TestIndex, std::string> mTestIdxNames;
    
};

enum class TestIndex
{
    MATRIX_BASICS = 1,
    MATRIX_BASICS_2,
    VECTOR_BASICS,
    DRAW_GRADIENT_FLAG_IMG,
    CLIP_SEGMENT,
    CLIP_TRIANGLE_LIST,
    RASTER_DRAW_LINE,
    RASTER_DRAW_TRIANGLE,
    LOAD_MESH_FROM_OBJ,
    VERTEX_DATA_OP,
    PROJECTION,
    RENDER_MESH,
    CAMERA_MATRIX,
    SHADER_CACHE,
    SHADER_RENDER,
    LOAD_TEXTURE,
    SHADER_WITH_TEXTURE,
    SHADER_WITH_LIGHT,
    SHADER_WITH_PBR,
};