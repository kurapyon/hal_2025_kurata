//=============================================================================
//
// ベジェ弾処理 [bezierbullet.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "bezierbullet.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)			// テクスチャの数

#define	BULLET_WIDTH			(48.0f)		// 頂点サイズ
#define	BULLET_HEIGHT			(48.0f)		// 頂点サイズ
#define	BULLET_SPEED			(5.0f)		// 弾の移動スピード
#define BEZIERBULLET_SCL		(1.0f)		// 弾のスケール
#define BEZIERBULLET_HP			(60.0f)		// 弾の出現時間
#define BEZIERBULLET_UPSPD		(0.01f)		// スピードの上昇値
#define BEZIERBULLET_GOALSPD	(1.0f)		// スピードの上限
#define TEX_DEVX				(2)			// x軸方向のテクスチャ分割数
#define TEX_DEVY				(5)			// y軸方向のテクスチャ分割数

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBezierBullet(void);
// ベジェ曲線
XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s);
XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s);
// アニメーション
void BezierAnimation(int i);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static VERTEX_3D* g_Vertex = NULL;

static BEZIERBULLET					g_BezierBullet[MAX_BEZIERBULLET];	// 弾ワーク
static int							g_TexNo;			// テクスチャ番号
static BOOL							g_bAlpaTest;		// アルファテストON/OFF

static char* g_TextureName[] =
{
	"data/TEXTURE/blt.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBezierBullet(void)
{
	g_Vertex = new VERTEX_3D[4];
	MakeVertexBezierBullet();

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

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BEZIERBULLET; nCntBullet++)
	{
		ZeroMemory(&g_BezierBullet[nCntBullet].material, sizeof(g_BezierBullet[nCntBullet].material));
		g_BezierBullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_BezierBullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_BezierBullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_BezierBullet[nCntBullet].move = { 0.0f, 0.0f, 0.0f };
		g_BezierBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_BezierBullet[nCntBullet].spd = BULLET_SPEED;
		g_BezierBullet[nCntBullet].hp = BEZIERBULLET_HP;
		g_BezierBullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_BezierBullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_BezierBullet[nCntBullet].use = FALSE;

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBezierBullet(void)
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

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBezierBullet(void)
{
	for (int i = 0; i < MAX_BEZIERBULLET; i++)
	{
		if (g_BezierBullet[i].use)
		{
			// アニメーション
			//BezierAnimation(i);

			// 弾の移動処理 ベジェ曲線
			g_BezierBullet[i].s += BEZIERBULLET_UPSPD;

			if (g_BezierBullet[i].s > BEZIERBULLET_GOALSPD)
			{
				g_BezierBullet[i].use = FALSE;
				g_BezierBullet[i].s = 0.0f;
			}

			XMVECTOR vP0 = XMLoadFloat3(&g_BezierBullet[i].p0);
			XMVECTOR vP1 = XMLoadFloat3(&g_BezierBullet[i].p1);
			XMVECTOR vP2 = XMLoadFloat3(&g_BezierBullet[i].p2t);

			float s = g_BezierBullet[i].s * g_BezierBullet[i].s;
			XMVECTOR vAns = QuadraticBezierCurve(vP0, vP1, vP2, s);

			XMStoreFloat3(&g_BezierBullet[i].pos, vAns);
			XMVECTOR vold_pos = XMLoadFloat3(&g_BezierBullet[i].pos);

			XMVECTOR vec = vAns - vold_pos;        //移動前とあとの座標の差分（角度）
			float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);        //その差分を使って角度を求めている
		}

#ifdef _DEBUG
#endif 



	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBezierBullet(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE) SetAlphaTestEnable(TRUE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ライティングを無効
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	for (int i = 0; i < MAX_BEZIERBULLET; i++)
	{
		if (g_BezierBullet[i].use)
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
			mtxScl = XMMatrixScaling(g_BezierBullet[i].scl.x * BEZIERBULLET_SCL, g_BezierBullet[i].scl.y * BEZIERBULLET_SCL, g_BezierBullet[i].scl.z * BEZIERBULLET_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_BezierBullet[i].pos.x, g_BezierBullet[i].pos.y, g_BezierBullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_BezierBullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

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

	// αテストを戻す
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBezierBullet(void)
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

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[1].Position = { fWidth / 2.0f,  fHeight / 2.0f , 0.0f };
	vertex[2].Position = { -fWidth / 2.0f, -fHeight / 2.0f , 0.0f };
	vertex[3].Position = { fWidth / 2.0f, -fHeight / 2.0f , 0.0f };

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

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
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BEZIERBULLET; nCntBullet++)
	{
		if (!g_BezierBullet[nCntBullet].use)
		{
			g_BezierBullet[nCntBullet].pos = pos;

			// スタート座標
			g_BezierBullet[nCntBullet].p0 = pos;

			// 中間座標
			g_BezierBullet[nCntBullet].p1 = pos;
			g_BezierBullet[nCntBullet].p1.x = 0.0f;
			g_BezierBullet[nCntBullet].p1.y = 500.0f;
			g_BezierBullet[nCntBullet].p1.z = 0.0f;

			// ターゲット座標
			g_BezierBullet[nCntBullet].p2t = tpos;

			g_BezierBullet[nCntBullet].rot = rot;
			g_BezierBullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_BezierBullet[nCntBullet].use = TRUE;

			nIdxBullet = nCntBullet;
			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
BEZIERBULLET* GetBezierBullet(void)
{
	return &(g_BezierBullet[0]);
}

//=============================================================================
// ベジェ曲線関連
//=============================================================================

XMVECTOR LinearInterpolation(XMVECTOR P0, XMVECTOR P1, float s)
{
	XMVECTOR A0;
	A0 = P0 + (P1 - P0) * s;
	return A0;
}

XMVECTOR QuadraticBezierCurve(XMVECTOR p0, XMVECTOR p1, XMVECTOR p2, float s)
{
	XMVECTOR a0 = LinearInterpolation(p0, p1, s);
	XMVECTOR a1 = LinearInterpolation(p1, p2, s);
	XMVECTOR b0 = LinearInterpolation(a0, a1, s);

	return b0;
}

//=============================================================================
// アニメーション
//=============================================================================
void BezierAnimation(int i)
{
	float tw = 1.0f / TEX_DEVX;
	float th = 1.0f / TEX_DEVY;
	float tx = ((int)(g_BezierBullet[i].animCnt) % TEX_DEVX) * tw;	// テクスチャの左上X座標
	float ty = ((int)(g_BezierBullet[i].animCnt) / TEX_DEVY) * th;	// テクスチャの左上Y座標

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
	g_BezierBullet[i].animCnt += 0.5f;

	if ((int)(g_BezierBullet[i].animCnt) > TEX_DEVX * TEX_DEVY)
	{	// アニメーションが終了したら
		g_BezierBullet[i].animCnt = 0.0f;			// ループさせる
	}
}
