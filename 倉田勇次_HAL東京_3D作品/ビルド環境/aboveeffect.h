//=============================================================================
//
//炎処理 [aboveeffect.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	ABOVE_WIDTH		(192.0f)	// 頂点サイズ
#define	ABOVE_HEIGHT	(192.0f)	// 頂点サイズ
#define ABOVE_SIZE		(20.0f)		// 当たり判定のサイズ
#define MAX_ABOVE		(1)			// 最大数
#define ABOVE_ATK_CNT	(180.0f)	// この種類の攻撃をする回数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		animCnt;		// アニメーションカウント
	BOOL		use;			// 使用しているかどうか

} ABOVE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitAbove(void);
void  UninitAbove(void);
void  UpdateAbove(void);
void    DrawAbove(void);
ABOVE* GetAboveEffect(void);


int SetAbove(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


