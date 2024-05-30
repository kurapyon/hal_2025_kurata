//=============================================================================
//
// ボス処理 [boss.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "model.h"
#include "boss.h"
#include "debugproc.h"
#include "collision.h"
#include "player.h"
#include "fade.h"
#include "sound.h"
#include "keyframe.h"
#include "bezierbullet.h"
#include "beambullet.h"
#include "particlesphere.h"
#include "particlestart.h"
#include "deco.h"
#include "beziereffect.h"
#include "aboveeffect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// アニメーションのリスト
enum BOSSANIM_LIST
{
	// アイドル状態
	BOSSANIM_IDLE_BODY = 0,
	BOSSANIM_IDLE_HEAD,
	BOSSANIM_IDLE_LARMTOP,
	BOSSANIM_IDLE_LARMBTM,
	BOSSANIM_IDLE_LLEGTOP,
	BOSSANIM_IDLE_LLEGBTM,
	BOSSANIM_IDLE_RARMTOP,
	BOSSANIM_IDLE_RARMBTM,
	BOSSANIM_IDLE_RLEGTOP,
	BOSSANIM_IDLE_RLEGBTM,
	BOSSANIM_IDLE_TAIL,

	// 動き状態
	BOSSANIM_move_BODY,
	BOSSANIM_move_HEAD,
	BOSSANIM_move_LARMTOP,
	BOSSANIM_move_LARMBTM,
	BOSSANIM_move_LLEGTOP,
	BOSSANIM_move_LLEGBTM,
	BOSSANIM_move_RARMTOP,
	BOSSANIM_move_RARMBTM,
	BOSSANIM_move_RLEGTOP,
	BOSSANIM_move_RLEGBTM,
	BOSSANIM_move_TAIL,

	BOSSANIM_LIST_MAX,
};

// ボス出現位置
#define START_POS_X_1			(  0.0f)
#define START_POS_X_2			( -60.0f)
#define START_POS_Z_1			(   0.0f)
#define START_POS_Z_2			(-450.0f)
#define START_POS_X_3			((START_POS_X_1 + START_POS_X_2) / 2)
#define START_POS_Z_3			((START_POS_Z_1 + START_POS_Z_2) / 2)

// ディゾルブ
#define BOSS_DISSOLVE			(1.0f)
#define BOSS_VIEWANGLE			(25)

// フィールド内にとどまらせる
#define BOSS_FIELD_X_1			(MAP_W)
#define BOSS_FIELD_Z_1			(MAP_H)
#define BOSS_FIELD_X_2			(-MAP_W)
#define BOSS_FIELD_Z_2			(-MAP_H)

// 回転の補間
#define ROTATE_INTERPOLTION			(0.02f)
// 補間をやめる角度
#define STOP_INTERPOLTION_ANGLE		(10.0f)
// ホーミングをやめる角度
#define STOP_INTERPOLTION_DISTANCE  (10.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// アニメーション関数
void AttackBoss(int i);
void SetTblBossIdle(int i);
void SetTblBossSearch(int i);

// ボスの行動関数
void SearchPlayer(int i);
void SearchView(int i);
void BossMovePatrol(int i);
void BossAtkBezier(int i);
void BossAbove(int i);
void BossAtkDeathblow(int i);
void BossSetPatrolPoint(void);
BOOL BossDeathblow(int i);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOSS		g_Boss[MAX_BOSS];						// ボス
static BOSS		g_Parts[MAX_BOSS][BOSSPARTS_MAX];		// ボスのパーツ用
static BOSS_MODEL	g_Model[MAX_BOSS][BOSSPARTS_MAX];	// ボスのモデル

// ライト
static LIGHT		g_Light;

// ぶつかったポリゴンの法線ベクトル（向き）
static XMFLOAT3 Normalboss;

// 変数の初期化
static XMFLOAT3 bpos = { 0.0f,0.0f,0.0f };
static XMFLOAT3 brot = { 0.0f,0.0f,0.0f };

// プレイヤーとの距離
static float g_Distance;

// ボスのアクションカウンター
static float g_DeathblowCnt = 0.0f;	// 必殺技を出す時間
static float g_SphereCnt    = 0.0f;	// 球体上のパーティクルを出す時間
static float g_AboveAtkCnt  = 0;	// 上からの攻撃の回数を数える

// 発射した弾の数
static int g_BulletNum;
// ボスとプレイヤーのなす角
static float g_ViewAngle;

// 球体上のパーティクルを出した後に必殺技
static BOOL g_SphereEnd;

// ボスの移動関係
static int g_MoveCnt = 0;						// ボスが動く時間
XMFLOAT3   g_PatrolPos = { 0.0f,0.0f, 0.0f };	// パトロールするための座標

// 縁をチカチカさせる
static int g_EdgeFlickering = 0;

// ゲームを開始するためのギミックを設置
XMFLOAT3 startpos  = { START_POS_X_3 , PLAYER_OFFSET_Y , START_POS_Z_2 };
XMFLOAT3 startrot  = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 startmove = { 0.0f, 0.0f, 0.0f };
XMFLOAT4 startcol  = { 1.0f, 1.0f, 1.0f, 1.0f };

// アニメーションのディレクトリの配列
static char* g_AnimDir[] =
{
	// 待機状態
	"data/ANIMDATA/BOSS/Boss_Idle/boss_Body.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_Head.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_LArmTop.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_LArmBtm.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_LLegTop.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_LLegBtm.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_RArmTop.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_RArmBtm.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_RLegTop.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_RLegBtm.csv",
	"data/ANIMDATA/BOSS/Boss_Idle/boss_tail.csv",

	// 動き状態
	"data/ANIMDATA/BOSS/Boss_move/boss_Body.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_Head.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_LArmTop.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_LArmBtm.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_LLegTop.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_LLegBtm.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_RArmTop.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_RArmBtm.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_RLegTop.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_RLegBtm.csv",
	"data/ANIMDATA/BOSS/Boss_move/boss_tail.csv",

};

// モデルデータのディレクトリ
static char* g_ModelAdr[] =
{
	"data/MODEL/BOSS/bossBody.obj",
	"data/MODEL/BOSS/bossHead.obj",
	"data/MODEL/BOSS/bossLArmTop.obj",
	"data/MODEL/BOSS/bossLArmBtm.obj",
	"data/MODEL/BOSS/bossLLegTop.obj",
	"data/MODEL/BOSS/bossLLegBtm.obj",
	"data/MODEL/BOSS/bossRArmTop.obj",
	"data/MODEL/BOSS/bossRArmBtm.obj",
	"data/MODEL/BOSS/bossRLegTop.obj",
	"data/MODEL/BOSS/bossRLegBtm.obj",
	"data/MODEL/BOSS/bossTail.obj",
};

// ボスの階層アニメーションデータ
// 待機状態
static std::vector<INTERPOLATION_DATA> idle_body;
static std::vector<INTERPOLATION_DATA> idle_head;
static std::vector<INTERPOLATION_DATA> idle_lArmTop;
static std::vector<INTERPOLATION_DATA> idle_lArmBtm;
static std::vector<INTERPOLATION_DATA> idle_lLegTop;
static std::vector<INTERPOLATION_DATA> idle_lLegBtm;
static std::vector<INTERPOLATION_DATA> idle_RArmTop;
static std::vector<INTERPOLATION_DATA> idle_RArmBtm;
static std::vector<INTERPOLATION_DATA> idle_RLegTop;
static std::vector<INTERPOLATION_DATA> idle_RLegBtm;
static std::vector<INTERPOLATION_DATA> idle_tail;

// 動き状態
static std::vector<INTERPOLATION_DATA> move_body;
static std::vector<INTERPOLATION_DATA> move_head;
static std::vector<INTERPOLATION_DATA> move_lArmTop;
static std::vector<INTERPOLATION_DATA> move_lArmBtm;
static std::vector<INTERPOLATION_DATA> move_lLegTop;
static std::vector<INTERPOLATION_DATA> move_lLegBtm;
static std::vector<INTERPOLATION_DATA> move_RArmTop;
static std::vector<INTERPOLATION_DATA> move_RArmBtm;
static std::vector<INTERPOLATION_DATA> move_RLegTop;
static std::vector<INTERPOLATION_DATA> move_RLegBtm;
static std::vector<INTERPOLATION_DATA> move_tail;


// アニメーションデータのアドレス
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// アイドル状態
	&idle_body,
	&idle_head,
	&idle_lArmTop,
	&idle_lArmBtm,
	&idle_lLegTop,
	&idle_lLegBtm,
	&idle_RArmTop,
	&idle_RArmBtm,
	&idle_RLegTop,
	&idle_RLegBtm,
	&idle_tail,

	// 動き状態
	&move_body,
	&move_head,
	&move_lArmTop,
	&move_lArmBtm,
	&move_lLegTop,
	&move_lLegBtm,
	&move_RArmTop,
	&move_RArmBtm,
	&move_RLegTop,
	&move_RLegBtm,
	&move_tail,
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoss(void)
{

	// モデルの読込
	for (int i = 0; i < MAX_BOSS; i++)
	{
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// アニメーションデータをファイルから読み込み
	for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// ボスの初期化
	for (int i = 0; i < MAX_BOSS; i++)
	{
		g_Boss[i].pos = XMFLOAT3(22.0f, BOSS_OFFSET_Y, 600.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(BOSS_SCL, BOSS_SCL, BOSS_SCL);
		g_Boss[i].use = FALSE;
		g_Boss[i].spd = VALUE_BOSS_MOVE;	// 移動スピード
		g_Boss[i].size = BOSS_SIZE;			// 当たり判定の大きさ
		g_Boss[i].hp = BOSS_HP;				// プレイヤーのライフ
		g_Boss[i].state  = BOSS_STATE_IDLE;	// ステイト管理
		g_Boss[i].setTbl = BOSS_STATE_IDLE;
		g_Boss[i].close = FALSE;			// 今プレイヤーが近いかどうか
		g_Boss[i].WhereMove = 0;			// どこに移動するか
		g_Boss[i].ViewStayCnt = 0;			// 視野に何秒入っているかどうか
		g_Boss[i].ViewingAngle = FALSE;		// 視野角に入っているかどうか
		g_Boss[i].view = FALSE;

		// dissolve
		g_Boss[i].dissolve.Enable = TRUE;
		g_Boss[i].dissolve.threshold = 1.0f;

		// アニメーション
		g_Boss[i].parent = NULL;			// 本体（親）なのでNULLを入れる

		// パトロール用
		g_PatrolPos = g_Boss[i].pos;

		 //階層アニメーションの初期化
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			g_Parts[i][j].use = TRUE;

			// 位置・回転・スケールの初期設定
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Parts[i][j].modelIdx = j;

			// 階層アニメーション用のメンバー変数の初期化
			g_Parts[i][j].time = 0.0f;												// 線形補間用のタイマーをクリア
			g_Parts[i][j].tblNo = j + BOSSANIM_move_BODY;							// 再生する行動データテーブルNoをセット
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();		// 再生する行動データテーブルのレコード数をセット

			// 親子関係
			switch (j)
			{
			case BOSSPARTS_BODY:
				g_Parts[i][j].parent = &g_Boss[i];
				break;

			case BOSSPARTS_LARMBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_LARMTOP];
				break;

			case BOSSPARTS_RARMBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_RARMTOP];
				break;

			case BOSSPARTS_LLEGBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_LLEGTOP];
				break;

			case BOSSPARTS_RLEGBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_RLEGTOP];
				break;

			default:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_BODY];
				break;
			}
		}

		// クォータニオンの初期化
		XMStoreFloat4(&g_Boss[i].Quaternion, XMQuaternionIdentity());
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		// モデルの解放処理
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Model[i][j].load = FALSE;
		}

		// アニメーションテーブルをファイルから初期化
		for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
		{
			g_TblAdr[i]->clear();
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBoss(void)
{
	PLAYER* player = GetPlayer();
	PARTICLESTART* particlestart = GetPArticleStart();

	for (int i = 0; i < MAX_BOSS; i++)
	{
		// ボスが出現前なら表示する
		if (g_Boss[i].use == FALSE)
		{
			SetParticleStart(startpos, startmove, startrot, startcol,
				START_PARTICLE_SCALE, START_PARTICLE_SCALE,
				START_PARTICLE_LIFE);

			// プレイヤーがこの座標についたら
			if (START_POS_X_2 <= player[0].pos.x &&
				player[0].pos.x <= START_POS_X_1 &&
				START_POS_Z_2 <= player[0].pos.z &&
				player[0].pos.z <= START_POS_Z_1)
			{
				g_Boss[i].use = TRUE;
				particlestart[0].use = FALSE;
			}
		}

		if (g_Boss[i].use == FALSE) continue;

		if (g_Boss[i].dissolve.threshold >= 0.0f)
		{
			// ディゾルブエフェクトを付ける
			g_Boss[i].dissolve.threshold -= 0.005f;
		}

		switch (g_Boss[i].state)
		{
			// プレイヤーが視野に入っているかどうか判定してステイトを変える
		case BOSS_STATE_IDLE:
			SetTblBossSearch(i);
			SearchView(i);
			BossMovePatrol(i);
			break;

			// プレイヤーとの距離を測り攻撃を決める
		case BOSS_STATE_SEARCH:
			SetTblBossSearch(i);
			SearchPlayer(i);
			break;

			// 弾の発射(ベジェ)
		case BOSS_STATE_BEZIER:
			SetTblBossSearch(i);
			BossAtkBezier(i);
			break;

			// 上からの攻撃
		case BOSS_STATE_ABOVE:
			SetTblBossSearch(i);
			BossAbove(i);
			break;

			// 必殺技
		case BOSS_STATE_DEATHBLOW:
			SetTblBossSearch(i);
			BossAtkDeathblow(i);
			break;

		default:
			break;
		}

		// 階層アニメーション
		HierarchyBossAnimation(i);

#ifdef _DEBUG
		PrintDebugProc("Boss pos.x:%f,pos.y:%f,pos.z:%f\n", g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
		PrintDebugProc("PatrolPos pos.x:%f,pos.y:%f,pos.z:%f\n", g_PatrolPos.x, g_PatrolPos.y, g_PatrolPos.z);
		PrintDebugProc("viwe:%d,viwecnt:%d\n", g_Boss[i].view, g_Boss[i].ViewStayCnt);
#endif

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use == FALSE) continue;
		// dissolve
		SetDissolve(&g_Boss[i].dissolve);
		SetAlphaTestEnable(g_Boss[i].dissolve.Enable);

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Boss[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

		// 階層アニメーション
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			// 使われているなら処理する
			if (g_Parts[i][j].use == FALSE) continue;

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// モデル描画
			DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
		}
		// dissolve 描画後にFALSEに
		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(g_Boss[i].dissolve.Enable);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ボス情報を取得
//=============================================================================
BOSS* GetBoss(void)
{
	return &g_Boss[0];
}

//=============================================================================
// 関数化
//=============================================================================

// 子の当たり判定(ボスの番号、パーツの番号、パーツ半径、エネミーの座標、エネミーの半径)
BOOL ChildBossCollision(int i, int index, float r, XMFLOAT3 epos, float er)
{
	// パーツの子の付け根の座標を求めている
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[i][index].mtxWorld);

	// 求めた座標を入れてる
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BCの当たり判定の結果を返す
	return CollisionBC(pos, epos, r, er);
}

//階層アニメーション
void HierarchyBossAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// 使われているなら処理する
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i][j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i][j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;				// 移動先テーブルの番号を求めている
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[i][j].tblNo];	// 行動テーブルのアドレスを取得(vector型で)

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i][j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i][j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i][j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる

				// 繰り返さないアニメーションの場合
				//switch (g_Boss[i].setTbl)
				//{
				//case BOSSSTATE_DIG:
				//	g_Parts[i][j].tblMax = -1;
				//	break;
				//}
			}
		}
	}
}

//=============================================================================
// 行動パターン
//=============================================================================

// 視野に指定の時間以上いた場合攻撃する
void SearchView(int i)
{
	PLAYER* player = GetPlayer();

	// ディゾルブが終わるまで行動させない
	if (g_Boss[i].dissolve.threshold <= 0.0f)
	{
		XMVECTOR dirVec, targetVec;				// ボスの視線ベクトル, プレイヤーとのベクトル

		// ボスの向きをベクトルに変換
		float x = sinf(g_Boss[i].rot.y + XM_PI);
		float z = cosf(g_Boss[i].rot.y + XM_PI);				// モデルの向きを考慮

		dirVec = XMVector3Normalize(XMVectorSet(x, 0.0f, z, 0.0f));

		// ボスからプレイヤーへのベクトル
		XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);
		targetVec = XMVector3Normalize(XMLoadFloat3(&player[0].pos) - bpos);

		float cos = dotProduct(&dirVec, &targetVec);				// 内積を求めてプレイヤーとのなす角を計算
		g_ViewAngle = acosf(cos);

		// 0と0の除算の場合エラーを起こさない。後ろに戻す
		if (isnan(g_ViewAngle))	g_ViewAngle = XM_PI;

		g_ViewAngle = XMConvertToDegrees(g_ViewAngle);

		// 視野角に入っているか判定
		g_Boss[i].ViewingAngle = (g_ViewAngle <= BOSS_VIEWANGLE) ? TRUE : FALSE;

		// 視野の中にいたら
		if (g_Boss[i].view == TRUE)
		{
			// 指定の時間以上視野の中にいたらプレイヤーとの距離を測る
			g_Boss[i].ViewStayCnt++;
			// この時間以上居たら攻撃させる
			if (g_Boss[i].ViewStayCnt > VIEMSTAY_TIME)
			{
				// ステイとを切り替える
				g_Boss[i].state = BOSS_STATE_SEARCH;

				// 初期化
				g_Boss[i].ViewingAngle = FALSE;
				g_Boss[i].view = FALSE;
				g_Boss[i].edge = FALSE;
				g_Boss[i].ViewStayCnt = 0;
			}
		}
	}
}

 //プレイヤーとの距離とボスの体力によって攻撃を変える
void SearchPlayer(int i)
{
	PLAYER* player = GetPlayer();

	// プレイヤーとの距離に応じて攻撃させる
	// プレイヤーとボスの距離を計算して攻撃を管理する
	// temp = 一時的な計算用の変数
	XMVECTOR temp = XMLoadFloat3(&player->pos) - XMLoadFloat3(&g_Boss[i].pos);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物が出てくる）
	XMStoreFloat(&g_Distance, temp);

	if ((g_Distance / 1000.0f > APART_DISTANCE) && (g_Boss[i].state == BOSS_STATE_SEARCH))
	{
		// 遠ければベジェ攻撃
		g_Boss[i].state = BOSS_STATE_BEZIER;
	}
	else
	{
		// 近ければ上からの攻撃
		g_Boss[i].state = BOSS_STATE_ABOVE;
	}

	// ボスの体力が一定の値なら必殺技発動
	if (BossDeathblow(i) == TRUE)
	{
		g_Boss[i].state = BOSS_STATE_DEATHBLOW;
	}
}

// ボスが一定の体力になったら必殺技を発動する
BOOL BossDeathblow(int i)
{
	// 必殺技発動
	BOOL Deathblow;

	// ボスの体力が一定以下なら
	if (BOSS_HP_BELOW_1 > g_Boss[i].hp && g_Boss[i].hp > BOSS_HP_BELOW_2)
	{
		Deathblow = TRUE;
		return Deathblow;
	}
	else if (BOSS_HP_BELOW_3 > g_Boss[i].hp && g_Boss[i].hp > BOSS_HP_BELOW_4)
	{
		Deathblow = TRUE;
		return Deathblow;
	}
	else
	{
		// それ以外は発動しない
		Deathblow = FALSE;
		return Deathblow;
	}
}


// ボスの行き先を決める
void BossSetPatrolPoint(void)
{
	g_PatrolPos.x = rand() % (int)(MAP_W * 2.0f) - MAP_W;
	g_PatrolPos.z = rand() % (int)(MAP_H * 2.0f) - MAP_H;
}

// 決めた行先にゆっくり歩く
void BossMovePatrol(int i)
{
	// プレイヤーを視認しているときは動かない
	if (g_Boss[i].view == TRUE) return;

	// ディゾルブが終わるまで行動させない
	if (g_Boss[i].dissolve.threshold <= 0.0f)
	{
		g_MoveCnt++;
		// このカウント以内なら動かす
		if (g_MoveCnt > BOSS_PATROL_INTERVAL)
		{
			g_MoveCnt = 0;
			BossSetPatrolPoint();
		}

		// 座標の記憶
		XMFLOAT3 BeforePos = g_Boss[i].pos;

		// ボスとパトロールポスとの距離が出る
		float x = g_Boss[0].pos.x - g_PatrolPos.x;
		float z = g_Boss[0].pos.z - g_PatrolPos.z;

		// パトロールポスから一定以上の距離が離れていたら処理
		if ((x * x + z * z) >= STOP_INTERPOLTION_DISTANCE)
		{	// パトロールポイントにホーミングさせる
			// //計算したい情報を変換して、最後に元に戻す。
			XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);

			// 一回で(x.y.z)を計算できる。自分とプレイヤーの差を求めている
			XMVECTOR vec = XMLoadFloat3(&g_PatrolPos) - bpos;

			// 角度を求めている(ゴール)
			float angle = atan2f(vec.m128_f32[0], vec.m128_f32[2]);

			// スピードを求めている
			float speed = BOSS_SPEED;

			// ボスをホーミングさせる
			g_Boss[i].pos.x += sinf(angle) * speed;
			g_Boss[i].pos.z += cosf(angle) * speed;

			// 外積を使って右左決める
			XMFLOAT3 cross = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 brot = { sinf(g_Boss[i].rot.y), 0.0f, cosf(g_Boss[i].rot.y) };
			XMVECTOR bvec = XMLoadFloat3(&brot);

			XMStoreFloat3(&cross, XMVector3Cross(bvec, vec));
			// ボスの向きを変える際に補間してゆっくり向きを変える
			if (cross.y >= STOP_INTERPOLTION_DISTANCE)
			{
				g_Boss[i].rot.y -= ROTATE_INTERPOLTION;
			}
			else if (cross.y <= -STOP_INTERPOLTION_DISTANCE)
			{
				g_Boss[i].rot.y += ROTATE_INTERPOLTION;
			}
		}

		// フィールド内から出さない(外だったら)
		if (BOSS_FIELD_X_1 < g_Boss[i].pos.x || g_Boss[i].pos.x < BOSS_FIELD_X_2 ||
			BOSS_FIELD_Z_1 < g_Boss[i].pos.z || g_Boss[i].pos.z < BOSS_FIELD_Z_2)
		{
			// 前の座標に戻す
			g_Boss[i].pos = BeforePos;
		}

		// ボスが障害物と当たったら
		if (ColliderHitBoss() == TRUE)
		{
			g_Boss[0].pos = BeforePos;
		}
	}
}

//=============================================================================
// 攻撃系
//=============================================================================

// ベジェ攻撃状態
void BossAtkBezier(int i)
{
		BEZIERBULLET* bezierbullet = GetBezierBullet();
		PLAYER* player = GetPlayer();

		XMFLOAT3 Spos = g_Boss[i].pos;
		XMFLOAT3 Gpos = player[0].pos;
		SetBossBullet(Spos, player[0].rot, Gpos);

		XMFLOAT4 col = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 Epos = Gpos;
		Epos.y = 0.0f;
		SetBezier(Epos, BEZIER_WIDTH, BEZIER_HEIGHT, col);
		g_BulletNum++;

	if (g_BulletNum >= MAX_BEZIERBULLET)
	{
		// ステイとを切り替える
		g_Boss[i].state = BOSS_STATE_IDLE;

		// 初期化
		g_BulletNum = 0;
	}
}

// 上からの攻撃
void BossAbove(int i)
{
	PLAYER* player = GetPlayer();

	XMFLOAT3 Spos = player[0].pos;
	XMFLOAT4 col = { 1.0,1.0f,1.0f, 1.0f };
	SetAbove(Spos, ABOVE_WIDTH, ABOVE_HEIGHT, col);
	g_AboveAtkCnt++;

	// もし、カウント以上になったらステイとを戻す
	if (g_AboveAtkCnt >= ABOVE_ATK_CNT)
	{
		// ステイとを切り替える
		g_Boss[i].state = BOSS_STATE_IDLE;

		// 初期化
		g_AboveAtkCnt = 0;
	}

}

// 必殺技(ビーム状の攻撃)
void BossAtkDeathblow(int i)
{
	// プレイヤーのHPが減ってきたら、必殺技を出す
	g_SphereCnt++;

	// 球体状のパーティクルを出現させる時間
	if (g_SphereCnt < SPHERE_TIME && g_SphereEnd == FALSE)
	{
		SetParticleSphere();
	}

	// 球体を出し終わったらビームを打つ
	if (g_SphereCnt > SPHERE_TIME)
	{
		g_SphereEnd = TRUE;
		g_SphereCnt = 0.0f;
	}
	
	// 発射位置・回転を決める
	if (g_SphereEnd == TRUE)
	{
		bpos = g_Boss[i].pos;
		bpos.y -= BEAM_OFFSET_Y;

		brot = g_Boss[i].rot;
		SetBeamBullet(bpos, brot);
		g_DeathblowCnt++;

		// 設定した時間ビームを打ったのなら
		if (g_DeathblowCnt > BEAMBULLET_TIME)
		{
			// 初期化
			g_DeathblowCnt = 0.0f;
			g_SphereEnd = FALSE;

			// ステイト変更
			g_Boss[i].state = BOSS_STATE_IDLE;
		}
	}
}

//=============================================================================
// アニメーションのSTATE管理
//=============================================================================

// 状態遷移(アニメーション)
void AttackBoss(int i)
{
	BOOL ans = TRUE;

	// すべてのパーツの最大テーブル数が -1 になっている場合(≒アニメーションが終了している場合)、状態遷移させる
	for (int p = 0; p < BOSSPARTS_MAX; p++)
	{
		if (g_Parts[i][p].tblMax != -1) ans = FALSE;
	}

	if (ans == TRUE) g_Boss[i].state = BOSS_STATE_IDLE;
}

 //アイドル状態(アニメーション)
void SetTblBossIdle(int i)
{
	switch (g_Boss[i].setTbl)
	{
	case BOSS_STATE_IDLE:
		break;

	default:
		for (int p = 0; p < BOSSPARTS_MAX; p++)
		{
			if (g_Parts[i][p].use == FALSE) continue;

			g_Parts[i][p].time = 0.0f;
			g_Parts[i][p].tblNo = p + BOSSANIM_move_BODY;
			g_Parts[i][p].tblMax = (int)g_TblAdr[g_Parts[i][p].tblNo]->size();

		}
		g_Boss[i].setTbl = BOSS_STATE_IDLE;
		break;
	}
}

//探索状態(アニメーション)
void SetTblBossSearch(int i)
{
	switch (g_Boss[i].setTbl)
	{
	case BOSS_STATE_SEARCH:
		break;

	default:
		for (int p = 0; p < BOSSPARTS_MAX; p++)
		{
			if (g_Parts[i][p].use == FALSE) continue;

			g_Parts[i][p].time = 0.0f;
			g_Parts[i][p].tblNo = p + BOSSANIM_IDLE_BODY;
			g_Parts[i][p].tblMax = (int)g_TblAdr[g_Parts[i][p].tblNo]->size();

		}
		g_Boss[i].setTbl = BOSS_STATE_SEARCH;
		break;
	}
}

//=============================================================================
// 当たり判定関連
//=============================================================================

// ボスにダメージを与える
void DamageBoss(float damage)
{
	// ボスのライフを減らす
	g_Boss[0].hp -= damage;

	if (g_Boss[0].hp <= 0.0f)					// ボスのHPが0以下なら
	{
		g_Boss[0].use = FALSE;

		DECO* deco = GetDeco();
		deco[DECO_CLEAR].use = TRUE;
		SetSoundFade(FADE_OUT, SOUND_LABEL_BGM_GAMECLEAR);
	}
}