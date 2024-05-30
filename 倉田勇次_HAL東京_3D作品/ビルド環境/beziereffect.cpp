//=============================================================================
//
// �G�t�F�N�g(��)���� [fire.cpp]
// Author :�q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "beziereffect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

// �x�W�F�U���̃G�t�F�N�g
#define BEZIER_TEX_DEVX		(5)			// x�������̃e�N�X�`��������
#define BEZIER_TEX_DEVY		(3)			// y�������̃e�N�X�`��������
#define	MAX_BEZIER			(2)			// �ő吔

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

} BEZIER;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBezier(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static VERTEX_3D* g_Vertex = NULL;
static BEZIER				g_beziereffect[MAX_BEZIER];	// �G�t�F�N�g���[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/electric_ripples.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBezier(void)
{
	g_Vertex = new VERTEX_3D[4];
	MakeVertexBezier();

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
	for(int i = 0; i < MAX_BEZIER; i++)
	{
		ZeroMemory(&g_beziereffect[i].material, sizeof(g_beziereffect[i].material));
		g_beziereffect[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_beziereffect[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_beziereffect[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_beziereffect[i].fWidth = BEZIER_WIDTH;
		g_beziereffect[i].fHeight = BEZIER_HEIGHT;
		g_beziereffect[i].use = FALSE;
	}

	g_bAlpaTest = TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBezier(void)
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
void UpdateBezier(void)
{
	for (int i = 0; i < MAX_BEZIER; i++)
	{
		if (g_beziereffect[i].use)
		{
			float tw = 1.0f / BEZIER_TEX_DEVX;
			float th = 1.0f / BEZIER_TEX_DEVY;
			float tx = ((int)(g_beziereffect[i].animCnt) % BEZIER_TEX_DEVX) * tw;	// �e�N�X�`���̍���X���W
			float ty = ((int)(g_beziereffect[i].animCnt) / BEZIER_TEX_DEVX) * th;	// �e�N�X�`���̍���Y���W

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
			g_beziereffect[i].animCnt += 0.3f;

			if ((int)(g_beziereffect[i].animCnt) > BEZIER_TEX_DEVX * BEZIER_TEX_DEVY)
			{	// �A�j���[�V�������I�������珉����
				g_beziereffect[i].use = FALSE;
				g_beziereffect[i].animCnt = 0.0f;

			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBezier(void)
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

	for(int i = 0; i < MAX_BEZIER; i++)
	{
		if(g_beziereffect[i].use)
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
			mtxScl = XMMatrixScaling(g_beziereffect[i].scl.x, g_beziereffect[i].scl.y, g_beziereffect[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_beziereffect[i].pos.x, g_beziereffect[i].pos.y, g_beziereffect[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_beziereffect[i].material);

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
HRESULT MakeVertexBezier(void)
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

	float fWidth = BEZIER_WIDTH;
	float fHeight = BEZIER_HEIGHT;

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
// �A�j���[�V�����G�t�F�N�g
//=============================================================================
int SetBezier(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdx = -1;

	for (int nIdx = 0; nIdx < MAX_BEZIER; nIdx++)
	{
		if (!g_beziereffect[nIdx].use)
		{
			g_beziereffect[nIdx].pos = pos;
			g_beziereffect[nIdx].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_beziereffect[nIdx].fWidth = fWidth;
			g_beziereffect[nIdx].fHeight = fHeight;
			g_beziereffect[nIdx].use = TRUE;

			nIdx = nIdx;

			break;
		}
	}

	return nIdx;
}
