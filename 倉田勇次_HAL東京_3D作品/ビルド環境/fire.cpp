//=============================================================================
//
// エフェクト(炎)処理 [fire.cpp]
// Author :倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "fire.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

// 炎エフェクト
#define	FIRE_WIDTH			(24.0f)		// 頂点サイズ
#define	FIRE_HEIGHT			(32.0f)		// 頂点サイズ
#define FIRE_POSX			(940.0f)	// 炎を置くx座標の基準値
#define FIRE_POSZ			(800.0f)	// 炎を置くz座標の基準値
#define FIRE_INTERVAL		(130.0f)	// 炎を置く間隔
#define FIRE_NUM			(28)		// 1列に炎を置く数
#define FIRE_TEX_DEVX		(8)			// x軸方向のテクスチャ分割数
#define FIRE_TEX_DEVY		(1)			// y軸方向のテクスチャ分割数
#define	MAX_FIRE			(60)		// 炎最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		animCnt;		// アニメーションカウント
	BOOL		use;			// 使用しているかどうか

} FIRE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexFIre(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static VERTEX_3D* g_Vertex = NULL;
static FIRE					g_fire[MAX_FIRE];	// エフェクトワーク
static BOOL					g_bAlpaTest;		// アルファテストON/OFF

static int					g_TexNo;			// テクスチャ番号

static char *g_TextureName[] =
{
	"data/TEXTURE/fire.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFire(void)
{
	g_Vertex = new VERTEX_3D[4];
	MakeVertexFIre();

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

	// エフェクトの初期化
	for(int i = 0; i < MAX_FIRE; i++)
	{
		ZeroMemory(&g_fire[i].material, sizeof(g_fire[i].material));
		g_fire[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_fire[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_fire[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_fire[i].fWidth = FIRE_WIDTH;
		g_fire[i].fHeight = FIRE_HEIGHT;
		g_fire[i].use = FALSE;
	}

	g_bAlpaTest = TRUE;

	// 炎の設定
	for (int i = 0; i < FIRE_NUM / 2; i++)
	{
		// 右の列
		SetFire(XMFLOAT3(FIRE_POSX, 0.0f, FIRE_POSZ - FIRE_INTERVAL * i), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// 左の列
		SetFire(XMFLOAT3(-FIRE_POSX, 0.0f, FIRE_POSZ - FIRE_INTERVAL * i), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	for (int i = 0; i < FIRE_NUM / 2; i++)
	{
		// 上の列
		SetFire(XMFLOAT3(-(FIRE_POSZ / 2) + FIRE_INTERVAL * i, 0.0f, FIRE_POSZ), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// 下の列
		SetFire(XMFLOAT3(-(FIRE_POSZ / 2) + FIRE_INTERVAL * i, 0.0f, -FIRE_POSZ), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFire(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFire(void)
{
	for (int i = 0; i < MAX_FIRE; i++)
	{
		if (g_fire[i].use)
		{
			float tw = 1.0f / FIRE_TEX_DEVX;
			float th = 1.0f / FIRE_TEX_DEVY;
			float tx = ((int)(g_fire[i].animCnt) % FIRE_TEX_DEVX) * tw;	// テクスチャの左上X座標
			float ty = ((int)(g_fire[i].animCnt) / FIRE_TEX_DEVX) * th;	// テクスチャの左上Y座標

			// 頂点情報を更新
			g_Vertex[0].TexCoord = XMFLOAT2(tx, ty);
			g_Vertex[1].TexCoord = XMFLOAT2(tx + tw, ty);
			g_Vertex[2].TexCoord = XMFLOAT2(tx, ty + th);
			g_Vertex[3].TexCoord = XMFLOAT2(tx + tw, ty + th);

			// 頂点バッファに値をセットする
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			// 全頂点情報を上書き
			memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);

			GetDeviceContext()->Unmap(g_VertexBuffer, 0);

			// アニメーションを進める
			g_fire[i].animCnt += 0.3f;

			if ((int)(g_fire[i].animCnt) > FIRE_TEX_DEVX * FIRE_TEX_DEVY)
			{	// アニメーションが終了したら
				g_fire[i].animCnt = 0.0f;			// ループさせる
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFire(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE) SetAlphaTestEnable(TRUE);

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_FIRE; i++)
	{
		if(g_fire[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
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
			mtxScl = XMMatrixScaling(g_fire[i].scl.x, g_fire[i].scl.y, g_fire[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_fire[i].pos.x, g_fire[i].pos.y, g_fire[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_fire[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexFIre(void)
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

	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	float fWidth = FIRE_WIDTH;
	float fHeight = FIRE_HEIGHT;

	// 頂点座標の設定
	g_Vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	g_Vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	g_Vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	g_Vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	g_Vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	g_Vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	g_Vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	g_Vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 炎のアニメーションエフェクト
//=============================================================================
int SetFire(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdx = -1;

	for (int nIdx = 0; nIdx < MAX_FIRE; nIdx++)
	{
		if (!g_fire[nIdx].use)
		{
			g_fire[nIdx].pos = pos;
			g_fire[nIdx].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_fire[nIdx].fWidth = fWidth;
			g_fire[nIdx].fHeight = fHeight;
			g_fire[nIdx].use = TRUE;

			nIdx = nIdx;

			break;
		}
	}

	return nIdx;
}
