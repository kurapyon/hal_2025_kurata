//=============================================================================
//
// �R���e�i���� [container.cpp]
// Author : �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "obstacle.h"
#include "container.h"
#include "collision.h"
#include "boss.h"
#include "player.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_YCONTAINER	"data/MODEL/obstacle/yellowContainer.obj"	// �ǂݍ��ރ��f����
#define	MODEL_GCONTAINER	"data/MODEL/obstacle/greenContainer.obj"	// �ǂݍ��ރ��f����
#define	MODEL_COLLIDER		"data/MODEL/collider/collider.obj"			// �ǂݍ��ރ��f����
#define INFO_YCONTAINER		"data/FIELDDATA/yellowCont.csv"				// �ǂݍ���csv��
#define INFO_GCONTAINER		"data/FIELDDATA/greenCont1.csv"			 	// �ǂݍ���csv��
#define INFO_COLLIDER		"data/FIELDDATA/collider.csv"				// �ǂݍ���csv��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void HiddenCheckView(void);
void HiddenCheckTranslucent(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static std::vector<OBSTACLE_DATA> g_YContainer;
static std::vector<OBSTACLE_DATA> g_GContainer;
static std::vector<OBSTACLE_DATA> g_collider;

static DX11_MODEL		g_model[COLOR_CONTAINER_MAX];
static DX11_MODEL		g_colliderModel;

BOOL isHit = FALSE;			// �v���C���[����Q���ɉB��Ă�����
BOOL isTranslucent = FALSE;	// �v���C���[�ƃJ�����̊Ԃɂ��邩�ǂ����̔���

XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

static BOOL				g_load;
std::vector<POLYGON> colliderPolygons;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitContainer(void)
{
	LoadModel(MODEL_COLLIDER,	&g_colliderModel);
	LoadModel(MODEL_YCONTAINER, &g_model[COLOR_YELLOW]);
	LoadModel(MODEL_GCONTAINER, &g_model[COLOR_GREEN]);

	// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
	GetModelDiffuse(&g_model[COLOR_YELLOW], &diffuse[0]);
	GetModelDiffuse(&g_model[COLOR_GREEN], &diffuse[0]);

	LoadFieldDataCSV(INFO_COLLIDER,   g_collider);
	LoadFieldDataCSV(INFO_YCONTAINER, g_YContainer);
	LoadFieldDataCSV(INFO_GCONTAINER, g_GContainer);

	static std::vector<MODEL> colliderModel;

	MakeVertexLists(MODEL_COLLIDER, &colliderPolygons);

	g_load = TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitContainer(void)
{
	if (g_load)
	{
		for (int i = 0; i < COLOR_CONTAINER_MAX; i++)
		{
			// ���f���̉������
			UnloadModel(&g_model[i]);
		}
		// ���f���̉������
		UnloadModel(&g_colliderModel);

		// �f�[�^�̉������
		g_collider.clear();
		g_YContainer.clear();
		g_GContainer.clear();
		g_load = FALSE;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateContainer(void)
{

	// �v���C���[���B��Ă��邩���肷��
	HiddenCheckView();
	//HiddenCheckTranslucent();
#ifdef _DEBUG
	PrintDebugProc("isHit:%d\n", isHit);
	PrintDebugProc("isTranslucent:%d\n", isTranslucent);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawContainer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���F�R���e�i
	for (int i = 0; i < g_YContainer.size(); i++)
	{
			// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_YContainer[i].scl.x, g_YContainer[i].scl.y, g_YContainer[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_YContainer[i].rot.x, g_YContainer[i].rot.y + XM_PI, g_YContainer[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_YContainer[i].pos.x, g_YContainer[i].pos.y, g_YContainer[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_model[COLOR_YELLOW]);
	}

	// �΃R���e�i
	for (int i = 0; i < g_GContainer.size(); i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GContainer[i].scl.x, g_GContainer[i].scl.y, g_GContainer[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GContainer[i].rot.x, g_GContainer[i].rot.y + XM_PI, g_GContainer[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GContainer[i].pos.x, g_GContainer[i].pos.y, g_GContainer[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_model[COLOR_GREEN]);
	}

	// �R���C�_�[
	for (int i = 0; i < g_collider.size(); i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_collider[i].scl.x, g_collider[i].scl.y, g_collider[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_collider[i].rot.x, g_collider[i].rot.y + XM_PI, g_collider[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_collider[i].pos.x, g_collider[i].pos.y, g_collider[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

#ifdef _DEBUG
		// ���f���`��
		DrawModelWireFrame(&g_colliderModel);
#endif

	}


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

std::vector<OBSTACLE_DATA> *GetCollider(void)
{
	return &g_collider;
}

// �v���C���[����Q���ɉB��Ă��Ȃ������肷��
void HiddenCheckView(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	// ����p�ɓ����Ă�����
	if (boss[0].ViewingAngle == TRUE)
	{
		for (int c = 0; c < g_collider.size(); c++)
		{
			if (isHit == TRUE)
			{
				break;
			}

			XMFLOAT3 pos = g_collider[c].pos;
			XMFLOAT3 scl = g_collider[c].scl;

			// DirectX��Maya�̃X�P�[������ϊ�
			float scale = COLLIDER_SCL;
			scl.x *= scale;
			scl.y *= scale;
			scl.z *= scale;
			scl.x /= 2.0f;
			scl.y /= 2.0f;
			scl.z /= 2.0f;

			// boss����player�ɑ΂��ė���΂��A�R���W�����ƗႪ�������Ă��邩���肷��
			for (int i = 0; i < colliderPolygons.size(); i++)
			{
				XMFLOAT3 hit;
				XMFLOAT3 normal;

				XMFLOAT3 polygonPosArray[3];

				for (int p = 0; p < 3; p++)
				{
					polygonPosArray[p] = colliderPolygons[i].vertex[p].Position;

					polygonPosArray[p].x /= 5;
					polygonPosArray[p].y /= 5;
					polygonPosArray[p].z /= 5;

					polygonPosArray[p].x *= scl.x;
					polygonPosArray[p].y *= scl.y;
					polygonPosArray[p].z *= scl.z;

					polygonPosArray[p].x += pos.x;
					polygonPosArray[p].y += pos.y;
					polygonPosArray[p].z += pos.z;
				}

				XMFLOAT3 BossPos = boss[0].pos;
				XMFLOAT3 PlayerPos = player[0].pos;

				float yAve = (polygonPosArray[0].y + polygonPosArray[1].y + polygonPosArray[2].y) / 3.0f;
				BossPos.y = yAve;
				PlayerPos.y = yAve;
				isHit = RayCast(polygonPosArray[0], polygonPosArray[1], polygonPosArray[2],
					BossPos, PlayerPos, &hit, &normal);

				// ���C���������Ă�����I���
				if (isHit == TRUE)
				{
					break;
				}
			}
		}

		// ����p�ɓ����Ă��Ă��A���C�ɂ��������Ă���
		if (isHit == TRUE)
		{
			// �v���C���[���B��Ă���
			boss[0].view = FALSE;
			isHit = FALSE;
		}
		else
		{
			// ����ɓ����Ă���
			boss[0].view = TRUE;

		}
	}

}

// �J�����ƃv���C���[�̊Ԃɏ�Q�����������甼�����ɂ���
void HiddenCheckTranslucent(void)
{
	PLAYER* player = GetPlayer();
	CAMERA* camera = GetCamera();

	for (int c = 0; c < g_collider.size(); c++)
	{
		if (isTranslucent == TRUE)
		{
			break;
		}

		XMFLOAT3 pos = g_collider[c].pos;
		XMFLOAT3 scl = g_collider[c].scl;

		// DirectX��Maya�̃X�P�[������ϊ�
		float scale = COLLIDER_SCL;
		scl.x *= scale;
		scl.y *= scale;
		scl.z *= scale;
		scl.x /= 2.0f;
		scl.y /= 2.0f;
		scl.z /= 2.0f;

		// camera����player�ɑ΂��ė���΂��A�R���W�����ƗႪ�������Ă��邩���肷��
		for (int i = 0; i < colliderPolygons.size(); i++)
		{
			XMFLOAT3 hit;
			XMFLOAT3 normal;

			XMFLOAT3 polygonPosArray[3];

			for (int p = 0; p < 3; p++)
			{
				polygonPosArray[p] = colliderPolygons[i].vertex[p].Position;

				polygonPosArray[p].x /= 5;
				polygonPosArray[p].y /= 5;
				polygonPosArray[p].z /= 5;

				polygonPosArray[p].x *= scl.x;
				polygonPosArray[p].y *= scl.y;
				polygonPosArray[p].z *= scl.z;

				polygonPosArray[p].x += pos.x;
				polygonPosArray[p].y += pos.y;
				polygonPosArray[p].z += pos.z;
			}

			XMFLOAT3 CameraPos = camera[0].pos;
			XMFLOAT3 PlayerPos = player[0].pos;

			float yAve = (polygonPosArray[0].y + polygonPosArray[1].y + polygonPosArray[2].y) / 3.0f;
			CameraPos.y = yAve;
			PlayerPos.y = yAve;

			// ���C�L���X�g
			isTranslucent = RayCast(polygonPosArray[0], polygonPosArray[1], polygonPosArray[2],
				CameraPos, PlayerPos, &hit, &normal);

			// ���C���������Ă�����
			if (isTranslucent == TRUE)
			{
				break;
			}

		}

		// ���C���������Ă�����
		if (isTranslucent == TRUE)
		{
			for (int j = 0; j < g_model[COLOR_YELLOW].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_YELLOW], j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));
			}

			for (int j = 0; j < g_model[COLOR_GREEN].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_GREEN], j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));
			}

		}
		else
		{
			for (int j = 0; j < g_model[COLOR_YELLOW].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_YELLOW], j, diffuse[0]);
			}

			for (int j = 0; j < g_model[COLOR_GREEN].SubsetNum; j++)
			{
				SetModelDiffuse(&g_model[COLOR_GREEN], j, diffuse[0]);
			}
		}

	}
}