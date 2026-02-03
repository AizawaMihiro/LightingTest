#include "Stage.h"
#include "Engine/Model.h"
#include<vector>
#include "resource.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "Engine/Direct3D.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"), mode_(0), select_(0), pConstantBuffer_(nullptr)
{

	hRoom_ = -1;
	hDonut_ = -1;
	hBall_ = -1;
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	InitConstantBuffer();
	hRoom_ = Model::Load("Room.fbx");
	assert(hRoom_ >= 0);
	hDonut_ = Model::Load("Donut.fbx");
	assert(hDonut_ >= 0);
	hBall_ = Model::Load("Bulet.fbx");
	assert(hBall_ >= 0);

	Camera::SetPosition(XMVectorSet(0.0f, 1.0f, -3.0f, 0.0f));
	Camera::SetTarget(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Stage::Update()
{
	transform_.rotate_.y += 1.0f;

	//移動できる光源を実装
	if (Input::IsKey(DIK_A)) {
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.x -= 0.01f;
		Direct3D::SetLightPos(pos);
	}
	if (Input::IsKey(DIK_D)) {
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.x += 0.01f;
		Direct3D::SetLightPos(pos);
	}
	if (Input::IsKey(DIK_W)) {
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.z += 0.01f;
		Direct3D::SetLightPos(pos);
	}
	if (Input::IsKey(DIK_S)) {
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.z -= 0.01f;
		Direct3D::SetLightPos(pos);
	}
	if (Input::IsKey(DIK_UP))
	{
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.y += 0.01f;
		Direct3D::SetLightPos(pos);
	}
	if (Input::IsKey(DIK_DOWN))
	{
		XMFLOAT4 pos = Direct3D::GetLightPos();
		pos.y -= 0.01f;
		Direct3D::SetLightPos(pos);
	}

	//コンスタントバッファ用データ作成
	CONSTANTBUFFER_STAGE cb;
	cb.lightPos = Direct3D::GetLightPos();
	XMStoreFloat4(&cb.eyePos, Camera::GetPosition());

	//コンスタントバッファにデータ転送
	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データ値を送る
	Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

	//コンスタントバッファの更新
	Direct3D::pContext->VSSetConstantBuffers(1, 1, &pConstantBuffer_);//頂点シェーダー用
	Direct3D::pContext->PSSetConstantBuffers(1, 1, &pConstantBuffer_);//ピクセルシェーダー用
}

void Stage::Draw()
{
	Transform t;
	t.position_ = { Direct3D::GetLightPos().x, Direct3D::GetLightPos().y, Direct3D::GetLightPos().z };
	t.scale_ = { 0.1f,0.1f,0.1f };
	Model::SetTransform(hBall_, t);
	Model::Draw(hBall_);

	Transform tRoom;
	tRoom.position_ = { 0.0f,0.0f,0.0f };
	tRoom.rotate_ = { 0.0f,0.0f,0.0f };
	Model::SetTransform(hRoom_, tRoom);
	Model::Draw(hRoom_);

	static Transform tDonut;
	tDonut.position_ = { 0.0f,0.5f,0.0f };
	tDonut.scale_ = { 0.25f,0.25f,0.25f };
	tDonut.rotate_.y += 1.0f;
	Model::SetTransform(hDonut_, tDonut);
	Model::Draw(hDonut_);

	//Model::SetTransform(hModel, t);
	//Model::Draw(hModel);
	//RayCastData rayData{
	//	{0.0f,0.0f,5.0f,0.0f},//startPos
	//	{0.0f,-1.0f,0.0f,0.0f},//dir
	//	false,//isHit
	//	0.0f//dist
	//};
	//Model::RayCast(hModel, rayData);
	//if (rayData.isHit)
	//{
	//	MessageBoxA(NULL, "Hit!", "RayCast", MB_OK);
	//}

	//ImGui::Begin("Stage Manu");
	//ImGui::Text("Modelhandle:",hModel);
	//ImGui::End();

	ImGui::Begin("Stage Manu");
	ImGui::Text("Stage rot: %f", tDonut.rotate_.y);
	ImGui::Text("Light Pos: X:%f", Direct3D::GetLightPos().x);
	ImGui::Text("Light Pos: Y:%f", Direct3D::GetLightPos().y);
	ImGui::Text("Light Pos: Z:%f", Direct3D::GetLightPos().z);
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
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"ステージ用コンスタントバッファの生成に失敗しました", L"エラー", MB_OK);
	}
}
