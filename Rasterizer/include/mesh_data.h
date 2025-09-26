#pragma once

#include <string>

#include "vector.h"


class MeshData
{
public:
    static void LoadMeshFromFile(const std::string& filePath, MeshData& meshData);

public:
    MeshData();
    virtual ~MeshData();

public:
    std::vector<Vector3f>& VertexList();
    std::vector<Vector2f>& UVList();
    std::vector<Vector3f>& NormalList();

    std::vector<Vector3i>& VertexIndexList();
    std::vector<Vector3i>& UVIndexList();
    std::vector<Vector3i>& NormalIndexList();

public:
    size_t NumFace() const;
    size_t NumVertex() const;

public:
    void Reset();

protected:
    void OnLoaded();

protected:
    std::vector<Vector3f> mVertexList;
    std::vector<Vector2f> mUVList;
    std::vector<Vector3f> mNormalList;

protected:
    std::vector<Vector3i> mVertexIndexList;
    std::vector<Vector3i> mUVIndexList;
    std::vector<Vector3i> mNormalIndexList;

protected:
    size_t mNumFace;
    size_t mNumVertex;
};




