#pragma once

#include <string>
#include "Transform.h"
#include <list>
#include "SphereCollider.h"

using std::string;
using std::list;

class GameObject
{
protected:
	list<GameObject *> childList_;
	Transform	transform_;
	GameObject*	pParent_;
	string	objectName_;
	SphereCollider* pCollider_;


public:
	GameObject();
	GameObject(GameObject* parent, const string& name);
	virtual ~GameObject();

	virtual void Initialize() =0;
	virtual void Update() =0;
	virtual void Draw() =0;
	virtual void Release() =0;
	void DrawSub();
	void UpdateSub();
	void ReleaseSub();

	void SetPosition(XMFLOAT3 position);
	void SetPosition(float x, float y, float z);
	XMFLOAT3 GetPosition() { return transform_.position_; }
	
	void KillMe() { isDead_ = true; }

	GameObject* FindObject(const string& name);

	void AddCollider(SphereCollider* pCollider);
	void Collision(GameObject* pTarget);
	void RoundRobin(GameObject* pTarget);

	/// <summary>
	/// ìñÇΩÇËîªíËÇ≈ìñÇΩÇ¡ÇΩÇ∆Ç»Ç¡ÇΩèÍçáåƒÇ—èoÇ≥ÇÍÇÈä÷êî
	/// </summary>
	/// <param name="pTarget">ìñÇΩÇ¡ÇΩëäéË</param>
	virtual void onCollision(GameObject* pTarget);

	string GetObjectName() { return objectName_; }
	void SetDeadFlag(bool flag) { isDead_ = flag; }

	template <class T>
	GameObject* Instantiate(GameObject* parent)
	{
		T* obj = new T(parent);
		obj->Initialize();
		childList_.push_back(obj);
		return obj;
	}

private:
	bool isDead_;
	GameObject* GetRootJob();
	GameObject* FindChildObject(const string& name);
};