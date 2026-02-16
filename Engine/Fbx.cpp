#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include <filesystem>
#include<DirectXCollision.h>

namespace fs = std::filesystem;

Fbx::Fbx() :
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	pConstantBuffer_(nullptr),
	vertexCount_(0),
	polygonCount_(0),
	materialCount_(0)
{
}

HRESULT Fbx::Load(std::string fileName)
{	
	//テクスチャのパスをAssetsに変更
	std::string subDir("Assets");
	fs::path  currPath, basePath;
	currPath = fs::current_path();
	basePath = currPath;
	currPath = currPath / subDir;
	//fs::path subPath(currPath.string() + "\\" + subDir);
	assert(fs::exists(currPath));//Assetsフォルダがあるか確認
	fs::current_path(currPath);

	//マネージャを生成
	FbxManager* pFbxManager = FbxManager::Create();

	//インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	//シーンオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	FbxAxisSystem sceneAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem myAxisSystem(FbxAxisSystem::DirectX);

	if (sceneAxisSystem != myAxisSystem)
	{
		myAxisSystem.DeepConvertScene(pFbxScene);
	}
	FbxSystemUnit sceneSystemUnit = pFbxScene->GetGlobalSettings().GetSystemUnit();
	if (sceneSystemUnit != FbxSystemUnit::cm)
	{
		FbxSystemUnit::cm.ConvertScene(pFbxScene);
	}

	//メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode();
	FbxNode* pNode = rootNode->GetChild(0);
	FbxMesh* mesh = pNode->GetMesh();

	//各情報の個数を取得

	vertexCount_ = mesh->GetControlPointsCount();	//頂点の数
	polygonCount_ = mesh->GetPolygonCount();	//ポリゴンの数
	materialCount_ = pNode->GetMaterialCount();	//マテリアルの数

	InitVertex(mesh);		//頂点バッファ準備
	InitIndex(mesh);		//インデックスバッファ準備
	InitConstantBuffer();	//コンスタントバッファ準備
	InitMaterial(pNode);	//マテリアル準備

	fs::current_path(basePath);	//カレントディレクトリを元に戻す

	//マネージャ解放
	pFbxManager->Destroy();
	return S_OK;
}

void Fbx::Draw(Transform& transform)
{
	Direct3D::SetShader(SHADER_TYPE::SHADER_3D);
	transform.Calclation();


	//for (int i = 0; i < materialCount_; i++)
	//{
	//	if (pMaterialList_[i].pTexture)
	//	{
	//		cb.materialFlag = TRUE;
	//		cb.diffuse = XMFLOAT4(1, 1, 1, 1);
	//	}
	//	else
	//	{
	//		cb.materialFlag = FALSE;
	//		cb.diffuse = pMaterialList_[i].diffuse;
	//	}
	//}

	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	for (int i = 0; i < materialCount_; i++)
	{
		CONSTANT_BUFFER cb;
		cb.matWVP = transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix();
		cb.matNormal = transform.GetNormalMatrix();
		cb.matWorld = transform.GetWorldMatrix();
		//コンスタントバッファにデータ転送
		cb.ambient = pMaterialList_[i].ambient;
		cb.specular = pMaterialList_[i].specular;
		cb.shininess = { pMaterialList_[i].shininess, pMaterialList_[i].shininess, pMaterialList_[i].shininess, pMaterialList_[i].shininess };
		cb.diffuse = pMaterialList_[i].diffuse;
		cb.diffuseFactor = pMaterialList_[i].factor;
		cb.materialFlag = pMaterialList_[i].pTexture != nullptr;

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データ値を送る
		Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		//コンスタントバッファ
		Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
		Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用


		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		Direct3D::pContext->DrawIndexed(indexCount_[i], 0, 0);
	}
	
}

void Fbx::DrawPseudoNormal(Transform& transform)
{
	Direct3D::SetShader(SHADER_TYPE::SHADER_NORMALMAP);
	transform.Calclation();

	//頂点バッファ
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	for (int i = 0; i < materialCount_; i++)
	{
		CONSTANT_BUFFER cb;
		cb.matWVP = transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix();
		cb.matNormal = transform.GetNormalMatrix();
		cb.matWorld = transform.GetWorldMatrix();
		//コンスタントバッファにデータ転送
		cb.ambient = pMaterialList_[i].ambient;
		cb.specular = pMaterialList_[i].specular;
		cb.shininess = { pMaterialList_[i].shininess, pMaterialList_[i].shininess, pMaterialList_[i].shininess, pMaterialList_[i].shininess };
		cb.diffuse = pMaterialList_[i].diffuse;
		cb.diffuseFactor = pMaterialList_[i].factor;
		cb.materialFlag = pMaterialList_[i].pTexture != nullptr;

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのリソースアクセスを一時止める
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データ値を送る
		Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

		// インデックスバッファーをセット
		stride = sizeof(int);
		offset = 0;
		Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		//コンスタントバッファ
		Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
		Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

		//カラーテクスチャがある場合はセット
		if (pMaterialList_[i].pTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		//ノーマルマップのテクスチャがあればセット
		if (pMaterialList_[i].pNormalTexture)
		{
			ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
			Direct3D::pContext->PSSetSamplers(1, 1, &pSampler);
			ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
			Direct3D::pContext->PSSetShaderResources(1, 1, &pSRV);
		}

		Direct3D::pContext->DrawIndexed(indexCount_[i], 0, 0);
	}
}

void Fbx::Release()
{/*
	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pConstantBuffer_);*/
}

void Fbx::InitVertex(FbxMesh* mesh)
{
	//頂点情報を入れる配列
	//VERTEX* vertices = new VERTEX[vertexCount_];
	pVertices_.resize(vertexCount_);

	//tangentの確認
	FbxLayerElementTangent* tangentElement = mesh->GetElementTangent();

	//全ポリゴン
	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3頂点分
		for (int vertex = 0; vertex < 3; vertex++)
		{
			//調べる頂点の番号
			int index = mesh->GetPolygonVertex(poly, vertex);

			//頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			//vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);
			pVertices_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

			//頂点のUV
			FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			//vertices[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 1.0f);
			pVertices_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 1.0f);

			//頂点の法線
			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(poly, vertex, normal);
			//vertices[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0.0f);
			pVertices_[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0.0f);

			if (tangentElement!= nullptr)
			{
				int tangentIndex = 0;
				//接点ごとに接線の情報があるか
				if (tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					tangentIndex = poly * 3 + vertex;
				}
				//頂点ごとに接線の情報が無い場合、ポリゴンごとに接線の情報があるか
				if (tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					tangentIndex = tangentElement->GetIndexArray().GetAt(tangentIndex);
				}
				FbxVector4 tangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
				pVertices_[index].tangent = { (float)tangent[0],(float)tangent[1] ,(float)tangent[2] ,0.0f };
			}
			else
			{
				pVertices_[index].tangent = { 0.0f,0.0f,0.0f,0.0f };
			}
		}
	}

	for (int i = 0; i < vertexCount_; i++)
	{
		XMVECTOR N = XMVector3Normalize(pVertices_[i].normal);
		XMVECTOR T = XMVector3Normalize(pVertices_[i].tangent);
		//従法線は外積で求める
		XMVECTOR B = XMVector3Normalize(XMVector3Cross(N, T));
		pVertices_[i].binormal = B;
	}

	// 頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX)*vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	//data_vertex.pSysMem = vertices;
	data_vertex.pSysMem = pVertices_.data();
	Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
}

void Fbx::InitIndex(FbxMesh* mesh)
{
	//インデックスバッファ用配列
	pIndexBuffer_ = new ID3D11Buffer*[materialCount_];

	//int* index = new int[polygonCount_ * 3];
	pIndex_.resize(materialCount_);
	indexCount_ = std::vector<int>(materialCount_);
	//indexCount_.resize(materialCount_);

	for (int i = 0; i < materialCount_; i++)
	{
		//int count = 0;
		auto& index = pIndex_[i];

		//全ポリゴン
		for (DWORD poly = 0; poly < polygonCount_; poly++)
		{
			//ポリゴンのマテリアル番号を取得
			FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(poly);

			//マテリアル番号が同じならインデックス情報として追加
			if (mtlId == i)
			{
				for (DWORD vertex = 0; vertex < 3; vertex++)
				{
					//index[count] = mesh->GetPolygonVertex(poly, vertex);
					index.push_back(mesh->GetPolygonVertex(poly, vertex));
					//count++;
				}
			}
		}
		//indexCount_[i] = count;
		indexCount_[i] = static_cast<int>(index.size());

		// インデックスバッファを生成する
		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		//bd.ByteWidth = sizeof(int) * polygonCount_ * 3;
		bd.ByteWidth = sizeof(int) * indexCount_[i];
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		//InitData.pSysMem = index;
		InitData.pSysMem = index.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		Direct3D::pDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
	}
}

void Fbx::InitConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	// コンスタントバッファの作成
	Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
}

void Fbx::InitMaterial(FbxNode* pNode)
{
	//vectorに格納する方法	陽悦先生
	pMaterialList_.resize(materialCount_);
	for (int i = 0; i < materialCount_; i++)
	{
		//i番目のマテリアル情報を取得
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);

		//テクスチャ情報
		FbxProperty lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

		//テクスチャの数数
		int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

		//テクスチャあり
		if (fileTextureCount > 0)
		{
			FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
			const char* textureFilePath = textureInfo->GetRelativeFileName();
			fs::path tPath(textureFilePath);
			if (fs::is_regular_file(tPath))
			{
				pMaterialList_[i].pTexture = new Texture;
				pMaterialList_[i].pTexture->Load(tPath.string());
			}
			else
			{
				pMaterialList_[i].pTexture = nullptr;
			}
			//ノーマルマップのテクスチャも読み込む
			fs::path normalTexturePath = "textureNormal.png";//まだ存在しないファイル
			if (fs::is_regular_file(normalTexturePath))
			{
				pMaterialList_[i].pNormalTexture = new Texture;
				pMaterialList_[i].pNormalTexture->Load(normalTexturePath.string());
			}
			else
			{
				pMaterialList_[i].pNormalTexture = nullptr;
			}

			FbxSurfacePhong* pMaterial = (FbxSurfacePhong*)pNode->GetMaterial(i);
			FbxDouble  diffuseFactor = pMaterial->DiffuseFactor;
			FbxDouble3 diffuseColor = pMaterial->Diffuse;
			FbxDouble3  ambient = pMaterial->Ambient;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuseColor[0], (float)diffuseColor[1], (float)diffuseColor[2], 1.0f);
			pMaterialList_[i].factor = XMFLOAT4((float)diffuseFactor, (float)diffuseFactor, (float)diffuseFactor, (float)diffuseFactor);
			pMaterialList_[i].ambient = { (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f };
			//あなたはフォンのパラメータを持ってますか？
			if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				FbxDouble3 specular = pMaterial->Specular;
				FbxDouble shininess = pMaterial->Shininess; //4つとも同じ値でセット
				//ここで、自分のpMaterialList_[i]に値を設定
				pMaterialList_[i].specular = { (float)specular[0],(float)specular[1], (float)specular[2], 1.0f };
				pMaterialList_[i].shininess = shininess;
			}
		}
		//テクスチャ無し
		else
		{
			pMaterialList_[i].pTexture = nullptr;

			//マテリアルの色を取得
			FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i);
			FbxDouble3  diffuse = pMaterial->Diffuse;
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
			FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pNode->GetMaterial(i);
			FbxDouble factor = pPhong->DiffuseFactor;//拡散反射強度
			FbxDouble3 ambient = pPhong->Ambient;//環境光反射率

			//マテリアル情報を格納
			pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], (float)factor);
			pMaterialList_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
			pMaterialList_[i].factor = XMFLOAT4((float)factor, (float)factor, (float)factor, (float)factor);

			if (pPhong->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				FbxDouble specular = pPhong->SpecularFactor;//鏡面反射率
				FbxDouble shininess = pPhong->Shininess;//光沢度

				pMaterialList_[i].specular = XMFLOAT4((float)pPhong->SpecularFactor, (float)pPhong->SpecularFactor, (float)pPhong->SpecularFactor, 1.0f);
				pMaterialList_[i].shininess = (float)pPhong->Shininess;
			}
			else
			{
				pMaterialList_[i].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
				pMaterialList_[i].shininess = 10.0f;
			}
		}
		
	}

}

void Fbx::RayCast(RayCastData& rayData)
{
	for (int material = 0;material < materialCount_;material++)
	{
		auto& indices = pIndex_[material];
		for (int i = 0; i < indices.size(); i+=3)
		{
			//頂点情報を取得
			VERTEX v0, v1, v2;
			v0 = pVertices_[indices[i + 0]];
			v1 = pVertices_[indices[i + 1]];
			v2 = pVertices_[indices[i + 2]];
			//頂点座標をXMFLOAT3に変換
			XMFLOAT3 v0Pos, v1Pos, v2Pos;
			XMStoreFloat3(&v0Pos, v0.position);
			XMStoreFloat3(&v1Pos, v1.position);
			XMStoreFloat3(&v2Pos, v2.position);
			//当たり判定
			//rayData.isHit = Math::InterSects(XMLoadFloat4(&rayData.startPos), XMLoadFloat4(&rayData.dir), v0Pos, v1Pos, v2Pos,rayData.dist);
			//rayData.isHit = Math::InterSects(rayData.startPos, rayData.dir, v0Pos, v1Pos, v2Pos, rayData.dist);
			rayData.isHit = TriangleTests::Intersects(
				XMLoadFloat4(&rayData.startPos),
				XMLoadFloat4(&rayData.dir),
				XMLoadFloat3(&v0Pos),
				XMLoadFloat3(&v1Pos),
				XMLoadFloat3(&v2Pos),
				rayData.dist);//DirectXCollision.hの関数を使用

			if (rayData.isHit)
			{
				return;
			}
		}
	}
	rayData.isHit = false;
}

float Math::Det(XMVECTOR& a, XMVECTOR& b, XMVECTOR& c)
{
	//行列式計算
	XMFLOAT3A fa, fb, fc;
	XMStoreFloat3A(&fa, a);
	XMStoreFloat3A(&fb, b);
	XMStoreFloat3A(&fc, c);
	//サラスの公式を使って計算
	return fa.x * fb.y * fc.z + fb.x * fc.y * fa.z + fc.x * fa.y * fb.z
		- fc.x * fb.y * fa.z - fb.x * fa.y * fc.z - fa.x * fc.y * fb.z;
}

bool Math::InterSects(XMFLOAT3 origin, XMFLOAT3 direction, XMFLOAT3 v0, XMFLOAT3 v1, XMFLOAT3 v2, float& dist)
{
	//vectorに変換
	XMVECTOR vOrigin = XMLoadFloat3(&origin);
	XMVECTOR vDirection = XMLoadFloat3(&direction);
	XMVECTOR vV0 = XMLoadFloat3(&v0);
	XMVECTOR vV1 = XMLoadFloat3(&v1);
	XMVECTOR vV2 = XMLoadFloat3(&v2);

	//辺のベクトル
	XMVECTOR vEdge1 = XMVectorSubtract(vV1, vV0);
	XMVECTOR vEdge2 = XMVectorSubtract(vV2, vV0);
	//行列式計算のためXMFLOAT3に変換
	XMFLOAT3 edge1, edge2;
	XMStoreFloat3(&edge1, vEdge1);
	XMStoreFloat3(&edge2, vEdge2);

	//基準点v0からRayの始点へのベクトル
	XMVECTOR d = XMVectorSubtract(vOrigin, vV0);

	//directionを反転
	XMVECTOR dirN = XMVectorNegate(vDirection);

	//行列式
	float det = Math::Det(vEdge1, vEdge2, dirN);
	//平行
	if (det <= 0.0f)
	{
		return false;
	}

	//u,v,t計算
	float u = Math::Det(d, vEdge2, dirN) / det;
	float v = Math::Det(vEdge1, d, dirN) / det;
	float t = Math::Det(vEdge1, vEdge2, d) / det;

	//判定
	if (u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f && t >= 0.0f)
	{
		return true;
	}

	return false;
}
