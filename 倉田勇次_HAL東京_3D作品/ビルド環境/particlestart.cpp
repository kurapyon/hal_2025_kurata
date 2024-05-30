//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "beambullet.h"
#include "particlestart.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)			// テクスチャの数

#define	PARTICLE_SIZE_X			(40.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y			(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE		(5.0f)		// 移動速度

#define	MAX_PARTICLE			(1)		// パーティクル最大数

#define STARTPARTICLE_START		(200.0f)	// パーティクルのスタート位置
#define STARTPARTICLE_SPEED		(10.0f)		// パーティクルのスピード
#define STARTPARTICLE_DECREASE	(0.9f)		// パーティクルのライフ減少時間

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticleStart(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;						// テクスチャ番号

static PARTICLESTART				g_ParticleStart[MAX_PARTICLE];	// パーティクルワーク
static XMFLOAT3						g_posBase;						// ビルボード発生位置
static float						g_fWidthBase = 5.0f;			// 基準の幅
static float						g_fHeightBase = 10.0f;			// 基準の高さ
static float						g_roty = 0.0f;					// 移動方向
static float						g_spd = 0.0f;					// 移動スピード

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticleStart(void)
{
	// 頂点情報の作成
	MakeVertexParticleStart();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_ParticleStart[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleStart[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleStart[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleStart[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_ParticleStart[nCntParticle].material, sizeof(g_ParticleStart[nCntParticle].material));
		g_ParticleStart[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleStart[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_ParticleStart[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_ParticleStart[nCntParticle].nLife = 0;
		g_ParticleStart[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticleStart(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticleStart(void)
{
	BEAMBULLET* beambullet = GetBeamBullet();

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_ParticleStart[nCntParticle].use == FALSE)
		{// 使用中

			// 少しずつ小さくして残像を見している
			g_ParticleStart[nCntParticle].scale.x *= STARTPARTICLE_DECREASE;
			g_ParticleStart[nCntParticle].scale.y *= STARTPARTICLE_DECREASE;
			g_ParticleStart[nCntParticle].scale.z *= STARTPARTICLE_DECREASE;

			//g_ParticleStart[nCntParticle].nLife--;
			//if (g_ParticleStart[nCntParticle].nLife <= 0)
			//{
			//	g_ParticleStart[nCntParticle].use = FALSE;
			//}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticleStart(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// セピア
	SetSepia(1);

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_ParticleStart[nCntParticle].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_ParticleStart[nCntParticle].scale.x, g_ParticleStart[nCntParticle].scale.y, g_ParticleStart[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_ParticleStart[nCntParticle].pos.x, g_ParticleStart[nCntParticle].pos.y, g_ParticleStart[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_ParticleStart[nCntParticle].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// セピア
	SetSepia(0);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticleStart(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticleStart(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleStart[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetParticleStart(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_ParticleStart[nCntParticle].use)
		{
			g_ParticleStart[nCntParticle].pos = pos;
			g_ParticleStart[nCntParticle].rot = rot;
			g_ParticleStart[nCntParticle].scale = { START_PARTICLE_SCALE, START_PARTICLE_SCALE, START_PARTICLE_SCALE };
			g_ParticleStart[nCntParticle].move = move;
			g_ParticleStart[nCntParticle].material.Diffuse = col;
			g_ParticleStart[nCntParticle].fSizeX = fSizeX;
			g_ParticleStart[nCntParticle].fSizeY = fSizeY;
			g_ParticleStart[nCntParticle].nLife = nLife;
			g_ParticleStart[nCntParticle].use = TRUE;

			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}

//=============================================================================
// パーティクル情報を取得
//=============================================================================
PARTICLESTART* GetPArticleStart(void)
{
	return &g_ParticleStart[0];
}