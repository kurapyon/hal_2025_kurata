//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GP11A132 09 �q�c�E��
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
// �}�N����`
//*****************************************************************************
// ���f���̓ǂݍ���
#define MODEL_ENEMY			"data/MODEL/ENEMY/enemy/enemy01.obj"
#define MODEL_TEMPLE		"data/MODEL/ENEMY/enemy/temple.obj"

// �G�l�~�[
#define	VALUE_MOVE					(2.0f)				// �ړ���
#define	VALUE_ROTATE				(XM_PI * 0.02f)		// ��]��
#define VALUE_SIZE					(5.0f)				// �X�P�[��
#define ENEMY_SHADOW_SIZE			(0.4f)				// �e�̑傫��
#define ENEMY_OFFSET_Y				(30.0f)				// �G�l�~�[�̑��������킹��
// �U���J�E���g
#define ENEMY_USUALLY_CNTLOWER		(1000)				// �ʏ�U���̃J�E���g
#define ENEMY_USUALLY_CNTUPPER		(2000)				// �ʏ�U���̃J�E���g
// ��_���[�W
#define ENEMY_RECEIVEWAIT			(180)				// ��_���[�W���̐F�ύX����
// dissolve
#define ENEMY_DISSOLVE				(1.0f)

// �{�X
#define	VALUE_BOSSMOVE				(2.0f)				// �ړ���
#define	VALUE_BOSSROTATE			(XM_PI * 0.02f)		// ��]��
#define VALUE_BOSSSIZE				(5.0f)				// �X�P�[��
#define ENEMYBOSS_SHADOW_SIZE		(0.4f)				// �e�̑傫��
#define ENEMYBOSS_SPEED				(3.0f)				// �{�X�̈ړ��X�s�[�h
#define ENEMYBOSS_OFFSET_Y			(40.0f)				// �G�l�~�[�̑��������킹��
#define ENEMYBOSS_PARTS_MAX			(2)					// �v���C���[�̃p�[�c�̐�
// �U���J�E���g
#define ENEMYBOSS_USUALLY_CNTLOWER	(1000)			// �ʏ�U���̃J�E���g
#define ENEMYBOSS_USUALLY_CNTUPPER	(2000)			// �ʏ�U���̃J�E���g
// ��_���[�W
#define ENEMBOSS_RECEIVEWAIT		(180)			// ��_���[�W���̐F�ύX����
#define ENEMYBOSS_DMGCNT			(10.0f)			// �_���[�W�J�E���g
// dissolve
#define ENEMYBOSS_DISSOLVE			(1.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �G�l�~�[�����֐�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];						// �G�l�~�[

int g_Enemy_load = 0;
int g_EnemyBoss_load = 0;

// ���C�̓�������




// �G�l�~�[������o��������
int	firstcnt;

//�@�G�l�~�[
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
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{

	LoadModel(MODEL_ENEMY, &g_Enemy[0].model);
	LoadModel(MODEL_TEMPLE, &g_Enemy[1].model);

	// �ϐ��̏�����
	int	firstcnt = 0;


	for (int i = 0; i < MAX_ENEMY; i++)
	{

		g_Enemy[i].load = TRUE;
		g_Enemy[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�
		// dissolve
		g_Enemy[i].dissolve.Enable = TRUE;
		g_Enemy[i].dissolve.threshold = 1.0f;
	}

	// �������W
	g_Enemy[0].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 400.0f);
	g_Enemy[1].pos = XMFLOAT3(0.0f, 1.0f, 400.0f);

	// �X�P�[��
	g_Enemy[0].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
	g_Enemy[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// ���C�t
	g_Enemy[0].hp = ENEMY_HP;
	g_Enemy[1].hp = TEMPLE_HP;

	// �{�X��ڍs
	g_Enemy[0].chenge = FALSE;

	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[0].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[0].tblNo = tbl_enemy;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[0].tblMax = sizeof(move_tbl_enemy) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// 1�Ԃ������`��Ԃœ������Ă݂�
	g_Enemy[1].time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_Enemy[1].tblNo = tbl_temple;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Enemy[1].tblMax = sizeof(move_tbl_temple) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{

	BULLET* bullet = GetBullet();
	ENEMYATC* enemyatc = GetEnemyAtc();
	PLAYER* player = GetPlayer();


	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē��������I
	for (int i = 0; i < MAX_ENEMY; i++)
	{

		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			// �ړ�����
			if (g_Enemy[i].tblMax > 0)	// ���`��Ԃ����s����H
			{	// ���`��Ԃ̏���
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

				Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}

			}

			// �ʏ�U������
			{
				XMFLOAT3 epos = g_Enemy[0].pos;
				XMFLOAT3 erot = g_Enemy[0].rot;
				SetEnemyBullet(epos, erot);
			}

		}
	}

	// �G�l�~�[��|������{�X���
	if (g_Enemy[0].hp <= 0.0f)
	{
		g_Enemy[0].chenge = TRUE;
	}

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Enemy00:%f,Enemy01:%f\n", g_Enemy[0].hp, g_Enemy[1].hp);
#endif
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// �G�l�~�[
	PLAYER* player = GetPlayer();
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//// dissolve
		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		//// ���f���`��
		DrawModel(&g_Enemy[i].model);

	}

	//// dissolve �`����FALSE��
	SetDissolveEnable(FALSE);
	SetAlphaTestEnable(g_Enemy[0].dissolve.Enable);
	SetAlphaTestEnable(g_Enemy[1].dissolve.Enable);

	SetFuchi(0);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}