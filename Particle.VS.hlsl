#include"Vector4.h"
#include"Particle.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
};
StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);



struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD;
};




VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId:SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, (float32_t3x3) gTransformationMatrices[instanceId].WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.position, (float32_t3x3) gTransformationMatrices[instanceId].World));
    return output;
}



    
  

    
