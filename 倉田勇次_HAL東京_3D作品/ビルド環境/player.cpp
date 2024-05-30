//=============================================================================
//
// ���f������ [player.cpp]
// Author :�q�c�E��
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
// �}�N����`
//*****************************************************************************

// ���f����
#define	MODEL_PLAYER_BODY		"data/MODEL/PLAYER/Player/player_body.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HEAD		"data/MODEL/PLAYER/Player/player_head.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_L		"data/MODEL/PLAYER/Player/player_arm_l.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_R		"data/MODEL/PLAYER/Player/player_arm_r.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_THINGS_L	"data/MODEL/PLAYER/Player/player_things_l.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_THINGS_R	"data/MODEL/PLAYER/Player/player_things_r.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_L		"data/MODEL/PLAYER/Player/player_leg_l.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_R		"data/MODEL/PLAYER/Player/player_leg_r.obj"		// �ǂݍ��ރ��f����

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �A�j���[�V�����̃f�B���N�g��
// �ҋ@���
#define ANIM_IDLE_BODY		"data/ANIMDATA/PLAYER/Player_Idle/Player_Body.csv"
#define ANIM_IDLE_HEAD		"data/ANIMDATA/PLAYER/Player_Idle/Player_Head.csv"
#define ANIM_IDLE_LARM		"data/ANIMDATA/PLAYER/Player_Idle/Player_Arm_L.csv"
#define ANIM_IDLE_RARM		"data/ANIMDATA/PLAYER/Player_Idle/Player_Arm_R.csv"
#define ANIM_IDLE_LTHINGS	"data/ANIMDATA/PLAYER/Player_Idle/Player_Things_L.csv"
#define ANIM_IDLE_RTHINGS	"data/ANIMDATA/PLAYER/Player_Idle/Player_Things_R.csv"
#define ANIM_IDLE_LLEG		"data/ANIMDATA/PLAYER/Player_Idle/Player_Leg_L.csv"
#define ANIM_IDLE_RLEG		"data/ANIMDATA/PLAYER/Player_Idle/Player_Leg_R.csv"

// �ړ����
#define ANIM_WALK_BODY		"data/ANIMDATA/PLAYER/Player_Walk/Player_Body.csv"
#define ANIM_WALK_HEAD		"data/ANIMDATA/PLAYER/Player_Walk/Player_Head.csv"
#define ANIM_WALK_LARM		"data/ANIMDATA/PLAYER/Player_Walk/Player_Arm_L.csv"
#define ANIM_WALK_RARM		"data/ANIMDATA/PLAYER/Player_Walk/Player_Arm_R.csv"
#define ANIM_WALK_LTHINGS	"data/ANIMDATA/PLAYER/Player_Walk/Player_Things_L.csv"
#define ANIM_WALK_RTHINGS	"data/ANIMDATA/PLAYER/Player_Walk/Player_Things_R.csv"
#define ANIM_WALK_LLEG		"data/ANIMDATA/PLAYER/Player_Walk/Player_Leg_L.csv"
#define ANIM_WALK_RLEG		"data/ANIMDATA/PLAYER/Player_Walk/Player_Leg_R.csv"

// �\�����
#define ANIM_STANCE_BODY	"data/ANIMDATA/PLAYER/Player_Stance/Player_Body.csv"
#define ANIM_STANCE_HEAD	"data/ANIMDATA/PLAYER/Player_Stance/Player_Head.csv"
#define ANIM_STANCE_LARM	"data/ANIMDATA/PLAYER/Player_Stance/Player_Arm_L.csv"
#define ANIM_STANCE_RARM	"data/ANIMDATA/PLAYER/Player_Stance/Player_Arm_R.csv"
#define ANIM_STANCE_LTHINGS	"data/ANIMDATA/PLAYER/Player_Stance/Player_Things_L.csv"
#define ANIM_STANCE_RTHINGS	"data/ANIMDATA/PLAYER/Player_Stance/Player_Things_R.csv"
#define ANIM_STANCE_LLEG	"data/ANIMDATA/PLAYER/Player_Stance/Player_Leg_L.csv"
#define ANIM_STANCE_RLEG	"data/ANIMDATA/PLAYER/Player_Stance/Player_Leg_R.csv"

// �U�����
#define ANIM_ATK_BODY		"data/ANIMDATA/PLAYER/Player_Atc/Player_Body.csv"
#define ANIM_ATK_HEAD		"data/ANIMDATA/PLAYER/Player_Atc/Player_Head.csv"
#define ANIM_ATK_LARM		"data/ANIMDATA/PLAYER/Player_Atc/Player_Arm_L.csv"
#define ANIM_ATK_RARM		"data/ANIMDATA/PLAYER/Player_Atc/Player_Arm_R.csv"
#define ANIM_ATK_LTHINGS	"data/ANIMDATA/PLAYER/Player_Atc/Player_Things_L.csv"
#define ANIM_ATK_RTHINGS	"data/ANIMDATA/PLAYER/Player_Atc/Player_Things_R.csv"
#define ANIM_ATK_LLEG		"data/ANIMDATA/PLAYER/Player_Atc/Player_Leg_L.csv"
#define ANIM_ATK_RLEG		"data/ANIMDATA/PLAYER/Player_Atc/Player_Leg_R.csv"

// ���ǉ����������͂����ɒǉ�
//--------------------------------------------------------------------------------------

#define	VALUE_MOVE					(4.0f)				// �ړ���
#define	VALUE_ROTATE				(D3DX_PI * 0.02f)	// ��]��
#define PLAYER_PARTS_MAX			(8)					// �v���C���[�̃p�[�c�̐�
#define GRAVITY_PLUS				(10000)				// �d�͔���
#define GRAVITY_MINUS				(-100.0)			// �d�͔���
#define GRAVITY						(15.0)
#define PLAYER_PARTS_SIZE			(2.0f)				// �v���C���[�̃p�[�c�̃T�C�Y

// �e�̔���
#define PLAYER_BULLET				(10.0f)				// �e�̔��˗p

// ��_���[�W
#define PLAYER_RECEIVEWAIT			(180)				// ��_���[�W���̐F�ύX����

// �����_���[�W���W
#define FALLINGPOS_Y				(-500.0f)

// �v���C���[�̓���ɃJ�����̍������]
#define INVERSION					(3.0f)

// �I�_�p�}�N����`
#define LASTSTOP_X					(0.0f)
#define LASTSTOP_Y					(0.0f)
#define LASTSTOP_Z					(1000.0f)

// �J�����V�F�C�N�p�J�E���g
#define SHAKECNT					(2.0f)

// �Q�[���J�n�ʒu
#define START_POS_X_1				(  0.0f)
#define START_POS_X_2				( -60.0f)
#define START_POS_Z_1				(   0.0f)
#define START_POS_Z_2				(-450.0f)

// ���G����
#define INVINCIBILITY_TIME			(180)	// ���G����
#define INVINCIBILITY_ACTION		(20.0f) // �������̎���
#define FLICKERING_END				(170)	// ���̎��ԂɃ`�J�`�J���I��点��

// ����t���鎞��
#define FUCHI_TIME					(30)	// ���̎��Ԃ͐�΂ɉ���t����

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �A�j���[�V�����̃��X�g
enum ANIM_LIST
{
	// �ҋ@���
	IDLE_BODY,
	IDLE_HEAD,
	IDLE_LARM,
	IDLE_RARM,
	IDLE_LTHINGS,
	IDLE_RTHINGS,
	IDLE_LLEG,
	IDLE_RLEG,

	// �ړ����
	WALK_BODY,
	WALK_HEAD,
	WALK_LARM,
	WALK_RARM,
	WALK_LTHINGS,
	WALK_RTHINGS,
	WALK_LLEG,
	WALK_RLEG,

	// �\�����
	STANCE_BODY,
	STANCE_HEAD,
	STANCE_LARM,
	STANCE_RARM,
	STANCE_LTHINGS,
	STANCE_RTHINGS,
	STANCE_LLEG,
	STANCE_RLEG,

	// �U�����
	ATK_BODY,
	ATK_HEAD,
	ATK_LARM,
	ATK_RARM,
	ATK_LTHINGS,
	ATK_RTHINGS,
	ATK_LLEG,
	ATK_RLEG,

	// ���ǉ����������͂����ɒǉ�

	ANIM_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// �A�j���[�V�����֐�
void InputWalkPlayer(void);
void InputStancePlayer(void);
void InputAttackPlayer(void);
void InputMoveCamera(void);
void WalkPlayer(void);
void AttackPlayer(void);
void SetTblPlayerAnim(int state);

// �֐���
void HierarchyAnimation(int partmax);
void PlayerGravity(void);
void SparkAppearance(void);
void Invincibility_Mode(void);
void FuchiFlickering(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player[MAX_PLAYER];					// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];				// �v���C���[�̃p�[�c�p

static PLAYER_MODEL g_PartModel[PLAYER_PARTS_MAX];			// �v���C���[���f��

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

static LIGHT g_Light;						// ���C�g
XMFLOAT3     LastStop;						// �I�_�p�̕ϐ�
XMFLOAT3	 GravityNormal;					// �Ԃ������|���S���̖@���x�N�g���i�����j
XMFLOAT3	 BossNormal;					// �Ԃ������G�Ƃ̖@���x�N�g���i�����j
static int   g_jumpCnt = 0;					// �W�����v�֌W
static int	 g_sparkCnt = 0;				// �ΉԂ��o������
static float g_camerashake = 1.0f;			// �J�����V�F�C�N
static float g_camerashakeCnt = 0.0f;		// �J�����V�F�C�N
int			 cnt;							// �o���b�g�J�E���g(adove)
int			 Invincibility_Cnt = 0;			// ���G���Ԃ��J�E���g����
static float g_PlayerOpacity = 1.0f;		// �����x
static float g_PlayerOpacityCnt = 0.0f;		// �����x�J�E���^�[
static float g_FuchiCnt	= 0.0f;				// ����t����J�E���^�[

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
static char* g_AnimDir[] = 
{
	// �ҋ@���
	ANIM_IDLE_BODY,
	ANIM_IDLE_HEAD,
	ANIM_IDLE_LARM,
	ANIM_IDLE_RARM,
	ANIM_IDLE_LTHINGS,
	ANIM_IDLE_RTHINGS,
	ANIM_IDLE_LLEG,
	ANIM_IDLE_RLEG,

	// �ړ����
	ANIM_WALK_BODY,
	ANIM_WALK_HEAD,
	ANIM_WALK_LARM,
	ANIM_WALK_RARM,
	ANIM_WALK_LTHINGS,
	ANIM_WALK_RTHINGS,
	ANIM_WALK_LLEG,
	ANIM_WALK_RLEG,

	// �\�����
	ANIM_STANCE_BODY,
	ANIM_STANCE_HEAD,
	ANIM_STANCE_LARM,
	ANIM_STANCE_RARM,
	ANIM_STANCE_LTHINGS,
	ANIM_STANCE_RTHINGS,
	ANIM_STANCE_LLEG,
	ANIM_STANCE_RLEG,

	// �U�����
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
// �L�[�t���[���ύX����
// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �ҋ@���
static std::vector<INTERPOLATION_DATA> idle_body;
static std::vector<INTERPOLATION_DATA> idle_head;
static std::vector<INTERPOLATION_DATA> idle_larm;
static std::vector<INTERPOLATION_DATA> idle_rarm;
static std::vector<INTERPOLATION_DATA> idle_lthings;
static std::vector<INTERPOLATION_DATA> idle_rthings;
static std::vector<INTERPOLATION_DATA> idle_lleg;
static std::vector<INTERPOLATION_DATA> idle_rleg;

// �ړ����
static std::vector<INTERPOLATION_DATA> walk_body;
static std::vector<INTERPOLATION_DATA> walk_head;
static std::vector<INTERPOLATION_DATA> walk_larm;
static std::vector<INTERPOLATION_DATA> walk_rarm;
static std::vector<INTERPOLATION_DATA> walk_lthings;
static std::vector<INTERPOLATION_DATA> walk_rthings;
static std::vector<INTERPOLATION_DATA> walk_lleg;
static std::vector<INTERPOLATION_DATA> walk_rleg;

// �\�����
static std::vector<INTERPOLATION_DATA> stance_body;
static std::vector<INTERPOLATION_DATA> stance_head;
static std::vector<INTERPOLATION_DATA> stance_larm;
static std::vector<INTERPOLATION_DATA> stance_rarm;
static std::vector<INTERPOLATION_DATA> stance_lthings;
static std::vector<INTERPOLATION_DATA> stance_rthings;
static std::vector<INTERPOLATION_DATA> stance_lleg;
static std::vector<INTERPOLATION_DATA> stance_rleg;

// �U�����
static std::vector<INTERPOLATION_DATA> atk_body;
static std::vector<INTERPOLATION_DATA> atk_head;
static std::vector<INTERPOLATION_DATA> atk_larm;
static std::vector<INTERPOLATION_DATA> atk_rarm;
static std::vector<INTERPOLATION_DATA> atk_lthings;
static std::vector<INTERPOLATION_DATA> atk_rthings;
static std::vector<INTERPOLATION_DATA> atk_lleg;
static std::vector<INTERPOLATION_DATA> atk_rleg;

//--------------------------------------------------------------------------------------
// �L�[�t���[���ύX����
// �v���C���[�̊K�w�A�j���[�V�����f�[�^�̔z��
static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// �ҋ@���
	&idle_body,
	&idle_head,
	&idle_larm,
	&idle_rarm,
	&idle_lthings,
	&idle_rthings,
	&idle_lleg,
	&idle_rleg,

	// �ړ����
	&walk_body,
	&walk_head,
	&walk_larm,
	&walk_rarm,
	&walk_lthings,
	&walk_rthings,
	&walk_lleg,
	&walk_rleg,
	
	// �\�����
	&stance_body,
	&stance_head,
	&stance_larm,
	&stance_rarm,
	&stance_lthings,
	&stance_rthings,
	&stance_lleg,
	&stance_rleg,

	// �U�����
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
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	//--------------------------------------------------------------------------------------
	// �L�[�t���[���ύX����
	// �A�j���[�V�����f�[�^���t�@�C������ǂݍ��� 
	for (int i = 0; i < ANIM_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}
	//--------------------------------------------------------------------------------------

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_Player[i].pos				= XMFLOAT3(20.0f, PLAYER_OFFSET_Y, -600.0f);// ���W
		g_Player[i].rot				= XMFLOAT3(0.0f, 0.0f, 0.0f);	// ����
		g_Player[i].scl				= XMFLOAT3(2.0f, 2.0f, 2.0f);	// �X�P�[��
		g_Player[i].move			= XMFLOAT3(1.0f, 1.0f, 1.0f);	// �o���b�g�̓���
		g_Player[i].spd				= 0.0f;							// �ړ��X�s�[�h�N���A
		g_Player[i].use				= TRUE;							// TRUE:�����Ă�
		g_Player[i].size			= PLAYER_SIZE;					// �����蔻��̑傫��
		g_Player[i].falling			= FALSE;						//
		g_Player[i].bulletX			= 0.0f;							// �o���b�g
		g_Player[i].hp				= PLAYER_HP;					// �v���C���[�̃��C�t
		g_Player[i].state			= PLAYER_STATE_IDLE;			// �A�j���[�V����
		g_Player[i].setTbl			= PLAYER_STATE_IDLE;			// �A�j���[�V����
		g_Player[i].reload			= PLAYER_RELOAD;				// �e�̎c�e��
		g_Player[i].stance			= FALSE;						// �\����Ԃ̔���
		g_Player[i].attack			= FALSE;						// �U���������ǂ���
		g_Player[i].ModelIdx		= NULL;							// ���f���C���f�b�N�X
		g_Player[i].parent			= NULL;							// �{�́i�e�j�Ȃ̂�NULL������
		g_Player[i].invincibility	= FALSE;						// ���G����

		 //�K�w�A�j���[�V�����̏�����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[j].use = TRUE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[j].parent = &g_Player[i];		// �����ɐe�̃A�h���X������

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[j].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[j].tblNo = j + IDLE_BODY;				// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[j].tblMax = (int)g_TblAdr[g_Parts[j].tblNo]->size();

			// ���f���̓ǂݍ���
			LoadModel(g_ModelName[j], &g_PartModel[j].model);
			g_PartModel[j].load = TRUE;
			// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
			GetModelDiffuse(&g_PartModel[j].model, &g_PartModel[j].diffuse[0]);


			// �e�q�֌W
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

		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// �p�[�c�̉������
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_PartModel[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_PartModel[i].model);
			g_PartModel[i].load = FALSE;
		}
	}

	// �A�j���[�V�����e�[�u�����t�@�C�����珉����
	for (int i = 0; i < ANIM_MAX; i++)
	{
		g_TblAdr[i]->clear();
	}
}

//=============================================================================
// �X�V����
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

			// �J��������
			InputMoveCamera();

			// �d��
			PlayerGravity();

			// �K�w�A�j���[�V����
			SetTblPlayerAnim(g_Player[i].state);
			HierarchyAnimation(PLAYER_PARTS_MAX);

			// ���G���Ԃ̐ݒ�
			Invincibility_Mode();

			// �|�C���g���C�g�̃e�X�g
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
			// �p������
			//////////////////////////////////////////////////////////////////////

			XMVECTOR vx, nvx, up;
			XMVECTOR quat;
			float len, angle;

			g_Player[i].UpVector = GravityNormal;
			up = { 0.0f, 1.0f, 0.0f, 0.0f };
			vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

			//�����̃x�N�g�������߂Ă���
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
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	BOSS* boss = GetBoss();

	// �����A����̒��ɓ����Ă��܂�����
	FuchiFlickering();

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == FALSE) return;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player[i].scl.x * PLAYER_PARTS_SIZE, g_Player[i].scl.y * PLAYER_PARTS_SIZE, g_Player[i].scl.z * PLAYER_PARTS_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �N�H�[�^�j�I���𔽉f
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);
		
		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);

		// �K�w�A�j���[�V����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[j].scl.x, g_Parts[j].scl.y, g_Parts[j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[j].rot.x, g_Parts[j].rot.y, g_Parts[j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[j].pos.x, g_Parts[j].pos.y, g_Parts[j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[j].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Parts[j].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������
			if (g_Parts[j].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);
				
			DrawModel(&g_PartModel[j].model);
		}

		SetFuchi(0);

	}
}

//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
		return &g_Player[0];

}

//=============================================================================
// ���͔���
//=============================================================================
// �L�[�{�[�h�̂����ꂩ�������ꂽ��
BOOL AnyKeyPress(void)
{
	return (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_DOWN));
}

// �X�e�B�b�N�̂̂����ꂩ�������ꂽ��
BOOL AnyStickPress(void)
{
	return (IsButtonPressed(0, L_STICK_LEFT) || IsButtonPressed(0, L_STICK_RIGHT) || IsButtonPressed(0, L_STICK_UP) || IsButtonPressed(0, L_STICK_DOWN));
}

//=============================================================================
// �ėp�֐�
//=============================================================================

// �q�̓����蔻��(�p�[�c�̔ԍ��A�p�[�c���a�A�G�l�~�[�̍��W�A�G�l�~�[�̔��a)
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er)
{
	// �p�[�c�̎q�̕t�����̍��W�����߂Ă���
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[index].mtxWorld);

	// ���߂����W�����Ă�
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//BC�̓����蔻��̌��ʂ�Ԃ�
	return CollisionBC(pos, epos, r, er);
}

 //�K�w�A�j���[�V����
void HierarchyAnimation(int partmax)
{
	for (int j = 0; j < partmax; j++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[j].use == TRUE) && (g_Parts[j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			//INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[j].tblNo];	// �s���e�[�u���̃A�h���X���擾

			//--------------------------------------------------------------------------------------
			// �L�[�t���[���ύX����
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[j].tblNo];
			//--------------------------------------------------------------------------------------

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���

				//--------------------------------------------------------------------------------------
				// �L�[�t���[���ύX����
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

// �d�͏���
void PlayerGravity(void)
{
	// ���C�L���X�g���đ����̍��������߂�E�d�͔���
	XMFLOAT3 HitPosition;				// ��_ �n�ʂ̓��������Ƃ���
	g_Player[0].falling = RayHitField(g_Player[0].pos, &HitPosition, &GravityNormal);	// �������Ă�����
	BOOL gravity = TRUE;				// �d�͂����邩�ǂ���
	float distance = 0.0f;				// �n�ʂƃv���C���[�̍�
	float FallingPos = FALLINGPOS_Y;	// �����_���[�W���W

	if (g_Player[0].falling)
	{
		distance = g_Player[0].pos.y - HitPosition.y;	// �v���C���[�ƒn�ʂ̍����̍����o��

		// �n�ʂɖ��܂�Ȃ�����
		if (distance <= GRAVITY_PLUS && distance >= GRAVITY_MINUS)
		{
			// �d�͂��Ȃ���
			gravity = FALSE;
		}
	}

	if (gravity == TRUE)
	{
		// �d�͂�t����
		g_Player[0].pos.y -= GRAVITY;

		// ���̍��W��艺�Ȃ�_���[�W��������
		if (g_Player[0].pos.y <= FALLINGPOS_Y)
		{
			g_Player[0].hp -= 1.0f;
			g_Player[0].pos = XMFLOAT3(22.0f, PLAYER_OFFSET_Y, -600.0f);
		}

	}

	// �����n�ʂ�艺�Ȃ猳�̈ʒu�ɖ߂�
	if (distance < 0.0f && g_Player[0].falling == TRUE)
	{
		g_Player[0].pos.y -= distance - PLAYER_OFFSET_Y;
	}

}

//=============================================================================
// �A�j���[�V������STATE�Ǘ�
//=============================================================================

// �L�[����
void InputWalkPlayer(void)
{
	// ���W�̋L��
	g_Player[0].oldpos = g_Player[0].pos;

	if (g_Player[0].stance == FALSE)
	{
		// -----�L�[�{�[�h---------------
		if (AnyKeyPress())
		{
			g_Player[0].spd = VALUE_MOVE;
			g_Player[0].state = PLAYER_STATE_WALK;

			// �΂ߕ����Ɉړ�����
			float x = 0.0f;
			float z = 0.0f;

			if (GetKeyboardPress(DIK_LEFT))  x = -1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) x = 1.0f;
			if (GetKeyboardPress(DIK_UP))	 z = -1.0f;
			if (GetKeyboardPress(DIK_DOWN))  z = 1.0f;

			float angle = atan2f(x, z) * -1;
			g_Player[0].dir = angle;
		}

		// -----�X�e�B�b�N---------------
		if (AnyStickPress())
		{
			g_Player[0].spd = VALUE_MOVE;
			g_Player[0].state = PLAYER_STATE_WALK;

			//// �L�[���͂��擾���Ă���
			DIJOYSTATE2		dijs = GetDeviceState();

			// �v���C���[��360�x�񂵂Ă���
			float angle = atan2f((float)dijs.lX, (float)dijs.lY) * -1;

			g_Player[0].dir = angle;

		}
	}
}

// �ړ�����
void WalkPlayer(void)
{
	// Key���͂���������ړ���������
	CAMERA* cam = GetCamera();

	// ���W�̋L��
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

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player[0].pos.x -= sinf(g_Player[0].rot.y) * g_Player[0].spd;
		g_Player[0].pos.z -= cosf(g_Player[0].rot.y) * g_Player[0].spd;

		// ����������Ă���
		g_Player[0].spd *= 0.9f;
	}
	else if (g_Player[0].spd <= 0.0f)
	{
		g_Player[0].state = PLAYER_STATE_IDLE;

	}

	// ���̍��W�ɖ߂�
	if (ColliderHitPlayer() == TRUE)
	{
		g_Player[0].pos = oldPos;
	}


}

// �\�����
void InputStancePlayer(void)
{
	CAMERA* cam = GetCamera();

	// �{�^���������Ă���ԍ\����ԂɂȂ�
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

	// �����[�h(�e��5���ȉ����A�A�C�h����ԂȂ�)
	if ((GetKeyboardTrigger(DIK_TAB) || IsButtonTriggered(0, BUTTON_L))
		&& g_Player[0].reload <= 5
		&& g_Player[0].state == PLAYER_STATE_IDLE)
	{
		g_Player[0].reload = PLAYER_RELOAD;
	}

}

 //�U������
//void InputAttackPlayer(void)
//{
//	CAMERA* cam = GetCamera();
//	SPARK* spark = GetSpark();
//	// �e���ˏ���
//	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R)) 
//		&& g_Player->reload != 0 
//		&& g_Player[0].stance == TRUE)
//	{
//		// �łƂ��ɑO����������
//		g_Player[0].rot.y = cam->rot.y + XM_PI;
//
//		// �c�e�������炷
//		g_Player[0].reload--;
//
//		// ���[���h�}�g���b�N�X�̏�����
//		// �����s��̏�����
//		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
//		mtxWorld = XMMatrixIdentity();
//
//		// �A�t�B���ϊ�
//		// �ړ��𔽉f
//		float rot = g_Player[0].rot.z;
//		mtxTranslate = XMMatrixTranslation(0, -25, 500); // �x�N�g���̋���(�v���C���[��Z����)
//
//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
//
//		// ��]�𔽉f		
//		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
//		XMFLOAT4X4 result;
//		XMStoreFloat4x4(&result, mtxWorld);
//
//		// �x�N�g��
//		XMFLOAT3 vector;
//
//		// �s��̗v�f�����o���ăx�N�g���ɑ���
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
//		// �v���C���[�ƃJ�����̍���
//		float ofset_y = vPos.y - vector.y;
//
//		// �v���C���[�̓���ɃJ�����̍����𔽓]������
//		vector.y += ofset_y * INVERSION;
//
//		// �J����
//		XMVECTOR vA = XMLoadFloat3(&vector);
//		// �v���C���[�̍��W
//		XMVECTOR vB = XMLoadFloat3(&vPos);
//
//		// vAB�̐��K��
//		// ���������߂āA���K�����Ă���(A,B)
//		XMVECTOR vAB = XMVector3Normalize(vA - vB);
//
//		XMStoreFloat3(&g_Player[0].move, vAB);
//		SetPlayerBullet(vPos, g_Player[0].move);
//
//		// �e���ˎ��ɉΉԂ��o��
//		SparkAppearance();
//
//		g_Player[0].state = PLAYER_STATE_ATK;
//
//		// ���ʉ�
//		PlaySound(SOUND_LABEL_SE_player_atc);
//
//	}
//}


void InputAttackPlayer(void)
{
	CAMERA* cam = GetCamera();
	SPARK* spark = GetSpark();
	// �e���ˏ���
	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_R)) 
		&& g_Player->reload != 0 
		&& g_Player[0].stance == TRUE)
	{
		// �łƂ��ɑO����������
		g_Player[0].rot.y = cam->rot.y + XM_PI;

		// �c�e�������炷
		g_Player[0].reload--;

		// ���[���h�}�g���b�N�X�̏�����
		// �����s��̏�����
		XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
		mtxWorld = XMMatrixIdentity();

		// �A�t�B���ϊ�
		// �ړ��𔽉f
		float rot = g_Player[0].rot.z;
		mtxTranslate = XMMatrixTranslation(0, -25, 500); // �x�N�g���̋���(�v���C���[��Z����)

		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ��]�𔽉f		
		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		// �x�N�g��
		XMFLOAT3 vector;

		// �s��̗v�f�����o���ăx�N�g���ɑ���
		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		vector.x += cam->pos.x;
		vector.y += cam->pos.y + 15.0f;
		vector.z += cam->pos.z;

		g_Player[0].move = { 0.0f,0.0f,0.0f };
		XMFLOAT3 vPos = g_Player[0].pos;
		vPos.y += PLAYER_BULLET;

		// �v���C���[�ƃJ�����̍���
		float ofset_y = vPos.y - vector.y;

		// �v���C���[�̓���ɃJ�����̍����𔽓]������
		vector.y += ofset_y * INVERSION;

		// �J����
		XMVECTOR vA = XMLoadFloat3(&vector);
		// �v���C���[�̍��W
		XMVECTOR vB = XMLoadFloat3(&vPos);

		// vAB�̐��K��
		// ���������߂āA���K�����Ă���(A,B)
		XMVECTOR vAB = XMVector3Normalize(vA - vB);

		XMStoreFloat3(&g_Player[0].move, vAB);
		SetPlayerBullet(vPos, g_Player[0].move);

		// �e���ˎ��ɉΉԂ��o��
		SparkAppearance();

		g_Player[0].state = PLAYER_STATE_ATK;

		// ���ʉ�
		PlaySound(SOUND_LABEL_SE_player_atc);

	}
}

// �J��������
void InputMoveCamera(void)
{
	CAMERA* camera = GetCamera();

	if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, R_STICK_RIGHT))
	{// ���_����u���v
		camera->rot.y += VALUE_ROTATE_CAMERA;
		if (camera->rot.y > XM_PI)
		{
			camera->rot.y -= XM_PI * 2.0f;
		}

		camera->pos.x = camera->at.x - sinf(camera->rot.y) * camera->len;
		camera->pos.z = camera->at.z - cosf(camera->rot.y) * camera->len;
	}

	if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, R_STICK_LEFT))
	{// ���_����u�E�v
		camera->rot.y -= VALUE_ROTATE_CAMERA;
		if (camera->rot.y < -XM_PI)
		{
			camera->rot.y += XM_PI * 2.0f;
		}

		camera->pos.x = camera->at.x - sinf(camera->rot.y) * camera->len;
		camera->pos.z = camera->at.z - cosf(camera->rot.y) * camera->len;
	}

	if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, R_STICK_UP))
	{// ���_�ړ��u��v
		camera->pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, R_STICK_DOWN))
	{// ���_�ړ��u���v
		camera->pos.y += VALUE_MOVE_CAMERA;
	}

	// �J�����������ɖ߂�
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

// ��ԑJ��
void AttackPlayer(void)
{
	
	BOOL ans = TRUE;

	// ���ׂẴp�[�c�̍ő�e�[�u������ -1 �ɂȂ��Ă���ꍇ(���A�j���[�V�������I�����Ă���ꍇ)�A��ԑJ�ڂ�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].tblMax != -1) ans = FALSE;
	}

	if (ans == TRUE) g_Player[0].state = PLAYER_STATE_IDLE;
	
}

// �A�j���[�V�������Z�b�g
void SetTblPlayerAnim(int state)
{
	// �ړI�̃X�e�[�g�ƍ��̃X�e�[�g����v���Ă��Ȃ����
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

// �v���C���[�Ƀ_���[�W��^����
void DamagePlayer(float damage)
{
	// �v���C���[�̃��C�t�����炷
	if (g_Player[0].invincibility == FALSE)
	{
		g_Player[0].hp -= damage;
	}

	// ���G���ԃX�^�[�g
	g_Player[0].invincibility = TRUE;


	if (g_Player[0].hp <= 0.0f)			// HP��0�ȉ���������
	{
		DECO* deco = GetDeco();

		// �v���C���[�͓|�����
		g_Player[0].use = FALSE;
		SetMonochrome(true);
		deco[DECO_FAILED].use = TRUE;
		SetSoundFade(FADE_OUT, SOUND_LABEL_BGM_GAMEOVER);
	}
}

// �_���[�W���󂯂��疳�G���Ԃ�݂���
void Invincibility_Mode(void)
{
	if (g_Player[0].invincibility == TRUE)
	{
		// ���G���ԁE�������̃J�E���g
		Invincibility_Cnt++;
		g_PlayerOpacityCnt++;
		float angle = (XM_PI / INVINCIBILITY_ACTION) * g_PlayerOpacityCnt;

		// ���G���Ԃ̊ԓ_�ł�����
		for (int j = 0; j < g_PartModel[0].model.SubsetNum; j++)
		{
			for (int m = 0; m < PLAYER_PARTS_MAX; m++)
			{
				SetModelDiffuse(&g_PartModel[m].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, g_PlayerOpacity * fabsf(sinf(XM_PI / 2 + angle))));
			}
		}

		// ���G���Ԃ𒴂����疳�����ɂ���
		if (Invincibility_Cnt > INVINCIBILITY_TIME)
		{
			g_Player[0].invincibility = FALSE;
			Invincibility_Cnt = 0;
			g_PlayerOpacityCnt = 0.0f;

			// ���G���Ԃ��I�������F��߂�
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

// ���ȏ㎋��̒��ɂ�����Ԃ��`�J�`�J������
void FuchiFlickering(void)
{
	BOSS* boss = GetBoss();
	if (boss[0].view == TRUE)
	{
		// �����A�{�X�̎���ɂ����牏��t���Čx������
		SetFuchi(1);
		g_FuchiCnt++;
	}

	if(boss[0].view == FALSE && g_FuchiCnt >= FUCHI_TIME)
	{
		SetFuchi(0);
	}
}

// �ΉԂ��o��������
void SparkAppearance(void)
{
	SPARK* spark = GetSpark();
	if (spark[0].sparkcnt < SPARK_TIME)
	{
		// �e��ł�����ΉԂ�����
		XMFLOAT3 SparkPos = g_Player[0].pos;
		SparkPos.y += SPARK_PLUS_POS_Y;
		SparkPos.z += SPARK_PLUS_POS_Z;
		XMFLOAT4 SparkCol = { 1.0f,1.0f,1.0f,0.5f };

		SetSpark(SparkPos, SPARK_SCL, SPARK_SCL, SparkCol);
	}
}