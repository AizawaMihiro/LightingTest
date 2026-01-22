#pragma once
#include <string>
#include <vector>
#include "Fbx.h"

namespace Model
{
	struct ModelData
	{
		Fbx* pFbx_;
		Transform transform_;
		std::string filename_;
	};
	int Load(const std::string& filename);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel);
	void Release();

	void RayCast(int hModel, RayCastData& rayData);
}
