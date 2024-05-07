float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
};
    struct VertexShaderInput
    {
        float32_t4 position : POSITIONO;
    };
    VertexShaderOutput main(VertexShaderInput input)
    {
        VertexShaderOutput output;
        output.position = input.position;
        return output;
    }
    
    struct PixelShaderOutput
    {
        float32_t4 color : SV_TARGETO;
    };
    PixelShaderOutput main()
    {
        PixelShaderOutput output;
        output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
        return output;
    }
