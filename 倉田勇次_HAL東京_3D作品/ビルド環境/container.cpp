//=============================================================================
//
// コンテナ処理 [container.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "obstacle.h"
#include "container.h"
#include "collision.h"
#include "boss.h"
#include "player.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_YCONTAINER	"data/MODEL/obstacle/yellowContainer.obj"	// 読み込むモデル名
#define	MODEL_GCONTAINER	"data/MODEL/obstacle/greenContainer.obj"	// 読み込むモデル名
#define	MODEL_COLLIDER		"data/MODEL/collider/collider.obj"			// 読み込むモデル名
#define INFO_YCONTAINER		"data/FIELDDATA/yellowCont.csv"				// 読み込むcsv名
#define INFO_GCONTAINER		"data/FIELDDATA/greenCont1.csv"			 	// 読み込むcsv名
#define INFO_COLLIDER		"data/FIELDDATA/collider.csv"				// 読み込むcsv名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void HiddenCheckView(void);
void HiddenCheckTranslucent(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static std::vector<OBSTACLE_DATA> g_YContainer;
static std::vector<OBSTACLE_DATA> g_GContainer;
static std::vector<OBSTACLE_DATA> g_collider;

static DX11_MODEL		g_model[COLOR_CONTAINER_MAX];
static DX11_MODEL		g_colliderModel;

BOOL isHit = FALSE;			// プレイヤーが障害物に隠れていたら
BOOL isTranslucent = FALSE;	// プレイヤーとカメラの間にあるかどうかの判定

XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

static BOOL				g_load;
std::vector<POLYGON> colliderPolygons;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitContainer(void)
{
	LoadModel(MODEL_COLLIDER,	&g_colliderModel);
	LoadModel(MODEL_YCONTAINER, &g_model[COLOR_YELLOW]);
	LoadModel(MODEL_GCONTAINER, &g_model[COLOR_GREEN]);

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&g_model[COLOR_YELLOW], &diffuse[0]);
	GetModelDiffuse(&g_model[COLOR_GREEN], &diffuse[0]);

	LoadFieldDataCSV(INFO_COLLIDER,   g_collider);
	LoadFieldDataCSV(INFO_YCONTAINER, g_YContainer);
	LoadFieldDataCSV(INFO_GCONTAINER, g_GContainer);

	static std::vector<MODEL> colliderModel;

	MakeVertexLists(MODEL_COLLIDER, &colliderPolygons);

	g_load = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitContainer(void)
{
	if (g_load)
	{
		for (int i = 0; i < COLOR_CONTAINER_MAX; i++)
		{
			// モデルの解放処理
			UnloadModel(&g_model[i]);
		}
		// モデルの解放処理
		UnloadModel(&g_colliderModel);

		// データの解放処理
		g_collider.clear();
		g_YContainer.clear();
		g_GContainer.clear();
		g_load = FALSE;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateContainer(void)
{

	// プレイヤーが隠れているか判定する
	HiddenCheckView();
	//HiddenCheckTranslucent();
#ifdef _DEBUG
	PrintDebugProc("isHit:%d\n", isHit);
	PrintDebugProc("isTranslucent:%d\n", isTranslucent);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawContainer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 黄色コンテナ
	for (int i = 0; i < g_YContainer.size(); i++)
	{
			// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_YContainer[i].scl.x, g_YContainer[i].scl.y, g_YContainer[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_YContainer[i].rot.x, g_YContainer[i].rot.y + XM_PI, g_YContainer[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_YContainer[i].pos.x, g_YContainer[i].pos.y, g_YContainer[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_model[COLOR_YELLOW]);
	}

	// 緑コンテナ
	for (int i = 0; i < g_GContainer.size(); i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_GContainer[i].scl.x, g_GContainer[i].scl.y, g_GContainer[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_GContainer[i].rot.x, g_GContainer[i].rot.y + XM_PI, g_GContainer[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_GContainer[i].pos.x, g_GContainer[i].pos.y, g_GContainer[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_model[COLOR_GREEN]);
	}

	// コライダー
	for (int i = 0; i < g_collider.size(); i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_collider[i].scl.x, g_collider[i].scl.y, g_collider[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_collider[i].rot.x, g_collider[i].rot.y + XM_PI, g_collider[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_collider[i].pos.x, g_collider[i].pos.y, g_collider[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

#ifdef _DEBUG
		// モデル描画
		DrawModelWireFrame(&g_colliderModel);
#endif

	}


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

std::vector<OBSTACLE_DATA> *GetCollider(void)
{
	return &g_collider;
}

// プレイヤーが障害物に隠れていないか判定する
void HiddenCheckView(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	// 視野角に入っていたら
	if (boss[0].ViewingAngle == TRUE)
	{
		for (int c = 0; c < g_collider.size(); c++)
		{
			if (isHit == TRUE)
			{
				break;
			}

			XMFLOAT3 pos = g_collider[c].pos;
			XMFLOAT3 scl = g_collider[c].scl;

			// DirectXとMayaのスケール差を変換
			float scale = COLLIDER_SCL;
			scl.x *= scale;
			scl.y *= scale;
			scl.z *= scale;
			scl.x /= 2.0f;
			scl.y /= 2.0f;
			scl.z /= 2.0f;

			// bossからplayerに対して例を飛ばし、コリジョンと例が当たっているか判定する
			for (int i = 0; i < colliderPolygons.size(); i++)
			{
				XMFLOAT3 hit;
				XMFLOAT3 normal;

				XMFLOAT3 polygonPosArray[3];

				for (int p = 0; p < 3; p++)
				{
					polygonPosArray[p] = colliderPolygons[i].vertex[p].Position;

					polygonPosArray[p].x /= 5;
					polygonPosArray[p].y /= 5;
					polygonPosArray[p].z /= 5;

					polygonPosArray[p].x *= scl.x;
					polygonPosArray[p].y *= scl.y;
					polygonPosArray[p].z *= scl.z;

					polygonPosArray[p].x += pos.x;
					polygonPosArray[p].y += pos.y;
					polygonPosArray[p].z += pos.z;
				}

				XMFLOAT3 BossPos = boss[0].pos;
				XMFLOAT3 PlayerPos = player[0].pos;

				float yAve = (polygonPosArray[0].y + polygonPosArray[1].y + polygonPosArray[2].y) / 3.0f;
				BossPos.y = yAve;
				PlayerPos.y = yAve;
				isHit = RayCast(polygonPosArray[0], polygonPosArray[1], polygonPosArray[2],
					BossPos, PlayerPos, &hit, &normal);

				// レイが当たっていたら終わる
				if (isHit == TRUE)
				{
					break;
				}
			}
		}

		// 視野角に入っていてかつ、レイにもあたっている
		if (isHit == TRUE)
		{
			// プレイヤーが隠れている
			boss[0].view = FALSE;
			isHit = FALSE;
		}
		else
		{
			// 視野に入っている
			boss[0].view = TRUE;

		}
	}

}

// カメラとプレイヤーの間に障害物があったら半透明にする
void HiddenCheckTranslucent(void)
{
	PLAYER* player = GetPlayer();
	CAMERA* camera = GetCamera();

	for (int c = 0; c < g_collider.size(); c++)
	{
		if (isTranslucent == TRUE)
		{
			break;
		}

		XMFLOAT3 pos = g_collider[c].pos;
		XMFLOAT3 scl = g_collider[c].scl;

		// DirectXとMayaのスケール差を変換
		float scale = COLLIDER_SCL;
		scl.x *= scale;
		scl.y *= scale;
		scl.z *= scale;
		scl.x /= 2.0f;
		scl.y /= 2.0f;
		scl.z /= 2.0f;

		// cameraからplayerに対して例を飛ばし、コリジョンと例が当たっているか判定する
		for (int i = 0; i < colliderPolygons.size(); i++)
		{
			XMFLOAT3 hit;
			XMFLOAT3 normal;

			XMFLOAT3 polygonPosArray[3];

			for (int p = 0; p < 3; p++)
			{
				polygonPosArray[p] = colliderPolygons[i].vertex[p].Position;

				polygonPosArray[p].x /= 5;
				polygonPosArray[p].y /= 5;
				polygonPosArray[p].z /= 5;

				polygonPosArray[p].x *= scl.x;
				polygonPosArray[p].y *= scl.y;
				polygonPosArray[p].z *= scl.z;

				polygonPosArray[p].x += pos.x;
				polygonPosArray[p].y += pos.y;
				polygonPosArray[p].z += pos.z;
			}

			XMFLOAT3 CameraPos = camera[0].pos;
			XMFLOAT3 PlayerPos = player[0].pos;

			float yAve = (polygonPosArray[0].y + polygonPosArray[1].y + polygonPosArray[2].y) / 3.0f;
			CameraPos.y = yAve;
			PlayerPos.y = yAve;

			// レイキャスト
			isTranslucent = RayCast(polygonPosArray[0], polygonPosArray[1], polygonPosArray[2],
				CameraPos, PlayerPos, &hit, &normal);

			// レイが当たっていたら
			if (isTranslucent == TRUE)
			{
				break;
			}

		}

		// レイが当たっていたら
		if (isTranslucent == TRUE)
		{
			for (int j = 0; j < g_model[COLOR_YELLOW].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_YELLOW], j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));
			}

			for (int j = 0; j < g_model[COLOR_GREEN].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_GREEN], j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));
			}

		}
		else
		{
			for (int j = 0; j < g_model[COLOR_YELLOW].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_YELLOW], j, diffuse[0]);
			}

			for (int j = 0; j < g_model[COLOR_GREEN].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_GREEN], j, diffuse[0]);
			}
		}

	}
}