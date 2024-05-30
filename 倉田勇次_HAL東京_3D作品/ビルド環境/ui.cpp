//=============================================================================
//
// UI処理 [ui.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "player.h"
#include "boss.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH			(25.0f)		// HPの表示TEXサイズ
#define TEXTURE_HEIGHT			(25.0f)		// HPの表示TEXサイズ
#define TEXTURE_RELOAD_HEIGHT	(300.0f)	// リロード表示サイズ
#define TEXTURE_RELOAD_ACTION	(20.0f)		// リロード表示カウント

#define AIM_SIZE				(20.0f)		// クロスヘアサイズ
#define TEXTURE_MAX				(7)			// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void Player_Ui(void);
void Bullet_Ui(void);
void Reload_Ui(void);
void Aim_Ui(void);
void Boss_Ui(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	// プレイヤーのHP
	"data/TEXTURE/heart_main10.png",
	"data/TEXTURE/heartbase10.png",

	// 弾の残弾数
	"data/TEXTURE/Tamaui.png",

	// エイム表示
	"data/TEXTURE/aim.png",

	// リロード表示
	"data/TEXTURE/reload.png",

	// ボスのHP
	"data/TEXTURE/hp_base.png",
	"data/TEXTURE/hp.png",


}; 

enum Tex
{
	heart_main10 = 0,
	heartbase10,
	tama_ui,
	aim_ui,
	reload,
	hp_base,
	hp,
};

static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static float					g_UiOpacity;				// 透明度
static float					g_UiOpacityCnt;				// 透明度カウンター
static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUi(void)
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

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// UIの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 300.0f, 10.0f, 0.0f };
	g_TexNo = 0;
	g_UiOpacity	 = 1.0f;
	g_UiOpacityCnt = 0.0f;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUi(void)
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
void UpdateUi(void)
{
	// チカチカさせる
	PLAYER* player = GetPlayer();
	if (player->reload == 0)
	{
		g_UiOpacityCnt++;
	}
	else
	{
		g_UiOpacityCnt = 0.0f;

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUi(void)
{
	PLAYER* player = GetPlayer();

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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ALPHABLEND);

	if (player->use == TRUE)
	{
		// プレイヤーのHP
		Player_Ui();

		// 残弾数
		Bullet_Ui();

		// リロード表示
		Reload_Ui();

		// エイム表示
		Aim_Ui();
	}

	// ボスのHP
	Boss_Ui();
}

// プレイヤーUI
void Player_Ui(void)
{
	PLAYER* player = GetPlayer();
	// 下敷きのゲージ（枠的な物）
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heartbase10]);

	//ゲージの位置やテクスチャー座標を反映
	float px = 10.0f;	// ゲージの表示位置X
	float py = 10.0f;	// ゲージの表示位置Y
	float pw = 300.0f;	// ゲージの表示幅
	float ph = 30.0f;	// ゲージの表示高さ

	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// ライフ
	//ゲージの位置やテクスチャー座標を反映
	px = 10.0f;	// ゲージの表示位置X
	py = 10.0f;		// ゲージの表示位置Y
	pw = 300.0f;	// ゲージの表示幅
	ph = 30.0f;		// ゲージの表示高さ

	tw = 1.0f;		// テクスチャの幅
	th = 1.0f;		// テクスチャの高さ
	tx = 0.0f;		// テクスチャの左上X座標
	ty = 0.0f;		// テクスチャの左上Y座標

	pw = pw * (float)player[0].hp / PLAYER_HP;
	tw = tw * (float)player[0].hp / PLAYER_HP;

	//pw_log = (float)(g_Player[0].life  / MAX_HP);
	//tw_log = tw;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heart_main10]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

// 弾の残り数
void Bullet_Ui(void)
{
	PLAYER* player = GetPlayer();

	for (int i = 0; i < player->reload; i++)
	{
		//ゲージの位置やテクスチャー座標を反映
		float px = 50.0f;						// ゲージの表示位置X
		float py = 300.0f - TEXTURE_HEIGHT * i;	// ゲージの表示位置Y
		float pw = 50.0f;						// ゲージの表示幅
		float ph = TEXTURE_HEIGHT;				// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tama_ui]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

// リロードUI
void Reload_Ui(void)
{
	PLAYER* player = GetPlayer();

	if (player->reload == 0)
	{
		//ゲージの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH / 2;		// ゲージの表示位置X
		float py = SCREEN_HEIGHT / 2;		// ゲージの表示位置Y
		float pw = 600.0f;					// ゲージの表示幅
		float ph = TEXTURE_RELOAD_HEIGHT;	// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float rot = 0.0f;
		float angle = (XM_PI / TEXTURE_RELOAD_ACTION) * g_UiOpacityCnt;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[reload]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_UiOpacity * fabsf(sinf(XM_PI / 2 + angle))), rot);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

// エイムUI
void Aim_Ui(void)
{
	PLAYER* player = GetPlayer();

	if (player->stance == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[aim_ui]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			SCREEN_CENTER_X, SCREEN_CENTER_Y,
			AIM_SIZE * 2, AIM_SIZE * 2,
			0.0f, 0.0f, 1.0, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

// ボスUI
void Boss_Ui(void)
{
	BOSS* boss = GetBoss();

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (boss->use == TRUE)
	{
		// ライフ
		//ゲージの位置やテクスチャー座標を反映
		float px = 650.0f;	// ゲージの表示位置X
		float py = 17.0f;	// ゲージの表示位置Y
		float pw = 250.0f;	// ゲージの表示幅
		float ph = 10.0f;	// ゲージの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		pw = pw * (float)boss->hp / BOSS_HP;
		tw = tw * (float)boss->hp / BOSS_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[hp]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// 下敷きのゲージ（枠的な物）
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[hp_base]);

		//ゲージの位置やテクスチャー座標を反映
		px = 630.0f;// ゲージの表示位置X
		py = 15.0f;	// ゲージの表示位置Y
		pw = 300.0f;// ゲージの表示幅
		ph = 18.0f;	// ゲージの表示高さ

		tw = 1.0f;	// テクスチャの幅
		th = 1.0f;	// テクスチャの高さ
		tx = 0.0f;	// テクスチャの左上X座標
		ty = 0.0f;	// テクスチャの左上Y座標

	  // １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

}