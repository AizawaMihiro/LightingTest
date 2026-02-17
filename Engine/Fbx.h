#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Texture.h"
#include <vector>

#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

namespace Math {
	float Det(XMVECTOR& a, XMVECTOR& b, XMVECTOR& c);
	bool InterSects(XMFLOAT3 origin, XMFLOAT3 direction, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2,float& dist);
}

struct RayCastData {
	XMFLOAT4 startPos;//Rayの始点
	XMFLOAT4 dir;;//Rayの方向(正規化済み)
	bool isHit;//当たったかどうか
	float dist;//始点からの距離
};

class Fbx
{
	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matWorld;
		XMMATRIX	matNormal;
		XMFLOAT4	diffuse;//材質の拡散反射光成分
		XMFLOAT4	diffuseFactor;//拡散反射光の強さ
		XMFLOAT4	specular;//材質の鏡面反射光成分
		XMFLOAT4	shininess;//鏡面反射の鋭さ
		XMFLOAT4	ambient;//材質の環境光成分
		BOOL	materialFlag;	//マテリアルがあるかどうか
	};

	struct VERTEX
	{
		XMVECTOR position;
		XMVECTOR uv;
		XMVECTOR normal;//法線
		XMVECTOR tangent;//接線
		XMVECTOR binormal;//従法線
	};

	struct MATERIAL
	{
		Texture* pTexture;
		Texture* pNormalTexture;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;
		float shininess;
		XMFLOAT4 factor;
	};

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	//MATERIAL* pMaterialList_;
	//pMaterialList_をvectorで管理するためmaterialList_を追加
	std::vector<MATERIAL> pMaterialList_;
	std::vector<int> indexCount_;	//マテリアルごとのインデックス数を格納する配列

	int vertexCount_;
	int polygonCount_;
	int materialCount_;

	std::vector<VERTEX> pVertices_;//頂点データ
	std::vector<std::vector<int>> pIndex_;//マテリアルごとのインデックスデータ
	
public:
	Fbx();
	HRESULT Load(std::string fileName);
	void    Draw(Transform& transform);
	void	DrawPseudoNormal(Transform& transform);
	void    Release();

	void InitVertex(FbxMesh* mesh);
	void InitIndex(FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(FbxNode* pNode);
	void RayCast(RayCastData& rayData);
};