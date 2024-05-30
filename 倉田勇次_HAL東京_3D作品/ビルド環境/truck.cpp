//=============================================================================
//
// トラック処理 [truck.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "obstacle.h"
#include "truck.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_TRUCK		"data/MODEL/obstacle/truck.obj"			// 読み込むモデル名
#define INFO_TRUCK		"data/FIELDDATA/truck.csv"				// 読み込むcsv名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static std::vector<OBSTACLE_DATA> g_Truck;
static DX11_MODEL		g_model;
static BOOL				g_load;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTruck(void)
{
	LoadModel(MODEL_TRUCK, &g_model);
	LoadFieldDataCSV(INFO_TRUCK, g_Truck);
	g_load = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTruck(void)
{
	if (g_load)
	{
		// モデルの解放処理
		UnloadModel(&g_model);

		// データの解放処理
		g_Truck.clear();
		g_load = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTruck(void)
{
#ifdef _DEBUG
#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTruck(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < g_Truck.size(); i++)
	{
			// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Truck[i].scl.x, g_Truck[i].scl.y, g_Truck[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Truck[i].rot.x, g_Truck[i].rot.y + XM_PI, g_Truck[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Truck[i].pos.x, g_Truck[i].pos.y, g_Truck[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}
