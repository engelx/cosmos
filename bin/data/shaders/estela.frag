#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect fbo;
varying vec2 texCoord;

void main(){
    //obtenemos los pixeles para procesar
    vec4 texel0 = texture(tex0, texCoord);//silueta
    vec4 texel1 = texture(fbo, texCoord);//estela

    //establecemos 2 buffer de procesamiento para el suavizado
    vec4 texel2 = vec4(vec3(0.0),1.0);
    vec4 texel3 = vec4(vec3(0.0),1.0);

    //se usa una tecnica simple de interpolación para el suavizado
    float exp = 2.0;
    float mult = 1.0;
    float pasos = 2.0;
    float ciclos = 0.0;
    for(float x = exp/-2.0; x <= exp/2.0; x++){
       for(float y = exp/-2.0; y <= exp/2.0; y++){
           for(float p = 1.0; p <= pasos; p++){
               texel2.rgb += texture(tex0, texCoord+vec2(x*mult*p,y*mult*p)).rgb;
               texel3.rgb += texture(fbo, texCoord+vec2(x*mult*p,y*mult*p)).rgb -  16.0/256.0;
               ciclos += 1.0;
           }
       }
    }
    texel2.rgb /= ciclos;
    texel3.rgb /= ciclos;

    //el valor final es el maximo entre los valores interpolados de la silueta y estela
    texel2.rgb = max(texel2.rgb, texel3.rgb);
    gl_FragColor = texel2;
}
