#include "TestScene.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Stage.h"
#include "Controller.h"

TestScene::TestScene(GameObject* parent)
	:GameObject(parent, "TestScene")
{
	//hModel_ = Model::Load("cube.fbx");
	//assert(hModel_ >= 0);
	//transform_.position_.y = -2.0f;

	Instantiate<Stage>(this);
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
}

void TestScene::Update()
{
	//transform_.rotate_.y += 2.0f;

	////スペースキーが押されたらSceneManagerを取得
	////そしてシーンを切り替える
	//if (Input::IsKeyDown(DIK_SPACE))
	//{
	//	GameObject* sceneManager = FindObject("SceneManager");
	//	if (sceneManager)
	//	{
	//		((SceneManager*)sceneManager)->ChangeScene(SCENE_ID_PLAY);
	//	}
	//}

	Controller::Update();
}

void TestScene::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void TestScene::Release()
{
}
