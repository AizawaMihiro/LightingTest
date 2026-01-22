#include "TitleScene.h"
#include "Engine/Input.h"
#include "Engine/SceneManager.h"

TitleScene::TitleScene(GameObject* parent)
	:GameObject(parent, "TitleScene")
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
}

void TitleScene::Update()
{
	//スペースキーが押されたらSceneManagerを取得
	//そしてシーンを切り替える
	if (Input::IsKeyDown(DIK_SPACE))
	{
		GameObject* sceneManager = FindObject("SceneManager");
		if (sceneManager)
		{
			((SceneManager*)sceneManager)->ChangeScene(SCENE_ID_PLAY);
		}
	}
}

void TitleScene::Draw()
{
}

void TitleScene::Release()
{
}
