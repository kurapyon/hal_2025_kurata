//=============================================================================
//
// �g���b�N���� [truck.cpp]
// Author : �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "obstacle.h"
#include "truck.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_TRUCK		"data/MODEL/obstacle/truck.obj"			// �ǂݍ��ރ��f����
#define INFO_TRUCK		"data/FIELDDATA/truck.csv"				// �ǂݍ���csv��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static std::vector<OBSTACLE_DATA> g_Truck;
static DX11_MODEL		g_model;
static BOOL				g_load;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTruck(void)
{
	LoadModel(MODEL_TRUCK, &g_model);
	LoadFieldDataCSV(INFO_TRUCK, g_Truck);
	g_load = TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTruck(void)
{
	if (g_load)
	{
		// ���f���̉������
		UnloadModel(&g_model);

		// �f�[�^�̉������
		g_Truck.clear();
		g_load = FALSE;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTruck(void)
{
#ifdef _DEBUG
#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTruck(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < g_Truck.size(); i++)
	{
			// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Truck[i].scl.x, g_Truck[i].scl.y, g_Truck[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Truck[i].rot.x, g_Truck[i].rot.y + XM_PI, g_Truck[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Truck[i].pos.x, g_Truck[i].pos.y, g_Truck[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}
