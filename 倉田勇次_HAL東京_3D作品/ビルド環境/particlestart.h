//=============================================================================
//
// �p�[�e�B�N��ATC���� [particleatc.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define START_PARTICLE_LIFE		(120)	// �p�[�e�B�N���̃��C�t
#define START_PARTICLE_SCALE	(1.0f)	// �p�[�e�B�N���̊g�嗦

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef class
{
	public:
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

} PARTICLESTART;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleStart(void);
void UninitParticleStart(void);
void UpdateParticleStart(void);
void   DrawParticleStart(void);

void SetColorParticleStart(int nIdxParticle, XMFLOAT4 col);

// �p�[�e�B�N��
int SetParticleStart(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
PARTICLESTART* GetPArticleStart(void);
