#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "ofImage.h"


class imageProcess{
public:


    void suma (ofImage &interno, ofImage externo);     // +
    void resta (ofImage &interno, ofImage externo);    // -
    void mult (ofImage &interno, ofImage externo);     // *
    void maximos (ofImage &interno, ofImage externo);  // >
    void minimos (ofImage &interno, ofImage externo);  // <
    void promedio (ofImage &interno, ofImage externo); // p


    //operaciones con escalares
    void set (ofImage &interno, unsigned char scalar);  // =
    void suma (ofImage &interno, unsigned char scalar); // +
    void resta(ofImage &interno, unsigned char scalar); // -
    void mult(ofImage &interno, unsigned char scalar);  // *
    void div (ofImage &interno, unsigned char scalar);  // /
    void maximos (ofImage &interno, unsigned char scalar);  // >
    void minimos (ofImage &interno, unsigned char scalar);  // <
    void promedio (ofImage &interno, unsigned char scalar); // p

    //alteraciones especificas
    void aplanar (ofImage &interno, unsigned char umbral);
    void seccionar (ofImage &interno, unsigned char lejos, unsigned char cerca);


    void reconocimientoUsuarios (ofImage &interno, ofImage externo, int tolerancia);
    void mapSeccion (ofImage &interno, unsigned char min, unsigned char max, unsigned char newMin, unsigned char newMax);
    void blancoyNegro(ofImage &interno);

    unsigned char profundidadPromedio(ofImage interno, ofRectangle area);


private:
     void operacion(ofImage &interno, ofImage externo, char operador);
     void operacion(ofImage &interno, unsigned char scalar, char operador);

};

#endif
