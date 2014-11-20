#version 330

uniform vec4 shadowColor;

uniform sampler2D tex;

// In: per fragment information
in vec4 fPosition;

// Out: final color of this fragmant
out vec4 fragColor;

void main() {
    fragColor = clamp(shadowColor, 0., 1.);
    
    vec4 surfaceColorTmp = texture(tex, vec2(0,0));
}