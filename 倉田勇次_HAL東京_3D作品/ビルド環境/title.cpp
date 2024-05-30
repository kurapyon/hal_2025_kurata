//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GP11A132 09 �q�c�E��
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "sound.h"
#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y(960)
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y(540)
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			 

// �J�[�\���̈ʒu
#define TITLE_CUR_1_X				(20.0f)
#define TITLE_CUR_1_Y				(350.0f)

#define TITLE_CUR_2_X				(420.0f)
#define TITLE_CUR_2_Y				(350.0f)

#define TITLE_CUR_3_X				(680.0f)
#define TITLE_CUR_3_Y				(350.0f)

// �J�[�\���̕��ƍ���
#define CURSOR_WIDTH				(40)
#define CURSOR_HEIGHT				(40)

// �����x�J�E���g
#define TEXTURE_TITLE_ACTION	(60.0f)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum 
{
	title,
	cursor_title,
	button,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/cursor.png",
	"data/TEXTURE/button.png",
};

XMFLOAT3					g_Pos;					// �|���S���̍��W
XMFLOAT3					g_bPos;					// �|���S���̍��W

BOOL						g_Use;					// TRUE:�g���Ă���  FALSE:���g�p
float						g_w, g_h;				// ���ƍ���
int							g_TexNo;				// �e�N�X�`���ԍ�
static BOOL					g_Load = FALSE;			// �Q�[�����[�h

static float				g_TitleOpacity;			// �����x
static float				g_TitleOpacityCnt;		// �����x�J�E���^�[

// �J�[�\���̕ϐ�
int cursor;

// ���X�ɕ����яオ����ϐ�
float g_scroll;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// ������
	g_scroll = 0.0f;

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TexNo = 0;
	g_bPos = XMFLOAT3(TEXTURE_WIDTH / 2.25f, TEXTURE_HEIGHT / 1.1f, 0.0f);
	g_TitleOpacity = 1.0f;			// �����x
	g_TitleOpacityCnt = 0.0f;		// �����x�J�E���^�[
	cursor = 0;						// �J�[�\���̕ϐ�

	// ���ʉ�
	PlaySound(SOUND_LABEL_BGM_TITLE);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitle(void)
{
	// �^�C�g����ʂ����X�ɕ����яオ�点��
	g_scroll += 0.005f;
	g_TitleOpacityCnt++;

	// �J�[�\���̈ړ�����(�㉺�̈ړ��̈ړ�)
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, R_STICK_RIGHT) || IsButtonTriggered(0, STICK_RIGHT))
	{
		cursor = (++cursor + TITLE_MAX) % TITLE_MAX;
	}
	if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, R_STICK_LEFT) || IsButtonTriggered(0, STICK_LEFT))
	{
		cursor = (--cursor + TITLE_MAX) % TITLE_MAX;
	}

	switch (cursor)
	{
	case TITLE_GAMESTART:		// �Q�[���X�^�[�g
		g_Pos = XMFLOAT3(TITLE_CUR_1_X, TITLE_CUR_1_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			SetFadeMode(FADE_OUT, MODE_GAME);
		}
		break;

	case TITLE_TUTORIALE:		// �`���[�g���A��
		g_Pos = XMFLOAT3(TITLE_CUR_2_X, TITLE_CUR_2_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			SetFadeMode(FADE_OUT, MODE_TUTORIAL);
		}
		break;

	case TITLE_EXIT:		// �Q�[���I��
		g_Pos = XMFLOAT3(TITLE_CUR_3_X, TITLE_CUR_3_Y, 0.0f);
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			GameLoop();
		}

		break;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_scroll);
	SetMaterial(material);

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[title]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �J�[�\���̕\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[cursor_title]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, g_Pos.x, g_Pos.y, CURSOR_WIDTH, CURSOR_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	// ���̈ē���`��
	{
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_bPos.x;			// �Q�[�W�̕\���ʒuX
		float py = g_bPos.y;			// �Q�[�W�̕\���ʒuY
		float pw = TEXTURE_WIDTH / 2;	// �Q�[�W�̕\����
		float ph = TEXTURE_HEIGHT / 4;	// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float rot = 0.0f;
		float angle = (XM_PI / TEXTURE_TITLE_ACTION) * g_TitleOpacityCnt;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[button]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_TitleOpacity * fabsf(sinf(XM_PI / 2 + angle))), rot);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}





