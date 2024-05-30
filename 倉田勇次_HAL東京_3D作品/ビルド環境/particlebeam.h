//=============================================================================
//
// �p�[�e�B�N���r�[������ [particlebeam.h]
// Author : �q�c�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define POS_BASE_ATC		 (0.0f)
#define WIDTH_BASE_ATC		 (5.0f)
#define HEIGHTBASE_ATC		 (100.0f)
#define PARTICLEATC_LIFE	 (320)		// �p�[�e�B�N���̃��C�t
#define PARTICLEATC_SCALE	 (3.0f)		// �p�[�e�B�N���̊g�嗦
#define PARTICLEATC_DECREASE (0.99f)	// �p�[�e�B�N���̃��C�t��������
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticleBeam(void);
void UninitParticleBeam(void);
void UpdateParticleBeam(void);
void   DrawParticleBeam(void);

void SetColorParticleBeam(int nIdxParticle, XMFLOAT4 col);

// �p�[�e�B�N��
int SetParticleBeam(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);

