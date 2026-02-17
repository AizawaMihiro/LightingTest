#include "Model.h"
#include "Direct3D.h"

namespace Model
{
	//モデルデータリスト
	std::vector<ModelData*> modelList;
}

int Model::Load(const std::string& filename)
{
	ModelData* pModelData = new ModelData;

	pModelData->filename_ = filename;
	pModelData->pFbx_ = nullptr;

	for (auto& itr:modelList)
	{
		if (itr->filename_ == filename)
		{
			pModelData->pFbx_ = itr->pFbx_;
			break;
		}
	}

	if (pModelData->pFbx_ == nullptr)
	{
		//Fbx読み込み
		pModelData->pFbx_ = new Fbx();
		pModelData->pFbx_->Load(filename.c_str());
	}
	modelList.push_back(pModelData);

	return (int)(modelList.size() - 1);
}

void Model::SetTransform(int hModel, Transform transform)
{
	modelList[hModel]->transform_ = transform;
}

void Model::Draw(int hModel)
{
	modelList[hModel]->pFbx_->Draw(modelList[hModel]->transform_);
}

void Model::DrawPseudoNormal(int hModel)
{
	modelList[hModel]->pFbx_->Draw(modelList[hModel]->transform_);
}

void Model::Release()
{
	bool isReffered = false;//参照されているかどうか
	for (int i = 0; i < modelList.size(); i++)
	{
		isReffered = false;
		for (int j = i; j < modelList.size(); j++)
		{
			if (modelList[i]->pFbx_ == modelList[j]->pFbx_)
			{
				isReffered = true;
				break;
			}
		}
		if (isReffered)
		{
			SAFE_DELETE(modelList[i]->pFbx_);
		}
		SAFE_DELETE(modelList[i]);
	}
	modelList.clear();
}

void Model::RayCast(int hModel, RayCastData& rayData)
{
	//対象モデルのトランスフォームをカリキュレート
	modelList[hModel]->transform_.Calclation();
	//ワールド行列の逆行列を取得
	XMMATRIX worldMatrix = modelList[hModel]->transform_.GetWorldMatrix();
	XMMATRIX wInverse = XMMatrixInverse(nullptr, worldMatrix);

	//Rayの通過点を求める(ワールド空間での始点からdir方向に進む直線上の点)
	//方向ベクトルを少し伸ばす
	XMVECTOR vDirVec{ rayData.dir.x + rayData.dir.x,rayData.dir.y + rayData.dir.y, rayData.dir.z + rayData.dir.z,0.0f };

	//rayDataのstartPosをモデルのローカル空間に変換
	XMVECTOR vStartPos = XMLoadFloat4(&rayData.startPos);

	vStartPos = XMVector3Transform(vStartPos, wInverse);
	XMStoreFloat4(&rayData.startPos, vStartPos);//変換結果を格納

	//rayDataのdirをモデルのローカル空間に変換
	vDirVec = XMVector3Transform(vDirVec, wInverse);
	
	XMVECTOR dirAtLocal = XMVectorSubtract(vDirVec, vStartPos);
	dirAtLocal = XMVector3Normalize(dirAtLocal);//正規化
	XMStoreFloat4(&rayData.dir, dirAtLocal);//変換結果を格納

	//FbxのRayCastを呼び出す
	modelList[hModel]->pFbx_->RayCast(rayData);
}
