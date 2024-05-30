//=============================================================================
//
// ビーム処理 [beambullet.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	BEAMBULLET_MAX_BULLET	(128)		// 弾最大数
#define BEAMBULLET_LIFE			(60)		// 弾のライフ
#define BEAMBULLET_TIME			(260.0f)	// ビームを打つ時間


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	XMFLOAT3	spd;			// 移動量
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			blife;			// バレットのライフ
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか


} BEAMBULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBeamBullet(void);
void UninitBeamBullet(void);
void UpdateBeamBullet(void);
void   DrawBeamBullet(void);

int SetBeamBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BEAMBULLET*GetBeamBullet(void);

