//=============================================================================
//
// パーティクルATC処理 [particleatc.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define START_PARTICLE_LIFE		(120)	// パーティクルのライフ
#define START_PARTICLE_SCALE	(1.0f)	// パーティクルの拡大率

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nLife;			// 寿命
	BOOL			use;			// 使用しているかどうか

} PARTICLESTART;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleStart(void);
void UninitParticleStart(void);
void UpdateParticleStart(void);
void   DrawParticleStart(void);

void SetColorParticleStart(int nIdxParticle, XMFLOAT4 col);

// パーティクル
int SetParticleStart(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
PARTICLESTART* GetPArticleStart(void);
