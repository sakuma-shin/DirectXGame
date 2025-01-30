struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
};

struct Camera
{
    float32_t3 worldPosition;
    float32_t padding;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float32_t intensity;
};

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    int32_t enableLighting;
    float32_t3 padding;
    float32_t shininess;
    
};

