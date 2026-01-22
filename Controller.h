#pragma once

class Controller
{
public:
	static void Initialize();
	static void Update();
private:
	static bool IsPressUp();
	static bool IsPressDown();
	static bool IsPressLeft();
	static bool IsPressRight();
};

