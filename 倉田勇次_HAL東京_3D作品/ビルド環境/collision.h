//=============================================================================
//
// 当たり判定処理 [collision.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

BOOL CollisionBB(XMFLOAT3 center1, XMFLOAT3 size1, XMFLOAT3 center2, XMFLOAT3 size2);
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);

float dotProduct(XMVECTOR *v1, XMVECTOR *v2);
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2);
BOOL RayCast(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 pos0, XMFLOAT3 pos1, XMFLOAT3 *hit, XMFLOAT3 *normal);
BOOL CapsuleCollision(XMFLOAT3 apos, XMFLOAT3 bpos, XMFLOAT3 cpos, float r1, float r2);

