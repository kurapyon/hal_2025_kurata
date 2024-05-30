//=============================================================================
//
// ベジェ弾処理 [bezierbullet.cpp]
// Author : 倉田勇次
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BEZIERBULLET		(5)				// 弾最大数

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
	XMFLOAT3	p0;				// ベジェ曲線
	XMFLOAT3	p1;				// ベジェ曲線
	XMFLOAT3	p2t;			// ベジェ曲線
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		hp;				// 弾の出現時間
	float		s;				// ベジェ曲線
	float		animCnt;		// アニメーションカウント
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか


} BEZIERBULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBezierBullet(void);
void	UninitBezierBullet(void);
void	UpdateBezierBullet(void);
void	DrawBezierBullet(void);
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos);

BEZIERBULLET *GetBezierBullet(void);

