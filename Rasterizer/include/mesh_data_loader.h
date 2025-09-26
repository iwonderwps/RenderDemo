#pragma once

#include <string>
#include <vector>


class MeshData;

class MeshDataLoader
{
public:
    MeshDataLoader();
    virtual ~MeshDataLoader();

public:
    virtual void LoadMeshFromFile(const std::string& filePath, MeshData& meshData);

public:
    static std::vector<int> SplitStrToIntList(const std::string& aStr, const char& sep, const int& nullVal);
};


class ObjFileMeshLoader : public MeshDataLoader
{
public:
    static MeshDataLoader& GetInstance();

protected:
    ObjFileMeshLoader();

public:
    virtual ~ObjFileMeshLoader();

public:
    virtual void LoadMeshFromFile(const std::string& filePath, MeshData& meshData) override;
};