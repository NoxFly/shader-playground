/**
 * @author NoxFly
 *
 * Optimizations from wikipedia :
 * https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
 */
#version 460 core

#include <helpers/common>
#include <helpers/colorUtils>


const uint maxIt = uint(max(0, 128 + 20 * iIncrement));

const vec4 mandelbrotRes = vec4(-2.5, -1, 1, 1);


bool isInMainBulb(vec2 coords) {
    float first = coords.x - 0.25;
	const float p = sqrt(first * first + coords.y * coords.y);
	
    return (coords.x < p - 2 * (p * p) + 0.25)
	 || (((coords.x + 1) * (coords.x + 1) + coords.y * coords.y) < 0.0625);
}

float mandelbrot(vec2 coords) {
    if(isInMainBulb(coords)) {
        return 0.0;
    }

    vec2 pos = vec2(0.0, 0.0);
    vec2 pos2 = vec2(0.0, 0.0);
    uint i = 0;

    while(pos2.x + pos2.y <= 4.0 && i < maxIt) {
        pos = vec2(
            pos2.x - pos2.y + coords.x,
            2.0 * pos.x * pos.y + coords.y
        );
        pos2 = vec2(
            pos.x * pos.x,
            pos.y * pos.y
        );
        i++;
    }

    return 1.0 - float(i) / maxIt;
}



vec3 hash13(float m) {
    const float x = fract(sin(m) * 5625.246);
    const float y = fract(sin(m + x) * 2216.486);
    const float z = fract(sin(x + y) * 8276.352);
    return vec3(x, y, z);
}


void mainImage() {
    vec2 mdbt = (fragCoord / uvResolution * (mandelbrotRes.zw + abs(mandelbrotRes.xy)) + mandelbrotRes.xy) / fZoom;

    mdbt += fvCenter;

    const float v = mandelbrot(mdbt);

    vec3 color = vec3(0.0);

    if(iMode == 0) {
        color = vec3(smoothColor(map(v, 0.0, 1.0, 0.3, 1.0)));
    }
    else if(iMode == 1) {
        color = hash13(v);
        color = pow(color, vec3(0.45));
    }
    else {
        if(v < 0.5) {
			color = smoothColor(hash13(v));
			color = pow(color, vec3(0.45));
		}
    }   

    fragColor = vec4(color, 1.0);
}