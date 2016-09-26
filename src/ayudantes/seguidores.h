#ifndef SEGUIDORES_H
#define SEGUIDORES_H

#include "ofMain.h"
#include "ofxCv.h"
using namespace ofxCv;
using namespace cv;

/********************************************************************************
*                                                                               *
* Clase Sujeto                                                                  *
* Contiene la información para distiguir un sujeto seguido en el espacio con    *
* variables para identificalo de maneras simples                                *
*                                                                               *
*                                                                               *
*                                                                               *
********************************************************************************/
class Sujeto_{
public:

    //Metodo constructor
    Sujeto_();

    //Un punto que muestra el centro promediado del sujeto según la silueta capturada
    ofPoint centro;
    ofPoint centroFijado;
    ofPoint centroAnterior; //referencia para comparar

    //Un contenedor de 4 puntos que lleva los puntos mas extremos del sujeto
    ofPolyline contenedor;
    ofPolyline contenedorAnterior;

    //Un contenedor de 4 puntos que lleva las coordenadas más distantes arriba, abajo, izquierda y derecha
    ofPolyline cruz;
    ofPolyline cruzAnterior;

    //un contenedor que en gran cantidad de puntos describe al sujeto
    ofPolyline silueta;
    ofPolyline siluetaAnterior;

    //un rectangulo que contiene al sujeto
    ofRectangle caja;
    ofRectangle cajaAnterior;


    int tiempoUltimo;//cuando fue visto por ultima vez
    int tiempoAnterior;//usado para calcular movimiento
    int tiempoInicial;//cuando fue visto inicialmente
    int tiempoVida;//cuanto va a mantenerse vivo desde que no ha sido vista


    bool muerto;//si el seguimiendo ya expiró
    bool visible;//si fue visible el ultimo frame
    int id;//identificador numerico del sujeto

    //funciones de actualización de las variables
    void setTiempoVida(int tiempoVida);

    void update();
    bool update(ofPoint centro,
                ofPolyline contenedor,
                ofPolyline cruz,
                ofPolyline silueta,
                ofRectangle caja
         );
    void iniciar(ofPoint centro,
                 ofPolyline contenedor,
                 ofPolyline cruz,
                 ofPolyline silueta,
                 ofRectangle caja,
                 int tiempoInicial,
                 int tiempoVida,
                 int id,
                 bool visible,
                 bool muerto
         );

    int distanciaFijo;
    int tiempoFijo;

    float movimiento();


};

//creamos el tipo
typedef Sujeto_ Sujeto;

/********************************************************************************
*                                                                               *
* Clase Seguidor                                                                  *
*                                *
*                                                                               *
*                                                                               *
*                                                                               *
********************************************************************************/

class Seguidor_{
public:
    Seguidor_ ();
    ofxCv::ContourFinder buscadorContornos;

    void procesarImagen(ofImage externo);
    int nuevoIdSujeto();

    void setSeguidos();
    vector<Sujeto> Seguidos;
    vector<Sujeto> getPresencias();
    int tiempoOlvido;
    int distanciaOlvido;


private:

    int ultimoId;
    int nuevoIdSeguido();

};


typedef Seguidor_ Seguidor;

#endif // SEGUIDORES_H
