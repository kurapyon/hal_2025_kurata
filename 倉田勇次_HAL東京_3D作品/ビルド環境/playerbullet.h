//=============================================================================
//
// 弾発射処理 [playerbullet.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET		(256)	// 弾最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	move;			// 方向・移動
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか


} PLAYERBULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayerBullet(void);
void UninitPlayerBullet(void);
void UpdatePlayerBullet(void);
void DrawPlayerBullet(void);

int SetPlayerBullet(XMFLOAT3 pos, XMFLOAT3 move);

PLAYERBULLET *GetPlayerBullet(void);

