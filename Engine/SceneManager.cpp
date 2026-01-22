#include "SceneManager.h"
#include "Direct3D.h"
#include "../Playscene.h"
#include "../TestScene.h"
//#include "../ClearScene.h"

SceneManager::SceneManager(GameObject* parent)
	:GameObject(parent, "SceneManager")
{
	currentSceneID_ = SCENE_ID_TEST;
	nextSceneID_ = currentSceneID_;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	Instantiate<TestScene>(this);
}

void SceneManager::Update()
{
	if (currentSceneID_ != nextSceneID_)
	{
		//シーン切り替え処理
		auto scene = childList_.front();//現在のシーン取得
		scene->ReleaseSub();//子オブジェクト解放
		SAFE_DELETE(scene);//シーンオブジェクト削除
		childList_.clear();//子オブジェクトリストクリア


		switch (nextSceneID_)
		{
		case SCENE_ID_TEST:
			Instantiate<TestScene>(this);
			break;
		case SCENE_ID_PLAY:
			Instantiate<PlayScene>(this);
			break;
		case SCENE_ID_CLEAR:
			//Instantiate<ClearScene>(this);
			break;
		case SCENE_ID_MAX:
			break;
		default:
			break;
		}
		currentSceneID_ = nextSceneID_;
	}
}

void SceneManager::Draw()
{
}

void SceneManager::Release()
{
}

void SceneManager::ChangeScene(SCENE_ID nextSceneID)
{
	nextSceneID_ = nextSceneID;
}
