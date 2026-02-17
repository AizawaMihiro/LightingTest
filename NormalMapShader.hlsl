//───────────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D g_texture : register(t0); //テクスチャー
Texture2D g_normalmap : register(t1); //テクスチャー
SamplerState g_sampler : register(s0); //サンプラー
SamplerState g_normalSample : register(s1); //サンプラー

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
struct VS_OUT
{
    float4 wpos : POSITION0; //ワールド座標
    float4 spos : SV_POSITION; //スクリーン座標
    float2 uv : TEXCOORD; //UV座標
    float4 normal : NORMAL; //法線ベクトル
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float4 eyev : POSITION1; //視線ベクトル
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent: TANGENT,float4 binormal:BINORMAL)
{
	//ピクセルシェーダーへ渡す情報
    VS_OUT outData;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
    outData.spos = mul(pos, matWVP);
    //ワールド座標もピクセルシェーダーへ
    outData.wpos = mul(pos, matWorld);
    
    normal.w = 0; //法線ベクトルの成分を0に
    outData.normal = mul(normal, matNormal);
    
    tangent.w = 0; //接線ベクトルの成分を0に
    outData.tangent = mul(tangent, matNormal);
    
    binormal.w = 0; //従法線ベクトルの成分を0に
    outData.binormal = mul(binormal, matNormal);
    
    //uv.w = 0; //w成分は0にする
    outData.uv = uv.xy; //UV座標はそのまま
    
    outData.eyev = outData.wpos - eyePosition;

    //normal = mul(normal, matNormal); //法線ベクトルをワールド・ビュー・プロジェクション行列で変換
    //normal = normalize(normal); //法線ベクトルを正規化=長さ1に)
    //normal.w = 0; //w成分は0にする
    //float4 light = float4(-1, 0.5, -0.7, 0);
    //light = normalize(light);
    //light.w = 0;
    //outData.color = clamp(dot(normal, light), 0, 1);

	//まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    //法線マップから法線を取得
    float3 normalMap = g_normalmap.Sample(g_normalSample, inData.uv).xyz;//転写未遂
    
    float3 T = normalize(inData.tangent.xyz);
    float3 B = normalize(inData.binormal.xyz);
    float3 N = normalize(inData.normal.xyz);
    float3x3 TBN = float3x3(T, B, N);
    float3 wNormal = mul(normalMap, TBN); //ワールド空間の法線ベクトルを計算
    
    //float4 lightDir = float4(-1, 0.5, -0.7, 0);
    float4 diffuse;
    float4 ambientColor = ambient;
    float4 ambientFactor = float4(0.2, 0.2, 0.2, 1.0); //ここ変更すると暗くなりすぎるので後回し
    float3 dir = normalize(lightPosisiton.xyz - inData.wpos.xyz);
    
    //光源にかかわる変数
    float3 k = { 0.2f, 0.2f, 0.1f }; //環境光係数
    float len = length(lightPosisiton.xyz - inData.wpos.xyz); //光源からの距離
    float dTerm = 1.0f / (k.x + k.y * len + k.z * len * len); //減衰計算
    //float dTerm = 1.0f;
    
    diffuse = diffuseColor * diffusefactor * clamp(dot(wNormal, dir), 0, 1) * dTerm;
    
    float3 L = normalize(lightPosisiton.xyz - inData.wpos.xyz); //光源ベクトル
    float ndotl = saturate(dot(wNormal, L));
    float spec = 0.0f;
    if (ndotl > 0.0f)
    {
        float3 R = reflect(L, wNormal); //正反射ベクトル
        float3 V = normalize(inData.eyev.xyz); //正規化視線ベクトル
        spec = pow(saturate(dot(R, V)), 32.0) * ndotl;
    }
    float4 specularCol = specular * spec;
    
    float4 diffuseTerm;
    float4 specularTerm = specularCol * dTerm;
    float4 ambientTerm;
    float4 color;
    if (useTexture)
    {
        diffuseTerm = diffuse * g_texture.Sample(g_sampler, inData.uv);
        ambientTerm = ambientFactor * g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
        diffuseTerm = diffuse * dTerm;
        ambientTerm = ambientFactor * diffuseColor;
    }
    color = diffuseTerm + specularTerm + ambientTerm;
    return color;
}