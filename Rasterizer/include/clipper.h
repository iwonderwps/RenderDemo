#pragma once

#include <unordered_map>
#include <functional>

#include "vector.h"
#include "vertex_data.h"


class Clipper
{
    using real_t = float;
    using large_int = unsigned long long;
    using VAT = VertexDataAttribute;

public:
    static const int CLIP_BASE_DIAG_LEFT = 1;
    static const int CLIP_BASE_DIAG_RIGHT = 2;
    static const int CLIP_BASE_DIAG_END = 3;

public:
    struct TriangleEdgeIndex
    {
        size_t ia;
        size_t ib;
        bool operator==(const TriangleEdgeIndex& other) const;
        TriangleEdgeIndex(size_t a, size_t b);
        TriangleEdgeIndex(const TriangleEdgeIndex& other);
        inline void updateData(size_t a, size_t b);
    };
    struct TiangleVertexClipOutInfo
    {
        size_t outterIdxOffset[3] = { 0 };
        size_t innerIdxOffset[3] = { 0 };
    };

    using IndexedTriListClippingEdgeRecordType = std::unordered_map<TriangleEdgeIndex, size_t>;
    using IndexedTriListClippingVtxRecordType = std::unordered_map<size_t, size_t>;

public:
    Clipper();
    virtual ~Clipper();

public:
    virtual int clipLineSeg(Vector4f& aIn, Vector4f& bIn, Vector4f& xOut, Vector4f& yOut);

public:
    using lineSegClipFuncType = std::function<int(
        typename real_t d1, typename real_t d2,
        Vector4f& p1In, Vector4f& p2In,
        Vector4f& p1Out, Vector4f& p2Out
        )>;
public:
    static inline Vector4f lerp(const Vector4f& a, const Vector4f& b, float t);
    static float d2clippingPlane_CLIP_BASE_DIAG_LEFT(typename real_t component, typename real_t w);
    static float d2clippingPlane_CLIP_BASE_DIAG_RIGHT(typename real_t component, typename real_t w);
    static int lineSegClipClip_CLIP_BASE_DIAG_LEFT(
        typename real_t d1, typename real_t d2,
        Vector4f& p1In, Vector4f& p2In,
        Vector4f& p1Out, Vector4f& p2Out
    );
    static int lineSegClipClip_CLIP_BASE_DIAG_RIGHT(
        typename real_t d1, typename real_t d2,
        Vector4f& p1In, Vector4f& p2In,
        Vector4f& p1Out, Vector4f& p2Out
    );
    static int lineSegClipAccept(
        typename real_t d1, typename real_t d2,
        Vector4f& p1In, Vector4f& p2In,
        Vector4f& p1Out, Vector4f& p2Out
    );
    static int lineSegClipReject(
        typename real_t d1, typename real_t d2,
        Vector4f& p1In, Vector4f& p2In,
        Vector4f& p1Out, Vector4f& p2Out
    );
    // 目前只裁剪线段和三角形，等于0时根据图形整体做特殊处理
    static inline bool isClippingPlaneInnerSide_CLIP_BASE_DIAG_LEFT(float d);
    // 目前只裁剪线段和三角形，等于0时根据图形整体做特殊处理
    static inline bool isClippingPlaneInnerSide_CLIP_BASE_DIAG_RIGHT(float d);

public:
    virtual large_int clipIndexedTriangleList(
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );

public:
    using indexedTriangleListClipFuncType = std::function<int(
        float* dList,
        size_t triIdxList[3],
        TiangleVertexClipOutInfo* clipOutInfo,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
        )>;
public:
    static int indexedTriangleListClipC2(
        float* dList,
        size_t triIdxList[3],
        TiangleVertexClipOutInfo* clipOutInfo,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );
    static int indexedTriangleListClipC1(
        float* dList,
        size_t triIdxList[3],
        TiangleVertexClipOutInfo* clipOutInfo,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );
    static int indexedTriangleListClipAccept(
        float* dList,
        size_t triIdxList[3],
        TiangleVertexClipOutInfo* clipOutInfo,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );
    static int indexedTriangleListClipReject(
        float* dList,
        size_t triIdxList[3],
        TiangleVertexClipOutInfo* clipOutInfo,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );
public:
    // 输出1个在当前裁剪平面内的顶点。该顶点可能在之前的生成过程中已输出，此时只需产生新的下标数据，不需要产生新的顶点数据。
    inline static bool addOneClippedIndexedTriangleVtx(
        size_t idxVtx,
        IndexedTriListClippingVtxRecordType& mpRecordedVtx,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );
    // 输出1个当前裁剪平面和当前边的交点作为裁剪后的顶点。给定一个裁剪平面，2个原三角形的顶点确定1个交点。若交点在之前的过程中已输出，则只需产生新的下标数据。
    inline static bool addOneClippedIndexedTriangleEdgeCross(
        TriangleEdgeIndex& edge,
        IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross,
        float* dList,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );

public:
    large_int clipIndexedTriangleListAgainstBaseDiagPlane(
        int icomp,
        std::function<float(typename real_t component, typename real_t w)> d2plane,
        std::function<bool(float)> isPlaneInner,
        std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
        std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
    );

protected:
    std::unordered_map<int, std::unordered_map<int, lineSegClipFuncType>> mpLineSegClipFuncLeft;
    decltype(mpLineSegClipFuncLeft) mpLineSegClipFuncRight;

    std::unordered_map<int, std::unordered_map<int, indexedTriangleListClipFuncType>> mpIndexedTriangleListClipFunc;
};

template<>
struct std::hash<Clipper::TriangleEdgeIndex>
{
    size_t operator()(const Clipper::TriangleEdgeIndex& edge) const noexcept
    {
        //size_t small = edge.ia < edge.ib ? edge.ia : edge.ib;
        //size_t big = edge.ia ^ edge.ib ^ small;
        //return (big << (sizeof(size_t) * 8 / 2)) ^ small;
        return (edge.ia << (sizeof(size_t) * 8 / 2)) ^ edge.ib;
    }
};