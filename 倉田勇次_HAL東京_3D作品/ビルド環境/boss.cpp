//=============================================================================
//
// �{�X���� [boss.cpp]
// Author : �q�c�E��
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
// �}�N����`
//*****************************************************************************
// �A�j���[�V�����̃��X�g
enum BOSSANIM_LIST
{
	// �A�C�h�����
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

	// �������
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

// �{�X�o���ʒu
#define START_POS_X_1			(  0.0f)
#define START_POS_X_2			( -60.0f)
#define START_POS_Z_1			(   0.0f)
#define START_POS_Z_2			(-450.0f)
#define START_POS_X_3			((START_POS_X_1 + START_POS_X_2) / 2)
#define START_POS_Z_3			((START_POS_Z_1 + START_POS_Z_2) / 2)

// �f�B�]���u
#define BOSS_DISSOLVE			(1.0f)
#define BOSS_VIEWANGLE			(25)

// �t�B�[���h���ɂƂǂ܂点��
#define BOSS_FIELD_X_1			(MAP_W)
#define BOSS_FIELD_Z_1			(MAP_H)
#define BOSS_FIELD_X_2			(-MAP_W)
#define BOSS_FIELD_Z_2			(-MAP_H)

// ��]�̕��
#define ROTATE_INTERPOLTION			(0.02f)
// ��Ԃ���߂�p�x
#define STOP_INTERPOLTION_ANGLE		(10.0f)
// �z�[�~���O����߂�p�x
#define STOP_INTERPOLTION_DISTANCE  (10.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �A�j���[�V�����֐�
void AttackBoss(int i);
void SetTblBossIdle(int i);
void SetTblBossSearch(int i);

// �{�X�̍s���֐�
void SearchPlayer(int i);
void SearchView(int i);
void BossMovePatrol(int i);
void BossAtkBezier(int i);
void BossAbove(int i);
void BossAtkDeathblow(int i);
void BossSetPatrolPoint(void);
BOOL BossDeathblow(int i);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOSS		g_Boss[MAX_BOSS];						// �{�X
static BOSS		g_Parts[MAX_BOSS][BOSSPARTS_MAX];		// �{�X�̃p�[�c�p
static BOSS_MODEL	g_Model[MAX_BOSS][BOSSPARTS_MAX];	// �{�X�̃��f��

// ���C�g
static LIGHT		g_Light;

// �Ԃ������|���S���̖@���x�N�g���i�����j
static XMFLOAT3 Normalboss;

// �ϐ��̏�����
static XMFLOAT3 bpos = { 0.0f,0.0f,0.0f };
static XMFLOAT3 brot = { 0.0f,0.0f,0.0f };

// �v���C���[�Ƃ̋���
static float g_Distance;

// �{�X�̃A�N�V�����J�E���^�[
static float g_DeathblowCnt = 0.0f;	// �K�E�Z���o������
static float g_SphereCnt    = 0.0f;	// ���̏�̃p�[�e�B�N�����o������
static float g_AboveAtkCnt  = 0;	// �ォ��̍U���̉񐔂𐔂���

// ���˂����e�̐�
static int g_BulletNum;
// �{�X�ƃv���C���[�̂Ȃ��p
static float g_ViewAngle;

// ���̏�̃p�[�e�B�N�����o������ɕK�E�Z
static BOOL g_SphereEnd;

// �{�X�̈ړ��֌W
static int g_MoveCnt = 0;						// �{�X����������
XMFLOAT3   g_PatrolPos = { 0.0f,0.0f, 0.0f };	// �p�g���[�����邽�߂̍��W

// �����`�J�`�J������
static int g_EdgeFlickering = 0;

// �Q�[�����J�n���邽�߂̃M�~�b�N��ݒu
XMFLOAT3 startpos  = { START_POS_X_3 , PLAYER_OFFSET_Y , START_POS_Z_2 };
XMFLOAT3 startrot  = { 0.0f, 0.0f, 0.0f };
XMFLOAT3 startmove = { 0.0f, 0.0f, 0.0f };
XMFLOAT4 startcol  = { 1.0f, 1.0f, 1.0f, 1.0f };

// �A�j���[�V�����̃f�B���N�g���̔z��
static char* g_AnimDir[] =
{
	// �ҋ@���
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

	// �������
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

// ���f���f�[�^�̃f�B���N�g��
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

// �{�X�̊K�w�A�j���[�V�����f�[�^
// �ҋ@���
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

// �������
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


// �A�j���[�V�����f�[�^�̃A�h���X
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// �A�C�h�����
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

	// �������
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
// ����������
//=============================================================================
HRESULT InitBoss(void)
{

	// ���f���̓Ǎ�
	for (int i = 0; i < MAX_BOSS; i++)
	{
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// �A�j���[�V�����f�[�^���t�@�C������ǂݍ���
	for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// �{�X�̏�����
	for (int i = 0; i < MAX_BOSS; i++)
	{
		g_Boss[i].pos = XMFLOAT3(22.0f, BOSS_OFFSET_Y, 600.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(BOSS_SCL, BOSS_SCL, BOSS_SCL);
		g_Boss[i].use = FALSE;
		g_Boss[i].spd = VALUE_BOSS_MOVE;	// �ړ��X�s�[�h
		g_Boss[i].size = BOSS_SIZE;			// �����蔻��̑傫��
		g_Boss[i].hp = BOSS_HP;				// �v���C���[�̃��C�t
		g_Boss[i].state  = BOSS_STATE_IDLE;	// �X�e�C�g�Ǘ�
		g_Boss[i].setTbl = BOSS_STATE_IDLE;
		g_Boss[i].close = FALSE;			// ���v���C���[���߂����ǂ���
		g_Boss[i].WhereMove = 0;			// �ǂ��Ɉړ����邩
		g_Boss[i].ViewStayCnt = 0;			// ����ɉ��b�����Ă��邩�ǂ���
		g_Boss[i].ViewingAngle = FALSE;		// ����p�ɓ����Ă��邩�ǂ���
		g_Boss[i].view = FALSE;

		// dissolve
		g_Boss[i].dissolve.Enable = TRUE;
		g_Boss[i].dissolve.threshold = 1.0f;

		// �A�j���[�V����
		g_Boss[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

		// �p�g���[���p
		g_PatrolPos = g_Boss[i].pos;

		 //�K�w�A�j���[�V�����̏�����
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			g_Parts[i][j].use = TRUE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Parts[i][j].modelIdx = j;

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[i][j].time = 0.0f;												// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i][j].tblNo = j + BOSSANIM_move_BODY;							// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();		// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			// �e�q�֌W
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

		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Boss[i].Quaternion, XMQuaternionIdentity());
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		// ���f���̉������
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Model[i][j].load = FALSE;
		}

		// �A�j���[�V�����e�[�u�����t�@�C�����珉����
		for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
		{
			g_TblAdr[i]->clear();
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBoss(void)
{
	PLAYER* player = GetPlayer();
	PARTICLESTART* particlestart = GetPArticleStart();

	for (int i = 0; i < MAX_BOSS; i++)
	{
		// �{�X���o���O�Ȃ�\������
		if (g_Boss[i].use == FALSE)
		{
			SetParticleStart(startpos, startmove, startrot, startcol,
				START_PARTICLE_SCALE, START_PARTICLE_SCALE,
				START_PARTICLE_LIFE);

			// �v���C���[�����̍��W�ɂ�����
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
			// �f�B�]���u�G�t�F�N�g��t����
			g_Boss[i].dissolve.threshold -= 0.005f;
		}

		switch (g_Boss[i].state)
		{
			// �v���C���[������ɓ����Ă��邩�ǂ������肵�ăX�e�C�g��ς���
		case BOSS_STATE_IDLE:
			SetTblBossSearch(i);
			SearchView(i);
			BossMovePatrol(i);
			break;

			// �v���C���[�Ƃ̋����𑪂�U�������߂�
		case BOSS_STATE_SEARCH:
			SetTblBossSearch(i);
			SearchPlayer(i);
			break;

			// �e�̔���(�x�W�F)
		case BOSS_STATE_BEZIER:
			SetTblBossSearch(i);
			BossAtkBezier(i);
			break;

			// �ォ��̍U��
		case BOSS_STATE_ABOVE:
			SetTblBossSearch(i);
			BossAbove(i);
			break;

			// �K�E�Z
		case BOSS_STATE_DEATHBLOW:
			SetTblBossSearch(i);
			BossAtkDeathblow(i);
			break;

		default:
			break;
		}

		// �K�w�A�j���[�V����
		HierarchyBossAnimation(i);

#ifdef _DEBUG
		PrintDebugProc("Boss pos.x:%f,pos.y:%f,pos.z:%f\n", g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
		PrintDebugProc("PatrolPos pos.x:%f,pos.y:%f,pos.z:%f\n", g_PatrolPos.x, g_PatrolPos.y, g_PatrolPos.z);
		PrintDebugProc("viwe:%d,viwecnt:%d\n", g_Boss[i].view, g_Boss[i].ViewStayCnt);
#endif

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBoss(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use == FALSE) continue;
		// dissolve
		SetDissolve(&g_Boss[i].dissolve);
		SetAlphaTestEnable(g_Boss[i].dissolve.Enable);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Boss[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

		// �K�w�A�j���[�V����
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			// �g���Ă���Ȃ珈������
			if (g_Parts[i][j].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// ���f���`��
			DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
		}
		// dissolve �`����FALSE��
		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(g_Boss[i].dissolve.Enable);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �{�X�����擾
//=============================================================================
BOSS* GetBoss(void)
{
	return &g_Boss[0];
}

//=============================================================================
// �֐���
//=============================================================================

// �q�̓����蔻��(�{�X�̔ԍ��A�p�[�c�̔ԍ��A�p�[�c���a�A�G�l�~�[�̍��W�A�G�l�~�[�̔��a)
BOOL ChildBossCollision(int i, int index, float r, XMFLOAT3 epos, float er)
{
	// �p�[�c�̎q�̕t�����̍��W�����߂Ă���
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[i][index].mtxWorld);

	// ���߂����W�����Ă�
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BC�̓����蔻��̌��ʂ�Ԃ�
	return CollisionBC(pos, epos, r, er);
}

//�K�w�A�j���[�V����
void HierarchyBossAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i][j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i][j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;				// �ړ���e�[�u���̔ԍ������߂Ă���
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[i][j].tblNo];	// �s���e�[�u���̃A�h���X���擾(vector�^��)

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i][j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[i][j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i][j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���

				// �J��Ԃ��Ȃ��A�j���[�V�����̏ꍇ
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
// �s���p�^�[��
//=============================================================================

// ����Ɏw��̎��Ԉȏア���ꍇ�U������
void SearchView(int i)
{
	PLAYER* player = GetPlayer();

	// �f�B�]���u���I���܂ōs�������Ȃ�
	if (g_Boss[i].dissolve.threshold <= 0.0f)
	{
		XMVECTOR dirVec, targetVec;				// �{�X�̎����x�N�g��, �v���C���[�Ƃ̃x�N�g��

		// �{�X�̌������x�N�g���ɕϊ�
		float x = sinf(g_Boss[i].rot.y + XM_PI);
		float z = cosf(g_Boss[i].rot.y + XM_PI);				// ���f���̌������l��

		dirVec = XMVector3Normalize(XMVectorSet(x, 0.0f, z, 0.0f));

		// �{�X����v���C���[�ւ̃x�N�g��
		XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);
		targetVec = XMVector3Normalize(XMLoadFloat3(&player[0].pos) - bpos);

		float cos = dotProduct(&dirVec, &targetVec);				// ���ς����߂ăv���C���[�Ƃ̂Ȃ��p���v�Z
		g_ViewAngle = acosf(cos);

		// 0��0�̏��Z�̏ꍇ�G���[���N�����Ȃ��B���ɖ߂�
		if (isnan(g_ViewAngle))	g_ViewAngle = XM_PI;

		g_ViewAngle = XMConvertToDegrees(g_ViewAngle);

		// ����p�ɓ����Ă��邩����
		g_Boss[i].ViewingAngle = (g_ViewAngle <= BOSS_VIEWANGLE) ? TRUE : FALSE;

		// ����̒��ɂ�����
		if (g_Boss[i].view == TRUE)
		{
			// �w��̎��Ԉȏ㎋��̒��ɂ�����v���C���[�Ƃ̋����𑪂�
			g_Boss[i].ViewStayCnt++;
			// ���̎��Ԉȏ㋏����U��������
			if (g_Boss[i].ViewStayCnt > VIEMSTAY_TIME)
			{
				// �X�e�C�Ƃ�؂�ւ���
				g_Boss[i].state = BOSS_STATE_SEARCH;

				// ������
				g_Boss[i].ViewingAngle = FALSE;
				g_Boss[i].view = FALSE;
				g_Boss[i].edge = FALSE;
				g_Boss[i].ViewStayCnt = 0;
			}
		}
	}
}

 //�v���C���[�Ƃ̋����ƃ{�X�̗̑͂ɂ���čU����ς���
void SearchPlayer(int i)
{
	PLAYER* player = GetPlayer();

	// �v���C���[�Ƃ̋����ɉ����čU��������
	// �v���C���[�ƃ{�X�̋������v�Z���čU�����Ǘ�����
	// temp = �ꎞ�I�Ȍv�Z�p�̕ϐ�
	XMVECTOR temp = XMLoadFloat3(&player->pos) - XMLoadFloat3(&g_Boss[i].pos);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�������o�Ă���j
	XMStoreFloat(&g_Distance, temp);

	if ((g_Distance / 1000.0f > APART_DISTANCE) && (g_Boss[i].state == BOSS_STATE_SEARCH))
	{
		// ������΃x�W�F�U��
		g_Boss[i].state = BOSS_STATE_BEZIER;
	}
	else
	{
		// �߂���Ώォ��̍U��
		g_Boss[i].state = BOSS_STATE_ABOVE;
	}

	// �{�X�̗̑͂����̒l�Ȃ�K�E�Z����
	if (BossDeathblow(i) == TRUE)
	{
		g_Boss[i].state = BOSS_STATE_DEATHBLOW;
	}
}

// �{�X�����̗̑͂ɂȂ�����K�E�Z�𔭓�����
BOOL BossDeathblow(int i)
{
	// �K�E�Z����
	BOOL Deathblow;

	// �{�X�̗̑͂����ȉ��Ȃ�
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
		// ����ȊO�͔������Ȃ�
		Deathblow = FALSE;
		return Deathblow;
	}
}


// �{�X�̍s��������߂�
void BossSetPatrolPoint(void)
{
	g_PatrolPos.x = rand() % (int)(MAP_W * 2.0f) - MAP_W;
	g_PatrolPos.z = rand() % (int)(MAP_H * 2.0f) - MAP_H;
}

// ���߂��s��ɂ���������
void BossMovePatrol(int i)
{
	// �v���C���[�����F���Ă���Ƃ��͓����Ȃ�
	if (g_Boss[i].view == TRUE) return;

	// �f�B�]���u���I���܂ōs�������Ȃ�
	if (g_Boss[i].dissolve.threshold <= 0.0f)
	{
		g_MoveCnt++;
		// ���̃J�E���g�ȓ��Ȃ瓮����
		if (g_MoveCnt > BOSS_PATROL_INTERVAL)
		{
			g_MoveCnt = 0;
			BossSetPatrolPoint();
		}

		// ���W�̋L��
		XMFLOAT3 BeforePos = g_Boss[i].pos;

		// �{�X�ƃp�g���[���|�X�Ƃ̋������o��
		float x = g_Boss[0].pos.x - g_PatrolPos.x;
		float z = g_Boss[0].pos.z - g_PatrolPos.z;

		// �p�g���[���|�X������ȏ�̋���������Ă����珈��
		if ((x * x + z * z) >= STOP_INTERPOLTION_DISTANCE)
		{	// �p�g���[���|�C���g�Ƀz�[�~���O������
			// //�v�Z����������ϊ����āA�Ō�Ɍ��ɖ߂��B
			XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);

			// ����(x.y.z)���v�Z�ł���B�����ƃv���C���[�̍������߂Ă���
			XMVECTOR vec = XMLoadFloat3(&g_PatrolPos) - bpos;

			// �p�x�����߂Ă���(�S�[��)
			float angle = atan2f(vec.m128_f32[0], vec.m128_f32[2]);

			// �X�s�[�h�����߂Ă���
			float speed = BOSS_SPEED;

			// �{�X���z�[�~���O������
			g_Boss[i].pos.x += sinf(angle) * speed;
			g_Boss[i].pos.z += cosf(angle) * speed;

			// �O�ς��g���ĉE�����߂�
			XMFLOAT3 cross = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 brot = { sinf(g_Boss[i].rot.y), 0.0f, cosf(g_Boss[i].rot.y) };
			XMVECTOR bvec = XMLoadFloat3(&brot);

			XMStoreFloat3(&cross, XMVector3Cross(bvec, vec));
			// �{�X�̌�����ς���ۂɕ�Ԃ��Ă�����������ς���
			if (cross.y >= STOP_INTERPOLTION_DISTANCE)
			{
				g_Boss[i].rot.y -= ROTATE_INTERPOLTION;
			}
			else if (cross.y <= -STOP_INTERPOLTION_DISTANCE)
			{
				g_Boss[i].rot.y += ROTATE_INTERPOLTION;
			}
		}

		// �t�B�[���h������o���Ȃ�(�O��������)
		if (BOSS_FIELD_X_1 < g_Boss[i].pos.x || g_Boss[i].pos.x < BOSS_FIELD_X_2 ||
			BOSS_FIELD_Z_1 < g_Boss[i].pos.z || g_Boss[i].pos.z < BOSS_FIELD_Z_2)
		{
			// �O�̍��W�ɖ߂�
			g_Boss[i].pos = BeforePos;
		}

		// �{�X����Q���Ɠ���������
		if (ColliderHitBoss() == TRUE)
		{
			g_Boss[0].pos = BeforePos;
		}
	}
}

//=============================================================================
// �U���n
//=============================================================================

// �x�W�F�U�����
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
		// �X�e�C�Ƃ�؂�ւ���
		g_Boss[i].state = BOSS_STATE_IDLE;

		// ������
		g_BulletNum = 0;
	}
}

// �ォ��̍U��
void BossAbove(int i)
{
	PLAYER* player = GetPlayer();

	XMFLOAT3 Spos = player[0].pos;
	XMFLOAT4 col = { 1.0,1.0f,1.0f, 1.0f };
	SetAbove(Spos, ABOVE_WIDTH, ABOVE_HEIGHT, col);
	g_AboveAtkCnt++;

	// �����A�J�E���g�ȏ�ɂȂ�����X�e�C�Ƃ�߂�
	if (g_AboveAtkCnt >= ABOVE_ATK_CNT)
	{
		// �X�e�C�Ƃ�؂�ւ���
		g_Boss[i].state = BOSS_STATE_IDLE;

		// ������
		g_AboveAtkCnt = 0;
	}

}

// �K�E�Z(�r�[����̍U��)
void BossAtkDeathblow(int i)
{
	// �v���C���[��HP�������Ă�����A�K�E�Z���o��
	g_SphereCnt++;

	// ���̏�̃p�[�e�B�N�����o�������鎞��
	if (g_SphereCnt < SPHERE_TIME && g_SphereEnd == FALSE)
	{
		SetParticleSphere();
	}

	// ���̂��o���I�������r�[����ł�
	if (g_SphereCnt > SPHERE_TIME)
	{
		g_SphereEnd = TRUE;
		g_SphereCnt = 0.0f;
	}
	
	// ���ˈʒu�E��]�����߂�
	if (g_SphereEnd == TRUE)
	{
		bpos = g_Boss[i].pos;
		bpos.y -= BEAM_OFFSET_Y;

		brot = g_Boss[i].rot;
		SetBeamBullet(bpos, brot);
		g_DeathblowCnt++;

		// �ݒ肵�����ԃr�[����ł����̂Ȃ�
		if (g_DeathblowCnt > BEAMBULLET_TIME)
		{
			// ������
			g_DeathblowCnt = 0.0f;
			g_SphereEnd = FALSE;

			// �X�e�C�g�ύX
			g_Boss[i].state = BOSS_STATE_IDLE;
		}
	}
}

//=============================================================================
// �A�j���[�V������STATE�Ǘ�
//=============================================================================

// ��ԑJ��(�A�j���[�V����)
void AttackBoss(int i)
{
	BOOL ans = TRUE;

	// ���ׂẴp�[�c�̍ő�e�[�u������ -1 �ɂȂ��Ă���ꍇ(���A�j���[�V�������I�����Ă���ꍇ)�A��ԑJ�ڂ�����
	for (int p = 0; p < BOSSPARTS_MAX; p++)
	{
		if (g_Parts[i][p].tblMax != -1) ans = FALSE;
	}

	if (ans == TRUE) g_Boss[i].state = BOSS_STATE_IDLE;
}

 //�A�C�h�����(�A�j���[�V����)
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

//�T�����(�A�j���[�V����)
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
// �����蔻��֘A
//=============================================================================

// �{�X�Ƀ_���[�W��^����
void DamageBoss(float damage)
{
	// �{�X�̃��C�t�����炷
	g_Boss[0].hp -= damage;

	if (g_Boss[0].hp <= 0.0f)					// �{�X��HP��0�ȉ��Ȃ�
	{
		g_Boss[0].use = FALSE;

		DECO* deco = GetDeco();
		deco[DECO_CLEAR].use = TRUE;
		SetSoundFade(FADE_OUT, SOUND_LABEL_BGM_GAMECLEAR);
	}
}