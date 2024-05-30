//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "sound.h"
#include "fade.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ(960)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ(540)
#define TEXTURE_MAX					(3)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			 

// カーソルの位置
#define TITLE_CUR_1_X				(20.0f)
#define TITLE_CUR_1_Y				(350.0f)

#define TITLE_CUR_2_X				(420.0f)
#define TITLE_CUR_2_Y				(350.0f)

#define TITLE_CUR_3_X				(680.0f)
#define TITLE_CUR_3_Y				(350.0f)

// カーソルの幅と高さ
#define CURSOR_WIDTH				(40)
#define CURSOR_HEIGHT				(40)

// 透明度カウント
#define TEXTURE_TITLE_ACTION	(60.0f)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum 
{
	title,
	cursor_title,
	button,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/cursor.png",
	"data/TEXTURE/button.png",
};

XMFLOAT3					g_Pos;					// ポリゴンの座標
XMFLOAT3					g_bPos;					// ポリゴンの座標

BOOL						g_Use;					// TRUE:使っている  FALSE:未使用
float						g_w, g_h;				// 幅と高さ
int							g_TexNo;				// テクスチャ番号
static BOOL					g_Load = FALSE;			// ゲームロード

static float				g_TitleOpacity;			// 透明度
static float				g_TitleOpacityCnt;		// 透明度カウンター

// カーソルの変数
int cursor;

// 徐々に浮かび上がせる変数
float g_scroll;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 初期化
	g_scroll = 0.0f;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TexNo = 0;
	g_bPos = XMFLOAT3(TEXTURE_WIDTH / 2.25f, TEXTURE_HEIGHT / 1.1f, 0.0f);
	g_TitleOpacity = 1.0f;			// 透明度
	g_TitleOpacityCnt = 0.0f;		// 透明度カウンター
	cursor = 0;						// カーソルの変数

	// 効果音
	PlaySound(SOUND_LABEL_BGM_TITLE);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	// タイトル画面を徐々に浮かび上がらせる
	g_scroll += 0.005f;
	g_TitleOpacityCnt++;

	// カーソルの移動処理(上下の移動の移動)
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, R_STICK_RIGHT) || IsButtonTriggered(0, STICK_RIGHT))
	{
		cursor = (++cursor + TITLE_MAX) % TITLE_MAX;
	}
	if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, R_STICK_LEFT) || IsButtonTriggered(0, STICK_LEFT))
	{
		cursor = (--cursor + TITLE_MAX) % TITLE_MAX;
	}

	switch (cursor)
	{
	case TITLE_GAMESTART:		// ゲームスタート
		g_Pos = XMFLOAT3(TITLE_CUR_1_X, TITLE_CUR_1_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			SetFadeMode(FADE_OUT, MODE_GAME);
		}
		break;

	case TITLE_TUTORIALE:		// チュートリアル
		g_Pos = XMFLOAT3(TITLE_CUR_2_X, TITLE_CUR_2_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			SetFadeMode(FADE_OUT, MODE_TUTORIAL);
		}
		break;

	case TITLE_EXIT:		// ゲーム終了
		g_Pos = XMFLOAT3(TITLE_CUR_3_X, TITLE_CUR_3_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			GameLoop();
		}

		break;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_scroll);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[title]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// カーソルの表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[cursor_title]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, g_Pos.x, g_Pos.y, CURSOR_WIDTH, CURSOR_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	// 下の案内を描画
	{
		//ゲージの位置やテクスチャー座標を反映
		float px = g_bPos.x;			// ゲージの表示位置X
		float py = g_bPos.y;			// ゲージの表示位置Y
		float pw = TEXTURE_WIDTH / 2;	// ゲージの表示幅
		float ph = TEXTURE_HEIGHT / 4;	// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float rot = 0.0f;
		float angle = (XM_PI / TEXTURE_TITLE_ACTION) * g_TitleOpacityCnt;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[button]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_TitleOpacity * fabsf(sinf(XM_PI / 2 + angle))), rot);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}





