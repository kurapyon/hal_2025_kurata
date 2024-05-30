//=============================================================================
//
// モデル処理 [player.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER			(1)					// プレイヤーの数
#define	PLAYER_SIZE			(20.0f)				// 当たり判定の大きさ

// プレイヤーのHP
#define PLAYER_HP			(10.0f)				// プレイヤーの体力
#define PLAYER_REDUCE		(1.0f)				// プレイヤーの体力の減量
#define PLAYER_OFFSET_Y		(54.0f)				// プレイヤーの足元をあわせる

// 弾の残弾数
#define PLAYER_RELOAD		(6)					// 弾の残弾数

// 子パーツのリスト
enum PLAYERPARTS_LIST
{
	PLAYERPARTS_BODY = 0,
	PLAYERPARTS_HEAD,
	PLAYERPARTS_ARM_L,
	PLAYERPARTS_ARM_R,
	PLAYERPARTS_THINGS_L,
	PLAYERPARTS_THINGS_R,
	PLAYERPARTS_LEG_L,
	PLAYERPARTS_LEG_R,

	PLAYERPARTS_MAX,
};
//*****************************************************************************
// グローバル変数
//*****************************************************************************

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class PLAYER
{
	public:

	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT4			Quaternion;			// クォータニオン
	XMFLOAT3			UpVector;			// 自分が立っている座標
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			oldpos;				// 座標の記憶
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			move;				// バレットの動き
	float				hp;					// ライフ
	float				spd;				// 移動スピード
	float				dir;				// 向き
	float				size;				// サイズ
	float				bulletX;
	float				time;				// 線形補間用
	int					state;				// ステート(状態）
	int					reload;				// 弾の残りの数
	int					ModelIdx;			// モデルインデックス
	int					jumpCnt;			// ジャンプ中のカウント
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					setTbl;				// セットしているテーブル
	BOOL				use;				// 生きているかどうか
	BOOL				falling;			// 落下判定
	BOOL				stance;				// 構え状態かの判定
	BOOL				jump;				// ジャンプフラグ
	BOOL				attack;				// 攻撃したかどうか
	BOOL				invincibility;		// 無敵時間


	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// dissolve
	DISSOLVE			dissolve;
};

struct PLAYER_MODEL
{
	BOOL				load;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	DX11_MODEL			model;							// モデル情報
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

// 入力判定
BOOL AnyStickPress(void);
BOOL AnyKeyPress(void);

// 当たり判定
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er);
// プレイヤーにダメージを与える
void DamagePlayer(float damage);