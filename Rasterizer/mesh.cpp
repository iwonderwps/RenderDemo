#include "mesh.h"

Mesh::Mesh():
    mMatLocalToWorld(Matrix::identity(4))
{

}

Mesh::~Mesh()
{
}

void Mesh::LoadMeshFromFile(const std::string& filePath)
{
    mMeshData.Reset();
    MeshData::LoadMeshFromFile(filePath, mMeshData);
}

MeshData& Mesh::GetMeshData()
{
    return mMeshData;
}

Matrix& Mesh::GetMatLocalToWorld()
{
    return mMatLocalToWorld;
}
