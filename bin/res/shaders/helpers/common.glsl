/**
 * @author NoxFly
 *
 * Optimizations from wikipedia :
 * https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
 */
const float PI = 3.1415926535;
const float TAU = PI * 2.0;


/**
 * Remap une valeur d'un intervalle à un autre.
 * value : valeur à remapper.
 * start1, stop1 : bornes de l'intervalle d'origine.
 * start2, stop2 : bornes de l'intervalle de destination.
 * Retourne la valeur remappée dans le nouvel intervalle.
 */
float map(float value, float start1, float stop1, float start2, float stop2) {
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

/**
 * Applique une interpolation lissée à une valeur t dans [0, 1].
 * t : valeur à lisser.
 * Retourne la valeur interpolée.
 */
float smoothColor(float t) {
    return t * t * (3.0 - 2.0 * t);
}

/**
 * Applique une interpolation lissée à chaque composante d'un vecteur couleur.
 * color : couleur à lisser (vec3).
 * Retourne la couleur lissée.
 */
vec3 smoothColor(vec3 color) {
	return vec3(smoothColor(color.r), smoothColor(color.g), smoothColor(color.b));
}

/**
 * Génère une valeur flottante pseudo-aléatoire à partir d'un vecteur 2D en entrée.
 *
 * @param p Le vecteur 2D d'entrée.
 * @return Une valeur flottante pseudo-aléatoire dans l'intervalle [0, 1).
 */
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

/**
* Génère un vecteur 2D pseudo-aléatoire à partir d'un vecteur 2D en entrée.
*
* @param p Le vecteur 2D d'entrée.
* @return Un vecteur 2D pseudo-aléatoire, chaque composante dans l'intervalle [0, 1).
*/
vec2 hash2(vec2 p) {
    return fract(sin(vec2(
        dot(p, vec2(127.1, 311.7)),
        dot(p, vec2(269.5, 183.3))
    )) * 43758.5453);
}

/**
 * Génère un bruit pseudo-aléatoire 2D à partir d'une position p.
 * p : position 2D (vec2).
 * Retourne une valeur de bruit dans [0, 1].
 */
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}
