void mainImage() {
    vec2 uv = fragCoord;

    vec2 center = uvResolution * 0.5;
    vec2 toCenter = uv - center;
    float dist = length(toCenter);
    float distortion = sin(dist * 0.1 - fTime * 10) * 20.0 / (dist * 0.01 + 1.0);
    vec2 distortedUV = uv + normalize(toCenter) * distortion;
    uv = distortedUV;


    fragColor = vec4(vec3(mod(floor(uv.x / 20.0) + floor(uv.y / 20.0), 2.0) * 0.3 + 0.4), 1.0);
}