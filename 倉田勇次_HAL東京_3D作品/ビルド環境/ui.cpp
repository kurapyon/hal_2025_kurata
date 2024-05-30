//=============================================================================
//
// UI���� [ui.cpp]
// Author : �q�c�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "player.h"
#include "boss.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH			(25.0f)		// HP�̕\��TEX�T�C�Y
#define TEXTURE_HEIGHT			(25.0f)		// HP�̕\��TEX�T�C�Y
#define TEXTURE_RELOAD_HEIGHT	(300.0f)	// �����[�h�\���T�C�Y
#define TEXTURE_RELOAD_ACTION	(20.0f)		// �����[�h�\���J�E���g

#define AIM_SIZE				(20.0f)		// �N���X�w�A�T�C�Y
#define TEXTURE_MAX				(7)			// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void Player_Ui(void);
void Bullet_Ui(void);
void Reload_Ui(void);
void Aim_Ui(void);
void Boss_Ui(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	// �v���C���[��HP
	"data/TEXTURE/heart_main10.png",
	"data/TEXTURE/heartbase10.png",

	// �e�̎c�e��
	"data/TEXTURE/Tamaui.png",

	// �G�C���\��
	"data/TEXTURE/aim.png",

	// �����[�h�\��
	"data/TEXTURE/reload.png",

	// �{�X��HP
	"data/TEXTURE/hp_base.png",
	"data/TEXTURE/hp.png",


}; 

enum Tex
{
	heart_main10 = 0,
	heartbase10,
	tama_ui,
	aim_ui,
	reload,
	hp_base,
	hp,
};

static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static float					g_UiOpacity;				// �����x
static float					g_UiOpacityCnt;				// �����x�J�E���^�[
static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUi(void)
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

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// UI�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 300.0f, 10.0f, 0.0f };
	g_TexNo = 0;
	g_UiOpacity	 = 1.0f;
	g_UiOpacityCnt = 0.0f;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUi(void)
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
void UpdateUi(void)
{
	// �`�J�`�J������
	PLAYER* player = GetPlayer();
	if (player->reload == 0)
	{
		g_UiOpacityCnt++;
	}
	else
	{
		g_UiOpacityCnt = 0.0f;

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUi(void)
{
	PLAYER* player = GetPlayer();

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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	if (player->use == TRUE)
	{
		// �v���C���[��HP
		Player_Ui();

		// �c�e��
		Bullet_Ui();

		// �����[�h�\��
		Reload_Ui();

		// �G�C���\��
		Aim_Ui();
	}

	// �{�X��HP
	Boss_Ui();
}

// �v���C���[UI
void Player_Ui(void)
{
	PLAYER* player = GetPlayer();
	// ���~���̃Q�[�W�i�g�I�ȕ��j
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heartbase10]);

	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 10.0f;	// �Q�[�W�̕\���ʒuX
	float py = 10.0f;	// �Q�[�W�̕\���ʒuY
	float pw = 300.0f;	// �Q�[�W�̕\����
	float ph = 30.0f;	// �Q�[�W�̕\������

	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// ���C�t
	//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
	px = 10.0f;	// �Q�[�W�̕\���ʒuX
	py = 10.0f;		// �Q�[�W�̕\���ʒuY
	pw = 300.0f;	// �Q�[�W�̕\����
	ph = 30.0f;		// �Q�[�W�̕\������

	tw = 1.0f;		// �e�N�X�`���̕�
	th = 1.0f;		// �e�N�X�`���̍���
	tx = 0.0f;		// �e�N�X�`���̍���X���W
	ty = 0.0f;		// �e�N�X�`���̍���Y���W

	pw = pw * (float)player[0].hp / PLAYER_HP;
	tw = tw * (float)player[0].hp / PLAYER_HP;

	//pw_log = (float)(g_Player[0].life  / MAX_HP);
	//tw_log = tw;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[heart_main10]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLeftTop(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

// �e�̎c�萔
void Bullet_Ui(void)
{
	PLAYER* player = GetPlayer();

	for (int i = 0; i < player->reload; i++)
	{
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 50.0f;						// �Q�[�W�̕\���ʒuX
		float py = 300.0f - TEXTURE_HEIGHT * i;	// �Q�[�W�̕\���ʒuY
		float pw = 50.0f;						// �Q�[�W�̕\����
		float ph = TEXTURE_HEIGHT;				// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tama_ui]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

// �����[�hUI
void Reload_Ui(void)
{
	PLAYER* player = GetPlayer();

	if (player->reload == 0)
	{
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH / 2;		// �Q�[�W�̕\���ʒuX
		float py = SCREEN_HEIGHT / 2;		// �Q�[�W�̕\���ʒuY
		float pw = 600.0f;					// �Q�[�W�̕\����
		float ph = TEXTURE_RELOAD_HEIGHT;	// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float rot = 0.0f;
		float angle = (XM_PI / TEXTURE_RELOAD_ACTION) * g_UiOpacityCnt;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[reload]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_UiOpacity * fabsf(sinf(XM_PI / 2 + angle))), rot);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

// �G�C��UI
void Aim_Ui(void)
{
	PLAYER* player = GetPlayer();

	if (player->stance == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[aim_ui]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			SCREEN_CENTER_X, SCREEN_CENTER_Y,
			AIM_SIZE * 2, AIM_SIZE * 2,
			0.0f, 0.0f, 1.0, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

// �{�XUI
void Boss_Ui(void)
{
	BOSS* boss = GetBoss();

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (boss->use == TRUE)
	{
		// ���C�t
		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = 650.0f;	// �Q�[�W�̕\���ʒuX
		float py = 17.0f;	// �Q�[�W�̕\���ʒuY
		float pw = 250.0f;	// �Q�[�W�̕\����
		float ph = 10.0f;	// �Q�[�W�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		pw = pw * (float)boss->hp / BOSS_HP;
		tw = tw * (float)boss->hp / BOSS_HP;

		//pw_log = (float)(g_Player[0].life  / MAX_HP);
		//tw_log = tw;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[hp]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// ���~���̃Q�[�W�i�g�I�ȕ��j
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[hp_base]);

		//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		px = 630.0f;// �Q�[�W�̕\���ʒuX
		py = 15.0f;	// �Q�[�W�̕\���ʒuY
		pw = 300.0f;// �Q�[�W�̕\����
		ph = 18.0f;	// �Q�[�W�̕\������

		tw = 1.0f;	// �e�N�X�`���̕�
		th = 1.0f;	// �e�N�X�`���̍���
		tx = 0.0f;	// �e�N�X�`���̍���X���W
		ty = 0.0f;	// �e�N�X�`���̍���Y���W

	  // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

}