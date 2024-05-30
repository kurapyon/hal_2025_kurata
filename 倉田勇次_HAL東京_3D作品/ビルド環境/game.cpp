//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "boss.h"
#include "player.h"
#include "meshfield.h"
#include "fire.h"
#include "playerbullet.h"
#include "beambullet.h"
#include "particlesphere.h"
#include "particlebeam.h"
#include "collision.h"
#include "debugproc.h"
#include "sky.h"
#include "ui.h"
#include "sound.h"
#include "particleeffect.h"
#include "particlestart.h"
#include "beziereffect.h"
#include "aboveeffect.h"
#include "bezierbullet.h"
#include "spark.h"
#include "deco.h"
#include "truck.h"
#include "container.h"
#include "obstacle.h"
#include <vector>

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// TPS視点の座標用(初期の位置の値)
#define		PERSPECTIVE_X				(20)
#define		PERSPECTIVE_Y				(35)
#define		PERSPECTIVE_Z				(-30)

// zしか使わないからzだけ用意している
#define		STANCE_PERSPECTIVE_Z		(200)

// 小さくするための数
#define		VALUE_SMALLER				(0.1f)

// 境界線
#define		DIFF_BORDER					(1.0f)

// イベントシーン用
#define		BOSS_CAMERAY			(400.0f)
#define		BOSS_ATY				(190.0f)
#define		BOSS_CAMERAZ			(-600.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);
void PlayerBulletHit(void);
void AboveEffectHit(void);
void BeamBulletHit(void);
void BezierBulletHit(void);
void BossHit(void);
void TpsPerspective(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

// 今の距離を入れる変数の宣言
static XMFLOAT3 nowDis;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 50, 50, 40.0f, 40.0f);

	// 今の距離を入れる変数の初期化
	nowDis = { PERSPECTIVE_X,PERSPECTIVE_Y,PERSPECTIVE_Z };

	// プレイヤーの初期化
	InitPlayer();

	// ボスの初期化
	InitBoss();

	// ビルボード
	InitAbove();
	InitBezier();
	// InitFire();
	InitSpark();

	// 弾の初期化
	InitPlayerBullet();
	InitBeamBullet();
	InitBezierBullet();

	// UIの初期化
	InitUi();

	// パーティクルの初期化
	InitParticleSphere();
	InitParticleBeam();
	InitParticleEffect();
	InitParticleStart();

	// 空の初期化
	InitSky();

	// 障害物の初期化
	InitTruck();
	InitContainer();

	// 装飾の初期化
	InitDeco();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_BATTLE);

	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticleSphere();
	UninitParticleBeam();
	UninitParticleEffect();
	UninitParticleStart();

	// UIの終了処理
	UninitUi();

	// 弾の終了処理
	UninitPlayerBullet();
	UninitBeamBullet();
	UninitBezierBullet();

	// ビルボード
	UninitAbove();
	UninitBezier();
	//UninitFire();
	UninitSpark();

	// 地面の終了処理
	UninitMeshField();

	// ボスの終了処理
	UninitBoss();

	// プレイヤーの終了処理
	UninitPlayer();

	// 空の終了処理
	UninitSky();

	// 障害物の終了処理
	UninitTruck();
	UninitContainer();

	// 装飾の終了処理
	UninitDeco();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	if (g_bPause == FALSE)
		return;

	// モノクロ
	//SetMonochrome(1);

	// プレイヤーの更新処理
	UpdatePlayer();

	// 地面処理の更新
	UpdateMeshField();

	// ボスの更新処理
	UpdateBoss();

	// ビルボード
	UpdateAbove();
	UpdateBezier();
	//UpdateFire();
	UpdateSpark();

	// 弾の更新処理
	UpdatePlayerBullet();
	UpdateBeamBullet();
	UpdateBezierBullet();

	// パーティクルの更新処理
	UpdateParticleSphere();
	UpdateParticleBeam();
	UpdateParticleEffect();
	UpdateParticleStart();

	// 当たり判定
	CheckHit();

	// UIの更新処理
	UpdateUi();

	// 空の更新処理
	UpdateSky();

	// 障害物の更新処理
	UpdateTruck();
	UpdateContainer();


	// 装飾の更新処理
	UpdateDeco();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理

	// 空の描画処理
	DrawSky();

	// 地面の描画処理
	DrawMeshField();

	// ボスの描画処理
	DrawBoss();

	// ビルボード
	DrawAbove();
	DrawBezier();
	//DrawFire();
	DrawSpark();

	// プレイヤーの描画処理
	DrawPlayer();

	// パーティクルの描画処理
	DrawParticleSphere();
	DrawParticleEffect();
	DrawParticleStart();
	DrawParticleBeam();

	// 弾の描画処理
	DrawPlayerBullet();
	DrawBezierBullet();
	DrawBeamBullet();


	// 障害物の描画処理
	DrawTruck();
	DrawContainer();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// UIの描画処理
	DrawUi();

	// 装飾の描画処理
	DrawDeco();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);


}

void DrawGame(void)
{
	CAMERA* cam = GetCamera();
	PLAYER* player = GetPlayer();

	// TPS視点
	TpsPerspective();

	switch (g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);

		// ChangeGameCamera();
		DrawGame0();
		break;
	}
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	PlayerBulletHit();
	BeamBulletHit();
	BezierBulletHit();
	AboveEffectHit();
	BossHit();
}

// プレイヤーの弾とボスの当たり判定
void PlayerBulletHit(void)
{
	BOSS* boss = GetBoss();
	PLAYER* player = GetPlayer();
	PLAYERBULLET* playerbullet = GetPlayerBullet();

	for (int i = 0; i < MAX_BULLET; i++)
	{
		//playerbulletの有効フラグをチェックする
		if (playerbullet[i].use == FALSE)
			continue;

		//ボスの有効フラグをチェックする
		if (boss->use == FALSE)
			continue;

		// プレイヤーの攻撃とボスの当たり判定
		if (boss->use == TRUE)
		{
			//BCの当たり判定
			if (CollisionBC(playerbullet[i].pos, boss->pos, playerbullet[i].fWidth, boss->size))
			{
				// 当たったから未使用に戻す
				playerbullet[i].use = FALSE;

				// 効果音
				PlaySound(SOUND_LABEL_SE_hit);

				// エフェクト用座標
				for (int p = 0; p < 20; p++)
				{
					XMFLOAT3 move = CalcParticleSparkMove(playerbullet[i].pos);
					SetParticleEffect(playerbullet[i].pos, move, PARTICLE_SPARK_LIFE, TRUE);
				}

				// ボスのライフを減らす
				DamageBoss(BOSS_REDUCE);
			}
		}
	}
}

// beambulletとプレイヤーの当たり判定
void BeamBulletHit(void)
{
	BEAMBULLET* beambullet = GetBeamBullet();
	PLAYER* player = GetPlayer();

	for (int i = 0; i < BEAMBULLET_MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (beambullet[i].use == FALSE)
			continue;

		//敵の有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(beambullet[i].pos, player[0].pos, beambullet[i].fWidth, player[0].size))
		{
			// 当たったから未使用に戻す
			//beambullet[i].use = FALSE;

			// プレイヤーのライフを減らす
			DamagePlayer(1.0f);
		}
	}
}

// bezierbulletとプレイヤーの当たり判定
void BezierBulletHit(void)
{
	BEZIERBULLET* bezierbullet = GetBezierBullet();
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_BEZIERBULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bezierbullet[i].use == FALSE)
			continue;

		//敵の有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(bezierbullet[i].pos, player[0].pos, bezierbullet[i].fWidth, player[0].size))
		{
			// 当たったから未使用に戻す
			bezierbullet[i].use = FALSE;
			// プレイヤーのライフを減らす
			DamagePlayer(1.0f);
		}
	}
}

 //AboveEffectとプレイヤーの当たり判定
void AboveEffectHit(void)
{
	ABOVE* aboveeffect = GetAboveEffect();
	PLAYER* player = GetPlayer();

	// above分ループさせている
	for (int i = 0; i < MAX_ABOVE; i++)
	{
		//playerの有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;

		//aboveの有効フラグをチェックする
		if (aboveeffect[i].use == FALSE)
			continue;

		// プレイヤーの攻撃とaboveの当たり判定
		if (player[0].use == TRUE)
		{

			//BCの当たり判定
			if (CollisionBC(aboveeffect[i].pos, player[0].pos, aboveeffect[i].fWidth, player[0].size))
			{
				// 効果音
				//PlaySound(SOUND_LABEL_SE_hit);

				// プレイヤーのライフを減らす
				DamagePlayer(1.0f);

				// プレイヤーのHPが0以下なら
				if (player[0].hp <= 0.0f)
				{
					// キャラクターは倒される
					player[0].use = FALSE;
				}
			}
		}
	}
}

// ボスとプレイヤーの当たり判定
void BossHit(void)
{
	BOSS* boss = GetBoss();
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_BOSS; i++)
	{
		// ボスの有効フラグをチェック
		if (boss[i].use == FALSE)
			continue;

		//プレイヤーの有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(boss[i].pos, player[0].pos, boss[i].size, player[0].size))
		{
			// プレイヤーのライフを減らす
			DamagePlayer(PLAYER_REDUCE);
		}
	}
}


//プレイヤーとコライダーの当たり判定
BOOL ColliderHitPlayer(void)
{
	PLAYER* player = GetPlayer();
	std::vector<OBSTACLE_DATA> *collider =  GetCollider();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		//プレイヤーの有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;
		for (int c = 0; c < collider->size(); c++)
		{
			XMFLOAT3 colliderpos = collider[0][c].pos;
			XMFLOAT3 colliderScl = collider[0][c].scl;
			float scl = COLLIDER_SCL;

			// sclの調整
			colliderScl.x *= scl;
			colliderScl.y = 1000.0f;
			colliderScl.z *= scl;

			//BBの当たり判定
			if (CollisionBB(player[i].pos, player[0].scl, colliderpos, colliderScl) == TRUE)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//ボスとコライダーの当たり判定
BOOL ColliderHitBoss(void)
{
	BOSS* boss = GetBoss();
	std::vector<OBSTACLE_DATA>* collider = GetCollider();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		//プレイヤーの有効フラグをチェックする
		if (boss[0].use == FALSE)
			continue;
		for (int c = 0; c < COLLIDER_MAX; c++)
		{
			XMFLOAT3 colliderpos = collider[0][c].pos;
			XMFLOAT3 colliderScl = collider[0][c].scl;
			float scl = COLLIDER_SCL;

			// sclの調整
			colliderScl.x *= scl;
			colliderScl.y = 1000.0f;
			colliderScl.z *= scl;

			//BBの当たり判定
			if (CollisionBB(boss[i].pos, boss[i].scl, collider[0][c].pos, collider[0][c].scl) == TRUE)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//=============================================================================
// 関数化
//=============================================================================
// TPS視点関数
void TpsPerspective(void)
{
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();
	BOSS* boss = GetBoss();

	// プレイヤーの注視点の取得
	// ワールドマトリックスの初期化
	// 正方行列の初期化
	XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
	mtxWorld = XMMatrixIdentity();
	// ベクトル
	XMFLOAT3 vector;

	if ((boss->dissolve.threshold >= 0.0f) && (boss->use))
	{	// イベントシーン
		cam->pos.y = BOSS_CAMERAY;
		cam->rot.y = XM_PIDIV4 / 2;

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(0, BOSS_ATY, BOSS_CAMERAZ);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		vector.x += boss->pos.x;
		vector.y += boss->pos.y;
		vector.z += boss->pos.z;
	}
	else
	{
		// アフィン変換(カメラのズームアップ)
		if (player->stance == TRUE)
		{
			// 構え状態
			// 差分入れる変数(z軸の計算)初期化
			float diff = 0.0f;

			// 差分(今の距離とゴール地点の距離の差)を計算していれる
			diff = STANCE_PERSPECTIVE_Z - nowDis.z;

			// 一定の値以下になったらやめる
			if (diff > DIFF_BORDER)
			{
				// 値を小さくするための式
				diff *= VALUE_SMALLER;

				// 最後の計算をする(差は足す)
				nowDis.z += diff;
			}
			else
			{
				// 一定の値以下になったらゴール地点を代入する
				nowDis.z = STANCE_PERSPECTIVE_Z;
			}
		}
		else
		{
			//	通常状態(値は変えない)初期化する
			nowDis.z = PERSPECTIVE_Z;
		}

		mtxTranslate = XMMatrixTranslation(nowDis.x, nowDis.y, nowDis.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		// 行列の要素を取り出してベクトルに足す
		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		// プレイヤーの座標を足す
		vector.x += player->pos.x;
		vector.y += player->pos.y;
		vector.z += player->pos.z;
	}


	SetCameraAT(vector);
	SetCamera();
}