const float PI = 3.1415926535;
const float TAU = PI * 2.0;


float map(float value, float start1, float stop1, float start2, float stop2) {
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

float smoothColor(float t) {
    return t * t * (3.0 - 2.0 * t);
}

vec3 smoothColor(vec3 color) {
	return vec3(smoothColor(color.r), smoothColor(color.g), smoothColor(color.b));
}