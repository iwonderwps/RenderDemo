#include "mesh_data.h"
#include "mesh_data_loader.h"


void MeshData::LoadMeshFromFile(const std::string& filePath, MeshData& meshData)
{
    ObjFileMeshLoader::GetInstance().LoadMeshFromFile(filePath, meshData);
    meshData.OnLoaded();
}

MeshData::MeshData():
    mNumFace(0), mNumVertex(0)
{    
}

MeshData::~MeshData()
{
}

std::vector<Vector3f>& MeshData::VertexList()
{
    return mVertexList;
}

std::vector<Vector2f>& MeshData::UVList()
{
    return mUVList;
}

std::vector<Vector3f>& MeshData::NormalList()
{
    return mNormalList;
}

std::vector<Vector3i>& MeshData::VertexIndexList()
{
    return mVertexIndexList;
}

std::vector<Vector3i>& MeshData::UVIndexList()
{
    return mUVIndexList;
}

std::vector<Vector3i>& MeshData::NormalIndexList()
{
    return mNormalIndexList;
}

size_t MeshData::NumFace() const
{
    return mNumFace;
}

size_t MeshData::NumVertex() const
{
    return mNumVertex;
}

void MeshData::Reset()
{
    mVertexList.clear();
    mUVList.clear();
    mNormalList.clear();
    mVertexIndexList.clear();
    mUVIndexList.clear();
    mNormalIndexList.clear();

    mNumFace = 0;
    mNumVertex = 0;
}

void MeshData::OnLoaded()
{
    mNumFace = mVertexIndexList.size();
    mNumVertex = mVertexList.size();
    assert(mNumFace == mUVIndexList.size() && mNumFace == mNormalIndexList.size());
}
