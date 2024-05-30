//=============================================================================
//
// �e���ˏ��� [playerbullet.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(256)	// �e�ő吔

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
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���


} PLAYERBULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayerBullet(void);
void UninitPlayerBullet(void);
void UpdatePlayerBullet(void);
void DrawPlayerBullet(void);

int SetPlayerBullet(XMFLOAT3 pos, XMFLOAT3 move);

PLAYERBULLET *GetPlayerBullet(void);

