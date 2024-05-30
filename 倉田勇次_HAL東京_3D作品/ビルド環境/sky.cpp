//=============================================================================
//
// 空処理 [sky.cpp]
// Author :倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "debugproc.h"
#include "sky.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MODEL_SKY			"data/MODEL/sky/sky.obj"	// モデルの読み込み

#define	VALUE_ROTATE		(0.001f)		// 回転量
#define VALUE_SIZE			(2.0f)			// サイズ
//=============================================================================
// グローバル変数
//=============================================================================
static SKY			g_Sky[MAX_SKY];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Sky[i].use = TRUE;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == TRUE)
		{
			// 回転させる
			g_Sky[i].rot.y += VALUE_ROTATE;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x * VALUE_SIZE, g_Sky[i].scl.y * VALUE_SIZE, g_Sky[i].scl.z * VALUE_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y + XM_PI, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		//// モデル描画
		DrawModel(&g_Sky[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_NONE);

}

//=============================================================================
// エネミーの取得
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
