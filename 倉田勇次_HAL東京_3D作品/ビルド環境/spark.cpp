//=============================================================================
//
// �r���{�[�h���� [tree.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"

#include "camera.h"
#include "player.h"
#include "spark.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SPARK_TEXTURE_MAX	(1)			// �e�N�X�`���̐�
#define	MAX_SPARK			(256)		// ���ő吔
#define	SPARK_WIDTH			(50.0f)		// ���_�T�C�Y
#define	SPARK_HEIGHT		(50.0f)		// ���_�T�C�Y

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSpark(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[SPARK_TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static SPARK	g_Spark[MAX_SPARK];	// �Ήԃ��[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static int					g_TexNo;			// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/spark.png",
};

enum texno
{
	spark = 0,
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSpark(void)
{
	MakeVertexSpark();

	// �e�N�X�`������
	for (int i = 0; i < SPARK_TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// �Ήԃ��[�N�̏�����
	for(int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{
		ZeroMemory(&g_Spark[nCntSpark].material, sizeof(g_Spark[nCntSpark].material));
		g_Spark[nCntSpark].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Spark[nCntSpark].pos	    = {0.0f, 0.0f, 0.0f};
		g_Spark[nCntSpark].scl	    = {1.0f, 1.0f, 1.0f};
		g_Spark[nCntSpark].fWidth   = SPARK_WIDTH;
		g_Spark[nCntSpark].fHeight  = SPARK_HEIGHT;
		g_Spark[nCntSpark].sparkcnt = 0;
		g_Spark[nCntSpark].use	    = FALSE;

	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSpark(void)
{
	for(int nCntTex = 0; nCntTex < SPARK_TEXTURE_MAX; nCntTex++)
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
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSpark(void)
{
	PLAYER* player = GetPlayer();

	for (int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{	
		// �o�����Ԃ𑫂�
		g_Spark[nCntSpark].sparkcnt++;

		// ��莞�Ԍo�߂����珉��������
		if (g_Spark[nCntSpark].sparkcnt > SPARK_TIME)
		{
			// ������
			g_Spark[nCntSpark].sparkcnt = 0;
			g_Spark[nCntSpark].use		= FALSE;
			player[0].attack			= FALSE;
		}
	}

#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSpark(void)
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

	for(int i = 0; i < SPARK_TEXTURE_MAX; i++)
	{
		if(g_Spark[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


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
			mtxScl = XMMatrixScaling(g_Spark[i].scl.x * SPARK_SCL, g_Spark[i].scl.y * SPARK_SCL, g_Spark[i].scl.z * SPARK_SCL);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Spark[i].pos.x, g_Spark[i].pos.y, g_Spark[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Spark[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[spark]);

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
HRESULT MakeVertexSpark(void)
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

	float fWidth = 60.0f;
	float fHeight = 60.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �ΉԂ̃p�����[�^���Z�b�g
//=============================================================================
int SetSpark(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxSPark = -1;

	for(int nCntSpark = 0; nCntSpark < SPARK_TEXTURE_MAX; nCntSpark++)
	{
		if(!g_Spark[nCntSpark].use)
		{
			g_Spark[nCntSpark].pos = pos;
			g_Spark[nCntSpark].scl = XMFLOAT3(SPARK_SCL, SPARK_SCL, SPARK_SCL);
			g_Spark[nCntSpark].fWidth = fWidth;
			g_Spark[nCntSpark].fHeight = fHeight;
			g_Spark[nCntSpark].use = TRUE;

			nIdxSPark = nCntSpark;

			break;
		}
	}
	return nIdxSPark;
}

SPARK* GetSpark(void)
{
	return &g_Spark[0];
}