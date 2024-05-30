//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GP11A132 09 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "fade.h"
#include "bullet.h"
#include "enemybullet.h"
#include "enemyatc.h"
#include "aboveparticle.h"
#include "particlesphere.h"
#include "particleatc.h"
#include "sound.h"
#include "keyframe.h"
#include "bossbullet.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
// モデルの読み込み
#define MODEL_ENEMY			"data/MODEL/ENEMY/enemy/enemy01.obj"
#define MODEL_TEMPLE		"data/MODEL/ENEMY/enemy/temple.obj"

// エネミー
#define	VALUE_MOVE					(2.0f)				// 移動量
#define	VALUE_ROTATE				(XM_PI * 0.02f)		// 回転量
#define VALUE_SIZE					(5.0f)				// スケール
#define ENEMY_SHADOW_SIZE			(0.4f)				// 影の大きさ
#define ENEMY_OFFSET_Y				(30.0f)				// エネミーの足元をあわせる
// 攻撃カウント
#define ENEMY_USUALLY_CNTLOWER		(1000)				// 通常攻撃のカウント
#define ENEMY_USUALLY_CNTUPPER		(2000)				// 通常攻撃のカウント
// 被ダメージ
#define ENEMY_RECEIVEWAIT			(180)				// 被ダメージ時の色変更時間
// dissolve
#define ENEMY_DISSOLVE				(1.0f)

// ボス
#define	VALUE_BOSSMOVE				(2.0f)				// 移動量
#define	VALUE_BOSSROTATE			(XM_PI * 0.02f)		// 回転量
#define VALUE_BOSSSIZE				(5.0f)				// スケール
#define ENEMYBOSS_SHADOW_SIZE		(0.4f)				// 影の大きさ
#define ENEMYBOSS_SPEED				(3.0f)				// ボスの移動スピード
#define ENEMYBOSS_OFFSET_Y			(40.0f)				// エネミーの足元をあわせる
#define ENEMYBOSS_PARTS_MAX			(2)					// プレイヤーのパーツの数
// 攻撃カウント
#define ENEMYBOSS_USUALLY_CNTLOWER	(1000)			// 通常攻撃のカウント
#define ENEMYBOSS_USUALLY_CNTUPPER	(2000)			// 通常攻撃のカウント
// 被ダメージ
#define ENEMBOSS_RECEIVEWAIT		(180)			// 被ダメージ時の色変更時間
#define ENEMYBOSS_DMGCNT			(10.0f)			// ダメージカウント
// dissolve
#define ENEMYBOSS_DISSOLVE			(1.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// エネミー処理関数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];						// エネミー

int g_Enemy_load = 0;
int g_EnemyBoss_load = 0;

// レイの当た判定




// エネミーが何回出現したか
int	firstcnt;

//　エネミー
static INTERPOLATION_DATA move_tbl_enemy[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  40.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 4,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  50.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  60.0f, 400.0f), XMFLOAT3(0.0f, XM_PI ,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  70.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.25f,0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  80.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f, 0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE), 120 },
	{ XMFLOAT3(0.0f,  30.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	   0.0f),   XMFLOAT3(VALUE_MOVE, VALUE_MOVE, VALUE_MOVE),   1 },
};

static INTERPOLATION_DATA move_tbl_temple[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, 0.0f,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI / 2,	  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI,		  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_PI * 1.5f,  0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },
	{ XMFLOAT3(0.0f, 1.0f, 400.0f), XMFLOAT3(0.0f, XM_2PI,	      0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   1 },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_enemy,
	move_tbl_temple,
};

enum 
{
	tbl_enemy,
	tbl_temple,
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{

	LoadModel(MODEL_ENEMY, &g_Enemy[0].model);
	LoadModel(MODEL_TEMPLE, &g_Enemy[1].model);

	// 変数の初期化
	int	firstcnt = 0;


	for (int i = 0; i < MAX_ENEMY; i++)
	{

		g_Enemy[i].load = TRUE;
		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
		g_Enemy[i].use = TRUE;			// TRUE:生きてる
		// dissolve
		g_Enemy[i].dissolve.Enable = TRUE;
		g_Enemy[i].dissolve.threshold = 1.0f;
	}

	// 初期座標
	g_Enemy[0].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f);
	g_Enemy[1].pos = XMFLOAT3(0.0f, 1.0f, 400.0f);

	// スケール
	g_Enemy[0].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
	g_Enemy[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// ライフ
	g_Enemy[0].hp = ENEMY_HP;
	g_Enemy[1].hp = TEMPLE_HP;

	// ボス戦移行
	g_Enemy[0].chenge = FALSE;

	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = tbl_enemy;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(move_tbl_enemy) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 1番だけ線形補間で動かしてみる
	g_Enemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[1].tblNo = tbl_temple;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[1].tblMax = sizeof(move_tbl_temple) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{

	BULLET* bullet = GetBullet();
	ENEMYATC* enemyatc = GetEnemyAtc();
	PLAYER* player = GetPlayer();


	// エネミーを動かす場合は、影も合わせて動かす事！
	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している

				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

			}

			// 通常攻撃処理
			{
				XMFLOAT3 epos = g_Enemy[0].pos;
				XMFLOAT3 erot = g_Enemy[0].rot;
				SetEnemyBullet(epos, erot);
			}

		}
	}

	// エネミーを倒したらボス戦へ
	if (g_Enemy[0].hp <= 0.0f)
	{
		g_Enemy[0].chenge = TRUE;
	}

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Enemy00:%f,Enemy01:%f\n", g_Enemy[0].hp, g_Enemy[1].hp);
#endif
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// エネミー
	PLAYER* player = GetPlayer();
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//// dissolve
		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//// モデル描画
		DrawModel(&g_Enemy[i].model);

	}

	//// dissolve 描画後にFALSEに
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_Enemy[0].dissolve.Enable);
	SetAlphaTestEnable(g_Enemy[1].dissolve.Enable);

	SetFuchi(0);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}