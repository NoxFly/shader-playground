/**
 * @author Dorian Thivolle
 */

#include <helpers/common>
#include <helpers/colorUtils>


const uint maxIt = 18;



vec3 colorV1(float v, float i) {
    vec3 color = vec3(0.0);
    
    if(v < 1.0) {
        v = 0.5 + sin(v * PI);

        if(v > 0.9) {
            color = vec3(1.0);
        }
        else {
            color = vec3(v, 0, 0);
        }
    }

    return color;
}

vec3 colorV2(float v, float i) {
    vec3 color = vec3(0.0);

    float hue = float((360.0 * 10 * i) / maxIt);
	float saturation = 1.;
	float value = 1.;

    color = hsv2rgb(hue, saturation, value);

    return color;
}




void mainImage() {
    vec2 uv = (fragCoord.xy / uvResolution) * 2.0 - 1.0;
    uv.x *= fRatio;

    vec2 c = vec2(uv.x * fZoom, uv.y * fZoom) + fvCenter;
    vec2 z = vec2(0.0, 0.0);

    float i;

    for (i = 0.0; i < maxIt; i += 1.0) {
        vec2 znew = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;

        if (length(znew) > 2.0) {
            break;
        }

        z = znew;
    }

    // 
    float v = i / float(maxIt);

    vec3 color = colorV1(v, i);
//	vec3 color = vec3(map(fragCoord.x, 0, uvResolution.x, 0, 1));

    fragColor = vec4(color, 1.0);
}