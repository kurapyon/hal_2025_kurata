//=============================================================================
//
// ���f������ [player.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER			(1)					// �v���C���[�̐�
#define	PLAYER_SIZE			(20.0f)				// �����蔻��̑傫��

// �v���C���[��HP
#define PLAYER_HP			(10.0f)				// �v���C���[�̗̑�
#define PLAYER_REDUCE		(1.0f)				// �v���C���[�̗̑͂̌���
#define PLAYER_OFFSET_Y		(54.0f)				// �v���C���[�̑��������킹��

// �e�̎c�e��
#define PLAYER_RELOAD		(6)					// �e�̎c�e��

// �q�p�[�c�̃��X�g
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
// �O���[�o���ϐ�
//*****************************************************************************

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class PLAYER
{
	public:

	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT4			Quaternion;			// �N�H�[�^�j�I��
	XMFLOAT3			UpVector;			// �����������Ă�����W
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			oldpos;				// ���W�̋L��
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			move;				// �o���b�g�̓���
	float				hp;					// ���C�t
	float				spd;				// �ړ��X�s�[�h
	float				dir;				// ����
	float				size;				// �T�C�Y
	float				bulletX;
	float				time;				// ���`��ԗp
	int					state;				// �X�e�[�g(��ԁj
	int					reload;				// �e�̎c��̐�
	int					ModelIdx;			// ���f���C���f�b�N�X
	int					jumpCnt;			// �W�����v���̃J�E���g
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	int					setTbl;				// �Z�b�g���Ă���e�[�u��
	BOOL				use;				// �����Ă��邩�ǂ���
	BOOL				falling;			// ��������
	BOOL				stance;				// �\����Ԃ��̔���
	BOOL				jump;				// �W�����v�t���O
	BOOL				attack;				// �U���������ǂ���
	BOOL				invincibility;		// ���G����


	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// dissolve
	DISSOLVE			dissolve;
};

struct PLAYER_MODEL
{
	BOOL				load;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	DX11_MODEL			model;							// ���f�����
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

// ���͔���
BOOL AnyStickPress(void);
BOOL AnyKeyPress(void);

// �����蔻��
BOOL ChildCollision(int index, float r, XMFLOAT3 epos, float er);
// �v���C���[�Ƀ_���[�W��^����
void DamagePlayer(float damage);