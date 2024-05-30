//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define MAX_ENEMY		(2)				// �G�l�~�[�̐�
#define	ENEMY_SIZE		(50.0f)			// �����蔻��̑傫��
#define ENEMY_HP		(5.0f)			// �G�l�~�[�̃��C�t
#define TEMPLE_HP		(1.0f)			// ��̃��C�t
#define ENEMY_REDUCE	(1.0f)			// �G�l�~�[�̗̑͂̌���

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;						// ���[���h�}�g���b�N�X
	XMFLOAT4			Quaternion;						// �N�H�[�^�j�I��
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	XMFLOAT3			UpVector;						// �����������Ă��鏊
	XMFLOAT3			pos;							// ���f���̍��W
	XMFLOAT3			rot;							// ���f���̌���(��])
	XMFLOAT3			scl;							// ���f���̑傫��(�X�P�[��)
	float				hp;								// HP(���C�t)
	float				spd;							// �ړ��X�s�[�h
	float				size;							// �����蔻��̑傫��
	float				time;							// ���`��ԗp
	int					shadowIdx;						// �e�̃C���f�b�N�X�ԍ�
	int					tblNo;							// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;							// ���̃e�[�u���̃f�[�^��
	BOOL				chenge;							// �{�X��ڍs
	BOOL				use;							// �g�p���Ă��邩
	BOOL				load;							// ���f���̃��[�h������̂�
	BOOL				chek;							// dissolve�ł̏o��

	// dissolve
	DISSOLVE			dissolve;

	// ���f�����
	DX11_MODEL			model;

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ENEMY* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);


