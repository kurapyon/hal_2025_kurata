//=============================================================================
//
// ビーム処理 [beambullet.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "beambullet.h"
#include "particlebeam.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	BEAMBULLET_WIDTH	(10.0f)			// 頂点サイズ
#define	BEAMBULLET_HEIGHT	(10.0f)			// 頂点サイズ
#define	BEAMBULLET_SPEED	(10.0f)			// 弾の移動スピード

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBeamBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BEAMBULLET					g_beambullet[BEAMBULLET_MAX_BULLET];	// 弾ワーク
static int							g_TexNo;							// テクスチャ番号

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBeamBullet(void)
{
	MakeVertexBeamBullet();

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < BEAMBULLET_MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_beambullet[nCntBullet].material, sizeof(g_beambullet[nCntBullet].material));
		g_beambullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_beambullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_beambullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_beambullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_beambullet[nCntBullet].spd = { BEAMBULLET_SPEED, BEAMBULLET_SPEED, BEAMBULLET_SPEED };
		g_beambullet[nCntBullet].fWidth = BEAMBULLET_WIDTH;
		g_beambullet[nCntBullet].fHeight = BEAMBULLET_HEIGHT;
		g_beambullet[nCntBullet].blife = BEAMBULLET_LIFE;
		g_beambullet[nCntBullet].use = FALSE;

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBeamBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBeamBullet(void)
{
	
	for (int i = 0; i < BEAMBULLET_MAX_BULLET; i++)
	{
		if (g_beambullet[i].use)
		{
			// 弾の移動処理
			g_beambullet[i].pos.x -= sinf(g_beambullet[i].rot.y) * g_beambullet[i].spd.x;
			g_beambullet[i].pos.z -= cosf(g_beambullet[i].rot.y) * g_beambullet[i].spd.z;

			// パーティクルを付ける
			XMFLOAT4 col  = { 1.0f, 0.0f, 1.0f, 1.0f };
			XMFLOAT3 move = { 0.0f, 0.0f, 0.0f};
			XMFLOAT3 rot  = g_beambullet[i].rot;

			SetParticleBeam(g_beambullet[i].pos,
				move,
				rot,
				col,
				g_beambullet[i].fWidth,
				g_beambullet[i].fHeight,
				PARTICLEATC_LIFE);


			// フィールドの外に出るまたは、ライフ時間が0になったら弾を消す処理
			g_beambullet[i].blife--;
			if (g_beambullet[i].pos.x < MAP_LEFT
				|| g_beambullet[i].pos.x > MAP_RIGHT
				|| g_beambullet[i].pos.y < MAP_DOWN
				|| g_beambullet[i].pos.z < MAP_DOWN
				|| g_beambullet[i].pos.z > MAP_TOP
				|| g_beambullet[i].blife <= 0)
			{
				g_beambullet[i].use = FALSE;
				g_beambullet[i].blife = BEAMBULLET_LIFE;
			}

//#ifdef _DEBUG
//			// デバッグ表示
//			PrintDebugProc("Bullet rotX: %f\n", g_beambullet[i].rot.x);
//#endif
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBeamBullet(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < BEAMBULLET_MAX_BULLET; i++)
	{
		if (g_beambullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_beambullet[i].scl.x, g_beambullet[i].scl.y, g_beambullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_beambullet[i].rot.x, g_beambullet[i].rot.y + XM_PI, g_beambullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_beambullet[i].pos.x, g_beambullet[i].pos.y, g_beambullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_beambullet[i].mtxWorld, mtxWorld);

			// マテリアル設定
			SetMaterial(g_beambullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBeamBullet(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BEAMBULLET_WIDTH;
	float fHeight = BEAMBULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBeamBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{

	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < BEAMBULLET_MAX_BULLET; nCntBullet++)
	{
		if (!g_beambullet[nCntBullet].use)
		{
			g_beambullet[nCntBullet].pos = pos;
			g_beambullet[nCntBullet].rot = rot;
			g_beambullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_beambullet[nCntBullet].use = TRUE;

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

BEAMBULLET* GetBeamBullet(void)
{
	return &(g_beambullet[0]);
}
