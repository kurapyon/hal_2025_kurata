//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define MAX_ENEMY		(2)				// エネミーの数
#define	ENEMY_SIZE		(50.0f)			// 当たり判定の大きさ
#define ENEMY_HP		(5.0f)			// エネミーのライフ
#define TEMPLE_HP		(1.0f)			// 城のライフ
#define ENEMY_REDUCE	(1.0f)			// エネミーの体力の減量

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;						// ワールドマトリックス
	XMFLOAT4			Quaternion;						// クォータニオン
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	XMFLOAT3			UpVector;						// 自分が立っている所
	XMFLOAT3			pos;							// モデルの座標
	XMFLOAT3			rot;							// モデルの向き(回転)
	XMFLOAT3			scl;							// モデルの大きさ(スケール)
	float				hp;								// HP(ライフ)
	float				spd;							// 移動スピード
	float				size;							// 当たり判定の大きさ
	float				time;							// 線形補間用
	int					shadowIdx;						// 影のインデックス番号
	int					tblNo;							// 行動データのテーブル番号
	int					tblMax;							// そのテーブルのデータ数
	BOOL				chenge;							// ボス戦移行
	BOOL				use;							// 使用しているか
	BOOL				load;							// モデルのロードをするのか
	BOOL				chek;							// dissolveでの出現

	// dissolve
	DISSOLVE			dissolve;

	// モデル情報
	DX11_MODEL			model;

	// 親は、NULL、子供は親のアドレスを入れる
	ENEMY* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);


