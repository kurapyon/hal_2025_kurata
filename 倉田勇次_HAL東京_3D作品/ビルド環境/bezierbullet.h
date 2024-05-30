//=============================================================================
//
// �x�W�F�e���� [bezierbullet.cpp]
// Author : �q�c�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BEZIERBULLET		(5)				// �e�ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	move;			// �����E�ړ�
	XMFLOAT3	scl;			// �X�P�[��
	XMFLOAT3	p0;				// �x�W�F�Ȑ�
	XMFLOAT3	p1;				// �x�W�F�Ȑ�
	XMFLOAT3	p2t;			// �x�W�F�Ȑ�
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	float		hp;				// �e�̏o������
	float		s;				// �x�W�F�Ȑ�
	float		animCnt;		// �A�j���[�V�����J�E���g
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} BEZIERBULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBezierBullet(void);
void	UninitBezierBullet(void);
void	UpdateBezierBullet(void);
void	DrawBezierBullet(void);
int SetBossBullet(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 tpos);

BEZIERBULLET *GetBezierBullet(void);

