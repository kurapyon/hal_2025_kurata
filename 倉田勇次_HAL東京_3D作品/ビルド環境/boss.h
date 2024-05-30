//=============================================================================
//
// ���f������ [boss.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// ��{�X�e�[�^�X
#define MAX_BOSS			(1)					// �{�X�̐�
#define BOSS_HP				(10.0f)				// �{�X�̗̑�
#define BOSS_REDUCE			(1.0f)				// �{�X�̗̑͂�����
#define BOSS_SCL			(0.07f)				// �{�X�̃X�P�[��
#define	VALUE_BOSS_MOVE		(45.0f)				// �{�X�̈ړ���
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)	// �{�X�̉�]��
#define	BOSS_SIZE			(300.0f)			// �{�X�̓����蔻��̑傫��
#define BOSS_OFFSET_Y		(180.0f)			// �{�X�̑��������킹��
#define BOSS_SPEED			(2.0f)				// �{�X�̃X�s�[�h

// �U���n
#define BEZIER_OFFSET_Y			(7.0f)		// �ʏ�U���̔��˂̍���
#define BOSS_NOTATC_TIME		(120)		// �U�������Ȃ�����
#define CLOSE_DISTANCE			(1000.0f)	// �v���C���[�Ƃ̍����߂��Ƃ�
#define APART_DISTANCE			(500.0f)	// �v���C���[�Ƃ̍��������Ƃ�
#define SPHERE_TIME				(300.0f)	// ���̏�̃p�[�e�B�N���̎���
#define VIEMSTAY_EDGE			(50)		// ���̎��ԉ߂�����x���Ƃ��ĉ���t����
#define VIEMSTAY_TIME			(120)		// ����̒��ɂ��̎��Ԉȏア��ƍU�������
#define BEAM_OFFSET_Y			(120.0f)	// �r�[�����ːݒ�
#define BOSS_PATROL_INTERVAL	(300)		// �p�g���[�������s����Ԋu
#define BOSS_HP_BELOW_1			(5.0f)		// �{�X�̗̑͂����ȉ��Ȃ�
#define BOSS_HP_BELOW_2			(4.0f)		// �{�X�̗̑͂����ȉ��Ȃ�
#define BOSS_HP_BELOW_3			(3.0f)		// �{�X�̗̑͂����ȉ��Ȃ�
#define BOSS_HP_BELOW_4			(2.0f)		// �{�X�̗̑͂����ȉ��Ȃ�

// �{�X�̃X�e�[�g���X�g
enum STATE_LIST_BOSS
{
	// �ŏ���0�ɂ���
	BOSS_STATE_IDLE = 0,	// �������
	BOSS_STATE_SEARCH,		// �����𑪂�
	BOSS_STATE_BEZIER,		// �x�W�F
	BOSS_STATE_ABOVE,		// �ォ��̍U��
	BOSS_STATE_DEATHBLOW,	// �K�E�Z

	BOSS_STATE_MAX,			// MAX
};

// �q�p�[�c�̃��X�g
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
// �\���̒�`
//*****************************************************************************
class BOSS
{
	public:

	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT4			Quaternion;			// �N�H�[�^�j�I��
	XMFLOAT3			UpVector;			// �����������Ă��鏊
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			oldpos;				// ���W�̋L��
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			move;				// �o���b�g�̓���
	float				hp;					// ���C�t
	float				spd;				// �ړ��X�s�[�h
	float				time;				// �K�w�A�j���[�V�����p
	float				size;				// �����蔻��̃T�C�Y
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	int					setTbl;				// �Z�b�g���Ă���e�[�u��
	int					state;				// �X�e�[�g(��ԁj
	int					modelIdx;			// ���f���C���f�b�N�X
	int					WhereMove;			// �ǂ��Ɉړ����邩
	int					ViewStayCnt;		// ���b�Ԏ���̒��ɂ��邩
	BOOL				use;				// �����Ă��邩
	BOOL				close;				// ���v���C���[���߂����ǂ���
	BOOL				ViewingAngle;		// ����p�ɓ����Ă��邩�ǂ���
	BOOL				view;				// ����p�ɓ����Ă��ă��C�Ƃ��������Ă��邩�ǂ���
	BOOL				edge;				// ���ȏ㎞�ԉ߂����牏��t����

	// dissolve
	DISSOLVE			dissolve;

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	BOSS* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X
};

struct BOSS_MODEL
{
	BOOL				load;				// ���[�h���邩�ǂ���
	DX11_MODEL			model;				// ���f�����
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBoss(void);
void  UninitBoss(void);
void  UpdateBoss(void);
void    DrawBoss(void);

BOSS *GetBoss(void);

// �����蔻��
BOOL ChildBossCollision(int i, int index, float r, XMFLOAT3 epos, float er);

// �K�w�A�j���[�V����
void HierarchyBossAnimation(int i);
void DamageBoss(float damage);
