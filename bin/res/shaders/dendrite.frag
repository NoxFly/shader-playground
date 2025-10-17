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

// Fonction de hash pour générer des positions pseudo-aléatoires
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

vec2 hash2(vec2 p) {
    return fract(sin(vec2(
        dot(p, vec2(127.1, 311.7)),
        dot(p, vec2(269.5, 183.3))
    )) * 43758.5453);
}

// Fonction de bruit pour l'aspect organique
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

// Génère une branche dendritique
float dendriteBranch(vec2 p, float angle, float time, int branchId) {
    // Rotation
    float ca = cos(angle);
    float sa = sin(angle);
    vec2 rotP = vec2(ca * p.x - sa * p.y, sa * p.x + ca * p.y);
    
    // Longueur de la branche basée sur le temps
    float growthSpeed = 0.3 + 0.1 * float(branchId % 3);
    float branchLength = time * growthSpeed;
    
    // Distance à la branche principale
    float dist = abs(rotP.y);
    
    // Épaisseur qui diminue le long de la branche
    float thickness = 0.02 * (1.0 - smoothstep(0.0, branchLength, rotP.x));
    
    // Modulation organique si mode 1
    if (iMode == 1) {
        float organicNoise = noise(rotP * 5.0 + fTime * 0.5) * 0.01;
        dist += organicNoise;
        thickness *= (1.0 + noise(rotP * 3.0) * 0.3);
    }
    // Modulation cristalline si mode 2
    else if (iMode == 2) {
        float crystalPattern = abs(sin(rotP.x * 20.0 + fTime)) * 0.005;
        dist += crystalPattern;
        thickness *= 0.8;
    }
    
    // La branche est visible si on est dans sa longueur et épaisseur
    float branch = 0.0;
    if (rotP.x > 0.0 && rotP.x < branchLength) {
        branch = smoothstep(thickness + 0.01, thickness, dist);
    }
    
    return branch;
}

// Génère des sous-branches récursives
float subBranches(vec2 p, float baseAngle, float time, int depth, int branchId) {
    if (depth <= 0 || time < 0.5) return 0.0;
    
    float result = 0.0;
    float timeOffset = float(depth) * 0.5;
    float adjustedTime = max(0.0, time - timeOffset);
    
    // Nombre de sous-branches basé sur la complexité
    int numSubBranches = 2 + (iIncrement % 3);
    
    for (int i = 0; i < numSubBranches; i++) {
        float branchAngle = baseAngle + (float(i) - 0.5) * (PI / 3.0);
        
        // Position de départ de la sous-branche
        float parentLength = adjustedTime * 0.3;
        float branchStartDist = parentLength * (0.3 + 0.4 * hash(vec2(float(depth), float(i))));
        
        vec2 branchStart = vec2(cos(baseAngle), sin(baseAngle)) * branchStartDist;
        vec2 localP = p - branchStart;
        
        float subBranch = dendriteBranch(localP, branchAngle, adjustedTime, branchId * 10 + i);
        result = max(result, subBranch);
    }
    
    return result;
}

// Palette de couleurs pour la dendrite
vec3 dendriteColor(float density, float dist, vec2 uv) {
    vec3 baseColor = vec3(0.1, 0.3, 0.6); // Bleu de base
    
    // Modification selon les flags booléens
    if (vbFlags[0]) baseColor = vec3(0.8, 0.3, 0.1); // Orange
    if (vbFlags[1]) baseColor = vec3(0.2, 0.8, 0.3); // Vert
    if (vbFlags[2]) baseColor = vec3(0.9, 0.1, 0.5); // Rose
    if (vbFlags[3]) baseColor = vec3(0.7, 0.2, 0.8); // Violet
    if (vbFlags[4]) baseColor = vec3(0.1, 0.9, 0.9); // Cyan
    
    // Gradient basé sur la distance au centre
    float centerDist = length(uv);
    vec3 gradientColor = mix(baseColor, baseColor * 1.5, density);
    
    // Mode organique : couleurs plus chaudes
    if (iMode == 1) {
        gradientColor = mix(gradientColor, vec3(0.9, 0.6, 0.3), 0.3);
    }
    // Mode cristallin : couleurs plus froides et brillantes
    else if (iMode == 2) {
        gradientColor = mix(gradientColor, vec3(0.3, 0.7, 1.0), 0.4);
        gradientColor += vec3(0.3) * (1.0 - smoothstep(0.0, 0.01, dist));
    }
    
    return gradientColor;
}

void mainImage() {
    // Coordonnées normalisées avec zoom et centre
    vec2 uv = (fragCoord - uvResolution.xy * 0.5) / (uvResolution.y * 0.5);
    uv = (uv / fZoom) + fvCenter;
    
    // Temps effectif (pause avec espace)
    float effectiveTime = vbKeyPressed[0] ? 0.0 : fTime;
    
    // Complexité basée sur iIncrement
    float complexity = 1.0 + float(iIncrement) * 0.1;
    effectiveTime *= complexity;
    
    // Initialisation
    float dendrite = 0.0;
    float minDist = 1000.0;
    
    // Nombre de branches principales
    int numMainBranches = 6 + (iIncrement % 4);
    
    // Génération des branches principales
    for (int i = 0; i < numMainBranches; i++) {
        float angle = (TAU / float(numMainBranches)) * float(i);
        
        // Offset de temps pour que les branches ne poussent pas toutes en même temps
        float timeOffset = float(i) * 0.3;
        float branchTime = max(0.0, effectiveTime - timeOffset);
        
        // Branche principale
        float branch = dendriteBranch(uv, angle, branchTime, i);
        dendrite = max(dendrite, branch);
        
        // Sous-branches (récursivité simulée avec 2 niveaux)
        float sub1 = subBranches(uv, angle, branchTime, 1, i);
        dendrite = max(dendrite, sub1);
        
        if (iIncrement > 5) {
            float sub2 = subBranches(uv, angle, branchTime, 2, i);
            dendrite = max(dendrite, sub2 * 0.7);
        }
        
        // Calculer la distance minimale pour les effets de lueur
        float ca = cos(angle);
        float sa = sin(angle);
        vec2 rotP = vec2(ca * uv.x - sa * uv.y, sa * uv.x + ca * uv.y);
        float dist = abs(rotP.y);
        minDist = min(minDist, dist);
    }
    
    // Noyau central
    float centerSize = 0.05 + sin(effectiveTime) * 0.01;
    float center = smoothstep(centerSize + 0.01, centerSize - 0.01, length(uv));
    dendrite = max(dendrite, center);
    
    // Lueur autour de la dendrite
    float glow = exp(-minDist * 30.0) * 0.3;
    
    // Couleur finale
    vec3 color = dendriteColor(dendrite + glow, minDist, uv);
    color *= (dendrite + glow * 0.5);
    
    // Fond
    vec3 bgColor = vec3(0.02, 0.02, 0.05);
    
    // Mode organique : fond plus chaud
    if (iMode == 1) {
        bgColor = vec3(0.05, 0.03, 0.02);
    }
    // Mode cristallin : effet de grille
    else if (iMode == 2) {
        float grid = max(
            smoothstep(0.02, 0.0, fract(uv.x * 10.0) - 0.5),
            smoothstep(0.02, 0.0, fract(uv.y * 10.0) - 0.5)
        ) * 0.1;
        bgColor += vec3(0.0, 0.05, 0.1) + vec3(grid);
    }
    
    vec3 finalColor = mix(bgColor, color, dendrite + glow);
    
    // Post-processing : légère vignette
    float vignette = 1.0 - length(uv) * 0.3;
    finalColor *= vignette;
    
    fragColor = vec4(finalColor, 1.0);
}
