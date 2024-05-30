//=============================================================================
//
// パーティクルビーム処理 [particlebeam.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define POS_BASE_ATC		 (0.0f)
#define WIDTH_BASE_ATC		 (5.0f)
#define HEIGHTBASE_ATC		 (100.0f)
#define PARTICLEATC_LIFE	 (320)		// パーティクルのライフ
#define PARTICLEATC_SCALE	 (3.0f)		// パーティクルの拡大率
#define PARTICLEATC_DECREASE (0.99f)	// パーティクルのライフ減少時間
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticleBeam(void);
void UninitParticleBeam(void);
void UpdateParticleBeam(void);
void   DrawParticleBeam(void);

void SetColorParticleBeam(int nIdxParticle, XMFLOAT4 col);

// パーティクル
int SetParticleBeam(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

