#include "test_sets.h"

int main(int argn, char** argv)
{
    TestSets& testInst = TestSets::GetInstance();

    //testInst.LaunchOne(TestIndex::MATRIX_BASICS_2);
    //testInst.LaunchOne(TestIndex::CLIP_TRIANGLE_LIST);
    //testInst.LaunchOne(TestIndex::RASTER_DRAW_LINE);
    //testInst.LaunchOne(TestIndex::RASTER_DRAW_TRIANGLE);
    //testInst.LaunchOne(TestIndex::LOAD_MESH_FROM_OBJ);
    //testInst.LaunchOne(TestIndex::VERTEX_DATA_OP);
    //testInst.LaunchOne(TestIndex::PROJECTION);
    //testInst.LaunchOne(TestIndex::RENDER_MESH);
    //testInst.LaunchOne(TestIndex::CAMERA_MATRIX);
    //testInst.LaunchOne(TestIndex::SHADER_CACHE);
    //testInst.LaunchOne(TestIndex::SHADER_RENDER);
    //testInst.LaunchOne(TestIndex::LOAD_TEXTURE);
    //testInst.LaunchOne(TestIndex::SHADER_WITH_TEXTURE);
    //testInst.LaunchOne(TestIndex::SHADER_WITH_LIGHT);
    testInst.LaunchOne(TestIndex::SHADER_WITH_PBR);

    return 0;
}