/**
 * @author NoxFly
 *
 * Optimizations from wikipedia :
 * https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
 */
vec3 hsv2rgb(float hue, float saturation, float value) {
	vec3 rgb = vec3(0.0);

	int i;
	float f, p, q, t;

	if (saturation == 0) {
		rgb = vec3(value);
		return rgb;
	}

	hue /= 60;
	i = int(floor(hue));
	f = hue - i;
	p = value * (1 - saturation);
	q = value * (1 - saturation * f);
	t = value * (1 - saturation * (1 - f));

	switch(i) {
		case 0:
			rgb = vec3(value, t, p);
			break;

		case 1:
			rgb = vec3(q, value, p);
			break;

		case 2:
			rgb = vec3(p, value, t);
			break;

		case 3:
			rgb = vec3(p, q, value);
			break;

		case 4:
			rgb = vec3(t, p, value);
			break;

		default:
			rgb = vec3(value, p, q);
			break;
	}

	return rgb;
}