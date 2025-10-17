/**
 * @author NoxFly
 * 
 * Simulation de croissance dendritique via algorithme DLA (Diffusion-Limited Aggregation)
 * 
 * Contrôles :
 * - Tab : Basculer entre mode géométrique (0), organique (1), et cristallin (2)
 * - I/D : Ajuster la densité/complexité
 * - Touches 0-9 : Activer/désactiver différentes couleurs
 * - Espace : Pause/reprise de la croissance
 * - Shift/Ctrl : Zoom in/out
 * - Flèches : Déplacer la caméra
 */
#version 460 core

#include <helpers/common>
#include <helpers/colorUtils>

// Paramètres de la dendrite
const int MAX_ITERATIONS = 150;
const float BRANCH_THRESHOLD = 0.45;
const float GROWTH_SPEED = 0.3;

/**
 * Calcule la distance au champ de dendrite le plus proche
 * Simule une structure fractale de croissance
 */
float dendriteField(vec2 p, float t, int mode) {
    float dist = length(p); // Distance au centre
    float angle = atan(p.y, p.x);
    
    // Nombre de branches principales selon le mode
    float branches = mode == 0 ? 6.0 : (mode == 1 ? 8.0 : 4.0);
    
    // Pattern de ramification angulaire
    float angularPattern = sin(angle * branches);
    
    // Complexité de la croissance (contrôlée par iIncrement)
    float complexity = 1.0 + float(iIncrement) * 0.1;
    
    // Croissance radiale avec bruit
    float radialGrowth = 0.0;
    for (int i = 0; i < 5; i++) {
        float scale = pow(2.0, float(i)) * complexity;
        vec2 noisePos = p * scale + vec2(t * GROWTH_SPEED);
        radialGrowth += noise(noisePos) / pow(2.0, float(i));
    }
    
    // Fonction de densité selon le mode
    float density;
    if (mode == 0) {
        // Mode géométrique : branches régulières
        density = angularPattern * 0.5 + 0.5;
    } else if (mode == 1) {
        // Mode organique : croissance irrégulière
        density = radialGrowth;
    } else {
        // Mode cristallin : symétrie hexagonale
        float hex = abs(sin(angle * branches)) + abs(cos(angle * branches * 0.5));
        density = hex * radialGrowth;
    }
    
    // Atténuation radiale (croissance limitée dans le temps)
    float maxRadius = t * 0.5 + 0.5;
    float growth = smoothstep(maxRadius, maxRadius - 0.3, dist);
    
    return density * growth;
}

/**
 * Calcule la structure de dendrite avec détails multi-échelles
 */
float dendrite(vec2 p, float t, int mode) {
    float structure = 0.0;
    float amplitude = 1.0;
    vec2 pos = p;
    
    // Pause si espace enfoncé
    float time = vbKeyPressed[0] == 1 ? 0.0 : t;
    
    // Accumulation multi-échelle
    for (int i = 0; i < 4; i++) {
        float scale = pow(2.0, float(i));
        structure += dendriteField(pos * scale, time, mode) * amplitude;
        amplitude *= 0.5;
    }
    
    return structure;
}

/**
 * Calcule la couleur selon la structure de dendrite
 */
vec3 getDendriteColor(float structure, float dist, int mode) {
    vec3 color = vec3(0.0);
    
    // Couleur de base selon le mode
    vec3 baseColor;
    
    if (mode == 0) {
        // Géométrique : bleu-cyan
        baseColor = vec3(0.2, 0.5, 1.0);
    } else if (mode == 1) {
        // Organique : vert-jaune
        baseColor = vec3(0.3, 0.9, 0.4);
    } else {
        // Cristallin : violet-magenta
        baseColor = vec3(0.8, 0.2, 1.0);
    }
    
    // Application des flags de couleur (touches 0-9)
    vec3 colorMod = vec3(1.0);
    if (vbFlags[0] == 1) colorMod.r *= 1.5;
    if (vbFlags[1] == 1) colorMod.g *= 1.5;
    if (vbFlags[2] == 1) colorMod.b *= 1.5;
    if (vbFlags[3] == 1) colorMod *= 0.5; // assombrir
    if (vbFlags[4] == 1) colorMod = 1.0 - colorMod; // inverser
    
    // Couleur en fonction de la distance au centre
    float hue = dist * 50.0 + structure * 100.0;
    vec3 rainbowColor = hsv2rgb(mod(hue, 360.0), 0.8, 1.0);
    
    // Mélange selon les flags
    if (vbFlags[5] == 1) {
        color = rainbowColor * colorMod;
    } else {
        color = baseColor * colorMod;
    }
    
    // Intensité basée sur la structure
    float intensity = smoothstep(BRANCH_THRESHOLD - 0.2, BRANCH_THRESHOLD + 0.1, structure);
    color *= intensity;
    
    // Points lumineux aux extrémités
    float glow = smoothstep(BRANCH_THRESHOLD + 0.15, BRANCH_THRESHOLD + 0.3, structure);
    color += vec3(1.0) * glow * 0.5;
    
    return color;
}

void mainImage() {
    // Coordonnées normalisées centrées
    vec2 uv = (fragCoord - uvResolution * 0.5) / uvResolution.y;
    
    // Application du zoom et du centre de caméra
    uv /= fZoom;
    uv += fvCenter / uvResolution.y;
    
    // Distance au centre
    float dist = length(uv);
    
    // Temps (peut être pausé avec espace)
    float time = vbKeyPressed[0] == 1 ? 0.0 : fTime;
    
    // Calcul de la structure de dendrite
    float structure = dendrite(uv, time, iMode);
    
    // Calcul de la couleur
    vec3 color = getDendriteColor(structure, dist, iMode);
    
    // Fond avec dégradé radial
    vec3 background = vec3(0.02, 0.02, 0.05) * (1.0 - dist * 0.3);
    
    // Composition finale
    color += background;
    
    // Post-processing : vignetage
    if (vbFlags[6] == 1) {
        float vignette = 1.0 - smoothstep(0.5, 1.5, dist);
        color *= vignette;
    }
    
    // Post-processing : glow
    if (vbFlags[7] == 1) {
        color += vec3(0.1, 0.2, 0.3) * smoothstep(0.6, 0.4, dist);
    }
    
    fragColor = vec4(color, 1.0);
}