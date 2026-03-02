//───────────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D g_texture : register(t0); //テクスチャー
SamplerState g_sampler : register(s0); //サンプラー
Texture2D g_toontexture : register(t1); //テクスチャー
SamplerState g_toonsampler : register(s1); //サンプラー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global : register(b0)
{
    row_major float4x4 matWVP; // ワールド・ビュー・プロジェクションの合成行列
    row_major float4x4 matWorld; // ワールド行列
    row_major float4x4 matNormal; //法線ベクトル変換行列
    float4 diffuseColor; //デフューズカラー
    float4 diffusefactor; //デフューズファクター
    float4 specular; //スペキュラカラー
    float4 shininess; //シャイニネス
    float4 ambient; //アンビエントカラー
    bool useTexture; //テクスチャーを使うかどうか
};

cbuffer gStage : register(b1)
{
    float4 lightPosisiton; //ライトの位置
    float4 eyePosition; //カメラの位置
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
//今回は使わない
struct VS_OUT
{
    float4 wpos : POSITION0; //ワールド座標
    float4 spos : SV_POSITION; //スクリーン座標
    float2 uv : TEXCOORD; //UV座標
    float4 normal : NORMAL; //法線ベクトル
    float4 eyev : POSITION1; //視線ベクトル
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
float4 VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//ピクセルシェーダーへ渡す情報
    float4  outPos;
    
    pos = pos + normal * 0.05f; //法線方向に少し頂点を移動させる
    
    pos = mul(pos, matWVP); //ローカル座標に、ワールド・ビュー・プロジェクション行列をかけてスクリーン座標に変換

    
    return pos;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(float4 pos : SV_POSITION) : SV_Target
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

//頂点をずらすことでトゥーンシェーダーのアウトラインを作るカリングトリック