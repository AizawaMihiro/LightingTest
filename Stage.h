#pragma once
#include "Engine/GameObject.h"
#include <Windows.h>

enum BROCKTYPE
{
	TYPE_DEFAULT, TYPE_GREEN, TYPE_SAND, TYPE_WATER, TYPE_WOOD, TYPE_STONE, TYPE_BRICK
};

struct sData
{
	BROCKTYPE type;
	int height;
};

namespace {
	const int STAGE_WIDTH = 15;
	const int STAGE_HEIGHT = 15;
	const int BLOCK_SIZE = 1;
}

class Stage :
    public GameObject
{
public:
	Stage(GameObject* parent);
	~Stage();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void SetBlock(int x, int y, BROCKTYPE type, int height) {
		sTable[y][x].type = type;
		sTable[y][x].height = height;
	}
	sData& GetBlock(int x, int y) {
		return sTable[y][x];
	}
	BOOL localProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL manuProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	sData sTable[STAGE_HEIGHT][STAGE_WIDTH];
	int hModels[7];
	int mode_;//0:上げ　1:下げ　2:種類変更
	int select_;//選択中のブロック種類
};

