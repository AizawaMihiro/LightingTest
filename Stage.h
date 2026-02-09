#pragma once
#include "Engine/GameObject.h"
#include <Windows.h>
#include <d3d11.h>

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

struct CONSTANTBUFFER_STAGE
{
	XMFLOAT4 lightPos;//ライトの位置
	XMFLOAT4 eyePos;//視点の位置
};

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
	BOOL localProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL manuProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int mode_;//0:上げ　1:下げ　2:種類変更
	int select_;//選択中のブロック種類

	ID3D11Buffer* pConstantBuffer_;
	void InitConstantBuffer();
	int hRoom_;
	int hDonut_;
	int hBall_;
	int hFloor_;
};

