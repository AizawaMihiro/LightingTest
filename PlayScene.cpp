#include "PlayScene.h"
#include "Stage.h"
#include "Controller.h"

PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent, "PlayScene")
{
	Instantiate<Stage>(this);
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
}

void PlayScene::Update()
{
	Controller::Update();
}

void PlayScene::Draw()
{
}

void PlayScene::Release()
{
}
