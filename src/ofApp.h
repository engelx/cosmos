#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ayudantes/ImageProcess.h"
#include "ayudantes/seguidores.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void setupGui();
    void update();
    void draw();
    void keyPressed(int key);
    void exit();


    /*variables por parametros*/
    vector<string> args;
    int sW;
    int sH;
    bool fullscreen;
    bool vertSync;
    int targetFPS;
    /*********************/

    /*variables kinect*/
    ofxKinect kinect;
    ofImage imagenKinect;
    /*********************/
    /*funciones kinect*/
    bool iniciarKinect();
    bool leerKinect();
    /*********************/

    /*funcion buscador de contornos*/
    ofImage espacioInicial;
    ofImage fondoRemovido;
    void crearEspacioInicial();
    bool haySujetos;
    /**********************/

    /*variables de trabajo*/
    imageProcess imgProc;
    /**********************/

    /*shader y salida a pantalla*/
    ofShader fusion;
    ofShader estela;
    ofFbo fbo[4];
    /**********************/

    /*videos*/
    ofVideoPlayer video;
    ofDirectory videos;
    int indiceVideoActual;
    int ultimoCambioVideo;
    void cambioVideo(bool cambiar);
    void cambioVideo(int tiempoVivo, int tiempoMilis);

    float movimientoTotal;
    vector<float> movimientoActual;
    int indiceMovimiento;
    int inicioMovimientoMaximo;
    float fadeout;
    float totalidadMascara;

    void calculoMovimiento(float movimiento);
    void calculoMovimiento();
    /**********************/

    /*sonidos*/
    ofSoundPlayer sonido;

    /*buscador de contornos*/
    Seguidor siluetas;
    int tiempoFijo;
    vector<float> distancias;
    int indiceDistancias;
    float distanciaTotal;
    void alteracionSonido(float distancia);
    void volumenSonido();
    /**********************/

    /*funciones y variables de apoyo*/
    ofPoint remapPunto(ofPoint in, ofPoint min, ofPoint max, ofPoint newMin, ofPoint newMax);
    ofPoint remapPuntoKtoS(ofPoint in);

    bool showFPS;
};

