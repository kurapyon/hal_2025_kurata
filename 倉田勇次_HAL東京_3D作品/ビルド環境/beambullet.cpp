//=============================================================================
//
// �r�[������ [beambullet.cpp]
// Author : �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "beambullet.h"
#include "particlebeam.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	BEAMBULLET_WIDTH	(10.0f)			// ���_�T�C�Y
#define	BEAMBULLET_HEIGHT	(10.0f)			// ���_�T�C�Y
#define	BEAMBULLET_SPEED	(10.0f)			// �e�̈ړ��X�s�[�h

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBeamBullet(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BEAMBULLET					g_beambullet[BEAMBULLET_MAX_BULLET];	// �e���[�N
static int							g_TexNo;							// �e�N�X�`���ԍ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBeamBullet(void)
{
	MakeVertexBeamBullet();

	// �e���[�N�̏�����
	for (int nCntBullet = 0; nCntBullet < BEAMBULLET_MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_beambullet[nCntBullet].material, sizeof(g_beambullet[nCntBullet].material));
		g_beambullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_beambullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_beambullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_beambullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_beambullet[nCntBullet].spd = { BEAMBULLET_SPEED, BEAMBULLET_SPEED, BEAMBULLET_SPEED };
		g_beambullet[nCntBullet].fWidth = BEAMBULLET_WIDTH;
		g_beambullet[nCntBullet].fHeight = BEAMBULLET_HEIGHT;
		g_beambullet[nCntBullet].blife = BEAMBULLET_LIFE;
		g_beambullet[nCntBullet].use = FALSE;

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBeamBullet(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBeamBullet(void)
{
	
	for (int i = 0; i < BEAMBULLET_MAX_BULLET; i++)
	{
		if (g_beambullet[i].use)
		{
			// �e�̈ړ�����
			g_beambullet[i].pos.x -= sinf(g_beambullet[i].rot.y) * g_beambullet[i].spd.x;
			g_beambullet[i].pos.z -= cosf(g_beambullet[i].rot.y) * g_beambullet[i].spd.z;

			// �p�[�e�B�N����t����
			XMFLOAT4 col  = { 1.0f, 0.0f, 1.0f, 1.0f };
			XMFLOAT3 move = { 0.0f, 0.0f, 0.0f};
			XMFLOAT3 rot  = g_beambullet[i].rot;

			SetParticleBeam(g_beambullet[i].pos,
				move,
				rot,
				col,
				g_beambullet[i].fWidth,
				g_beambullet[i].fHeight,
				PARTICLEATC_LIFE);


			// �t�B�[���h�̊O�ɏo��܂��́A���C�t���Ԃ�0�ɂȂ�����e����������
			g_beambullet[i].blife--;
			if (g_beambullet[i].pos.x < MAP_LEFT
				|| g_beambullet[i].pos.x > MAP_RIGHT
				|| g_beambullet[i].pos.y < MAP_DOWN
				|| g_beambullet[i].pos.z < MAP_DOWN
				|| g_beambullet[i].pos.z > MAP_TOP
				|| g_beambullet[i].blife <= 0)
			{
				g_beambullet[i].use = FALSE;
				g_beambullet[i].blife = BEAMBULLET_LIFE;
			}

//#ifdef _DEBUG
//			// �f�o�b�O�\��
//			PrintDebugProc("Bullet rotX: %f\n", g_beambullet[i].rot.x);
//#endif
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBeamBullet(void)
{
	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < BEAMBULLET_MAX_BULLET; i++)
	{
		if (g_beambullet[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_beambullet[i].scl.x, g_beambullet[i].scl.y, g_beambullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_beambullet[i].rot.x, g_beambullet[i].rot.y + XM_PI, g_beambullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_beambullet[i].pos.x, g_beambullet[i].pos.y, g_beambullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_beambullet[i].mtxWorld, mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_beambullet[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexBeamBullet(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = BEAMBULLET_WIDTH;
	float fHeight = BEAMBULLET_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = {  fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = {  fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
//=============================================================================
int SetBeamBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{

	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < BEAMBULLET_MAX_BULLET; nCntBullet++)
	{
		if (!g_beambullet[nCntBullet].use)
		{
			g_beambullet[nCntBullet].pos = pos;
			g_beambullet[nCntBullet].rot = rot;
			g_beambullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_beambullet[nCntBullet].use = TRUE;

			nIdxBullet = nCntBullet;

		//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

BEAMBULLET* GetBeamBullet(void)
{
	return &(g_beambullet[0]);
}
