//=============================================================================
//
// ��Q������ [obstacle.cpp]
// Author : �q�c�E��
//
//=============================================================================
#include "main.h"
#include "obstacle.h"
#include "model.h"

//=============================================================================
// �t�B�[���h�f�[�^�̏����� ��CSV
//=============================================================================
void LoadFieldDataCSV(char* FileName, std::vector<OBSTACLE_DATA>&tbl)
{
	FILE* file;
	char line[256];
	float tx, ty, tz, rx, ry, rz, sx, sy, sz;

	// �t�@�C�����J��
	file = fopen(FileName, "r");
	if (file == NULL) {
		perror("�t�@�C���̃I�[�v���Ɏ��s���܂���");
		return;
	}

	// �e�[�u���̃N���A
	tbl.clear();

	// �t�@�C����ǂݍ���ŏ���������
	fgets(line, sizeof(line), file);

	while (fgets(line, sizeof(line), file) != NULL)
	{
		(void)sscanf(line, "%f,%f,%f,%f,%f,%f,%f,%f,%f", &tx, &ty, &tz, &rx, &ry, &rz, &sx, &sy, &sz);

		tbl.push_back(
			{
			XMFLOAT3(tx * 10.0f, ty * 10.0f, -tz * 10.0f),
			XMFLOAT3(XMConvertToRadians(rx), XMConvertToRadians(-ry), XMConvertToRadians(rz)),
			XMFLOAT3(sx, sy, sz),
			}
		);
	}

	// �t�@�C�������
	fclose(file);
}