#include"Vector4.h"
#include"Particle.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
    
    float32_t4x4 World;
};
StructuredBuffer<ParticleForGPU> gParticle : register(t0);



struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD;
};




VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId:SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position,gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.position, (float32_t3x3) gTransformationMatrices[instanceId].World));
    output.color = gParticle[instanceId].color;
    output.color.w = gParticle[instanceId].color.w;
    return output;
}



    
  

    
