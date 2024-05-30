//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "debugproc.h"
#include "renderer.h"
#include "camera.h"
#include "beambullet.h"
#include "particlebeam.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(1)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X			(40.0f)		// ���_�T�C�Y
#define	PARTICLE_SIZE_Y			(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_PARTICLE		(5.0f)		// �ړ����x
#define	MAX_PARTICLE			(3000)		// �p�[�e�B�N���ő吔
#define PARTICLEATC_SPEED		(10.0f)		// �p�[�e�B�N���̃X�s�[�h

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

} PARTICLEBEAM;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticleBeam(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;						// �e�N�X�`���ԍ�

static PARTICLEBEAM					g_ParticleBeam[MAX_PARTICLE];	// �p�[�e�B�N�����[�N
static XMFLOAT3						g_posBase;						// �r���{�[�h�����ʒu
static float						g_fWidthBase = 5.0f;			// ��̕�
static float						g_fHeightBase = 10.0f;			// ��̍���
static float						g_roty = 0.0f;					// �ړ�����
static float						g_spd = 0.0f;					// �ړ��X�s�[�h

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticleBeam(void)
{
	// ���_���̍쐬
	MakeVertexParticleBeam();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �p�[�e�B�N�����[�N�̏�����
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_ParticleBeam[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleBeam[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ParticleBeam[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_ParticleBeam[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_ParticleBeam[nCntParticle].material, sizeof(g_ParticleBeam[nCntParticle].material));
		g_ParticleBeam[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_ParticleBeam[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_ParticleBeam[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_ParticleBeam[nCntParticle].nLife = 0;
		g_ParticleBeam[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticleBeam(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticleBeam(void)
{
	BEAMBULLET* beambullet = GetBeamBullet();

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_ParticleBeam[nCntParticle].use)
		{// �g�p��

			// ���������������Ďc���������Ă���
			g_ParticleBeam[nCntParticle].scale.x *= PARTICLEATC_DECREASE;
			g_ParticleBeam[nCntParticle].scale.y *= PARTICLEATC_DECREASE;
			g_ParticleBeam[nCntParticle].scale.z *= PARTICLEATC_DECREASE;


			g_ParticleBeam[nCntParticle].nLife--;
			if (g_ParticleBeam[nCntParticle].nLife <= 0)
			{
				g_ParticleBeam[nCntParticle].use = FALSE;
			}

		}

	}

#ifdef _DEBUG

	PrintDebugProc("PARTICLE HP %d\n", g_ParticleBeam[0].nLife);

#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticleBeam(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	SetSepia(1);

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_ParticleBeam[nCntParticle].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_ParticleBeam[nCntParticle].scale.x, g_ParticleBeam[nCntParticle].scale.y, g_ParticleBeam[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_ParticleBeam[nCntParticle].pos.x, g_ParticleBeam[nCntParticle].pos.y, g_ParticleBeam[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_ParticleBeam[nCntParticle].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �Z�s�A
	SetSepia(0);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticleBeam(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorParticleBeam(int nIdxParticle, XMFLOAT4 col)
{
	g_ParticleBeam[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetParticleBeam(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT3 rot, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_ParticleBeam[nCntParticle].use)
		{
			g_ParticleBeam[nCntParticle].pos = pos;
			g_ParticleBeam[nCntParticle].rot = rot;
			g_ParticleBeam[nCntParticle].scale = { PARTICLEATC_SCALE, PARTICLEATC_SCALE, PARTICLEATC_SCALE };
			g_ParticleBeam[nCntParticle].move = move;
			g_ParticleBeam[nCntParticle].material.Diffuse = col;
			g_ParticleBeam[nCntParticle].fSizeX = fSizeX;
			g_ParticleBeam[nCntParticle].fSizeY = fSizeY;
			g_ParticleBeam[nCntParticle].nLife = nLife;
			g_ParticleBeam[nCntParticle].use = TRUE;


			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}

