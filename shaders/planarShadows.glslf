#version 330

uniform vec4 shadowColor;

// In: per fragment information
in vec4 fPosition;

// Out: final color of this fragmant
out vec4 fragColor;

void main() {
    fragColor = clamp(shadowColor, 0., 1.);
}