//=============================================================================
//
// �󏈗� [sky.cpp]
// Author :�q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "debugproc.h"
#include "sky.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define MODEL_SKY			"data/MODEL/sky/sky.obj"	// ���f���̓ǂݍ���

#define	VALUE_ROTATE		(0.001f)		// ��]��
#define VALUE_SIZE			(2.0f)			// �T�C�Y
//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
static SKY			g_Sky[MAX_SKY];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_SKY, &g_Sky[i].model);
		g_Sky[i].load = TRUE;

		g_Sky[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Sky[i].use = TRUE;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = FALSE;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == TRUE)
		{
			// ��]������
			g_Sky[i].rot.y += VALUE_ROTATE;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x * VALUE_SIZE, g_Sky[i].scl.y * VALUE_SIZE, g_Sky[i].scl.z * VALUE_SIZE);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y + XM_PI, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);

		//// ���f���`��
		DrawModel(&g_Sky[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_NONE);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
SKY *GetSky()
{
	return &g_Sky[0];
}
