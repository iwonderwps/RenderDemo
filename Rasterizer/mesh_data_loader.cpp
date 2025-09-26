#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>

#include "vector.h"
#include "mesh_data_loader.h"
#include "mesh_data.h"


MeshDataLoader::MeshDataLoader()
{
}

MeshDataLoader::~MeshDataLoader()
{
}

void MeshDataLoader::LoadMeshFromFile(const std::string& filePath, MeshData& meshData)
{
    throw std::runtime_error("subclass methods should be called");
}

std::vector<int> MeshDataLoader::SplitStrToIntList(const std::string& aStr, const char& sep, const int& nullVal)
{
    std::vector<int> ret;
    std::stringstream ss(aStr);
    std::string value;
    while (std::getline(ss, value, sep)) {
        if (value == "") {
            ret.push_back(nullVal);
        }
        else {
            ret.push_back(std::stoi(value));
        }
    }
    return ret;
}

MeshDataLoader& ObjFileMeshLoader::GetInstance()
{
    static ObjFileMeshLoader loader;
    return loader;
}

ObjFileMeshLoader::ObjFileMeshLoader()
{
}

ObjFileMeshLoader::~ObjFileMeshLoader()
{
}

void ObjFileMeshLoader::LoadMeshFromFile(const std::string& filePath, MeshData& meshData)
{
    static const char sep = '/';
    static const int N = 3;

    std::string lineContent;
    std::string dataType, v1, v2, v3;
    Vector3i tempIndex;

    std::vector<Vector3f>& vertexList = meshData.VertexList();
    std::vector<Vector2f>& uvList = meshData.UVList();
    std::vector<Vector3f>& normalList = meshData.NormalList();

    std::vector<Vector3i>& vertexIndexList = meshData.VertexIndexList();
    std::vector<Vector3i>& uvIndexList = meshData.UVIndexList();
    std::vector<Vector3i>& normalIndexList = meshData.NormalIndexList();
    std::vector<Vector3i>* indexPList[N] = { &vertexIndexList, &uvIndexList, &normalIndexList };

    std::ifstream file;
    file.open(filePath.c_str());

    while (!file.eof()) {
        std::getline(file, lineContent);
        std::istringstream ssLine(lineContent);

        dataType.clear();
        ssLine >> dataType; // 空行会导致保持上次的值

        if (dataType == "v") {
            ssLine >> v1 >> v2 >> v3;
            Vector3f vertex = Vector3f::create({ std::stof(v1), std::stof(v2), std::stof(v3) });
            vertexList.push_back(std::move(vertex));
        }
        else if (dataType == "vt") {
            ssLine >> v1 >> v2;
            Vector2f uv = Vector2f::create({ std::stof(v1), std::stof(v2)});
            uvList.push_back(std::move(uv));
        }
        else if (dataType == "vn") {
            ssLine >> v1 >> v2 >> v3;
            Vector3f normal = Vector3f::create({ std::stof(v1), std::stof(v2), std::stof(v3) });
            normalList.push_back(std::move(normal));
        }
        else if (dataType == "f") {
            ssLine >> v1 >> v2 >> v3;
            std::vector<int> v1Index = SplitStrToIntList(v1, sep, 0);
            std::vector<int> v2Index = SplitStrToIntList(v2, sep, 0);
            std::vector<int> v3Index = SplitStrToIntList(v3, sep, 0);
            for (int i = 0; i < N; ++i) {
                std::vector<Vector3i>& curIndexList = *indexPList[i];
                tempIndex = Vector3i::create({ v1Index[i] - 1, v2Index[i] - 1, v3Index[i] - 1 });
                curIndexList.push_back(std::move(tempIndex));
            }
        }
    }

    file.close();
}
