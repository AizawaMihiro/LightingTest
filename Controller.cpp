#include "Controller.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"

void Controller::Initialize()
{
}

void Controller::Update()
{
    //キーを押した方向にカメラを回転させる
	//Shiftキーが押されている間はカメラの位置ごと変える
    if (Input::IsKey(DIK_LSHIFT))
    {
        if (IsPressUp())
        {
            Camera::SetPosition(Camera::GetPosition() + XMVectorSet(0, 0, 0.1f, 0));
        }
		if (IsPressDown())
		{
			Camera::SetPosition(Camera::GetPosition() + XMVectorSet(0, 0, -0.1f, 0));
		}
        if (IsPressLeft())
        {
            Camera::SetPosition(Camera::GetPosition() + XMVectorSet(-0.1f, 0, 0, 0));
        }
		if (IsPressRight())
		{
			Camera::SetPosition(Camera::GetPosition() + XMVectorSet(0.1f, 0, 0, 0));
		}
    }
    if (IsPressUp())
    {
		Camera::SetTarget(Camera::GetTarget() + XMVectorSet(0, 0, 0.1f, 0));
    }
    if (IsPressDown())
    {
		Camera::SetTarget(Camera::GetTarget() + XMVectorSet(0, 0, -0.1f, 0));
    }
	if (IsPressLeft())
	{
		Camera::SetTarget(Camera::GetTarget() + XMVectorSet(-0.1f, 0, 0, 0));
	}
    if (IsPressRight())
    {
		Camera::SetTarget(Camera::GetTarget() + XMVectorSet(0.1f, 0, 0, 0));
    }
}

bool Controller::IsPressUp()
{
    if (Input::IsKey(DIK_UP))
    {
		return true;
    }
    return false;
}

bool Controller::IsPressDown()
{
    if (Input::IsKey(DIK_DOWN))
    {
        return true;
    }
    return false;
}

bool Controller::IsPressLeft()
{
    if (Input::IsKey(DIK_LEFT))
    {
        return true;
    }
    return false;
}

bool Controller::IsPressRight()
{
    if (Input::IsKey(DIK_RIGHT))
    {
        return true;
    }
    return false;
}
