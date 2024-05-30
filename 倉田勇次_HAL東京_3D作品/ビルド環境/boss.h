//=============================================================================
//
// モデル処理 [boss.h]
// Author : 倉田勇次
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// 基本ステータス
#define MAX_BOSS			(1)					// ボスの数
#define BOSS_HP				(10.0f)				// ボスの体力
#define BOSS_REDUCE			(1.0f)				// ボスの体力を減量
#define BOSS_SCL			(0.07f)				// ボスのスケール
#define	VALUE_BOSS_MOVE		(45.0f)				// ボスの移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)	// ボスの回転量
#define	BOSS_SIZE			(300.0f)			// ボスの当たり判定の大きさ
#define BOSS_OFFSET_Y		(180.0f)			// ボスの足元をあわせる
#define BOSS_SPEED			(2.0f)				// ボスのスピード

// 攻撃系
#define BEZIER_OFFSET_Y			(7.0f)		// 通常攻撃の発射の高さ
#define BOSS_NOTATC_TIME		(120)		// 攻撃をしない時間
#define CLOSE_DISTANCE			(1000.0f)	// プレイヤーとの差が近いとき
#define APART_DISTANCE			(500.0f)	// プレイヤーとの差が遠いとき
#define SPHERE_TIME				(300.0f)	// 球体状のパーティクルの時間
#define VIEMSTAY_EDGE			(50)		// この時間過ぎたら警告として縁を付ける
#define VIEMSTAY_TIME			(120)		// 視野の中にこの時間以上いると攻撃される
#define BEAM_OFFSET_Y			(120.0f)	// ビーム発射設定
#define BOSS_PATROL_INTERVAL	(300)		// パトロールを実行する間隔
#define BOSS_HP_BELOW_1			(5.0f)		// ボスの体力が一定以下なら
#define BOSS_HP_BELOW_2			(4.0f)		// ボスの体力が一定以下なら
#define BOSS_HP_BELOW_3			(3.0f)		// ボスの体力が一定以下なら
#define BOSS_HP_BELOW_4			(2.0f)		// ボスの体力が一定以下なら

// ボスのステートリスト
enum STATE_LIST_BOSS
{
	// 最初は0にする
	BOSS_STATE_IDLE = 0,	// ゆっくり
	BOSS_STATE_SEARCH,		// 距離を測る
	BOSS_STATE_BEZIER,		// ベジェ
	BOSS_STATE_ABOVE,		// 上からの攻撃
	BOSS_STATE_DEATHBLOW,	// 必殺技

	BOSS_STATE_MAX,			// MAX
};

// 子パーツのリスト
enum BOSSPARTS_LIST
{
	BOSSPARTS_BODY = 0,
	BOSSPARTS_HEAD,
	BOSSPARTS_LARMTOP,
	BOSSPARTS_LARMBTM,
	BOSSPARTS_LLEGTOP,
	BOSSPARTS_LLEGBTM,
	BOSSPARTS_RARMTOP,
	BOSSPARTS_RARMBTM,
	BOSSPARTS_RLEGTOP,
	BOSSPARTS_RLEGBTM,
	BOSSPARTS_TAIL,

	BOSSPARTS_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class BOSS
{
	public:

	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT4			Quaternion;			// クォータニオン
	XMFLOAT3			UpVector;			// 自分が立っている所
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			oldpos;				// 座標の記憶
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			move;				// バレットの動き
	float				hp;					// ライフ
	float				spd;				// 移動スピード
	float				time;				// 階層アニメーション用
	float				size;				// 当たり判定のサイズ
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					setTbl;				// セットしているテーブル
	int					state;				// ステート(状態）
	int					modelIdx;			// モデルインデックス
	int					WhereMove;			// どこに移動するか
	int					ViewStayCnt;		// 何秒間視野の中にいるか
	BOOL				use;				// 生きているか
	BOOL				close;				// 今プレイヤーが近いかどうか
	BOOL				ViewingAngle;		// 視野角に入っているかどうか
	BOOL				view;				// 視野角に入っていてレイとも当たっているかどうか
	BOOL				edge;				// 一定以上時間過ぎたら縁を付ける

	// dissolve
	DISSOLVE			dissolve;

	// 親は、NULL、子供は親のアドレスを入れる
	BOSS* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス
};

struct BOSS_MODEL
{
	BOOL				load;				// ロードするかどうか
	DX11_MODEL			model;				// モデル情報
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBoss(void);
void  UninitBoss(void);
void  UpdateBoss(void);
void    DrawBoss(void);

BOSS *GetBoss(void);

// 当たり判定
BOOL ChildBossCollision(int i, int index, float r, XMFLOAT3 epos, float er);

// 階層アニメーション
void HierarchyBossAnimation(int i);
void DamageBoss(float damage);
