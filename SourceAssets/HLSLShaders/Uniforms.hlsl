// Vertex shader parameters
uniform float3 cCameraPos : register(C0);
uniform float3x3 cCameraRot : register(C1);
uniform float4 cDepthMode : register(C4);
uniform float cLightAtten : register(C5);
uniform float3 cLightDir : register(C6);
uniform float3 cLightPos : register(C7);
uniform float3x3 cLightVecRot: register(C8);
uniform float4x3 cModel : register(C11);
uniform float4x4 cSpotProj : register(C14);
uniform float4x4 cViewProj : register(C18);
uniform float4 cUOffset : register(C22);
uniform float4 cVOffset : register(C23);
uniform float3 cViewRightVector : register(C24);
uniform float3 cViewUpVector : register(C25);
uniform float4x4 cShadowProj[4] : register(C26);
uniform float4x3 cSkinMatrices[64] : register(C42);

// Pixel shader parameters
uniform float3 cAmbientColor : register(C0);
uniform float4 cFogParams : register(C1);
uniform float3 cFogColor : register(C2);
uniform float4 cLightColor : register(C3);
uniform float4 cMatDiffColor : register(C4);
uniform float3 cMatEmissiveColor : register(C5);
uniform float2 cMatSpecProperties : register(C6);
uniform float4 cSampleOffsets : register(C7);
uniform float4 cShadowCubeAdjust : register(C8);
uniform float2 cShadowCubeProj : register(C9);
uniform float2 cShadowFade : register(C10);
uniform float4 cShadowIntensity : register(C11);
uniform float4 cShadowSplits : register(C12);
