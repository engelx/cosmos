#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect video;
varying vec2 texCoord;
uniform float totalidadMascara;
uniform float fadeout;
uniform vec2 scrRes;


void main(){
    //se invierte la coordenada horizontal para crear un efecto de espejo
    vec2 espejo = textureSize(tex0);
    espejo = vec2(espejo.x - texCoord.x,texCoord.y);
    
    vec2 scalar = scrRes/textureSize(video);

    vec4 texel0 = texture(tex0, espejo); //siluetas + estela
    vec4 texel1 = texture(video, texCoord/scalar);//video
    
    //calculo de disolvencia de la mascara
    texel0.rgb = clamp(texel0.rgb + totalidadMascara,vec3(0.0),vec3(1.0));

    texel1.rgb = texel1.rgb+1.0-fadeout;
    texel1.rgb = clamp(texel1.rgb,vec3(0.0),vec3(1.0));
    //interpolacion simple entre silueta y estela
    vec4 texel2 = texel0 * texel1;
    
    //disolvencia de la imagen para intercambio suave de videos


    gl_FragColor = texel2;

}
