
struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCORRD;
};

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};