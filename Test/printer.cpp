#include <iostream>
#include <sstream>

#include "printer.h"
#include "mesh_data.h"
#include "vertex_data.h"


using namespace std;


std::string Printer::ToString(const Matrix& m)
{
    size_t nRow = m.rowNum();
    size_t nCol = m.colNum();

    stringstream ss;
    for (int i = 0; i < nRow; ++i)
    {
        for (int j = 0; j < nCol; ++j)
        {
            ss << m(i, j) << ", ";
        }
        if (i != nRow - 1)
        {
            ss << "\n";
        }
    }

    return ss.str();
}

void Printer::PrintMatrixContent(const Matrix& m)
{
    cout << ToString(m) << "\n" << endl;
}

void Printer::PrintMatrixInfo(const Matrix& m)
{
    cout << "Matrix:\n" << "size: row-" << m.rowNum() << ", col-" << m.colNum() << endl;
}

std::string Printer::ToString(const MeshData& meshData)
{
    stringstream ss;

    ss << "Num Face: " << meshData.NumFace() << ", Num Vertex: " << meshData.NumVertex();

    return ss.str();
}

void Printer::PrintMeshDataInfo(const MeshData& meshData)
{
    cout << ToString(meshData) << "\n" << endl;
}

std::string Printer::ToString(const VertexData& vertexData)
{
    stringstream ss;
    auto attrs = vertexData.AllVec4Attributes();
    size_t i = 0, last = attrs.size() - 1;
    for (auto p = attrs.begin(); p != attrs.end(); ++p)
    {
        ss << mVertexAttrNames[p->first] << ": " << ToString(p->second);
        if (i++ < last)
        {
            ss << "\n";
        }
    }
    return ss.str();
}

void Printer::PrintVertexData(const VertexData& vertexData)
{
    cout << ToString(vertexData) << endl;
}

std::unordered_map<enum VertexDataAttribute, std::string> Printer::mVertexAttrNames = {
    {VertexDataAttribute::POSITION, "POSITION"},
    {VertexDataAttribute::UV, "UV"},
    {VertexDataAttribute::NORMAL, "NORMAL"},
};

