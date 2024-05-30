//=============================================================================
//
//炎処理 [fire.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFire(void);
void  UninitFire(void);
void  UpdateFire(void);
void    DrawFire(void);

int SetFire(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


