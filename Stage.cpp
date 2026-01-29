#include "Stage.h"
#include "Engine/Model.h"
#include<vector>
#include "resource.h"
#include "Engine/Input.h"
#include "Engine/Direct3D.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), mode_(0), select_(0), pConstantBuffer_(nullptr)
{
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 15; x++)
		{
			SetBlock(x, y, TYPE_DEFAULT, 0);
		}
	}
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	//std::vector<string> modelNames = {
	//	"GlassCube.fbx",
	//	"GreenCube.fbx",
	//	"SandCube.fbx",
	//	"WaterCube.fbx",
	//	"WoodenCube.fbx",
	//	"StoneCube.fbx",
	//	"BrickCube.fbx"
	//};
	//for (int i = 0; i < modelNames.size(); i++)
	//{
	//	hModels[i] = Model::Load(modelNames[i]);
	//	assert(hModels[i] >= 0);
	//}

	std::string modelName = "Donut.fbx";
	hModel = Model::Load(modelName);
}

void Stage::Update()
{
	//transform_.rotate_.y += 2.0f;
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 15; x++)
		{
			sTable[y][x].type = (BROCKTYPE)select_;
		}
	}
}

void Stage::Draw()
{
	//Transform position;
	//for (int y = 0; y < 15; y++)
	//{
	//	for (int x = 0; x < 15; x++)
	//	{
	//		int DrawModel = hModels[sTable[y][x].type];
	//		position.position_.x = x * BLOCK_SIZE;
	//		position.position_.y = sTable[y][x].height * BLOCK_SIZE;
	//		position.position_.z = y * BLOCK_SIZE;
	//		Model::SetTransform(DrawModel, position);
	//		Model::Draw(DrawModel);
	//	}
	//}

	//コンスタントバッファの更新
	Direct3D::pContext->VSSetConstantBuffers(1, 1, &pConstantBuffer_);//頂点シェーダー用
	Direct3D::pContext->PSSetConstantBuffers(1, 1, &pConstantBuffer_);//ピクセルシェーダー用

	 
	Transform t;
	t.position_.x = 0.0f;
	t.position_.y = 0.0f;
	t.position_.z = 0.0f;
	t.scale_ = { 0.95,0.95,0.95 };

	Model::Draw(hModel);
	RayCastData rayData{
		{0.0f,0.0f,5.0f,0.0f},//startPos
		{0.0f,-1.0f,0.0f,0.0f},//dir
		false,//isHit
		0.0f//dist
	};
	Model::RayCast(hModel, rayData);
	if (rayData.isHit)
	{
		MessageBoxA(NULL, "Hit!", "RayCast", MB_OK);
	}

	ImGui::Begin("Stage Manu");
	ImGui::Text("Modelhandle:",hModel);
	ImGui::End();

}

void Stage::Release()
{
}

BOOL Stage::localProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

BOOL Stage::manuProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"default");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"草地");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"砂地");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"水場");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"木材");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"石材");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_ADDSTRING, 0, (LPARAM)L"レンガ");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_SETCURSEL, 0, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO1:
			mode_ = 0;
			return TRUE;
		case IDC_RADIO2:
			mode_ = 1;
			return TRUE;
		case IDC_RADIO3:
			mode_ = 2;
			return TRUE;
		case IDC_COMBO1:
			select_ = (int)SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void Stage::InitConstantBuffer()
{
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANTBUFFER_STAGE);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	//コンスタントバッファの生成
	HRESULT hr;
	hr = Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
}
