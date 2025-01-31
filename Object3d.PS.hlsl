#include"Object3d.hlsli"


ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1); //[3]
ConstantBuffer<DirectionalLight>gDirectionalLight:register(b2);  //[4]
//half lambert



struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t3 reflectLight = reflect(normalize(gDirectionalLight.direction), normalize(input.normal));
    
    
    PixelShaderOutput output;
    
    if (gMaterial.enableLighting != 0)
    { // Lightingする場合
        
      
        float RdotE = dot(reflectLight, toEye);
        //float specularPow = pow(saturate(RdotE), gMaterial.shininess);
    
        float NdotL = dot(normalize(input.normal), normalize(-gDirectionalLight.direction));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        float32_t3 halfVector = normalize(-gDirectionalLight.direction * toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
    
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // 鏡面反射
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    
        output.color.rgb = diffuse + specular;
        //output.color = gMaterial.color.textureColor;
        //output.color.a = gMaterial.color.a * textureColor.a;
        
        //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
      
    }
    else
    { // Lightingしない場合。前回までと同じ演算
        output.color = gMaterial.color * textureColor;
    }
    
    
    return output;
}


