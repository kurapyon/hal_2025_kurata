//=============================================================================
//
//������ [aboveeffect.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	ABOVE_WIDTH		(192.0f)	// ���_�T�C�Y
#define	ABOVE_HEIGHT	(192.0f)	// ���_�T�C�Y
#define ABOVE_SIZE		(20.0f)		// �����蔻��̃T�C�Y
#define MAX_ABOVE		(1)			// �ő吔
#define ABOVE_ATK_CNT	(180.0f)	// ���̎�ނ̍U���������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	float		animCnt;		// �A�j���[�V�����J�E���g
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} ABOVE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitAbove(void);
void  UninitAbove(void);
void  UpdateAbove(void);
void    DrawAbove(void);
ABOVE* GetAboveEffect(void);


int SetAbove(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


