#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "matrix.h"
#include "vector.h"


class MeshData;
class VertexData;

class Printer
{
public:
    Printer() {}
    virtual ~Printer() {}

public:
    static std::string ToString(const Matrix& m);
public:
    static void PrintMatrixContent(const Matrix& m);
    static void PrintMatrixInfo(const Matrix& m);

public:
    template <typename T, size_t N>
    static std::string ToString(const VectorTemplate<T, N>& v)
    {
        std::stringstream ss;
        for (size_t i = 0; i < N; ++i)
        {
            ss << v[i] << ", ";
        }
        return ss.str();
    }

    template <typename T, size_t N>
    static void PrintVectorContent(const VectorTemplate<T, N>& v)
    {
        std::cout << ToString(v) << "\n" << std::endl;
    }

public:
    template <typename T>
    static void PrintValue(const T val)
    {
        std::cout << val << "\n" << std::endl;
    }

public:
    static std::string ToString(const MeshData& meshData);
    static void PrintMeshDataInfo(const MeshData& meshData);

public:
    static std::string ToString(const VertexData& vertexData);
    static void PrintVertexData(const VertexData& vertexData);
    static std::unordered_map<enum VertexDataAttribute, std::string> mVertexAttrNames;
};

#endif
