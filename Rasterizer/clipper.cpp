#include <cassert>

#include "clipper.h"

#define _VP VAT::POSITION

inline Vector4f Clipper::lerp(const Vector4f& a, const Vector4f& b, float t)
{
    Vector4f ret = Vector4f::create();
    for (size_t i = 0; i < 4; ++i)
    {
        ret[i] = (1 - t) * a[i] + t * b[i];
    }
    return ret;
}

float Clipper::d2clippingPlane_CLIP_BASE_DIAG_LEFT(typename real_t component, typename real_t w) {
    return component - w;
}

float Clipper::d2clippingPlane_CLIP_BASE_DIAG_RIGHT(typename real_t component, typename real_t w) {
    return component + w;
}

int Clipper::lineSegClipClip_CLIP_BASE_DIAG_LEFT(
    typename real_t d1, typename real_t d2,
    Vector4f& p1In, Vector4f& p2In,
    Vector4f& p1Out, Vector4f& p2Out
)
{
    if (d1 > 0) {
        p1Out = p1In;
        p2Out = lerp(p1In, p2In, d1 / (d1 - d2));
    }
    else {
        p2Out = p2In;
        p1Out = lerp(p2In, p1In, d2 / (d2 - d1));
    }
    return 1;
}

int Clipper::lineSegClipClip_CLIP_BASE_DIAG_RIGHT(
    typename real_t d1, typename real_t d2,
    Vector4f& p1In, Vector4f& p2In,
    Vector4f& p1Out, Vector4f& p2Out
)
{
    if (d1 < 0) {
        p1Out = p1In;
        p2Out = lerp(p1In, p2In, d1 / (d1 - d2));
    }
    else {
        p2Out = p2In;
        p1Out = lerp(p2In, p1In, d2 / (d2 - d1));
    }
    return 1;
}

int Clipper::lineSegClipAccept(
    typename real_t d1, typename real_t d2,
    Vector4f& p1In, Vector4f& p2In,
    Vector4f& p1Out, Vector4f& p2Out
)
{
    p1Out = p1In;
    p2Out = p2In;
    return 1;
}

int Clipper::lineSegClipReject(
    typename real_t d1, typename real_t d2,
    Vector4f& p1In, Vector4f& p2In,
    Vector4f& p1Out, Vector4f& p2Out
)
{
    return 0;
}

inline bool Clipper::isClippingPlaneInnerSide_CLIP_BASE_DIAG_LEFT(float d)
{
    return d > 0;
}

inline bool Clipper::isClippingPlaneInnerSide_CLIP_BASE_DIAG_RIGHT(float d)
{
    return d < 0;
}

Clipper::Clipper()
{
    mpIndexedTriangleListClipFunc[3] = {
        {0, indexedTriangleListClipAccept},
    };
    mpIndexedTriangleListClipFunc[2] = {
        {1, indexedTriangleListClipAccept},
        {0, indexedTriangleListClipC2},
    };
    mpIndexedTriangleListClipFunc[1] = {
        {2, indexedTriangleListClipAccept},
        {1, indexedTriangleListClipC1},
        {0, indexedTriangleListClipC1},
    };
    mpIndexedTriangleListClipFunc[0] = {
        {3, indexedTriangleListClipReject},
        {2, indexedTriangleListClipReject},
        {1, indexedTriangleListClipReject},
        {0, indexedTriangleListClipReject},
    };

    mpLineSegClipFuncLeft[-1] = {
        {-1, lineSegClipReject},
        {0, lineSegClipReject},
        {1, lineSegClipClip_CLIP_BASE_DIAG_LEFT},
    };
    mpLineSegClipFuncLeft[0] = {
        {-1, lineSegClipReject},
        {0, lineSegClipReject},
        {1, lineSegClipAccept},
    };
    mpLineSegClipFuncLeft[1] = {
        {-1, lineSegClipClip_CLIP_BASE_DIAG_LEFT},
        {0, lineSegClipAccept},
        {1, lineSegClipAccept},
    };

    mpLineSegClipFuncRight[-1] = {
        {-1, lineSegClipAccept},
        {0, lineSegClipAccept},
        {1, lineSegClipClip_CLIP_BASE_DIAG_RIGHT},
    };
    mpLineSegClipFuncRight[0] = {
        {-1, lineSegClipAccept},
        {0, lineSegClipReject},
        {1, lineSegClipReject},
    };
    mpLineSegClipFuncRight[1] = {
        {-1, lineSegClipClip_CLIP_BASE_DIAG_RIGHT},
        {0, lineSegClipReject},
        {1, lineSegClipReject},
    };
}

Clipper::~Clipper()
{
}

int Clipper::clipLineSeg(Vector4f& p1In, Vector4f& p2In, Vector4f& p1Out, Vector4f& p2Out)
{
    Vector4f& aCur = p1In;
    Vector4f& bCur = p2In;

    Vector4f tmpx1 = Vector4f::create();
    Vector4f tmpy1 = Vector4f::create();
    Vector4f tmpx2 = Vector4f::create();
    Vector4f tmpy2 = Vector4f::create();

    real_t pa, pb, wa, wb, da, db;
    int ia, ib;

    // 可换为矩阵乘法实现
    for (int icomp = 0; icomp < 3; ++icomp) {
        pa = aCur[icomp];
        pb = bCur[icomp];
        wa = aCur[3];
        wb = bCur[3];
        da = this->d2clippingPlane_CLIP_BASE_DIAG_LEFT(pa, wa);
        db = this->d2clippingPlane_CLIP_BASE_DIAG_LEFT(pb, wb);
        ia = da < 0 ? -1 : da == 0 ? 0 : 1;
        ib = db < 0 ? -1 : db == 0 ? 0 : 1;
        if (this->mpLineSegClipFuncLeft[ia][ib](da, db, aCur, bCur, tmpx1, tmpy1) == 0) {
            return 0;
        }
        aCur = tmpx1;
        bCur = tmpy1;
        //
        pa = aCur[icomp];
        pb = bCur[icomp];
        wa = aCur[3];
        wb = bCur[3];
        da = this->d2clippingPlane_CLIP_BASE_DIAG_RIGHT(pa, wa);
        db = this->d2clippingPlane_CLIP_BASE_DIAG_RIGHT(pb, wb);
        ia = da < 0 ? -1 : da == 0 ? 0 : 1;
        ib = db < 0 ? -1 : db == 0 ? 0 : 1;
        if (this->mpLineSegClipFuncRight[ia][ib](da, db, aCur, bCur, tmpx2, tmpy2) == 0) {
            return 0;
        }
        aCur = tmpx2;
        bCur = tmpy2;
    }
    p1Out = std::move(aCur);
    p2Out = std::move(bCur);
    return 1;
}

inline bool Clipper::addOneClippedIndexedTriangleVtx
(
    size_t idxVtx, 
    IndexedTriListClippingVtxRecordType& mpRecordedVtx, 
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList, 
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    bool bRecorded = mpRecordedVtx.find(idxVtx) != mpRecordedVtx.end();
    if (bRecorded) {
        // 有记录时，不产生新的顶点，只记录下标
        size_t idxRecorded = mpRecordedVtx[idxVtx];
        outIdxList.push_back(idxRecorded);
    }
    else {
        // 未记录时，复制顶点，记录新的下标
        outVtxList.push_back(vtxList[idxVtx]);
        size_t idxRecorded = outVtxList.size() - 1;
        outIdxList.push_back(idxRecorded);
        mpRecordedVtx[idxVtx] = idxRecorded;
    }
    return bRecorded;
}

inline bool Clipper::addOneClippedIndexedTriangleEdgeCross
(
    TriangleEdgeIndex& edge,
    IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, 
    float* dList,
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    bool bRecorded = mpRecordedEdgeCross.find(edge) != mpRecordedEdgeCross.end();
    if (bRecorded) {
        // 有记录时，不产生新的顶点，只记录下标
        size_t idxRecorded = mpRecordedEdgeCross[edge];
        outIdxList.push_back(idxRecorded);
    }
    else {
        // 未记录时，复制顶点，记录新的下标
        // 此时只求交点，不考虑内外
        outVtxList.push_back(
            VertexData::Lerp(
                vtxList[edge.ia], vtxList[edge.ib], dList[edge.ia] / (dList[edge.ia] - dList[edge.ib])
            )
        );
        size_t idxRecorded = outVtxList.size() - 1;
        outIdxList.push_back(idxRecorded);
        mpRecordedEdgeCross[edge] = idxRecorded;
    }
    return bRecorded;
}

int Clipper::indexedTriangleListClipAccept
(
    float* dList, 
    size_t triIdxList[3], 
    TiangleVertexClipOutInfo* clipOutInfo, 
    IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx, 
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList, 
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    // 输出的下标列表会增长3
    int nInc = 0;
    // 保持输入的顶点顺序
    for (size_t i = 0; i < 3; ++i) {
        addOneClippedIndexedTriangleVtx(triIdxList[i], mpRecordedVtx, vtxList, idxList, outVtxList, outIdxList);
        nInc += 1;
    }
    assert(nInc == 3);
    return 1; // nInc / 3
}

int Clipper::indexedTriangleListClipReject
(
    float* dList,
    size_t triIdxList[3],
    TiangleVertexClipOutInfo* clipOutInfo,
    IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    // 不产生输出
    return 0;
}

int Clipper::indexedTriangleListClipC2
(
    float* dList,
    size_t triIdxList[3],
    TiangleVertexClipOutInfo* clipOutInfo,
    IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    // 下标列表增长6，产生2个三角形
    int nInc = 0;

    auto iIdxOffsetOut = clipOutInfo->outterIdxOffset[0];
    auto iIdxOffsetIn1 = (iIdxOffsetOut + 1) % 3;
    auto iIdxOffsetIn2 = (iIdxOffsetIn1 + 1) % 3;

    TriangleEdgeIndex edge1(triIdxList[iIdxOffsetOut], triIdxList[iIdxOffsetIn1]);
    TriangleEdgeIndex edge2(triIdxList[iIdxOffsetIn2], triIdxList[iIdxOffsetOut]);

    addOneClippedIndexedTriangleEdgeCross(edge1, mpRecordedEdgeCross, dList, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleVtx(triIdxList[iIdxOffsetIn1], mpRecordedVtx, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleVtx(triIdxList[iIdxOffsetIn2], mpRecordedVtx, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;

    addOneClippedIndexedTriangleEdgeCross(edge1, mpRecordedEdgeCross, dList, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleVtx(triIdxList[iIdxOffsetIn2], mpRecordedVtx, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleEdgeCross(edge2, mpRecordedEdgeCross, dList, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;

    assert(nInc == 6);
    return 2; // 6 / 3
}

int Clipper::indexedTriangleListClipC1
(
    float* dList,
    size_t triIdxList[3],
    TiangleVertexClipOutInfo* clipOutInfo,
    IndexedTriListClippingEdgeRecordType& mpRecordedEdgeCross, IndexedTriListClippingVtxRecordType& mpRecordedVtx,
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    // 下标列表增长3，产生1个三角形
    int nInc = 0;

    auto idxOffsetIn = clipOutInfo->innerIdxOffset[0];
    auto idxOffsetOut1 = (idxOffsetIn + 1) % 3;
    auto idxOffsetOut2 = (idxOffsetOut1 + 1) % 3;

    TriangleEdgeIndex edge1(triIdxList[idxOffsetIn], triIdxList[idxOffsetOut1]);
    TriangleEdgeIndex edge2(triIdxList[idxOffsetOut2], triIdxList[idxOffsetIn]);

    addOneClippedIndexedTriangleVtx(triIdxList[idxOffsetIn], mpRecordedVtx, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleEdgeCross(edge1, mpRecordedEdgeCross, dList, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;
    addOneClippedIndexedTriangleEdgeCross(edge2, mpRecordedEdgeCross, dList, vtxList, idxList, outVtxList, outIdxList);
    nInc += 1;

    assert(nInc == 3);
    return 1; // 3 / 3
}

Clipper::large_int Clipper::clipIndexedTriangleListAgainstBaseDiagPlane(
    int icomp,
    std::function<float(typename real_t component, typename real_t w)> d2plane,
    std::function<bool(float)> isPlaneInner,
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList,
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    assert(idxList.size() % 3 == 0);

    size_t nVtxCnt = vtxList.size();
    std::vector<float> dList(nVtxCnt);
    for (size_t iVtx = 0; iVtx < nVtxCnt; ++iVtx) {
        dList[iVtx] = d2plane(vtxList[iVtx][_VP][icomp], vtxList[iVtx][_VP][3]);
    }

    Clipper::IndexedTriListClippingEdgeRecordType mpEdgeRecord;
    Clipper::IndexedTriListClippingVtxRecordType mpVtxRecord;
    size_t curTriIdxList[3];

    size_t nIdxCnt = idxList.size();
    for (size_t iIdx = 0; iIdx < nIdxCnt; iIdx += 3) {
        int nz = 0, z = 0;
        int idxInner = 0, idxOutter = 0;
        Clipper::TiangleVertexClipOutInfo curClipOutInfo;
        for (int iTri = 0; iTri < 3; ++iTri) {
            auto curVtxIdx = idxList[iIdx + iTri];
            curTriIdxList[iTri] = curVtxIdx;
            float d = dList[curVtxIdx];
            if (d == 0) {
                z += 1;
            }
            else if (isPlaneInner(d)) {
                nz += 1;
                curClipOutInfo.innerIdxOffset[idxInner++] = iTri;
            }
            else {
                curClipOutInfo.outterIdxOffset[idxOutter++] = iTri;
            }
        }
        assert(nz + z <= 3);
        auto clipFunc = this->mpIndexedTriangleListClipFunc[nz][z];
        clipFunc(
            dList.data(),
            curTriIdxList,
            &curClipOutInfo,
            mpEdgeRecord, mpVtxRecord,
            vtxList, idxList,
            outVtxList, outIdxList
        );
    }

    assert(outIdxList.size() % 3 == 0);
    return outIdxList.size() / 3;
}

Clipper::large_int Clipper::clipIndexedTriangleList(
    std::vector<VertexData>& vtxList, std::vector<size_t>& idxList, 
    std::vector<VertexData>& outVtxList, std::vector<size_t>& outIdxList
)
{
    std::vector<VertexData>* pCurVtxList = &vtxList;
    std::vector<size_t>* pCurIdxList = &idxList;

    std::vector<VertexData> tmpVtxList1, tmpVtxList2;
    std::vector<size_t> tmpIdxList1, tmpIdxList2;

    large_int numTri = 0;

    for (int icomp = 0; icomp < 3; ++icomp) {
        tmpVtxList1.clear();
        tmpIdxList1.clear();
        numTri = this->clipIndexedTriangleListAgainstBaseDiagPlane(
            icomp,
            d2clippingPlane_CLIP_BASE_DIAG_LEFT, isClippingPlaneInnerSide_CLIP_BASE_DIAG_LEFT,
            *pCurVtxList, *pCurIdxList,
            tmpVtxList1, tmpIdxList1
        );
        pCurVtxList = &tmpVtxList1;
        pCurIdxList = &tmpIdxList1;
        if (numTri == 0) {
            break;
        }
        //
        tmpVtxList2.clear();
        tmpIdxList2.clear();
        numTri = this->clipIndexedTriangleListAgainstBaseDiagPlane(
            icomp,
            d2clippingPlane_CLIP_BASE_DIAG_RIGHT, isClippingPlaneInnerSide_CLIP_BASE_DIAG_RIGHT,
            *pCurVtxList, *pCurIdxList,
            tmpVtxList2, tmpIdxList2
        );
        pCurVtxList = &tmpVtxList2;
        pCurIdxList = &tmpIdxList2;
        if (numTri == 0) {
            break;
        }
    }
    outVtxList = std::move(*pCurVtxList);
    outIdxList = std::move(*pCurIdxList);
    return numTri;
}

bool Clipper::TriangleEdgeIndex::operator==(const TriangleEdgeIndex& other) const
{
    //return (this->ia == other.ia && this->ib == other.ib) || (this->ia == other.ib && this->ib == other.ia);
    return this->ia == other.ia && this->ib == other.ib;
}

Clipper::TriangleEdgeIndex::TriangleEdgeIndex(size_t a, size_t b)
{
    updateData(a, b);
}

Clipper::TriangleEdgeIndex::TriangleEdgeIndex(const TriangleEdgeIndex& other)
    : ia(other.ia), ib(other.ib)
{
}

inline void Clipper::TriangleEdgeIndex::updateData(size_t a, size_t b)
{
    assert(a != b);
    size_t small = a < b ? a : b;
    size_t big = a ^ b ^ small;
    this->ia = big;
    this->ib = small;
}