//=============================================================================
//
// �G�t�F�N�g(��)���� [fire.cpp]
// Author :�q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "fire.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

// ���G�t�F�N�g
#define	FIRE_WIDTH			(24.0f)		// ���_�T�C�Y
#define	FIRE_HEIGHT			(32.0f)		// ���_�T�C�Y
#define FIRE_POSX			(940.0f)	// ����u��x���W�̊�l
#define FIRE_POSZ			(800.0f)	// ����u��z���W�̊�l
#define FIRE_INTERVAL		(130.0f)	// ����u���Ԋu
#define FIRE_NUM			(28)		// 1��ɉ���u����
#define FIRE_TEX_DEVX		(8)			// x�������̃e�N�X�`��������
#define FIRE_TEX_DEVY		(1)			// y�������̃e�N�X�`��������
#define	MAX_FIRE			(60)		// ���ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	float		animCnt;		// �A�j���[�V�����J�E���g
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} FIRE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexFIre(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static VERTEX_3D* g_Vertex = NULL;
static FIRE					g_fire[MAX_FIRE];	// �G�t�F�N�g���[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/fire.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFire(void)
{
	g_Vertex = new VERTEX_3D[4];
	MakeVertexFIre();

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

	// �G�t�F�N�g�̏�����
	for(int i = 0; i < MAX_FIRE; i++)
	{
		ZeroMemory(&g_fire[i].material, sizeof(g_fire[i].material));
		g_fire[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_fire[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_fire[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_fire[i].fWidth = FIRE_WIDTH;
		g_fire[i].fHeight = FIRE_HEIGHT;
		g_fire[i].use = FALSE;
	}

	g_bAlpaTest = TRUE;

	// ���̐ݒ�
	for (int i = 0; i < FIRE_NUM / 2; i++)
	{
		// �E�̗�
		SetFire(XMFLOAT3(FIRE_POSX, 0.0f, FIRE_POSZ - FIRE_INTERVAL * i), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ���̗�
		SetFire(XMFLOAT3(-FIRE_POSX, 0.0f, FIRE_POSZ - FIRE_INTERVAL * i), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	for (int i = 0; i < FIRE_NUM / 2; i++)
	{
		// ��̗�
		SetFire(XMFLOAT3(-(FIRE_POSZ / 2) + FIRE_INTERVAL * i, 0.0f, FIRE_POSZ), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ���̗�
		SetFire(XMFLOAT3(-(FIRE_POSZ / 2) + FIRE_INTERVAL * i, 0.0f, -FIRE_POSZ), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFire(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateFire(void)
{
	for (int i = 0; i < MAX_FIRE; i++)
	{
		if (g_fire[i].use)
		{
			float tw = 1.0f / FIRE_TEX_DEVX;
			float th = 1.0f / FIRE_TEX_DEVY;
			float tx = ((int)(g_fire[i].animCnt) % FIRE_TEX_DEVX) * tw;	// �e�N�X�`���̍���X���W
			float ty = ((int)(g_fire[i].animCnt) / FIRE_TEX_DEVX) * th;	// �e�N�X�`���̍���Y���W

			// ���_�����X�V
			g_Vertex[0].TexCoord = XMFLOAT2(tx, ty);
			g_Vertex[1].TexCoord = XMFLOAT2(tx + tw, ty);
			g_Vertex[2].TexCoord = XMFLOAT2(tx, ty + th);
			g_Vertex[3].TexCoord = XMFLOAT2(tx + tw, ty + th);

			// ���_�o�b�t�@�ɒl���Z�b�g����
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			// �S���_�����㏑��
			memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);

			GetDeviceContext()->Unmap(g_VertexBuffer, 0);

			// �A�j���[�V������i�߂�
			g_fire[i].animCnt += 0.3f;

			if ((int)(g_fire[i].animCnt) > FIRE_TEX_DEVX * FIRE_TEX_DEVY)
			{	// �A�j���[�V�������I��������
				g_fire[i].animCnt = 0.0f;			// ���[�v������
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFire(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE) SetAlphaTestEnable(TRUE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_FIRE; i++)
	{
		if(g_fire[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
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
			mtxScl = XMMatrixScaling(g_fire[i].scl.x, g_fire[i].scl.y, g_fire[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_fire[i].pos.x, g_fire[i].pos.y, g_fire[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_fire[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexFIre(void)
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

	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	float fWidth = FIRE_WIDTH;
	float fHeight = FIRE_HEIGHT;

	// ���_���W�̐ݒ�
	g_Vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	g_Vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	g_Vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	g_Vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	g_Vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	g_Vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	g_Vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	g_Vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// ���̃A�j���[�V�����G�t�F�N�g
//=============================================================================
int SetFire(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdx = -1;

	for (int nIdx = 0; nIdx < MAX_FIRE; nIdx++)
	{
		if (!g_fire[nIdx].use)
		{
			g_fire[nIdx].pos = pos;
			g_fire[nIdx].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_fire[nIdx].fWidth = fWidth;
			g_fire[nIdx].fHeight = fHeight;
			g_fire[nIdx].use = TRUE;

			nIdx = nIdx;

			break;
		}
	}

	return nIdx;
}
