#include <iostream>
#include <memory>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "test_sets.h"

#include "matrix.h"
#include "vector.h"

#include "printer.h"

#include "camera.h"
#include "clipper.h"

#include "rasterizer.h"

#include "mesh_data.h"

#include "vertex_data.h"

#include "renderer.h"

#include "shader_pipeline.h"
#include "shader_base.h"

#include "texture.h"


using namespace std;


static unique_ptr<TestSets> pTestSetsInst(nullptr);


static void TestMatrixBasics()
{
    cout << "main.TestMatrixBasics - --" << endl;

    Matrix m1 = Matrix::create(4, 4);
    Matrix m2 = Matrix::identity(4);

    Printer::PrintMatrixContent(m1 + m2);
    Printer::PrintMatrixContent(m1 - m2);

    Matrix m3 = m1 * m2;
    Printer::PrintMatrixInfo(m3);
    Printer::PrintMatrixContent(m3);
}

static void TestMatrixBasics2()
{
    Matrix m1 = Matrix::identity(4);
    m1 = m1 * 3.0f;
    Matrix m2 = m1.inverse();

    Printer::PrintMatrixContent(m2);
    Printer::PrintMatrixContent(m1 * m2);
}

static void TestVectorBasics()
{
    cout << "main.TestVectorBasics - --" << endl;

    Matrix m1 = Matrix::identity(4);
    Matrix m2 = Matrix::identity(4);
    Matrix m3 = m1 + m2;
    Printer::PrintMatrixContent(m3);

    Vector4f v4f = Vector4f::ones();
    Printer::PrintVectorContent(v4f);
    Printer::PrintVectorContent(m3 * v4f);

    Vector3f v3f1 = Vector3f::create({ 1, 0, 0 });
    Vector3f v3f2 = Vector3f::create({ 0, 1, 0 });
    Printer::PrintVectorContent(v3f1 % v3f2);

    Vector2i v2i1 = Vector2i::create({ 1, 0 });
    Vector2i v2i2 = Vector2i::create({ 0, 1 });
    Printer::PrintValue(v2i1 % v2i2);
    Printer::PrintValue(v2i2 % v2i1);
    Printer::PrintValue(v2i1 % v2i1);

}

static void TestDrawGradientFlag() {
    int width = 800;
    int height = 800;
    int channelNum = 3;

    uint8_t* rgb_image;
    rgb_image = new uint8_t[width * height * channelNum];

    int offset = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t r = 0, g = 0, b = 0;
            if (y < height / 3) {
                r = static_cast<uint8_t>(float(x) / width * 255);
            }
            else if (y < height * 2 / 3) {
                g = static_cast<uint8_t>(float(x) / width * 255);
            }
            else {
                b = static_cast<uint8_t>(float(x) / width * 255);
            }
            rgb_image[offset++] = r;
            rgb_image[offset++] = g;
            rgb_image[offset++] = b;
        }
    }

    stbi_write_png("Output/TestDrawGradientFlag.png", width, height, channelNum, rgb_image, width * channelNum);

    delete[] rgb_image;
    rgb_image = nullptr;
}

static void TestClipLineSeg() {
    float n = -0.5f, f = -1.5f;
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };

    float Y0 = -1 * n * tan(fov / 2) * 2;
    float X0 = Y0 * aspect;

    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    // 检查投影
    Vector4f pt1 = Vector4f::create({ 0, 0, n, 1 });
    Vector4f pt2 = Vector4f::create({ 0, 0, (n + f) / 2 , 1 });
    Vector4f pt3 = Vector4f::create({ 0, 0, f , 1 });

    auto pt1ndcd = mpNDC * pt1;
    auto pt1ndc = pt1ndcd.perspectiveDivide();
    Printer::PrintVectorContent(pt1ndc);
    auto pt2ndcd = mpNDC * pt2;
    auto pt2ndc = pt2ndcd.perspectiveDivide();
    Printer::PrintVectorContent(pt2ndc);
    auto pt3ndcd = mpNDC * pt3;
    auto pt3ndc = pt3ndcd.perspectiveDivide();
    Printer::PrintVectorContent(pt3ndc);

    cout << endl;

    // 检查线段裁剪
    // 初始化顶点数据
    Clipper clipper;
    Vector4f v1, v2, x1, x2;
    auto transformThenPrint = [&clipper, &mpNDC, &v1, &v2, &x1, &x2]() {
        // 变换顶点坐标
        v1 = mpNDC * v1; 
        v2 = mpNDC * v2;
        // 在齐次空间裁剪
        bool bExist = clipper.clipLineSeg(v1, v2, x1, x2);
        cout << "clipping result:" << endl;
        if (bExist) {
            Printer::PrintVectorContent(x1.perspectiveDivide());
            Printer::PrintVectorContent(x2.perspectiveDivide());
        }
        else {
            cout << "clip out" << endl;
        }
        cout << endl;
    };
    // 测试1
    v1 = Vector4f::create({ 0, 0, (n + f) / 2, 1 });
    v2 = Vector4f::create({ X0 / 2, 0, -n, 1 });
    transformThenPrint();
    // 测试2
    v1 = Vector4f::create({ X0 / 2, Y0 / 2, n, 1 });
    v2 = Vector4f::create({ 0, 0, -n, 1 });
    transformThenPrint();
    // 测试3
    v1 = Vector4f::create({ -X0 / 2, -Y0 / 2, n, 1 });
    v2 = Vector4f::create({ X0 / 2 / n * f, Y0 / 2 / n * f, f, 1 });
    transformThenPrint();
    // 测试4
    v1 = Vector4f::create({ -X0 / 2, -Y0 / 2, n, 1 });
    v2 = Vector4f::create({ X0 / n * f, Y0 / n * f, f, 1 });
    transformThenPrint();
    // 测试5
    v1 = Vector4f::create({ 0, 0, 0, 1 });
    v2 = Vector4f::create({ 0, 0, n + f, 1 });
    transformThenPrint();
}

static void TestClipIndexedTriangleList()
{
    // 测试缓存
    Clipper::IndexedTriListClippingEdgeRecordType mpRecEdge;
    Clipper::TriangleEdgeIndex edge1(0, 1), edge2(1, 0), edge3(1, 2);
    mpRecEdge[edge1] = 1;
    cout << "find?: " << (mpRecEdge.find(edge2) != mpRecEdge.end()) << endl;
    cout << "find?: " << (mpRecEdge.find(edge3) != mpRecEdge.end()) << endl;
    cout << endl;

    // 相机设置
    float n = -0.5f, f = -1.5f;
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };

    float Y0 = -1 * n * tan(fov / 2) * 2;
    float X0 = Y0 * aspect;

    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    // 测试裁剪三角形构成的图形
    // std::vector<Vector4f>; 最开始的测试是 Vector4f 只有位置数据。 迭代改为 VertexData 包含全部顶点数据
    using ListVtx = std::vector<VertexData>;
    using ListPosVtx = std::vector<Vector4f>;
    using ListIdx = std::vector<size_t>;

    Clipper clipper;

    ListPosVtx outVtxList;
    ListIdx outIdxList;

    ListVtx vdataList;
    ListVtx outVdataList;
    VertexData tempVertexData({ VertexDataAttribute::POSITION });

    auto transformAndPrintClippedTriangle = [&clipper, &mpNDC, &n, &f, &X0, &Y0, &outVtxList, &outIdxList, &vdataList, &outVdataList, &tempVertexData](ListPosVtx& _vtxList, ListIdx& _idxList)
        {
            for (size_t i = 0; i < _vtxList.size(); ++i) {
                _vtxList[i] = mpNDC * _vtxList[i];
            }

            vdataList.clear();
            for (size_t i = 0; i < _vtxList.size(); ++i){
                tempVertexData[POSITION] = _vtxList[i];
                vdataList.push_back(tempVertexData);
            }

            auto numTri = clipper.clipIndexedTriangleList(vdataList, _idxList, outVdataList, outIdxList);
            cout << "num triangle after clipping: " << numTri;
            cout << endl;
            if (numTri == 0) {
                cout << "clipped out" << endl;
                return;
            }

            cout << "vertex:" << endl;
            for (size_t i = 0; i < outVdataList.size(); ++i) {
                cout << "[" << i << "]: ";
                Printer::PrintVectorContent(outVdataList[i][VertexDataAttribute::POSITION].perspectiveDivide());
            }

            cout << "index: " << endl;
            for (size_t i = 0; i < outIdxList.size(); ++i) {
                if (i % 3 == 0) {
                    cout << "Tri [" << i / 3 << "]: ";
                }
                cout << outIdxList[i] << ", ";
                if (i % 3 == 2) {
                    cout << endl;
                }
            }

            cout << endl;
        };

    // 三角形1 - 2个顶点在视椎体外
    ListPosVtx vtxList1 = {
        Vector4f::create({0, 0, (n + f) / 2, 1}),
        Vector4f::create({-X0 / 4, 0, n + 0.1f, 1}) ,
        Vector4f::create({X0 / 4, 0, n + 0.1f, 1}) ,
    };
    ListIdx idxList1 = {
        1, 0, 2,
    };
    transformAndPrintClippedTriangle(vtxList1, idxList1);

    // 三角形2 - 1个顶点在视椎体外
    ListPosVtx vtxList2 = {
        Vector4f::create({-X0 / 2, 0, (n + f) / 2, 1}),
        Vector4f::create({X0 / 2, 0, (n + f) / 2, 1}) ,
        Vector4f::create({0, 0, n + 0.1f, 1}) ,
    };
    ListIdx idxList2 = {
        0, 1, 2,
    };
    transformAndPrintClippedTriangle(vtxList2, idxList2);

    // 四边形1 - 一半在视椎体外
    ListPosVtx vtxList3 = {
        Vector4f::create({-X0 / 4, 0, n - 0.1f, 1}),
        Vector4f::create({X0 / 4, 0, n - 0.1f, 1}) ,
        Vector4f::create({X0 / 4, 0, n + 0.1f, 1}) ,
        Vector4f::create({-X0 / 4, 0, n + 0.1f, 1}) ,
    };
    ListIdx idxList3 = {
        0, 1, 3,
        1, 2, 3,
    };
    transformAndPrintClippedTriangle(vtxList3, idxList3);

    // 三角形3 - 全部在内 逆时针
    ListPosVtx vtxList4 = {
        Vector4f::create({-X0 / 2, 0, (n + f) / 2, 1}),
        Vector4f::create({X0 / 2, 0, (n + f) / 2, 1}) ,
        Vector4f::create({0, 0, n - 0.1f, 1}) ,
    };
    // 注意这个是逆时针缠绕。预期输出保持原有缠绕顺序（即，也输出逆时针），目前输出的缠绕方向符合预期（输出的下标列表仍是0, 1, 2 ，输出的顶点列表逆序）。
    ListIdx idxList4 = {
        2, 1, 0,
    };
    transformAndPrintClippedTriangle(vtxList4, idxList4);

    // 三角形4 - 全部在内 顺时针
    ListPosVtx vtxList5 = {
        Vector4f::create({-X0 / 2, 0, (n + f) / 2, 1}),
        Vector4f::create({X0 / 2, 0, (n + f) / 2, 1}) ,
        Vector4f::create({0, 0, n - 0.1f, 1}) ,
    };
    ListIdx idxList5 = {
        0, 1, 2,
    };
    transformAndPrintClippedTriangle(vtxList5, idxList5);

    // 四边形2 - 全部在外
    ListPosVtx vtxList6 = {
        Vector4f::create({-X0 / 4, 0, 0, 1}),
        Vector4f::create({X0 / 4, 0, 0, 1}) ,
        Vector4f::create({X0 / 4, 0, -n, 1}) ,
        Vector4f::create({-X0 / 4, 0, -n, 1}) ,
    };
    ListIdx idxList6 = {
        0, 1, 3,
        1, 2, 3,
    };
    transformAndPrintClippedTriangle(vtxList6, idxList6);
}

static float ScreenToNdcIntParam(int p, int A)
{
    return static_cast<float>(p) / static_cast<float>(A) * 2.0f - 1.0f;
}
static void ShowRasterDrawLineResult(const RasterizerResultLine& lineResult)
{
    cout << "x: " << lineResult.xScreen << ", y: " << lineResult.yScreen << ", t: " << lineResult.tInterpolate << endl;
}
static void TestRasterDrawLine()
{
    Rasterizer rasterizer;

    int screenWidth = 10, screenHeight = 10;
    Vector4f p1, p2;
    int testNum = 0;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight)});
    p2 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(5, screenHeight)});
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(5, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(5, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(5, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(5, screenWidth), ScreenToNdcIntParam(0, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;

    p1 = Vector4f::create({ ScreenToNdcIntParam(0, screenWidth), ScreenToNdcIntParam(10, screenHeight) });
    p2 = Vector4f::create({ ScreenToNdcIntParam(10, screenWidth), ScreenToNdcIntParam(5, screenHeight) });
    cout << "test idx: " << testNum++ << endl;
    rasterizer.DrawLine(p1, p2, screenWidth, screenHeight, ShowRasterDrawLineResult);
    cout << endl;
}

static void ShowRasterDrawTriangleResult(const RasterizerResultTriangle& triangleResult)
{
    float ac = triangleResult.apzBary * triangleResult.depthCorrected;
    float bc = triangleResult.bpzBary * triangleResult.depthCorrected;
    float cc = triangleResult.cpzBary * triangleResult.depthCorrected;

    cout << "x: " << triangleResult.xScreen << ", y: " << triangleResult.yScreen \
        << ", dp: " << triangleResult.depthProjected << ", dc: " << triangleResult.depthCorrected \
        << ", aForAttr: " << ac << ", bForAttr: " << bc << ", cForAttr: " << cc << ", sumABC: " << ac + bc + cc << endl;
}
static void TestRasterizerDrawTriangle()
{
    Rasterizer rasterizer;
    using ListVtx = std::vector<Vector4f>;

    int screenWidth = 10, screenHeight = 10;

    float n = -0.5f, f = -1.5f;
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    float Y0 = -1 * n * tan(fov / 2) * 2;
    float X0 = Y0 * aspect;

    ListVtx aTriangle = {
        Vector4f::create({-X0 / 2, 0, n, 1}),
        Vector4f::create({X0 / 2, 0, n, 1}) ,
        Vector4f::create({0, Y0 / 2, n, 1}) ,
    };

    ListVtx aTriangleNdc(3);

    for (size_t i = 0; i < aTriangle.size(); ++i) {
        aTriangleNdc[i] = mpNDC * aTriangle[i];
        aTriangleNdc[i].perspectiveDivideInPlace();
    }
    
    rasterizer.DrawTiangle(aTriangleNdc.data(), screenWidth, screenHeight, ShowRasterDrawTriangleResult);
}

static void TestLoadMeshFromObjFile()
{
    MeshData meshData;
    MeshData::LoadMeshFromFile("Res/teapot_mesh.obj", meshData);
    //MeshData::LoadMeshFromFile("Res/test_load.obj", meshData);
    Printer::PrintMeshDataInfo(meshData);
}

static void TestVertexDataOp()
{

    VertexData da{ VertexDataAttribute::POSITION, VertexDataAttribute::UV };
    VertexData db{ VertexDataAttribute::POSITION, VertexDataAttribute::UV };

    da[POSITION] = Vector4f::create({ 1.0f, 0.0f, 0.0f, 0.0f });
    db[POSITION] = Vector4f::create({ 2.0f, 0.0f, 0.0f, 0.0f });

    da[UV] = Vector4f::create({ 20.0f, 0.0f, 0.0f, 0.0f });
    db[UV] = Vector4f::create({ 30.0f, 0.0f, 0.0f, 0.0f });

    Printer::PrintVertexData(VertexData::Lerp(da, db, 0.6f));

    Matrix m1 = Matrix::translation3d(-1, -1, -1);
    da[POSITION][3] = 1.0f;
    da[POSITION] = m1 * da[POSITION];
    Printer::PrintVertexData(da);
}

static void TestProjection()
{
    Camera camera;

    float n = -0.1f, f = -1000.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    camera.SetCameraConfig(CameraConfig(n, f, fov, aspect));
    camera.SetCameraPosition(Vector3f::create({ 0, 0, 10 }));
    camera.LookAt(Vector3f::create({ 0, 0, 0 }), Vector3f::create({0, 1, 0}));
    
    Matrix view = camera.ViewMatrix();
    Matrix proj = camera.ProjectionMatrix();

    Printer::PrintMatrixContent(view);
    Printer::PrintMatrixContent(proj);

    Vector4f pos = Vector4f::create({ 2.5f, 0, 0 , 1});
    Printer::PrintVectorContent(pos);
    auto posViewed = view * pos;
    Printer::PrintVectorContent(posViewed);
    auto posProjcted = proj * posViewed;
    Printer::PrintVectorContent(posProjcted);
    auto posDived = posProjcted.perspectiveDivide();
    Printer::PrintVectorContent(posDived);
}

static void TestRenderMesh()
{
    Renderer renderer;

    renderer.Init();
    
    renderer.TestRenderMesh();
    
    renderer.Destroy();
}

static void TestCameraMatrix()
{
    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };

    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    Vector3f pos = Vector3f::create({ 1, 1, 1, });
    Vector3f target = Vector3f::zero();
    const Vector4f tpos = Vector4f::create({ 0, 0, 0, 1 });

    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(pos);
    
    Matrix cameraViewMat = camera.ViewMatrix();
    Printer::PrintMatrixContent(cameraViewMat);
    Printer::PrintVectorContent(cameraViewMat * tpos);

    camera.LookAt(camera.GetPosition() + Vector3f::create({ 0, 0, -1.0f }), upWorld);
    cameraViewMat = camera.ViewMatrix();
    Printer::PrintMatrixContent(cameraViewMat);
    Printer::PrintVectorContent(cameraViewMat * tpos);

    cameraViewMat = camera.LookAt(target, upWorld).ViewMatrix();
    Printer::PrintMatrixContent(cameraViewMat);
    Printer::PrintVectorContent(cameraViewMat * tpos);
}

static void TestShaderCache()
{
    ShaderPipeline shaderPipeline;

    shaderPipeline.RegisterGlobalCacheData("mvp", ShaderCacheDataType::MATRIX);
    shaderPipeline.RegisterCacheData("half_vec", ShaderCacheDataType::VEC3);
    
    auto& mvp = RefShaderPiplineGlobalCachedData(&shaderPipeline, "mvp", ShaderCacheDataType::MATRIX);
    auto& halv_vec = RefShaderPiplineCachedData(&shaderPipeline, "half_vec", ShaderCacheDataType::VEC3);

    Printer::PrintMatrixContent(mvp);
    Printer::PrintVectorContent(halv_vec);

    mvp = Matrix::identity(4) * 2.0f;
    halv_vec = halv_vec + Vector3f::create({ 1.0f, 2.0f, 0 });

    Printer::PrintMatrixContent(RefShaderPiplineGlobalCachedData(&shaderPipeline, "mvp", ShaderCacheDataType::MATRIX));
    Printer::PrintVectorContent(RefShaderPiplineCachedData(&shaderPipeline, "half_vec", ShaderCacheDataType::VEC3));
}

static void TestShaderRender()
{
    Renderer renderer;

    renderer.Init();

    renderer.TestShaderRender();

    renderer.Destroy();
}

static void TestLoadTexture()
{
    string filePath = "Res/TestDrawGradientFlag.png";
    Texture texture;
    texture.LoadTextureFromFile(filePath);

    Printer::PrintVectorContent(texture.Sample(0, 0));
    Printer::PrintVectorContent(texture.Sample(1, 0));

    Printer::PrintVectorContent(texture.Sample(0, 0.5));
    Printer::PrintVectorContent(texture.Sample(1, 0.5));

    Printer::PrintVectorContent(texture.Sample(0, 1));
    Printer::PrintVectorContent(texture.Sample(1, 1));
}

static void TestShaderWithTexture()
{
    Renderer renderer;

    renderer.Init();

    renderer.TestShaderWithTexture();

    renderer.Destroy();
}

static void TestShaderWithLight()
{
    Renderer renderer;

    renderer.Init();

    renderer.TestShaderWithLight();

    renderer.Destroy();
}

static void TestShaderWithPBR()
{
    Renderer renderer;

    renderer.Init();

    renderer.TestShaderWithPBR();

    renderer.Destroy();
}

#define _TEST_SETS_ADD_ONE_TEST(_testType, _testFunc) \
{\
    mTestFuncSets[(_testType)] = (_testFunc);\
    mTestIdxNames[(_testType)] = string(#_testType).erase(0, 11);\
}

TestSets::TestSets()
{
    _TEST_SETS_ADD_ONE_TEST(TestIndex::MATRIX_BASICS, TestMatrixBasics);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::MATRIX_BASICS_2, TestMatrixBasics2);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::VECTOR_BASICS, TestVectorBasics);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::DRAW_GRADIENT_FLAG_IMG, TestDrawGradientFlag);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::CLIP_SEGMENT, TestClipLineSeg);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::CLIP_TRIANGLE_LIST, TestClipIndexedTriangleList);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::RASTER_DRAW_LINE, TestRasterDrawLine);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::RASTER_DRAW_TRIANGLE, TestRasterizerDrawTriangle);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::LOAD_MESH_FROM_OBJ, TestLoadMeshFromObjFile);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::VERTEX_DATA_OP, TestVertexDataOp);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::PROJECTION, TestProjection);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::RENDER_MESH, TestRenderMesh);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::CAMERA_MATRIX, TestCameraMatrix);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::SHADER_CACHE, TestShaderCache);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::SHADER_RENDER, TestShaderRender);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::LOAD_TEXTURE, TestLoadTexture);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::SHADER_WITH_TEXTURE, TestShaderWithTexture);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::SHADER_WITH_LIGHT, TestShaderWithLight);
    _TEST_SETS_ADD_ONE_TEST(TestIndex::SHADER_WITH_PBR, TestShaderWithPBR);
}

TestSets& TestSets::GetInstance()
{
    if (pTestSetsInst == nullptr)
    {
        pTestSetsInst.reset(new TestSets());
    }
    return *pTestSetsInst;
}

TestSets::~TestSets()
{
}

void TestSets::LaunchAll()
{
    for (auto p = mTestFuncSets.begin(); p != mTestFuncSets.end(); ++p)
    {
        cout << "Run Test - " << mTestIdxNames[p->first] << endl;
        p->second();
    }
}

void TestSets::LaunchOne(TestIndex idx)
{
    if (mTestFuncSets.find(idx) != mTestFuncSets.end())
    {
        cout << "Run Test - " << mTestIdxNames[idx] << endl;
        mTestFuncSets[idx]();
    }
    else {
        cout << "No such test" << static_cast<int>(idx) << endl;
    }
}
