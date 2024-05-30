//=============================================================================
//
// モデル処理 [player.cpp]
// Author :倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "game.h"

#include "playerbullet.h"
#include "boss.h"
#include "camera.h"
#include "collision.h"
#include "fade.h"
#include "keyframe.h"
#include "light.h"
#include "meshfield.h"
#include "model.h"
#include "player.h"
#include "sound.h"
#include "spark.h"
#include "aboveeffect.h"
#include "deco.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// モデル名
#define	MODEL_PLAYER_BODY		"data/MODEL/PLAYER/Player/player_body.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_HEAD		"data/MODEL/PLAYER/Player/player_head.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_ARM_L		"data/MODEL/PLAYER/Player/player_arm_l.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_ARM_R		"data/MODEL/PLAYER/Player/player_arm_r.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_THINGS_L	"data/MODEL/PLAYER/Player/player_things_l.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_THINGS_R	"data/MODEL/PLAYER/Player/player_things_r.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LEG_L		"data/MODEL/PLAYER/Player/player_leg_l.obj"		// 読み込むモデル名
#define	MODEL_PLAYER_LEG_R		"data/MODEL/PLAYER/Player/player_leg_r.obj"		// 読み込むモデル名

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// アニメーションのディレクトリ
// 待機状態
#define ANIM_IDLE_BODY		"data/ANIMDATA/PLAYER/Player_Idle/Player_Body.csv"
#define ANIM_IDLE_HEAD		"data/ANIMDATA/PLAYER/Player_Idle/Player_Head.csv"
#define ANIM_IDLE_LARM		"data/ANIMDATA/PLAYER/Player_Idle/Player_Arm_L.csv"
#define ANIM_IDLE_RARM		"data/ANIMDATA/PLAYER/Player_Idle/Player_Arm_R.csv"
#define ANIM_IDLE_LTHINGS	"data/ANIMDATA/PLAYER/Player_Idle/Player_Things_L.csv"
#define ANIM_IDLE_RTHINGS	"data/ANIMDATA/PLAYER/Player_Idle/Player_Things_R.csv"
#define ANIM_IDLE_LLEG		"data/ANIMDATA/PLAYER/Player_Idle/Player_Leg_L.csv"
#define ANIM_IDLE_RLEG		"data/ANIMDATA/PLAYER/Player_Idle/Player_Leg_R.csv"

// 移動状態
#define ANIM_WALK_BODY		"data/ANIMDATA/PLAYER/Player_Walk/Player_Body.csv"
#define ANIM_WALK_HEAD		"data/ANIMDATA/PLAYER/Player_Walk/Player_Head.csv"
#define ANIM_WALK_LARM		"data/ANIMDATA/PLAYER/Player_Walk/Player_Arm_L.csv"
#define ANIM_WALK_RARM		"data/ANIMDATA/PLAYER/Player_Walk/Player_Arm_R.csv"
#define ANIM_WALK_LTHINGS	"data/ANIMDATA/PLAYER/Player_Walk/Player_Things_L.csv"
#define ANIM_WALK_RTHINGS	"data/ANIMDATA/PLAYER/Player_Walk/Player_Things_R.csv"
#define ANIM_WALK_LLEG		"data/ANIMDATA/PLAYER/Player_Walk/Player_Leg_L.csv"
#define ANIM_WALK_RLEG		"data/ANIMDATA/PLAYER/Player_Walk/Player_Leg_R.csv"

// 構え状態
#define ANIM_STANCE_BODY	"data/ANIMDATA/PLAYER/Player_Stance/Player_Body.csv"
#define ANIM_STANCE_HEAD	"data/ANIMDATA/PLAYER/Player_Stance/Player_Head.csv"
#define ANIM_STANCE_LARM	"data/ANIMDATA/PLAYER/Player_Stance/Player_Arm_L.csv"
#define ANIM_STANCE_RARM	"data/ANIMDATA/PLAYER/Player_Stance/Player_Arm_R.csv"
#define ANIM_STANCE_LTHINGS	"data/ANIMDATA/PLAYER/Player_Stance/Player_Things_L.csv"
#define ANIM_STANCE_RTHINGS	"data/ANIMDATA/PLAYER/Player_Stance/Player_Things_R.csv"
#define ANIM_STANCE_LLEG	"data/ANIMDATA/PLAYER/Player_Stance/Player_Leg_L.csv"
#define ANIM_STANCE_RLEG	"data/ANIMDATA/PLAYER/Player_Stance/Player_Leg_R.csv"

// 攻撃状態
#define ANIM_ATK_BODY		"data/ANIMDATA/PLAYER/Player_Atc/Player_Body.csv"
#define ANIM_ATK_HEAD		"data/ANIMDATA/PLAYER/Player_Atc/Player_Head.csv"
#define ANIM_ATK_LARM		"data/ANIMDATA/PLAYER/Player_Atc/Player_Arm_L.csv"
#define ANIM_ATK_RARM		"data/ANIMDATA/PLAYER/Player_Atc/Player_Arm_R.csv"
#define ANIM_ATK_LTHINGS	"data/ANIMDATA/PLAYER/Player_Atc/Player_Things_L.csv"
#define ANIM_ATK_RTHINGS	"data/ANIMDATA/PLAYER/Player_Atc/Player_Things_R.csv"
#define ANIM_ATK_LLEG		"data/ANIMDATA/PLAYER/Player_Atc/Player_Leg_L.csv"
#define ANIM_ATK_RLEG		"data/ANIMDATA/PLAYER/Player_Atc/Player_Leg_R.csv"

// ↑追加したい時はここに追加
//--------------------------------------------------------------------------------------

#define	VALUE_MOVE					(4.0f)				// 移動量
#define	VALUE_ROTATE				(D3DX_PI * 0.02f)	// 回転量
#define PLAYER_PARTS_MAX			(8)					// プレイヤーのパーツの数
#define GRAVITY_PLUS				(10000)				// 重力判定
#define GRAVITY_MINUS				(-100.0)			// 重力判定
#define GRAVITY						(15.0)
#define PLAYER_PARTS_SIZE			(2.0f)				// プレイヤーのパーツのサイズ

// 弾の発射
#define PLAYER_BULLET				(10.0f)				// 弾の発射用

// 被ダメージ
#define PLAYER_RECEIVEWAIT			(180)				// 被ダメージ時の色変更時間

// 落下ダメージ座標
#define FALLINGPOS_Y				(-500.0f)

// プレイヤーの頭基準にカメラの高さ反転
#define INVERSION					(3.0f)

// 終点用マクロ定義
#define LASTSTOP_X					(0.0f)
#define LASTSTOP_Y					(0.0f)
#define LASTSTOP_Z					(1000.0f)

// カメラシェイク用カウント
#define SHAKECNT					(2.0f)

// ゲーム開始位置
#define START_POS_X_1				(  0.0f)
#define START_POS_X_2				( -60.0f)
#define START_POS_Z_1				(   0.0f)
#define START_POS_Z_2				(-450.0f)

// 無敵時間
#define INVINCIBILITY_TIME			(180)	// 無敵時間
#define INVINCIBILITY_ACTION		(20.0f) // 透明化の時間
#define FLICKERING_END				(170)	// この時間にチカチカを終わらせる

// 縁を付ける時間
#define FUCHI_TIME					(30)	// この時間は絶対に縁を付ける

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// アニメーションのリスト
enum ANIM_LIST
{
	// 待機状態
	IDLE_BODY,
	IDLE_HEAD,
	IDLE_LARM,
	IDLE_RARM,
	IDLE_LTHINGS,
	IDLE_RTHINGS,
	IDLE_LLEG,
	IDLE_RLEG,

	// 移動状態
	WALK_BODY,
	WALK_HEAD,
	WALK_LARM,
	WALK_RARM,
	WALK_LTHINGS,
	WALK_RTHINGS,
	WALK_LLEG,
	WALK_RLEG,

	// 構え状態
	STANCE_BODY,
	STANCE_HEAD,
	STANCE_LARM,
	STANCE_RARM,
	STANCE_LTHINGS,
	STANCE_RTHINGS,
	STANCE_LLEG,
	STANCE_RLEG,

	// 攻撃状態
	ATK_BODY,
	ATK_HEAD,
	ATK_LARM,
	ATK_RARM,
	ATK_LTHINGS,
	ATK_RTHINGS,
	ATK_LLEG,
	ATK_RLEG,

	// ↑追加したい時はここに追加

	ANIM_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// アニメーション関数
void InputWalkPlayer(void);
void InputStancePlayer(void);
void InputAttackPlayer(void);
void InputMoveCamera(void);
void WalkPlayer(void);
void AttackPlayer(void);
void SetTblPlayerAnim(int state);

// 関数化
void HierarchyAnimation(int partmax);
void PlayerGravity(void);
void SparkAppearance(void);
void Invincibility_Mode(void);
void FuchiFlickering(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player[MAX_PLAYER];					// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];				// プレイヤーのパーツ用

static PLAYER_MODEL g_PartModel[PLAYER_PARTS_MAX];			// プレイヤーモデル

static char g_ModelName[PLAYER_PARTS_MAX][256]
{
	MODEL_PLAYER_BODY,
	MODEL_PLAYER_HEAD,
	MODEL_PLAYER_ARM_L,
	MODEL_PLAYER_ARM_R,
	MODEL_PLAYER_THINGS_L,
	MODEL_PLAYER_THINGS_R,
	MODEL_PLAYER_LEG_L,
	MODEL_PLAYER_LEG_R,
};

enum STATE_LIST_PLAYER
{
	PLAYER_STATE_IDLE = 0,
	PLAYER_STATE_WALK,
	PLAYER_STATE_STANCE,
	PLAYER_STATE_ATK,

	PLAYER_STATE_MAX,
};

static LIGHT g_Light;						// ライト
XMFLOAT3     LastStop;						// 終点用の変数
XMFLOAT3	 GravityNormal;					// ぶつかったポリゴンの法線ベクトル（向き）
XMFLOAT3	 BossNormal;					// ぶつかった敵との法線ベクトル（向き）
static int   g_jumpCnt = 0;					// ジャンプ関係
static int	 g_sparkCnt = 0;				// 火花を出す時間
static float g_camerashake = 1.0f;			// カメラシェイク
static float g_camerashakeCnt = 0.0f;		// カメラシェイク
int			 cnt;							// バレットカウント(adove)
int			 Invincibility_Cnt = 0;			// 無敵時間をカウントする
static float g_PlayerOpacity = 1.0f;		// 透明度
static float g_PlayerOpacityCnt = 0.0f;		// 透明度カウンター
static float g_FuchiCnt	= 0.0f;				// 縁を付けるカウンター

// プレイヤーの階層アニメーションデータ
static char* g_AnimDir[] = 
{
	// 待機状態
	ANIM_IDLE_BODY,
	ANIM_IDLE_HEAD,
	ANIM_IDLE_LARM,
	ANIM_IDLE_RARM,
	ANIM_IDLE_LTHINGS,
	ANIM_IDLE_RTHINGS,
	ANIM_IDLE_LLEG,
	ANIM_IDLE_RLEG,

	// 移動状態
	ANIM_WALK_BODY,
	ANIM_WALK_HEAD,
	ANIM_WALK_LARM,
	ANIM_WALK_RARM,
	ANIM_WALK_LTHINGS,
	ANIM_WALK_RTHINGS,
	ANIM_WALK_LLEG,
	ANIM_WALK_RLEG,

	// 構え状態
	ANIM_STANCE_BODY,
	ANIM_STANCE_HEAD,
	ANIM_STANCE_LARM,
	ANIM_STANCE_RARM,
	ANIM_STANCE_LTHINGS,
	ANIM_STANCE_RTHINGS,
	ANIM_STANCE_LLEG,
	ANIM_STANCE_RLEG,

	// 攻撃状態
	ANIM_ATK_BODY,
	ANIM_ATK_HEAD,
	ANIM_ATK_LARM,
	ANIM_ATK_RARM,
	ANIM_ATK_LTHINGS,
	ANIM_ATK_RTHINGS,
	ANIM_ATK_LLEG,
	ANIM_ATK_RLEG,
};

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// プレイヤーの階層アニメーションデータ
// 待機状態
static std::vector<INTERPOLATION_DATA> idle_body;
static std::vector<INTERPOLATION_DATA> idle_head;
static std::vector<INTERPOLATION_DATA> idle_larm;
static std::vector<INTERPOLATION_DATA> idle_rarm;
static std::vector<INTERPOLATION_DATA> idle_lthings;
static std::vector<INTERPOLATION_DATA> idle_rthings;
static std::vector<INTERPOLATION_DATA> idle_lleg;
static std::vector<INTERPOLATION_DATA> idle_rleg;

// 移動状態
static std::vector<INTERPOLATION_DATA> walk_body;
static std::vector<INTERPOLATION_DATA> walk_head;
static std::vector<INTERPOLATION_DATA> walk_larm;
static std::vector<INTERPOLATION_DATA> walk_rarm;
static std::vector<INTERPOLATION_DATA> walk_lthings;
static std::vector<INTERPOLATION_DATA> walk_rthings;
static std::vector<INTERPOLATION_DATA> walk_lleg;
static std::vector<INTERPOLATION_DATA> walk_rleg;

// 構え状態
static std::vector<INTERPOLATION_DATA> stance_body;
static std::vector<INTERPOLATION_DATA> stance_head;
static std::vector<INTERPOLATION_DATA> stance_larm;
static std::vector<INTERPOLATION_DATA> stance_rarm;
static std::vector<INTERPOLATION_DATA> stance_lthings;
static std::vector<INTERPOLATION_DATA> stance_rthings;
static std::vector<INTERPOLATION_DATA> stance_lleg;
static std::vector<INTERPOLATION_DATA> stance_rleg;

// 攻撃状態
static std::vector<INTERPOLATION_DATA> atk_body;
static std::vector<INTERPOLATION_DATA> atk_head;
static std::vector<INTERPOLATION_DATA> atk_larm;
static std::vector<INTERPOLATION_DATA> atk_rarm;
static std::vector<INTERPOLATION_DATA> atk_lthings;
static std::vector<INTERPOLATION_DATA> atk_rthings;
static std::vector<INTERPOLATION_DATA> atk_lleg;
static std::vector<INTERPOLATION_DATA> atk_rleg;

//--------------------------------------------------------------------------------------
// キーフレーム変更部分
// プレイヤーの階層アニメーションデータの配列
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// 待機状態
	&idle_body,
	&idle_head,
	&idle_larm,
	&idle_rarm,
	&idle_lthings,
	&idle_rthings,
	&idle_lleg,
	&idle_rleg,

	// 移動状態
	&walk_body,
	&walk_head,
	&walk_larm,
	&walk_rarm,
	&walk_lthings,
	&walk_rthings,
	&walk_lleg,
	&walk_rleg,
	
	// 構え状態
	&stance_body,
	&stance_head,
	&stance_larm,
	&stance_rarm,
	&stance_lthings,
	&stance_rthings,
	&stance_lleg,
	&stance_rleg,

	// 攻撃状態
	&atk_body,
	&atk_head,
	&atk_larm,
	&atk_rarm,
	&atk_lthings,
	&atk_rthings,
	&atk_lleg,
	&atk_rleg,

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	//--------------------------------------------------------------------------------------
	// キーフレーム変更部分
	// アニメーションデータをファイルから読み込む 
	for (int i = 0; i < ANIM_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}
	//--------------------------------------------------------------------------------------

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_Player[i].pos				= XMFLOAT3(20.0f, PLAYER_OFFSET_Y, -600.0f);// 座標
		g_Player[i].rot				= XMFLOAT3(0.0f, 0.0f, 0.0f);	// 向き
		g_Player[i].scl				= XMFLOAT3(2.0f, 2.0f, 2.0f);	// スケール
		g_Player[i].move			= XMFLOAT3(1.0f, 1.0f, 1.0f);	// バレットの動き
		g_Player[i].spd				= 0.0f;							// 移動スピードクリア
		g_Player[i].use				= TRUE;							// TRUE:生きてる
		g_Player[i].size			= PLAYER_SIZE;					// 当たり判定の大きさ
		g_Player[i].falling			= FALSE;						//
		g_Player[i].bulletX			= 0.0f;							// バレット
		g_Player[i].hp				= PLAYER_HP;					// プレイヤーのライフ
		g_Player[i].state			= PLAYER_STATE_IDLE;			// アニメーション
		g_Player[i].setTbl			= PLAYER_STATE_IDLE;			// アニメーション
		g_Player[i].reload			= PLAYER_RELOAD;				// 弾の残弾数
		g_Player[i].stance			= FALSE;						// 構え状態の判定
		g_Player[i].attack			= FALSE;						// 攻撃したかどうか
		g_Player[i].ModelIdx		= NULL;							// モデルインデックス
		g_Player[i].parent			= NULL;							// 本体（親）なのでNULLを入れる
		g_Player[i].invincibility	= FALSE;						// 無敵時間

		 //階層アニメーションの初期化
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[j].use = TRUE;

			// 位置・回転・スケールの初期設定
			g_Parts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[j].parent = &g_Player[i];		// ここに親のアドレスを入れる

			// 階層アニメーション用のメンバー変数の初期化
			g_Parts[j].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[j].tblNo = j + IDLE_BODY;				// 再生する行動データテーブルNoをセット
			g_Parts[j].tblMax = (int)g_TblAdr[g_Parts[j].tblNo]->size();

			// モデルの読み込み
			LoadModel(g_ModelName[j], &g_PartModel[j].model);
			g_PartModel[j].load = TRUE;
			// モデルのディフューズを保存しておく。色変え対応の為。
			GetModelDiffuse(&g_PartModel[j].model, &g_PartModel[j].diffuse[0]);


			// 親子関係
			switch (j)
			{
			case PLAYERPARTS_BODY:
				g_Parts[j].parent = &g_Player[0];
				break;

			case PLAYERPARTS_LEG_L:
				g_Parts[j].parent = &g_Parts[PLAYERPARTS_THINGS_L];
				break;

			case PLAYERPARTS_LEG_R:
				g_Parts[j].parent = &g_Parts[PLAYERPARTS_THINGS_R];
				break;

			default:
				g_Parts[j].parent = &g_Parts[0];
				break;
			}
		}

		// クォータニオンの初期化
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_PartModel[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_PartModel[i].model);
			g_PartModel[i].load = FALSE;
		}
	}

	// アニメーションテーブルをファイルから初期化
	for (int i = 0; i < ANIM_MAX; i++)
	{
		g_TblAdr[i]->clear();
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	DECO* deco = GetDeco();
	ABOVE* above = GetAboveEffect();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if ((g_Player[i].use) && (!deco[DECO_CLEAR].use))
		{
			switch (g_Player[i].state)
			{
			case PLAYER_STATE_IDLE:
				InputWalkPlayer();
				InputStancePlayer();
				break;

			case PLAYER_STATE_WALK:
				InputWalkPlayer();
				WalkPlayer();
				break;

			case PLAYER_STATE_STANCE:
				InputStancePlayer();
				InputAttackPlayer();
				AttackPlayer();
				break;

			case PLAYER_STATE_ATK:
				InputAttackPlayer();
				AttackPlayer();
				break;

			default:
				break;
			}

			// カメラ操作
			InputMoveCamera();

			// 重力
			PlayerGravity();

			// 階層アニメーション
			SetTblPlayerAnim(g_Player[i].state);
			HierarchyAnimation(PLAYER_PARTS_MAX);

			// 無敵時間の設定
			Invincibility_Mode();

			// ポイントライトのテスト
			{
				LIGHT* light = GetLightData(1);
				XMFLOAT3 pos = g_Player[i].pos;
				//pos.y += 20.0f;

				light->Position = pos;
				light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Type = LIGHT_TYPE_POINT;
				light->Enable = TRUE;
				SetLightData(1, light);
			}

			//////////////////////////////////////////////////////////////////////
			// 姿勢制御
			//////////////////////////////////////////////////////////////////////

			XMVECTOR vx, nvx, up;
			XMVECTOR quat;
			float len, angle;

			g_Player[i].UpVector = GravityNormal;
			up = { 0.0f, 1.0f, 0.0f, 0.0f };
			vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

			//垂直のベクトルを求めている
			nvx = XMVector3Length(vx);
			XMStoreFloat(&len, nvx);
			nvx = XMVector3Normalize(vx);
			//nvx = vx / len;
			angle = asinf(len);

			//quat = XMQuaternionIdentity();

			//	quat = XMQuaternionRotationAxis(nvx, angle);
			quat = XMQuaternionRotationNormal(nvx, angle);


			quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player[i].Quaternion), quat, 0.05f);
			XMStoreFloat4(&g_Player[i].Quaternion, quat);



#ifdef _DEBUG
			PrintDebugProc("PLAYER pos.x:%f,pos.y:%f,pos.z:%f\n", g_Player[0].pos.x, g_Player[0].pos.y, g_Player[0].pos.z);
			PrintDebugProc("PLAYER invincibility:%d, Invincibility_Cnt:%d\n", g_Player[0].invincibility,Invincibility_Cnt);
#endif
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	BOSS* boss = GetBoss();

	// もし、視野の中に入ってしまったら
	FuchiFlickering();

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == FALSE) return;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player[i].scl.x * PLAYER_PARTS_SIZE, g_Player[i].scl.y * PLAYER_PARTS_SIZE, g_Player[i].scl.z * PLAYER_PARTS_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);
		
		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);

		// 階層アニメーション
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// 使われているなら処理する
			if (g_Parts[j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);
				
			DrawModel(&g_PartModel[j].model);
		}

		SetFuchi(0);

	}
}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
		return &g_Player[0];

}

//=============================================================================
// 入力判定
//=============================================================================
// キーボードのいずれかが押されたら
BOOL AnyKeyPress(void)
{
	return (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_DOWN));
}

// スティックののいずれかが押されたら
BOOL AnyStickPress(void)
{
	return (IsButtonPressed(0, L_STICK_LEFT) || IsButtonPressed(0, L_STICK_RIGHT) || IsButtonPressed(0, L_STICK_UP) || IsButtonPressed(0, L_STICK_DOWN));
}

//=============================================================================
// 汎用関数
//=============================================================================

// 子の当たり判定(パーツの番号、パーツ半径、エネミーの座標、エネミーの半径)
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er)
{
	// パーツの子の付け根の座標を求めている
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[index].mtxWorld);

	// 求めた座標を入れてる
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BCの当たり判定の結果を返す
	return CollisionBC(pos, epos, r, er);
}

 //階層アニメーション
void HierarchyAnimation(int partmax)
{
	for (int j = 0; j < partmax; j++)
	{
		// 使われているなら処理する
		if ((g_Parts[j].use == TRUE) && (g_Parts[j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			//INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[j].tblNo];	// 行動テーブルのアドレスを取得

			//--------------------------------------------------------------------------------------
			// キーフレーム変更部分
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[j].tblNo];
			//--------------------------------------------------------------------------------------

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる

				//--------------------------------------------------------------------------------------
				// キーフレーム変更部分
				switch (g_Player[0].setTbl)
				{
				case PLAYER_STATE_ATK:
					g_Parts[j].tblMax = -1;
					break;
				}
				//--------------------------------------------------------------------------------------

			}
		}
	}
}

// 重力処理
void PlayerGravity(void)
{
	// レイキャストして足元の高さを求める・重力判定
	XMFLOAT3 HitPosition;				// 交点 地面の当たったところ
	g_Player[0].falling = RayHitField(g_Player[0].pos, &HitPosition, &GravityNormal);	// 当たっていたら
	BOOL gravity = TRUE;				// 重力をつけるかどうか
	float distance = 0.0f;				// 地面とプレイヤーの差
	float FallingPos = FALLINGPOS_Y;	// 落下ダメージ座標

	if (g_Player[0].falling)
	{
		distance = g_Player[0].pos.y - HitPosition.y;	// プレイヤーと地面の高さの差が出る

		// 地面に埋まらなくする
		if (distance <= GRAVITY_PLUS && distance >= GRAVITY_MINUS)
		{
			// 重力をなくす
			gravity = FALSE;
		}
	}

	if (gravity == TRUE)
	{
		// 重力を付ける
		g_Player[0].pos.y -= GRAVITY;

		// この座標より下ならダメージをうける
		if (g_Player[0].pos.y <= FALLINGPOS_Y)
		{
			g_Player[0].hp -= 1.0f;
			g_Player[0].pos = XMFLOAT3(22.0f, PLAYER_OFFSET_Y, -600.0f);
		}

	}

	// もし地面より下なら元の位置に戻す
	if (distance < 0.0f && g_Player[0].falling == TRUE)
	{
		g_Player[0].pos.y -= distance - PLAYER_OFFSET_Y;
	}

}

//=============================================================================
// アニメーションのSTATE管理
//=============================================================================

// キー入力
void InputWalkPlayer(void)
{
	// 座標の記憶
	g_Player[0].oldpos = g_Player[0].pos;

	if (g_Player[0].stance == FALSE)
	{
		// -----キーボード---------------
		if (AnyKeyPress())
		{
			g_Player[0].spd = VALUE_MOVE;
			g_Player[0].state = PLAYER_STATE_WALK;

			// 斜め方向に移動する
			float x = 0.0f;
			float z = 0.0f;

			if (GetKeyboardPress(DIK_LEFT))  x = -1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) x = 1.0f;
			if (GetKeyboardPress(DIK_UP))	 z = -1.0f;
			if (GetKeyboardPress(DIK_DOWN))  z = 1.0f;

			float angle = atan2f(x, z) * -1;
			g_Player[0].dir = angle;
		}

		// -----スティック---------------
		if (AnyStickPress())
		{
			g_Player[0].spd = VALUE_MOVE;
			g_Player[0].state = PLAYER_STATE_WALK;

			//// キー入力を取得している
			DIJOYSTATE2		dijs = GetDeviceState();

			// プレイヤーを360度回している
			float angle = atan2f((float)dijs.lX, (float)dijs.lY) * -1;

			g_Player[0].dir = angle;

		}
	}
}

// 移動処理
void WalkPlayer(void)
{
	// Key入力があったら移動処理する
	CAMERA* cam = GetCamera();

	// 座標の記憶
	XMFLOAT3 oldPos = g_Player[0].pos;

	switch (g_Player[0].state)
	{
	case PLAYER_STATE_WALK:
		if (g_Player[0].spd < VALUE_MOVE) g_Player[0].state = PLAYER_STATE_IDLE;
		break;

	default:
		break;
	}

	if (g_Player[0].spd > 0.0f)
	{
		g_Player[0].rot.y = g_Player[0].dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player[0].pos.x -= sinf(g_Player[0].rot.y) * g_Player[0].spd;
		g_Player[0].pos.z -= cosf(g_Player[0].rot.y) * g_Player[0].spd;

		// 慣性をいれている
		g_Player[0].spd *= 0.9f;
	}
	else if (g_Player[0].spd <= 0.0f)
	{
		g_Player[0].state = PLAYER_STATE_IDLE;

	}

	// 元の座標に戻す
	if (ColliderHitPlayer() == TRUE)
	{
		g_Player[0].pos = oldPos;
	}


}

// 構え状態
void InputStancePlayer(void)
{
	CAMERA* cam = GetCamera();

	// ボタンを押している間構え状態になる
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_R2))
	{
		g_Player[0].stance = TRUE;
		g_Player[0].state = PLAYER_STATE_STANCE;
		g_Player[0].rot.y = cam->rot.y + XM_PI;

	}

	if(GetKeyboardRelease(DIK_RETURN) || IsButtonRelease(0,BUTTON_R2))
	{
		g_Player[0].stance = FALSE;
		g_Player[0].state = PLAYER_STATE_IDLE;
		g_Player[0].rot.y = cam->rot.y + XM_PI;

	}

	// リロード(弾が5発以下かつ、アイドル状態なら)
	if ((GetKeyboardTrigger(DIK_TAB) || IsButtonTriggered(0, BUTTON_L))
		&& g_Player[0].reload <= 5
		&& g_Player[0].state == PLAYER_STATE_IDLE)
	{
		g_Player[0].reload = PLAYER_RELOAD;
	}

}

 //攻撃処理
//void InputAttackPlayer(void)
//{
//	CAMERA* cam = GetCamera();
//	SPARK* spark = GetSpark();
//	// 弾発射処理
//	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R)) 
//		&& g_Player->reload != 0 
//		&& g_Player[0].stance == TRUE)
//	{
//		// 打つときに前を向かせる
//		g_Player[0].rot.y = cam->rot.y + XM_PI;
//
//		// 残弾数を減らす
//		g_Player[0].reload--;
//
//		// ワールドマトリックスの初期化
//		// 正方行列の初期化
//		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
//		mtxWorld = XMMatrixIdentity();
//
//		// アフィン変換
//		// 移動を反映
//		float rot = g_Player[0].rot.z;
//		mtxTranslate = XMMatrixTranslation(0, -25, 500); // ベクトルの強さ(プレイヤーのZ方向)
//
//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
//
//		// 回転を反映		
//		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
//		XMFLOAT4X4 result;
//		XMStoreFloat4x4(&result, mtxWorld);
//
//		// ベクトル
//		XMFLOAT3 vector;
//
//		// 行列の要素を取り出してベクトルに足す
//		vector.x = result._41;
//		vector.y = result._42;
//		vector.z = result._43;
//
//		vector.x += cam->pos.x;
//		vector.y += cam->pos.y + 15.0f;
//		vector.z += cam->pos.z;
//
//		g_Player[0].move = { 0.0f,0.0f,0.0f };
//		XMFLOAT3 vPos = g_Player[0].pos;
//		vPos.y += PLAYER_BULLET;
//
//		// プレイヤーとカメラの差分
//		float ofset_y = vPos.y - vector.y;
//
//		// プレイヤーの頭基準にカメラの高さを反転させる
//		vector.y += ofset_y * INVERSION;
//
//		// カメラ
//		XMVECTOR vA = XMLoadFloat3(&vector);
//		// プレイヤーの座標
//		XMVECTOR vB = XMLoadFloat3(&vPos);
//
//		// vABの正規化
//		// 長さを求めて、正規化している(A,B)
//		XMVECTOR vAB = XMVector3Normalize(vA - vB);
//
//		XMStoreFloat3(&g_Player[0].move, vAB);
//		SetPlayerBullet(vPos, g_Player[0].move);
//
//		// 弾発射時に火花を出す
//		SparkAppearance();
//
//		g_Player[0].state = PLAYER_STATE_ATK;
//
//		// 効果音
//		PlaySound(SOUND_LABEL_SE_player_atc);
//
//	}
//}


void InputAttackPlayer(void)
{
	CAMERA* cam = GetCamera();
	SPARK* spark = GetSpark();
	// 弾発射処理
	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R)) 
		&& g_Player->reload != 0 
		&& g_Player[0].stance == TRUE)
	{
		// 打つときに前を向かせる
		g_Player[0].rot.y = cam->rot.y + XM_PI;

		// 残弾数を減らす
		g_Player[0].reload--;

		// ワールドマトリックスの初期化
		// 正方行列の初期化
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// アフィン変換
		// 移動を反映
		float rot = g_Player[0].rot.z;
		mtxTranslate = XMMatrixTranslation(0, -25, 500); // ベクトルの強さ(プレイヤーのZ方向)

		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 回転を反映		
		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		// ベクトル
		XMFLOAT3 vector;

		// 行列の要素を取り出してベクトルに足す
		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		vector.x += cam->pos.x;
		vector.y += cam->pos.y + 15.0f;
		vector.z += cam->pos.z;

		g_Player[0].move = { 0.0f,0.0f,0.0f };
		XMFLOAT3 vPos = g_Player[0].pos;
		vPos.y += PLAYER_BULLET;

		// プレイヤーとカメラの差分
		float ofset_y = vPos.y - vector.y;

		// プレイヤーの頭基準にカメラの高さを反転させる
		vector.y += ofset_y * INVERSION;

		// カメラ
		XMVECTOR vA = XMLoadFloat3(&vector);
		// プレイヤーの座標
		XMVECTOR vB = XMLoadFloat3(&vPos);

		// vABの正規化
		// 長さを求めて、正規化している(A,B)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);

		XMStoreFloat3(&g_Player[0].move, vAB);
		SetPlayerBullet(vPos, g_Player[0].move);

		// 弾発射時に火花を出す
		SparkAppearance();

		g_Player[0].state = PLAYER_STATE_ATK;

		// 効果音
		PlaySound(SOUND_LABEL_SE_player_atc);

	}
}

// カメラ操作
void InputMoveCamera(void)
{
	CAMERA* camera = GetCamera();

	if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, R_STICK_RIGHT))
	{// 視点旋回「左」
		camera->rot.y += VALUE_ROTATE_CAMERA;
		if (camera->rot.y > XM_PI)
		{
			camera->rot.y -= XM_PI * 2.0f;
		}

		camera->pos.x = camera->at.x - sinf(camera->rot.y) * camera->len;
		camera->pos.z = camera->at.z - cosf(camera->rot.y) * camera->len;
	}

	if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, R_STICK_LEFT))
	{// 視点旋回「右」
		camera->rot.y -= VALUE_ROTATE_CAMERA;
		if (camera->rot.y < -XM_PI)
		{
			camera->rot.y += XM_PI * 2.0f;
		}

		camera->pos.x = camera->at.x - sinf(camera->rot.y) * camera->len;
		camera->pos.z = camera->at.z - cosf(camera->rot.y) * camera->len;
	}

	if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, R_STICK_UP))
	{// 視点移動「上」
		camera->pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, R_STICK_DOWN))
	{// 視点移動「下」
		camera->pos.y += VALUE_MOVE_CAMERA;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R) || (IsButtonPressed(0, R_STICK_PRESS)))
	{
		camera->len += VALUE_MOVE_CAMERA;

		float targetPosX, targetPosZ;
		targetPosX = camera->at.x - sinf(g_Player[0].rot.y + XM_PI) * camera->len;
		targetPosZ = camera->at.z - cosf(g_Player[0].rot.y + XM_PI) * camera->len;
		
		SetCameraReset(targetPosX, targetPosZ);

		g_Player[0].bulletX = 0.0f;
	}
}

// 状態遷移
void AttackPlayer(void)
{
	
	BOOL ans = TRUE;

	// すべてのパーツの最大テーブル数が -1 になっている場合(≒アニメーションが終了している場合)、状態遷移させる
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].tblMax != -1) ans = FALSE;
	}

	if (ans == TRUE) g_Player[0].state = PLAYER_STATE_IDLE;
	
}

// アニメーションをセット
void SetTblPlayerAnim(int state)
{
	// 目的のステートと今のステートが一致していなければ
	if (g_Player[0].setTbl != state)
	{
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[i].use == FALSE) continue;

			g_Parts[i].time = 0.0f;
			g_Parts[i].tblNo = i + (PLAYER_PARTS_MAX * state);
			g_Parts[i].tblMax = (int)g_TblAdr[g_Parts[i].tblNo]->size();

		}
		g_Player[0].setTbl = state;
	}
}

// プレイヤーにダメージを与える
void DamagePlayer(float damage)
{
	// プレイヤーのライフを減らす
	if (g_Player[0].invincibility == FALSE)
	{
		g_Player[0].hp -= damage;
	}

	// 無敵時間スタート
	g_Player[0].invincibility = TRUE;


	if (g_Player[0].hp <= 0.0f)			// HPが0以下だったら
	{
		DECO* deco = GetDeco();

		// プレイヤーは倒される
		g_Player[0].use = FALSE;
		SetMonochrome(true);
		deco[DECO_FAILED].use = TRUE;
		SetSoundFade(FADE_OUT, SOUND_LABEL_BGM_GAMEOVER);
	}
}

// ダメージを受けたら無敵時間を設ける
void Invincibility_Mode(void)
{
	if (g_Player[0].invincibility == TRUE)
	{
		// 無敵時間・透明化のカウント
		Invincibility_Cnt++;
		g_PlayerOpacityCnt++;
		float angle = (XM_PI / INVINCIBILITY_ACTION) * g_PlayerOpacityCnt;

		// 無敵時間の間点滅させる
		for (int j = 0; j < g_PartModel[0].model.SubsetNum; j++)
		{
			for (int m = 0; m < PLAYER_PARTS_MAX; m++)
			{
				SetModelDiffuse(&g_PartModel[m].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, g_PlayerOpacity * fabsf(sinf(XM_PI / 2 + angle))));
			}
		}

		// 無敵時間を超えたら無効化にする
		if (Invincibility_Cnt > INVINCIBILITY_TIME)
		{
			g_Player[0].invincibility = FALSE;
			Invincibility_Cnt = 0;
			g_PlayerOpacityCnt = 0.0f;

			// 無敵時間が終わったら色を戻す
			for (int j = 0; j < g_PartModel[0].model.SubsetNum; j++)
			{
				for (int m = 0; m < PLAYER_PARTS_MAX; m++)
				{
					SetModelDiffuse(&g_PartModel[m].model, j, g_PartModel[0].diffuse[j]);
				}
			}
		}
	}
}

// 一定以上視野の中にいたら赤くチカチカさせる
void FuchiFlickering(void)
{
	BOSS* boss = GetBoss();
	if (boss[0].view == TRUE)
	{
		// もし、ボスの視野にいたら縁を付けて警告する
		SetFuchi(1);
		g_FuchiCnt++;
	}

	if(boss[0].view == FALSE && g_FuchiCnt >= FUCHI_TIME)
	{
		SetFuchi(0);
	}
}

// 火花を出現させる
void SparkAppearance(void)
{
	SPARK* spark = GetSpark();
	if (spark[0].sparkcnt < SPARK_TIME)
	{
		// 弾を打ったら火花をだす
		XMFLOAT3 SparkPos = g_Player[0].pos;
		SparkPos.y += SPARK_PLUS_POS_Y;
		SparkPos.z += SPARK_PLUS_POS_Z;
		XMFLOAT4 SparkCol = { 1.0f,1.0f,1.0f,0.5f };

		SetSpark(SparkPos, SPARK_SCL, SPARK_SCL, SparkCol);
	}
}