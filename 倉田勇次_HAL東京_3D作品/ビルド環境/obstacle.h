//=============================================================================
//
// 障害物処理 [obstacle.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once
#include <vector>

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class OBSTACLE_DATA
{
	public:
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadFieldDataCSV(char* FileName, std::vector<OBSTACLE_DATA>& tbl);

