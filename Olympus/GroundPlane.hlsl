cbuffer ViewProjMat	: register(b0)
{
    float4x4 ViewProj;
}

cbuffer ObjectMat	: register(b1)
{
    float4x4 viewInvProj;
	float4x4 viewPrev;

	float	 zNear;
	float	 zFar;
	float2	 padding;
}

Texture2D tex : register(t0);
Texture2D depth : register(t1);

struct VOut
{
	float4 posH : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

VOut VShader(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float2 texcoord : TEXCOORD )
{
    VOut output;

	output.posH = mul(ViewProj, float4(position, 1.0f));
    output.texcoord = texcoord;

    return output;
}

float4 PShader(VOut input) : SV_TARGET
{
	return float4(0.6f,0.6f,0.6f,1.0f);
}
//float4 PShader(VOut input) : SV_TARGET
//{   
//
//	//return tex.Sample(samTriLinearSam, input.texcoord);
//
//
//	float zFar = 100.0f;
//	float zNear = 1.0f;
//	
//	float4 color = depth.Sample( samLinear, input.texcoord );
//	color.r = 2*zFar*zNear / (zFar + zNear - (zFar - zNear)*(2*color.r -1));
//
//	float z_b = depth.Sample( samLinear, float2( .5, .5 ) ).r;
//	float midDepth = 2*zFar*zNear / (zFar + zNear - (zFar - zNear)*(2*z_b -1));
//	float blurFactor = 1.0;
//
//	float depthRange = .5 * (zFar - zNear );
//
//	if( color.r > midDepth - depthRange && color.r < midDepth + depthRange )
//	{
//		color = tex.Sample( samLinear, input.texcoord );
//		return color;
//	}
//	else
//	{
//		if( abs( midDepth + depthRange - color.r ) > abs( midDepth - depthRange - color.r ) )
//			blurFactor =  ( ( midDepth - depthRange - color.r ) - zNear ) / ( zFar - zNear );
//		else
//			blurFactor =  ( ( midDepth + depthRange - color.r ) - zNear ) / ( zFar - zNear );
//
//		blurFactor = abs( blurFactor );
//	}
//
//	color = (tex.Sample( samLinear, input.texcoord )) * 4;
//
//	float blur = .004;
//	
//	color += ( tex.Sample( samLinear, float2( input.texcoord.x+blur, input.texcoord.y ) ) ) * 2;
//	color += ( tex.Sample( samLinear, float2( input.texcoord.x-blur, input.texcoord.y ) ) ) * 2;
//	color += ( tex.Sample( samLinear, float2( input.texcoord.x, input.texcoord.y+blur ) ) ) * 2;
//	color += ( tex.Sample( samLinear, float2( input.texcoord.x, input.texcoord.y-blur ) ) ) * 2;
//
//	color += tex.Sample( samLinear, float2( input.texcoord.x-blur, input.texcoord.y-blur ) );
//	color += tex.Sample( samLinear, float2( input.texcoord.x+blur, input.texcoord.y-blur ) );
//	color += tex.Sample( samLinear, float2( input.texcoord.x-blur, input.texcoord.y+blur ) );
//	color += tex.Sample( samLinear, float2( input.texcoord.x+blur, input.texcoord.y+blur ) );
//
//	color = color / 16;
//	color.a = 1.0f;
//
//	return color;
//}
