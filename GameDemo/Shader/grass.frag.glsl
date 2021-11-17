#version 450

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

in TESE_OUT
{
	vec4 fragPosLightSpace;
	vec3 normal;
	vec2 uv;
} frag_in;

layout(location = 0) out vec4 outColor;

uniform sampler2D shadowMap;

float calculateShadow(vec4 FragPosLightSpace)
{
	vec3 ndcPos = FragPosLightSpace.xyz / FragPosLightSpace.w;
	vec3 winPos = ndcPos * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, winPos.xy).r;
	float currentDepth = winPos.z;
	return currentDepth > closestDepth ? 1.0 : 0.0;
}

void main() {
	vec3 normal = frag_in.normal;
	vec2 uv = frag_in.uv;

	vec3 upperColor = vec3(0.4,1,0.1);
	vec3 lowerColor = vec3(0.0,0.2,0.1);

	vec3 sunDirection = normalize(vec3(-1.0, 5.0, -3.0));

	vec3 upperDarkColor = vec3(0.2,0.75,0.05);
	vec3 lowerDarkColor = vec3(0.0,0.5,0.05);

	float NoL = clamp(dot(normal, sunDirection), 0.5, 1.0);

	vec3 mixedColor = mix(lowerColor, upperColor, uv.y);

	//float shadow = 0.0f;
	//shadow = calculateShadow(frag_in.fragPosLightSpace);
	//vec3 result = ((1.0 - shadow) + 0.3) * mixedColor;

	outColor = vec4(mixedColor * NoL, 1.0);
}
