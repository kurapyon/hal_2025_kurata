//=============================================================================
//
// 空処理 [sky.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SKY			(1)					// 空の数

#define	SKY_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class SKY
{
	public:
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色


	// 親は、NULL、子供は親のアドレスを入れる
	//ENEMY* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数

	// クォータニオン
	XMFLOAT4			Quaternion;
	XMFLOAT3			UpVector;			// 自分が立っている所

	// ジャンプ
	//BOOL		jump;			// ジャンプフラグ
	//float		jumpY;			// ジャンプの高さ
	//int			jumpCnt;		// ジャンプ中のカウント
	//float		jumpYMax;		// 

	// dissolve
	DISSOLVE			dissolve;



};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSky(void);
void	UninitSky(void);
void	UpdateSky(void);
void	DrawSky(void);

SKY *GetSky(void);

