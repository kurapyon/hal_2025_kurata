//=============================================================================
//
// 障害物処理 [obstacle.cpp]
// Author : 倉田勇次
//
//=============================================================================
#include "main.h"
#include "obstacle.h"
#include "model.h"

//=============================================================================
// フィールドデータの初期化 ※CSV
//=============================================================================
void LoadFieldDataCSV(char* FileName, std::vector<OBSTACLE_DATA>&tbl)
{
	FILE* file;
	char line[256];
	float tx, ty, tz, rx, ry, rz, sx, sy, sz;

	// ファイルを開く
	file = fopen(FileName, "r");
	if (file == NULL) {
		perror("ファイルのオープンに失敗しました");
		return;
	}

	// テーブルのクリア
	tbl.clear();

	// ファイルを読み込んで初期化する
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

	// ファイルを閉じる
	fclose(file);
}