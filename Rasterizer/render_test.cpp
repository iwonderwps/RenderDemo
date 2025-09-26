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
#include "shader_pipeline.h"
#include "shader_test.h"
#include "shader_trivial.h"
#include "shader_blinn_phong.h"
#include "shader_pbr.h"

#include "printer.h"


constexpr auto PI = (3.1415926f);


static Matrix _gMatModelTeapot = Matrix::rotationY3d(PI / 4) * Matrix::rotationX3d(-PI / 2) * Matrix::scale3d(0.2f, 0.2f, 0.2f);
static Matrix _gMatViewTeapot = Matrix::translation3d(0.0f, 0.0f, -300.0f);

void Renderer::TestRenderMesh()
{
    using VAT = VertexDataAttribute;

    int screenWidth = 640, screenHeight = 480;

    Vector3i color = Vector3i::create({ 255, 255, 255 });
    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    const Vector3f zeroWorld = Vector3f::create({ 0, 0, 0 });

    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    //Matrix mModel = Matrix::identity(4); // _gMatModelTeapot
    //Matrix mView = Matrix::translation3d(0.0f, 0.0f, -10.0f); // _gMatViewTeapot
    //Matrix mMV = mView * mModel;
    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(Vector3f::create({ -5.0f, 25.0f, 32.0f, }));

    Mesh mesh;
    Matrix& matMesh = mesh.GetMatLocalToWorld();
    matMesh = Matrix::identity(4);

    Vector3f targetPoint = Vector3f::create({ 0.0f, -30.0f, 0.0f });
    Matrix mMV = camera.LookAt(targetPoint, upWorld).ViewMatrix() * matMesh;
    Vector4f camPos;
    camPos.assign(camera.GetPosition(), 1.0f);

    mScreen->initialSetSize(screenWidth, screenHeight);
    ImageScreenOutputSettings imgScreenSettings;
    imgScreenSettings.startImageName = "test_clip_" + TimeUtility::getCurTimeStr(); // "test_mesh_pyramid_" + TimeUtility::getCurTimeStr();
    mScreen->resetOutputSettings(imgScreenSettings);
    Matrix depthBuffer = Matrix::create(screenWidth, screenHeight);

    mesh.LoadMeshFromFile("Res/test_pyramid.obj"); // "Res/test_pyramid.obj"
    MeshData& meshData = mesh.GetMeshData();

    std::vector<Vector3f>& posDataList = meshData.VertexList();
    std::vector<Vector3i>& posIdxDataList = meshData.VertexIndexList();
    std::vector<Vector2f>& uvDataList = meshData.UVList();
    std::vector<Vector3i>& uvIdxDataList = meshData.UVIndexList();

    std::vector<VertexData> triVertexData(3);
    std::vector<size_t> idxTriVertexData({ 0, 1, 2 });
    std::vector<VertexData> triVertexDataOut;
    std::vector<size_t> idxTriVertexDataOut;;
    VertexData& a = triVertexData[0];
    VertexData& b = triVertexData[1];
    VertexData& c = triVertexData[2];
    std::vector<Vector4f> triVertexPos(3);

    auto DrawToScreen = [this, &depthBuffer, &color](const RasterizerResultTriangle& rasterTriangleResult)
        {
            if (rasterTriangleResult.depthProjected > depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen))
            {
                depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen) = rasterTriangleResult.depthProjected;

                // 临时取代片段着色器
                Screen::Color3& fragColor = color;

                mScreen->drawPointDirect(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen, fragColor);
            }
        };

    mScreen->clearBuffer();
    depthBuffer.fill(-2.0f);
    size_t numToDraw = meshData.NumFace(); // std::min(static_cast<size_t>(10), meshData.NumFace())
    for (size_t i = 0; i < numToDraw; ++i)
    {
        const int& idxAp = posIdxDataList[i][0], & idxBp = posIdxDataList[i][1], & idxCp = posIdxDataList[i][2];
        const int& idxAuv = uvIdxDataList[i][0], & idxBuv = uvIdxDataList[i][1], & idxCuv = uvIdxDataList[i][2];

        // 默认背面剔除
        Matrix matToMeshLocal = matMesh.inverse();
        Vector3f camPosMeshLocal;
        camPosMeshLocal.assign(matToMeshLocal * camPos);
        Vector3f posALocal = posDataList[idxAp], posBLocal = posDataList[idxBp], posCLocal = posDataList[idxCp];
        Vector3f faceDirLocal = static_cast<Vector3f>(posBLocal - posALocal) % static_cast<Vector3f>(posCLocal - posALocal);
        Vector3f viewDir = camPosMeshLocal - posALocal;
        if (viewDir.dot(faceDirLocal) <= 0)
        {
            continue;
        }

        // 每次重新组装三角形 原始数据可能复用
        a.Reset(); b.Reset(); c.Reset();
        a[VAT::POSITION].assign(posDataList[idxAp]); a[VAT::POSITION][3] = 1.0f;
        b[VAT::POSITION].assign(posDataList[idxBp]); b[VAT::POSITION][3] = 1.0f;
        c[VAT::POSITION].assign(posDataList[idxCp]); c[VAT::POSITION][3] = 1.0f;
        a[VAT::UV].assign(uvDataList[idxAuv]);
        b[VAT::UV].assign(uvDataList[idxBuv]);
        c[VAT::UV].assign(uvDataList[idxCuv]);

        // replace basic vertex shader
        a[VAT::POSITION] = mMV * a[VAT::POSITION];
        b[VAT::POSITION] = mMV * b[VAT::POSITION];
        c[VAT::POSITION] = mMV * c[VAT::POSITION];
        a[VAT::POSITION] = mpNDC * a[VAT::POSITION];
        b[VAT::POSITION] = mpNDC * b[VAT::POSITION];
        c[VAT::POSITION] = mpNDC * c[VAT::POSITION];

        // 对视椎体边界裁剪
        triVertexDataOut.clear();
        idxTriVertexDataOut.clear();
        auto numTri = mClipper->clipIndexedTriangleList(triVertexData, idxTriVertexData, triVertexDataOut, idxTriVertexDataOut);
        if (numTri == 0)
        {
            continue;
        }

        // 输入只有1个刚组装的三角形，若在视椎体内，此处取出1个三角形，若跨越多个面，此处取出多个三角形，
        for (size_t idxOutTri = 0; idxOutTri < numTri; ++idxOutTri)
        {
            // 按顶点顺序取出 顶点可能复用 不取引用
            VertexData aOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 0]];
            VertexData bOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 1]];
            VertexData cOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 2]];

            // 从 clipping space 到 ndc
            aOut[VAT::POSITION].perspectiveDivideInPlace();
            bOut[VAT::POSITION].perspectiveDivideInPlace();
            cOut[VAT::POSITION].perspectiveDivideInPlace();

            triVertexPos[0].assign(aOut[VAT::POSITION]);
            triVertexPos[1].assign(bOut[VAT::POSITION]);
            triVertexPos[2].assign(cOut[VAT::POSITION]);

            mRasterizer->DrawTiangle(triVertexPos.data(), screenWidth, screenHeight, DrawToScreen);
        }

    }
    mScreen->flush();
}

void Renderer::TestShaderRender()
{
    using VAT = VertexDataAttribute;

    int screenWidth = 640, screenHeight = 480;

    Vector3i color = Vector3i::create({ 255, 255, 255 });
    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    const Vector3f zeroWorld = Vector3f::create({ 0, 0, 0 });

    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(Vector3f::create({ -5.0f, 25.0f, 32.0f, }));

    Mesh mesh;
    Matrix& matMesh = mesh.GetMatLocalToWorld();
    matMesh = Matrix::identity(4);

    Vector3f targetPoint = Vector3f::create({ 0.0f, 0.0f, 0.0f }); // { 0.0f, -30.0f, 0.0f }
    Matrix mMV = camera.LookAt(targetPoint, upWorld).ViewMatrix() * matMesh;
    Vector4f camPos;
    camPos.assign(camera.GetPosition(), 1.0f);

    mScreen->initialSetSize(screenWidth, screenHeight);
    ImageScreenOutputSettings imgScreenSettings;
    imgScreenSettings.startImageName = "test_shader_" + TimeUtility::getCurTimeStr();
    mScreen->resetOutputSettings(imgScreenSettings);
    Matrix depthBuffer = Matrix::create(screenWidth, screenHeight);

    mesh.LoadMeshFromFile("Res/test_pyramid.obj");
    MeshData& meshData = mesh.GetMeshData();

    ShaderPipeline pipeline;
    pipeline.AddShader(new ShaderVertexTest());
    pipeline.AddShader(new ShaderFragTest());
    pipeline.AddShader(new ShaderSetupTest());
    auto& shaderMV = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);

    std::vector<Vector3f>& posDataList = meshData.VertexList();
    std::vector<Vector3i>& posIdxDataList = meshData.VertexIndexList();
    std::vector<Vector2f>& uvDataList = meshData.UVList();
    std::vector<Vector3i>& uvIdxDataList = meshData.UVIndexList();

    std::vector<VertexData> triVertexData(3);
    std::vector<size_t> idxTriVertexData({ 0, 1, 2 });
    std::vector<VertexData> triVertexDataOut;
    std::vector<size_t> idxTriVertexDataOut;;
    VertexData& a = triVertexData[0];
    VertexData& b = triVertexData[1];
    VertexData& c = triVertexData[2];
    std::vector<Vector4f> triVertexPos(3);
    VertexData aOut;
    VertexData bOut;
    VertexData cOut;

    std::vector<std::string>* pVaryNameList = pipeline.RasterizeInterpolateNameList();
    std::vector<std::string> fragVaryVarNames;
    for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
    {
        std::string& varyName = pVaryNameList->at(iVaryName);
        fragVaryVarNames.push_back(pipeline.FragVarNameFromVertexVarName(varyName));
    }

    auto DrawToScreen = [this, &depthBuffer, &color, &pipeline, &fragVaryVarNames, &pVaryNameList, &aOut, &bOut, &cOut](const RasterizerResultTriangle& rasterTriangleResult)
        {
            if (rasterTriangleResult.depthProjected > depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen))
            {
                depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen) = rasterTriangleResult.depthProjected;

                ShaderBase* fragShader = pipeline.GetShader(ShaderType::FRAGMENT);
                auto& fragCoord = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
                Vector4f uv = mRasterizer->InterpolateTriangleAttributeVector4(aOut[VAT::UV], bOut[VAT::UV], cOut[VAT::UV], rasterTriangleResult);
                fragCoord[0] = uv[0];
                fragCoord[1] = uv[1];
                for (size_t iFName = 0; iFName < fragVaryVarNames.size(); ++iFName)
                {
                    std::string& fName = fragVaryVarNames[iFName];
                    std::string& vName = pVaryNameList->at(iFName);
                    auto& varyVar = RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC4);
                    varyVar = mRasterizer->InterpolateTriangleAttributeVector4(
                        RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC4),
                        RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC4),
                        RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC4),
                        rasterTriangleResult
                    );
                }
                fragShader->Fragment();
                auto& fragColor = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);
                Screen::Color3 screenColor = Vector3i::create(
                    { static_cast<int>(255 * fragColor[0]), static_cast<int>(255 * fragColor[1]), static_cast<int>(255 * fragColor[2]), }
                );
                mScreen->drawPointDirect(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen, screenColor);
            }
        };

    mScreen->clearBuffer();
    depthBuffer.fill(-2.0f);
    size_t numToDraw = meshData.NumFace(); // std::min(static_cast<size_t>(10), meshData.NumFace())
    for (size_t i = 0; i < numToDraw; ++i)
    {
        const int& idxAp = posIdxDataList[i][0], & idxBp = posIdxDataList[i][1], & idxCp = posIdxDataList[i][2];
        const int& idxAuv = uvIdxDataList[i][0], & idxBuv = uvIdxDataList[i][1], & idxCuv = uvIdxDataList[i][2];

        // 默认背面剔除
        Matrix matToMeshLocal = matMesh.inverse();
        Vector3f camPosMeshLocal;
        camPosMeshLocal.assign(matToMeshLocal * camPos);
        Vector3f posALocal = posDataList[idxAp], posBLocal = posDataList[idxBp], posCLocal = posDataList[idxCp];
        Vector3f faceDirLocal = static_cast<Vector3f>(posBLocal - posALocal) % static_cast<Vector3f>(posCLocal - posALocal);
        Vector3f viewDir = camPosMeshLocal - posALocal;
        if (viewDir.dot(faceDirLocal) <= 0)
        {
            continue;
        }

        // 每次重新组装三角形 原始数据可能复用
        a.Reset(); b.Reset(); c.Reset();
        a[VAT::POSITION].assign(posDataList[idxAp]); a[VAT::POSITION][3] = 1.0f;
        b[VAT::POSITION].assign(posDataList[idxBp]); b[VAT::POSITION][3] = 1.0f;
        c[VAT::POSITION].assign(posDataList[idxCp]); c[VAT::POSITION][3] = 1.0f;
        a[VAT::UV].assign(uvDataList[idxAuv]);
        b[VAT::UV].assign(uvDataList[idxBuv]);
        c[VAT::UV].assign(uvDataList[idxCuv]);

        // vertex shader
        ShaderBase* vertexShader = pipeline.GetShader(ShaderType::VERTEX);
        shaderMV = mMV;
        shaderMVP = mpNDC * mMV;
        for (int iVd = 0; iVd < 3; ++iVd)
        {
            VertexData& curVertexData = triVertexData[iVd];
            auto& vertexPosInput = RefShaderPiplineCachedData(&pipeline, ShaderTestVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
            vertexPosInput = curVertexData[VAT::POSITION];
            vertexShader->Vertex();
            curVertexData[VAT::POSITION] = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
            for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
            {
                std::string& varyName = pVaryNameList->at(iVaryName);
                RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC4) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC4);
            }
        }

        // 对视椎体边界裁剪
        triVertexDataOut.clear();
        idxTriVertexDataOut.clear();
        auto numTri = mClipper->clipIndexedTriangleList(triVertexData, idxTriVertexData, triVertexDataOut, idxTriVertexDataOut);
        if (numTri == 0)
        {
            continue;
        }

        // 输入只有1个刚组装的三角形，若在视椎体内，此处取出1个三角形，若跨越多个面，此处取出多个三角形，
        for (size_t idxOutTri = 0; idxOutTri < numTri; ++idxOutTri)
        {
            // 按顶点顺序取出 顶点可能复用 不取引用
            aOut.Reset(); bOut.Reset(); cOut.Reset();
            aOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 0]];
            bOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 1]];
            cOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 2]];

            // 从 clipping space 到 ndc
            aOut[VAT::POSITION].perspectiveDivideInPlace();
            bOut[VAT::POSITION].perspectiveDivideInPlace();
            cOut[VAT::POSITION].perspectiveDivideInPlace();

            triVertexPos[0].assign(aOut[VAT::POSITION]);
            triVertexPos[1].assign(bOut[VAT::POSITION]);
            triVertexPos[2].assign(cOut[VAT::POSITION]);

            mRasterizer->DrawTiangle(triVertexPos.data(), screenWidth, screenHeight, DrawToScreen);
        }

    }
    mScreen->flush();
}

void Renderer::TestShaderWithTexture()
{
    using VAT = VertexDataAttribute;

    int screenWidth = 640, screenHeight = 480;

    Vector3i color = Vector3i::create({ 255, 255, 255 });
    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    const Vector3f zeroWorld = Vector3f::create({ 0, 0, 0 });

    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(Vector3f::create({ 7.36f, 0.f, 6.9f, })); // 7.36f, 4.96f, 6.9f,

    Mesh mesh;
    Matrix& matMesh = mesh.GetMatLocalToWorld();
    matMesh = Matrix::identity(4);

    Vector3f targetPoint = Vector3f::create({ 0.0f, -4.96f, 0.0f });  // 0.0f, 0.0f, 0.0f
    Matrix mMV = camera.LookAt(targetPoint, upWorld).ViewMatrix() * matMesh;
    Vector4f camPos;
    camPos.assign(camera.GetPosition(), 1.0f);

    mScreen->initialSetSize(screenWidth, screenHeight);
    ImageScreenOutputSettings imgScreenSettings;
    imgScreenSettings.startImageName = "test_texture_ball" + TimeUtility::getCurTimeStr();
    mScreen->resetOutputSettings(imgScreenSettings);
    Matrix depthBuffer = Matrix::create(screenWidth, screenHeight);

    mesh.LoadMeshFromFile("Res/test_ball_texture_01.obj"); // 注意导出模型前 三角化 否则渲染结果有镂空
    MeshData& meshData = mesh.GetMeshData();

    ShaderPipeline pipeline;
    pipeline.AddShader(new ShaderVertexTrivial());
    pipeline.AddShader(new ShaderFragmentTrivial());
    pipeline.AddShader(new ShaderSetupTrivial());
    auto& shaderMV = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);

    auto& texture = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE);
    texture.LoadTextureFromFile("Res/ball_free_texture.jpg");

    std::vector<Vector3f>& posDataList = meshData.VertexList();
    std::vector<Vector3i>& posIdxDataList = meshData.VertexIndexList();
    std::vector<Vector2f>& uvDataList = meshData.UVList();
    std::vector<Vector3i>& uvIdxDataList = meshData.UVIndexList();

    std::vector<VertexData> triVertexData(3);
    std::vector<size_t> idxTriVertexData({ 0, 1, 2 });
    std::vector<VertexData> triVertexDataOut;
    std::vector<size_t> idxTriVertexDataOut;;
    VertexData& a = triVertexData[0];
    VertexData& b = triVertexData[1];
    VertexData& c = triVertexData[2];
    std::vector<Vector4f> triVertexPos(3);
    VertexData aOut;
    VertexData bOut;
    VertexData cOut;

    std::vector<std::string>* pVaryNameList = pipeline.RasterizeInterpolateNameList();
    std::vector<std::string> fragVaryVarNames;
    for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
    {
        std::string& varyName = pVaryNameList->at(iVaryName);
        fragVaryVarNames.push_back(pipeline.FragVarNameFromVertexVarName(varyName));
    }

    auto DrawToScreen = [this, &depthBuffer, &color, &pipeline, &fragVaryVarNames, &pVaryNameList, &aOut, &bOut, &cOut](const RasterizerResultTriangle& rasterTriangleResult)
        {
            if (rasterTriangleResult.depthProjected > depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen))
            {
                depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen) = rasterTriangleResult.depthProjected;

                ShaderBase* fragShader = pipeline.GetShader(ShaderType::FRAGMENT);
                auto& fragCoord = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
                Vector4f uv = mRasterizer->InterpolateTriangleAttributeVector4(aOut[VAT::UV], bOut[VAT::UV], cOut[VAT::UV], rasterTriangleResult);
                fragCoord[0] = uv[0];
                fragCoord[1] = uv[1];
                for (size_t iFName = 0; iFName < fragVaryVarNames.size(); ++iFName)
                {
                    std::string& fName = fragVaryVarNames[iFName];
                    std::string& vName = pVaryNameList->at(iFName);
                    auto& varyVar = RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC4);
                    varyVar = mRasterizer->InterpolateTriangleAttributeVector4(
                        RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC4),
                        RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC4),
                        RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC4),
                        rasterTriangleResult
                    );
                }
                fragShader->Fragment();
                auto& fragColor = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);
                Screen::Color3 screenColor = Vector3i::create(
                    { static_cast<int>(255 * fragColor[0]), static_cast<int>(255 * fragColor[1]), static_cast<int>(255 * fragColor[2]), }
                );
                mScreen->drawPointDirect(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen, screenColor);
            }
        };

    mScreen->clearBuffer();
    depthBuffer.fill(-2.0f);
    size_t numToDraw = meshData.NumFace(); // std::min(static_cast<size_t>(10), meshData.NumFace())
    for (size_t i = 0; i < numToDraw; ++i)
    {
        const int& idxAp = posIdxDataList[i][0], & idxBp = posIdxDataList[i][1], & idxCp = posIdxDataList[i][2];
        const int& idxAuv = uvIdxDataList[i][0], & idxBuv = uvIdxDataList[i][1], & idxCuv = uvIdxDataList[i][2];

        // 默认背面剔除
        Matrix matToMeshLocal = matMesh.inverse();
        Vector3f camPosMeshLocal;
        camPosMeshLocal.assign(matToMeshLocal * camPos);
        Vector3f posALocal = posDataList[idxAp], posBLocal = posDataList[idxBp], posCLocal = posDataList[idxCp];
        Vector3f faceDirLocal = static_cast<Vector3f>(posBLocal - posALocal) % static_cast<Vector3f>(posCLocal - posALocal);
        Vector3f viewDir = camPosMeshLocal - posALocal;
        if (viewDir.dot(faceDirLocal) <= 0)
        {
            continue;
        }

        // 每次重新组装三角形 原始数据可能复用
        a.Reset(); b.Reset(); c.Reset();
        a[VAT::POSITION].assign(posDataList[idxAp]); a[VAT::POSITION][3] = 1.0f;
        b[VAT::POSITION].assign(posDataList[idxBp]); b[VAT::POSITION][3] = 1.0f;
        c[VAT::POSITION].assign(posDataList[idxCp]); c[VAT::POSITION][3] = 1.0f;
        a[VAT::UV].assign(uvDataList[idxAuv]);
        b[VAT::UV].assign(uvDataList[idxBuv]);
        c[VAT::UV].assign(uvDataList[idxCuv]);

        // vertex shader
        ShaderBase* vertexShader = pipeline.GetShader(ShaderType::VERTEX);
        shaderMV = mMV;
        shaderMVP = mpNDC * mMV;
        for (int iVd = 0; iVd < 3; ++iVd)
        {
            VertexData& curVertexData = triVertexData[iVd];
            auto& vertexPosInput = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
            vertexPosInput = curVertexData[VAT::POSITION];
            vertexShader->Vertex();
            curVertexData[VAT::POSITION] = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
            for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
            {
                std::string& varyName = pVaryNameList->at(iVaryName);
                RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC4) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC4);
            }
        }

        // 对视椎体边界裁剪
        triVertexDataOut.clear();
        idxTriVertexDataOut.clear();
        auto numTri = mClipper->clipIndexedTriangleList(triVertexData, idxTriVertexData, triVertexDataOut, idxTriVertexDataOut);
        if (numTri == 0)
        {
            continue;
        }

        // 输入只有1个刚组装的三角形，若在视椎体内，此处取出1个三角形，若跨越多个面，此处取出多个三角形，
        for (size_t idxOutTri = 0; idxOutTri < numTri; ++idxOutTri)
        {
            // 按顶点顺序取出 顶点可能复用 不取引用
            aOut.Reset(); bOut.Reset(); cOut.Reset();
            aOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 0]];
            bOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 1]];
            cOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 2]];

            // 从 clipping space 到 ndc
            aOut[VAT::POSITION].perspectiveDivideInPlace();
            bOut[VAT::POSITION].perspectiveDivideInPlace();
            cOut[VAT::POSITION].perspectiveDivideInPlace();

            triVertexPos[0].assign(aOut[VAT::POSITION]);
            triVertexPos[1].assign(bOut[VAT::POSITION]);
            triVertexPos[2].assign(cOut[VAT::POSITION]);

            mRasterizer->DrawTiangle(triVertexPos.data(), screenWidth, screenHeight, DrawToScreen);
        }

    }
    mScreen->flush();
}

void Renderer::TestShaderWithLight()
{
    using VAT = VertexDataAttribute;

    int screenWidth = 640, screenHeight = 480;

    Vector3i color = Vector3i::create({ 255, 255, 255 });
    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    const Vector3f zeroWorld = Vector3f::create({ 0, 0, 0 });

    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(Vector3f::create({5.0f, 5.0f, 5.0f, }));  // 2.5f, 4.15f, 8.0f,   0.8f, 10.0f, 10.1f,

    Mesh mesh;
    Matrix& matMesh = mesh.GetMatLocalToWorld();
    matMesh = Matrix::identity(4);

    Vector3f targetPoint = Vector3f::create({ 0.0f, 0.0f, 0.0f, });  // 0.0f, 0.0f, 0.0f,  0.42f, 0.0f, 7.21f,
    Matrix mView = camera.LookAt(targetPoint, upWorld).ViewMatrix();
    Matrix mMV = mView * matMesh;
    Vector4f camPos;
    camPos.assign(camera.GetPosition(), 1.0f);

    mScreen->initialSetSize(screenWidth, screenHeight);
    ImageScreenOutputSettings imgScreenSettings;
    imgScreenSettings.startImageName = "test_torus_blinn_phong" + TimeUtility::getCurTimeStr();
    mScreen->resetOutputSettings(imgScreenSettings);
    Matrix depthBuffer = Matrix::create(screenWidth, screenHeight);

    mesh.LoadMeshFromFile("Res/test_torus2.obj"); // 注意导出模型前 三角化 否则渲染结果有镂空
    MeshData& meshData = mesh.GetMeshData();

    ShaderPipeline pipeline;
    pipeline.AddShader(new ShaderBlinnPhongVertex());
    pipeline.AddShader(new ShaderBlinnPhongFragment());
    pipeline.AddShader(new ShaderBlinnPhongSetup());

    auto& shaderMV = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderNormalMat = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::NORMAL_MAT, ShaderCacheDataType::MATRIX);
    shaderMV = mMV;
    shaderMVP = mpNDC * mMV;
    shaderNormalMat = mMV.inverse().transpose();

    auto& envAmbient = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelKa = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::ModelKaInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelKd = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::ModelKdInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelKs = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::ModelKsInput_VEC3, ShaderCacheDataType::VEC3);
    auto& modelShininess = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::ModelShininessInput_FLOAT, ShaderCacheDataType::FLOAT);
    auto& lightKa = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::LightKaInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightKd = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::LightKdInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightKs = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::LightKsInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightPosView = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBlinnPhongVars::LightPosViewInput_VEC3, ShaderCacheDataType::VEC3);

    Vector4f lightPosWorld = Vector4f::create({ 3.0f, 3.0f, -3.0f, 1.0f});
    lightPosView.assign(mView * lightPosWorld);
    Printer::PrintVectorContent(lightPosWorld);
    Printer::PrintMatrixContent(mView);
    Printer::PrintVectorContent(lightPosView);

    envAmbient = Vector3f::create({ 0.7f, 0.7f, 0.7f});
    modelKa = Vector3f::create({ 0.2473f, 0.1995f, 0.0745f});
    modelKd = Vector3f::create({ 0.7516f, 0.6065f, 0.2265f});
    modelKs = Vector3f::create({ 0.6283f, 0.5559f, 0.3661f});
    modelShininess = 51.2f;
    lightKa = Vector3f::create({ 0.0f, 0.0f, 0.0f});
    lightKd = Vector3f::create({ 1.0f, 1.0f, 1.0f});
    lightKs = Vector3f::create({ 1.0f, 1.0f, 1.0f});

    std::vector<Vector3f>& posDataList = meshData.VertexList();
    std::vector<Vector3i>& posIdxDataList = meshData.VertexIndexList();
    std::vector<Vector2f>& uvDataList = meshData.UVList();
    std::vector<Vector3i>& uvIdxDataList = meshData.UVIndexList();
    std::vector<Vector3f>& normalDataList = meshData.NormalList();
    std::vector<Vector3i>& normalIdxDataList = meshData.NormalIndexList();

    std::vector<VertexData> triVertexData(3);
    std::vector<size_t> idxTriVertexData({ 0, 1, 2 });
    std::vector<VertexData> triVertexDataOut;
    std::vector<size_t> idxTriVertexDataOut;;
    VertexData& a = triVertexData[0];
    VertexData& b = triVertexData[1];
    VertexData& c = triVertexData[2];
    std::vector<Vector4f> triVertexPos(3);
    VertexData aOut;
    VertexData bOut;
    VertexData cOut;

    std::vector<std::string>* pVaryNameList = pipeline.RasterizeInterpolateNameList();
    std::vector<std::string> fragVaryVarNames;
    for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
    {
        std::string& varyName = pVaryNameList->at(iVaryName);
        fragVaryVarNames.push_back(pipeline.FragVarNameFromVertexVarName(varyName));
        ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(varyName);
        switch (dataType)
        {
        case ShaderCacheDataType::VEC4:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            break;
        }
        case ShaderCacheDataType::VEC3:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            break;
        }
        case ShaderCacheDataType::VEC2:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            break;
        }
        case ShaderCacheDataType::FLOAT:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            break;
        }
        default:
            break;
        }
    }

    auto DrawToScreen = [this, &depthBuffer, &color, &pipeline, &fragVaryVarNames, &pVaryNameList, &aOut, &bOut, &cOut](const RasterizerResultTriangle& rasterTriangleResult)
        {
            if (rasterTriangleResult.depthProjected > depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen))
            {
                depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen) = rasterTriangleResult.depthProjected;

                ShaderBase* fragShader = pipeline.GetShader(ShaderType::FRAGMENT);
                auto& fragCoord = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
                Vector4f uv = mRasterizer->InterpolateTriangleAttributeVector4(aOut[VAT::UV], bOut[VAT::UV], cOut[VAT::UV], rasterTriangleResult);
                fragCoord[0] = uv[0];
                fragCoord[1] = uv[1];
                for (size_t iFName = 0; iFName < fragVaryVarNames.size(); ++iFName)
                {
                    std::string& fName = fragVaryVarNames[iFName];
                    std::string& vName = pVaryNameList->at(iFName);                    
                    ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(vName);
                    switch (dataType)
                    {
                    case ShaderCacheDataType::VEC4:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC4) = mRasterizer->InterpolateTriangleAttributeVector4(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC4),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC4),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC4),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::VEC3:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC3) = mRasterizer->InterpolateTriangleAttributeVector3(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC3),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC3),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC3),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::VEC2:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC2) = mRasterizer->InterpolateTriangleAttributeVector2(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC2),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC2),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC2),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::FLOAT:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::FLOAT) = mRasterizer->InterpolateTriangleAttributeFloat(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::FLOAT),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::FLOAT),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::FLOAT),
                            rasterTriangleResult
                        );
                        break;
                    }
                    default:
                        break;
                    }
                }
                fragShader->Fragment();
                auto& fragColor = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);
                fragColor[0] = std::max(0.0f, std::min(1.0f, fragColor[0]));
                fragColor[1] = std::max(0.0f, std::min(1.0f, fragColor[1]));
                fragColor[2] = std::max(0.0f, std::min(1.0f, fragColor[2]));
                fragColor[3] = std::max(0.0f, std::min(1.0f, fragColor[3]));
                Screen::Color3 screenColor = Vector3i::create(
                    { static_cast<int>(255 * fragColor[0]), static_cast<int>(255 * fragColor[1]), static_cast<int>(255 * fragColor[2]), }
                );
                mScreen->drawPointDirect(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen, screenColor);
            }
        };

    mScreen->clearBuffer();
    depthBuffer.fill(-2.0f);
    size_t numToDraw = meshData.NumFace(); // std::min(static_cast<size_t>(10), meshData.NumFace())
    for (size_t i = 0; i < numToDraw; ++i)
    {
        const int& idxAp = posIdxDataList[i][0], & idxBp = posIdxDataList[i][1], & idxCp = posIdxDataList[i][2];
        const int& idxAuv = uvIdxDataList[i][0], & idxBuv = uvIdxDataList[i][1], & idxCuv = uvIdxDataList[i][2];
        const int& idxAn = normalIdxDataList[i][0], & idxBn = normalIdxDataList[i][1], & idxCn = normalIdxDataList[i][2];

        // 默认背面剔除
        Matrix matToMeshLocal = matMesh.inverse();
        Vector3f camPosMeshLocal;
        camPosMeshLocal.assign(matToMeshLocal * camPos);
        Vector3f posALocal = posDataList[idxAp], posBLocal = posDataList[idxBp], posCLocal = posDataList[idxCp];
        Vector3f faceDirLocal = static_cast<Vector3f>(posBLocal - posALocal) % static_cast<Vector3f>(posCLocal - posALocal);
        Vector3f viewDir = camPosMeshLocal - posALocal;
        if (viewDir.dot(faceDirLocal) <= 0)
        {
            continue;
        }

        // 每次重新组装三角形 原始数据可能复用
        a.Reset(); b.Reset(); c.Reset();
        a[VAT::POSITION].assign(posDataList[idxAp]); a[VAT::POSITION][3] = 1.0f;
        b[VAT::POSITION].assign(posDataList[idxBp]); b[VAT::POSITION][3] = 1.0f;
        c[VAT::POSITION].assign(posDataList[idxCp]); c[VAT::POSITION][3] = 1.0f;
        a[VAT::UV].assign(uvDataList[idxAuv]);
        b[VAT::UV].assign(uvDataList[idxBuv]);
        c[VAT::UV].assign(uvDataList[idxCuv]);
        a[VAT::NORMAL].assign(normalDataList[idxAn]);
        b[VAT::NORMAL].assign(normalDataList[idxBn]);
        c[VAT::NORMAL].assign(normalDataList[idxCn]);

        // vertex shader
        ShaderBase* vertexShader = pipeline.GetShader(ShaderType::VERTEX);
        for (int iVd = 0; iVd < 3; ++iVd)
        {
            VertexData& curVertexData = triVertexData[iVd];
            
            auto& vertexPosInput = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
            vertexPosInput = curVertexData[VAT::POSITION];
            
            auto& vertexNormalInput = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);
            vertexNormalInput = curVertexData[VAT::NORMAL];

            vertexShader->Vertex();

            curVertexData[VAT::POSITION] = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
            for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
            {
                std::string& varyName = pVaryNameList->at(iVaryName);
                ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(varyName);
                switch (dataType)
                {
                case ShaderCacheDataType::VEC4:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC4) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC4);
                    break;
                case ShaderCacheDataType::VEC3:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC3) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC3);
                    break;
                case ShaderCacheDataType::VEC2:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC2) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC2);
                    break;
                case ShaderCacheDataType::FLOAT:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::FLOAT) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::FLOAT);
                    break;
                default:
                    break;
                }
            }
        }

        // 对视椎体边界裁剪
        triVertexDataOut.clear();
        idxTriVertexDataOut.clear();
        auto numTri = mClipper->clipIndexedTriangleList(triVertexData, idxTriVertexData, triVertexDataOut, idxTriVertexDataOut);
        if (numTri == 0)
        {
            continue;
        }

        // 输入只有1个刚组装的三角形，若在视椎体内，此处取出1个三角形，若跨越多个面，此处取出多个三角形，
        for (size_t idxOutTri = 0; idxOutTri < numTri; ++idxOutTri)
        {
            // 按顶点顺序取出 顶点可能复用 不取引用
            aOut.Reset(); bOut.Reset(); cOut.Reset();
            aOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 0]];
            bOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 1]];
            cOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 2]];

            // 从 clipping space 到 ndc
            aOut[VAT::POSITION].perspectiveDivideInPlace();
            bOut[VAT::POSITION].perspectiveDivideInPlace();
            cOut[VAT::POSITION].perspectiveDivideInPlace();

            triVertexPos[0].assign(aOut[VAT::POSITION]);
            triVertexPos[1].assign(bOut[VAT::POSITION]);
            triVertexPos[2].assign(cOut[VAT::POSITION]);

            mRasterizer->DrawTiangle(triVertexPos.data(), screenWidth, screenHeight, DrawToScreen);
        }

    }
    mScreen->flush();
}

void Renderer::TestShaderWithPBR()
{
    using VAT = VertexDataAttribute;

    int screenWidth = 640, screenHeight = 480;
    //int screenWidth = 1280, screenHeight = 960;

    Vector3i color = Vector3i::create({ 255, 255, 255 });
    const Vector3f upWorld = Vector3f::create({ 0, 1, 0 });
    const Vector3f zeroWorld = Vector3f::create({ 0, 0, 0 });

    float n = -0.1f, f = -100.0f; // -1000.0f
    float fov = 60.0f / 180 * 3.1415926f;
    float aspect = 4.0f / 3;

    CameraConfig cameraCfg = {
        n, f, fov, aspect
    };
    Matrix mpNDC = cameraCfg.createProjectMatrixClippingSpace();

    Camera camera;
    camera.SetCameraConfig(cameraCfg);
    camera.SetCameraPosition(Vector3f::create({ 5.0f, 5.0f, 5.0f, })); 

    Mesh mesh;
    Matrix& matMesh = mesh.GetMatLocalToWorld();
    matMesh = Matrix::identity(4);

    Vector3f targetPoint = Vector3f::create({ 0.0f, 0.0f, 0.0f, });
    Matrix mView = camera.LookAt(targetPoint, upWorld).ViewMatrix();
    Matrix mMV = mView * matMesh;
    Vector4f camPos;
    camPos.assign(camera.GetPosition(), 1.0f);

    mScreen->initialSetSize(screenWidth, screenHeight);
    ImageScreenOutputSettings imgScreenSettings;
    imgScreenSettings.startImageName = "test_ball_pbr" + TimeUtility::getCurTimeStr();
    mScreen->resetOutputSettings(imgScreenSettings);
    Matrix depthBuffer = Matrix::create(screenWidth, screenHeight);

    mesh.LoadMeshFromFile("Res/test_pbr_ball.obj");
    MeshData& meshData = mesh.GetMeshData();

    ShaderPipeline pipeline;
    pipeline.AddShader(new ShaderPBRVertex());
    pipeline.AddShader(new ShaderPBRFragment());
    pipeline.AddShader(new ShaderPBRSetup());

    auto& shaderMV = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MV_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderMVP = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::MVP_MAT, ShaderCacheDataType::MATRIX);
    auto& shaderNormalMat = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::NORMAL_MAT, ShaderCacheDataType::MATRIX);
    shaderMV = mMV;
    shaderMVP = mpNDC * mMV;
    shaderNormalMat = mMV.inverse().transpose();

    auto& envAmbient = RefShaderPiplineGlobalCachedData(&pipeline, ShaderPBRVars::EnvAmbientInput_VEC3, ShaderCacheDataType::VEC3);
    auto& lightPosViewArr = RefShaderPiplineGlobalCachedData(&pipeline, ShaderPBRVars::LightPositionsInput_MATRIX, ShaderCacheDataType::MATRIX);
    auto& lightColorArr = RefShaderPiplineGlobalCachedData(&pipeline, ShaderPBRVars::LightColorsInput_MATRIX, ShaderCacheDataType::MATRIX);
    envAmbient = Vector3f::create({ 0.03f, 0.03f, 0.03f });
    Vector4f lightPosWorld0 = Vector4f::create({ 3.0f, 3.0f, -3.0f, 1.0f });  // 3.0f, 3.0f, -3.0f, 1.0f
    Vector4f lightPosWorld1 = Vector4f::create({ 0.0f, 6.0f, 0.0f, 1.0f });
    Vector4f lightPosWorld2 = Vector4f::create({ -3.0f, -5.0f, 1.0f, 1.0f });  // -3.0f, -5.0f, 1.0f, 1.0f
    Vector3f lightPosView0, lightPosView1, lightPosView2; 
    lightPosView0.assign(mView * lightPosWorld0);
    lightPosView1.assign(mView * lightPosWorld1);
    lightPosView2.assign(mView * lightPosWorld2);
    lightPosViewArr = Matrix::create({
        {lightPosView0[0], lightPosView0[1], lightPosView0[2], },
        {lightPosView1[0], lightPosView1[1], lightPosView1[2], },
        {lightPosView2[0], lightPosView2[1], lightPosView2[2], },
    });
    lightColorArr = Matrix::create({
        {150.0f, 150.0f, 150.0f, },
        {80.0f, 80, 80, },
        {30.0f, 30, 30, },
    });
    Printer::PrintMatrixContent(lightPosViewArr);

    auto& albedoTex = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::Texture_01_Texture, ShaderCacheDataType::TEXTURE);
    auto& metallicTex = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::Texture_02_Texture, ShaderCacheDataType::TEXTURE);
    auto& roughnessTex = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::Texture_03_Texture, ShaderCacheDataType::TEXTURE);
    auto& aoTex = RefShaderPiplineGlobalCachedData(&pipeline, ShaderBaseVars::Texture_04_Texture, ShaderCacheDataType::TEXTURE);
    albedoTex.LoadTextureFromFile("Res/ball_free_pbr_basecolor.png");
    metallicTex.LoadTextureFromFile("Res/ball_free_pbr_metallic.png");
    roughnessTex.LoadTextureFromFile("Res/ball_free_pbr_roughness.png");
    aoTex.LoadTextureFromFile("Res/white.png");

    std::vector<Vector3f>& posDataList = meshData.VertexList();
    std::vector<Vector3i>& posIdxDataList = meshData.VertexIndexList();
    std::vector<Vector2f>& uvDataList = meshData.UVList();
    std::vector<Vector3i>& uvIdxDataList = meshData.UVIndexList();
    std::vector<Vector3f>& normalDataList = meshData.NormalList();
    std::vector<Vector3i>& normalIdxDataList = meshData.NormalIndexList();

    std::vector<VertexData> triVertexData(3);
    std::vector<size_t> idxTriVertexData({ 0, 1, 2 });
    std::vector<VertexData> triVertexDataOut;
    std::vector<size_t> idxTriVertexDataOut;;
    VertexData& a = triVertexData[0];
    VertexData& b = triVertexData[1];
    VertexData& c = triVertexData[2];
    std::vector<Vector4f> triVertexPos(3);
    VertexData aOut;
    VertexData bOut;
    VertexData cOut;

    std::vector<std::string>* pVaryNameList = pipeline.RasterizeInterpolateNameList();
    std::vector<std::string> fragVaryVarNames;
    for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
    {
        std::string& varyName = pVaryNameList->at(iVaryName);
        fragVaryVarNames.push_back(pipeline.FragVarNameFromVertexVarName(varyName));
        ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(varyName);
        switch (dataType)
        {
        case ShaderCacheDataType::VEC4:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC4);
            break;
        }
        case ShaderCacheDataType::VEC3:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC3);
            break;
        }
        case ShaderCacheDataType::VEC2:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::VEC2);
            break;
        }
        case ShaderCacheDataType::FLOAT:
        {
            a.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            b.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            c.RegisterAdditionalAttrs(varyName, ShaderCacheDataType::FLOAT);
            break;
        }
        default:
            break;
        }
    }

    auto DrawToScreen = [this, &depthBuffer, &color, &pipeline, &fragVaryVarNames, &pVaryNameList, &aOut, &bOut, &cOut](const RasterizerResultTriangle& rasterTriangleResult)
        {
            if (rasterTriangleResult.depthProjected > depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen))
            {
                depthBuffer(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen) = rasterTriangleResult.depthProjected;

                ShaderBase* fragShader = pipeline.GetShader(ShaderType::FRAGMENT);
                auto& fragCoord = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragCoord_VEC2, ShaderCacheDataType::VEC2);
                Vector4f uv = mRasterizer->InterpolateTriangleAttributeVector4(aOut[VAT::UV], bOut[VAT::UV], cOut[VAT::UV], rasterTriangleResult);
                fragCoord[0] = uv[0];
                fragCoord[1] = uv[1];
                for (size_t iFName = 0; iFName < fragVaryVarNames.size(); ++iFName)
                {
                    std::string& fName = fragVaryVarNames[iFName];
                    std::string& vName = pVaryNameList->at(iFName);
                    ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(vName);
                    switch (dataType)
                    {
                    case ShaderCacheDataType::VEC4:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC4) = mRasterizer->InterpolateTriangleAttributeVector4(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC4),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC4),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC4),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::VEC3:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC3) = mRasterizer->InterpolateTriangleAttributeVector3(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC3),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC3),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC3),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::VEC2:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::VEC2) = mRasterizer->InterpolateTriangleAttributeVector2(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::VEC2),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::VEC2),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::VEC2),
                            rasterTriangleResult
                        );
                        break;
                    }
                    case ShaderCacheDataType::FLOAT:
                    {
                        RefShaderPiplineCachedData(&pipeline, fName, ShaderCacheDataType::FLOAT) = mRasterizer->InterpolateTriangleAttributeFloat(
                            RefVertexDataAdditionalAttr(&aOut, vName, ShaderCacheDataType::FLOAT),
                            RefVertexDataAdditionalAttr(&bOut, vName, ShaderCacheDataType::FLOAT),
                            RefVertexDataAdditionalAttr(&cOut, vName, ShaderCacheDataType::FLOAT),
                            rasterTriangleResult
                        );
                        break;
                    }
                    default:
                        break;
                    }
                }
                fragShader->Fragment();
                auto& fragColor = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::FragColor_VEC4, ShaderCacheDataType::VEC4);
                fragColor[0] = std::max(0.0f, std::min(1.0f, fragColor[0]));
                fragColor[1] = std::max(0.0f, std::min(1.0f, fragColor[1]));
                fragColor[2] = std::max(0.0f, std::min(1.0f, fragColor[2]));
                fragColor[3] = std::max(0.0f, std::min(1.0f, fragColor[3]));
                Screen::Color3 screenColor = Vector3i::create(
                    { static_cast<int>(255 * fragColor[0]), static_cast<int>(255 * fragColor[1]), static_cast<int>(255 * fragColor[2]), }
                );
                mScreen->drawPointDirect(rasterTriangleResult.xScreen, rasterTriangleResult.yScreen, screenColor);
            }
        };

    mScreen->clearBuffer();
    depthBuffer.fill(-2.0f);
    size_t numToDraw = meshData.NumFace(); // std::min(static_cast<size_t>(10), meshData.NumFace())
    for (size_t i = 0; i < numToDraw; ++i)
    {
        const int& idxAp = posIdxDataList[i][0], & idxBp = posIdxDataList[i][1], & idxCp = posIdxDataList[i][2];
        const int& idxAuv = uvIdxDataList[i][0], & idxBuv = uvIdxDataList[i][1], & idxCuv = uvIdxDataList[i][2];
        const int& idxAn = normalIdxDataList[i][0], & idxBn = normalIdxDataList[i][1], & idxCn = normalIdxDataList[i][2];

        // 默认背面剔除
        Matrix matToMeshLocal = matMesh.inverse();
        Vector3f camPosMeshLocal;
        camPosMeshLocal.assign(matToMeshLocal * camPos);
        Vector3f posALocal = posDataList[idxAp], posBLocal = posDataList[idxBp], posCLocal = posDataList[idxCp];
        Vector3f faceDirLocal = static_cast<Vector3f>(posBLocal - posALocal) % static_cast<Vector3f>(posCLocal - posALocal);
        Vector3f viewDir = camPosMeshLocal - posALocal;
        if (viewDir.dot(faceDirLocal) <= 0)
        {
            continue;
        }

        // 每次重新组装三角形 原始数据可能复用
        a.Reset(); b.Reset(); c.Reset();
        a[VAT::POSITION].assign(posDataList[idxAp]); a[VAT::POSITION][3] = 1.0f;
        b[VAT::POSITION].assign(posDataList[idxBp]); b[VAT::POSITION][3] = 1.0f;
        c[VAT::POSITION].assign(posDataList[idxCp]); c[VAT::POSITION][3] = 1.0f;
        a[VAT::UV].assign(uvDataList[idxAuv]);
        b[VAT::UV].assign(uvDataList[idxBuv]);
        c[VAT::UV].assign(uvDataList[idxCuv]);
        a[VAT::NORMAL].assign(normalDataList[idxAn]);
        b[VAT::NORMAL].assign(normalDataList[idxBn]);
        c[VAT::NORMAL].assign(normalDataList[idxCn]);

        // vertex shader
        ShaderBase* vertexShader = pipeline.GetShader(ShaderType::VERTEX);
        for (int iVd = 0; iVd < 3; ++iVd)
        {
            VertexData& curVertexData = triVertexData[iVd];

            auto& vertexPosInput = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPosInput_VEC4, ShaderCacheDataType::VEC4);
            vertexPosInput = curVertexData[VAT::POSITION];

            auto& vertexNormalInput = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexNormalInput_VEC4, ShaderCacheDataType::VEC4);
            vertexNormalInput = curVertexData[VAT::NORMAL];

            vertexShader->Vertex();

            curVertexData[VAT::POSITION] = RefShaderPiplineCachedData(&pipeline, ShaderBaseVars::VertexPos_VEC4, ShaderCacheDataType::VEC4);
            for (size_t iVaryName = 0; iVaryName < pVaryNameList->size(); ++iVaryName)
            {
                std::string& varyName = pVaryNameList->at(iVaryName);
                ShaderCacheDataType dataType = pipeline.GetCacheDataTypeByName(varyName);
                switch (dataType)
                {
                case ShaderCacheDataType::VEC4:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC4) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC4);
                    break;
                case ShaderCacheDataType::VEC3:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC3) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC3);
                    break;
                case ShaderCacheDataType::VEC2:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::VEC2) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::VEC2);
                    break;
                case ShaderCacheDataType::FLOAT:
                    RefVertexDataAdditionalAttr(&curVertexData, varyName, ShaderCacheDataType::FLOAT) = RefShaderPiplineCachedData(&pipeline, varyName, ShaderCacheDataType::FLOAT);
                    break;
                default:
                    break;
                }
            }
        }

        // 对视椎体边界裁剪
        triVertexDataOut.clear();
        idxTriVertexDataOut.clear();
        auto numTri = mClipper->clipIndexedTriangleList(triVertexData, idxTriVertexData, triVertexDataOut, idxTriVertexDataOut);
        if (numTri == 0)
        {
            continue;
        }

        // 输入只有1个刚组装的三角形，若在视椎体内，此处取出1个三角形，若跨越多个面，此处取出多个三角形，
        for (size_t idxOutTri = 0; idxOutTri < numTri; ++idxOutTri)
        {
            // 按顶点顺序取出 顶点可能复用 不取引用
            aOut.Reset(); bOut.Reset(); cOut.Reset();
            aOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 0]];
            bOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 1]];
            cOut = triVertexDataOut[idxTriVertexDataOut[idxOutTri * 3 + 2]];

            // 从 clipping space 到 ndc
            aOut[VAT::POSITION].perspectiveDivideInPlace();
            bOut[VAT::POSITION].perspectiveDivideInPlace();
            cOut[VAT::POSITION].perspectiveDivideInPlace();

            triVertexPos[0].assign(aOut[VAT::POSITION]);
            triVertexPos[1].assign(bOut[VAT::POSITION]);
            triVertexPos[2].assign(cOut[VAT::POSITION]);

            mRasterizer->DrawTiangle(triVertexPos.data(), screenWidth, screenHeight, DrawToScreen);
        }

    }
    mScreen->flush();
}
