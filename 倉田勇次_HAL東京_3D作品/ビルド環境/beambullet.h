//=============================================================================
//
// �r�[������ [beambullet.h]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	BEAMBULLET_MAX_BULLET	(128)		// �e�ő吔
#define BEAMBULLET_LIFE			(60)		// �e�̃��C�t
#define BEAMBULLET_TIME			(260.0f)	// �r�[����ł���


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	XMFLOAT3	spd;			// �ړ���
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			blife;			// �o���b�g�̃��C�t
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} BEAMBULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBeamBullet(void);
void UninitBeamBullet(void);
void UpdateBeamBullet(void);
void   DrawBeamBullet(void);

int SetBeamBullet(XMFLOAT3 pos, XMFLOAT3 rot);

BEAMBULLET*GetBeamBullet(void);

