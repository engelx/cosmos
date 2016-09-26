#version 150

varying vec2 texCoord;
uniform mat4 modelViewProjectionMatrix;
in vec4 position;

void main() {
    gl_Position = modelViewProjectionMatrix * position;
    texCoord = position.xy;
}
