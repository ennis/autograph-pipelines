
vec3 cosPalette(  float t,  vec3 a,  vec3 b,  vec3 c, vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

vec4 packNormal(vec3 N)
{
	return vec4(N / 2.0 + vec3(0.5), 1.0);
}

vec3 unpackNormal(vec4 packedNormal)
{
	return normalize(2.0 * (packedNormal.xyz - vec3(0.5)));
}