#pragma once

#include <string>

#include "vector.h"
#include "mesh_data.h"


class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

public:
    void LoadMeshFromFile(const std::string& filePath);

public:
    MeshData& GetMeshData();
    Matrix& GetMatLocalToWorld();

protected:
    MeshData mMeshData;

protected:
    Matrix mMatLocalToWorld;
};