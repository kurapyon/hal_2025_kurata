//=============================================================================
//
// ビルボード処理 [tree.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"

#include "camera.h"
#include "player.h"
#include "spark.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SPARK_TEXTURE_MAX	(1)			// テクスチャの数
#define	MAX_SPARK			(256)		// 炎最大数
#define	SPARK_WIDTH			(50.0f)		// 頂点サイズ
#define	SPARK_HEIGHT		(50.0f)		// 頂点サイズ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexSpark(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[SPARK_TEXTURE_MAX] = { NULL };	// テクスチャ情報

static SPARK	g_Spark[MAX_SPARK];	// 火花ワーク
static BOOL					g_bAlpaTest;		// アルファテストON/OFF

static int					g_TexNo;			// テクスチャ番号

static char *g_TextureName[] =
{
	"data/TEXTURE/spark.png",
};

enum texno
{
	spark = 0,
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSpark(void)
{
	MakeVertexSpark();

	// テクスチャ生成
	for (int i = 0; i < SPARK_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 火花ワークの初期化
	for(int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{
		ZeroMemory(&g_Spark[nCntSpark].material, sizeof(g_Spark[nCntSpark].material));
		g_Spark[nCntSpark].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Spark[nCntSpark].pos	    = {0.0f, 0.0f, 0.0f};
		g_Spark[nCntSpark].scl	    = {1.0f, 1.0f, 1.0f};
		g_Spark[nCntSpark].fWidth   = SPARK_WIDTH;
		g_Spark[nCntSpark].fHeight  = SPARK_HEIGHT;
		g_Spark[nCntSpark].sparkcnt = 0;
		g_Spark[nCntSpark].use	    = FALSE;

	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSpark(void)
{
	for(int nCntTex = 0; nCntTex < SPARK_TEXTURE_MAX; nCntTex++)
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
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSpark(void)
{
	PLAYER* player = GetPlayer();

	for (int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{	
		// 出現時間を足す
		g_Spark[nCntSpark].sparkcnt++;

		// 一定時間経過したら初期化する
		if (g_Spark[nCntSpark].sparkcnt > SPARK_TIME)
		{
			// 初期化
			g_Spark[nCntSpark].sparkcnt = 0;
			g_Spark[nCntSpark].use		= FALSE;
			player[0].attack			= FALSE;
		}
	}

#ifdef _DEBUG
	// アルファテストON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSpark(void)
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

	for(int i = 0; i < SPARK_TEXTURE_MAX; i++)
	{
		if(g_Spark[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// 関数使った版
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


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
			mtxScl = XMMatrixScaling(g_Spark[i].scl.x * SPARK_SCL, g_Spark[i].scl.y * SPARK_SCL, g_Spark[i].scl.z * SPARK_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Spark[i].pos.x, g_Spark[i].pos.y, g_Spark[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Spark[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[spark]);

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
HRESULT MakeVertexSpark(void)
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

	float fWidth = 60.0f;
	float fHeight = 60.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 火花のパラメータをセット
//=============================================================================
int SetSpark(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxSPark = -1;

	for(int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{
		if(!g_Spark[nCntSpark].use)
		{
			g_Spark[nCntSpark].pos = pos;
			g_Spark[nCntSpark].scl = XMFLOAT3(SPARK_SCL, SPARK_SCL, SPARK_SCL);
			g_Spark[nCntSpark].fWidth = fWidth;
			g_Spark[nCntSpark].fHeight = fHeight;
			g_Spark[nCntSpark].use = TRUE;

			nIdxSPark = nCntSpark;

			break;
		}
	}
	return nIdxSPark;
}

SPARK* GetSpark(void)
{
	return &g_Spark[0];
}