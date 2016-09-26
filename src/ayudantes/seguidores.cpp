#include "seguidores.h"

 /*******************************************************************************
 *                                                                              *
 * Contructor:                                                                  *
 * Se predefinen los valores iniciales en el momento de creación del objeto     *
 *                                                                              *
 *                                                                              *
 *******************************************************************************/

Sujeto_::Sujeto_(){
    tiempoInicial = ofGetElapsedTimeMillis();
    tiempoUltimo = tiempoInicial;
    tiempoVida = 0;
    distanciaFijo = 10;
    tiempoFijo = 0;


    muerto = false;
    visible = false;
    id = -1;
}


 /*******************************************************************************
 *                                                                              *
 * setTiempoVida:                                                               *
 * Se asigna el tiempo de vida del sujeto                                       *
 *                                                                              *
 *                                                                              *
 *******************************************************************************/

void Sujeto_::setTiempoVida(int tiempoVida){
    this->tiempoVida = tiempoVida;

}

/********************************************************************************
*                                                                               *
* update:                                                                       *
* Se actualiza el sujeto si no tiene cambios                                    *
* visible es falso                                                              *
*                                                                               *
*                                                                               *
********************************************************************************/

void Sujeto_::update(){
    visible = false;
    if(muerto) return; //si el sujeto está muerto, no hay nada que hacer
    if(ofGetElapsedTimeMillis() - tiempoUltimo > tiempoVida)muerto = true;
}

/********************************************************************************
*                                                                               *
* update:                                                                       *
* Se actualiza el sujeto con cambios                                            *
* visible es verdadero                                                          *
*                                                                               *
*                                                                               *
********************************************************************************/


bool Sujeto_::update(ofPoint centro, ofPolyline contenedor, ofPolyline cruz, ofPolyline silueta, ofRectangle caja){
    if(muerto) return false; //si el sujeto está muerto, no hay nada que hacer
    //se actualiza el sujeto y se almacena el estado anterior


    this->centroAnterior = this->centro;
    this->centro = centro;

    if(this->centro.squareDistance(this->centroFijado) < distanciaFijo*distanciaFijo){

    }else{
        tiempoFijo = 0;
        this->centroFijado.set(0,0,0);
        if(this->centro.squareDistance(this->centroAnterior) < distanciaFijo*distanciaFijo){
            if(tiempoFijo == 0){
                this->centroFijado = this->centro;
                tiempoFijo = ofGetElapsedTimeMillis();
            }
        }
    }



    this->contenedorAnterior = this->contenedor;
    this->contenedor = contenedor;

    this->cruzAnterior = this->cruz;
    this->cruz = cruz;

    this->siluetaAnterior = this->silueta;
    this->silueta = silueta;

    this->cajaAnterior = this->caja;
    this->caja = caja;
    tiempoAnterior = tiempoUltimo;
    tiempoUltimo = ofGetElapsedTimeMillis();
    visible = true;



}

void Sujeto_::iniciar(ofPoint centro, ofPolyline contenedor, ofPolyline cruz, ofPolyline silueta, ofRectangle caja, int tiempoInicial, int tiempoVida, int id, bool visible, bool muerto){
    this->centro = centro;
    this->centroAnterior = this->centro;

    this->contenedor = contenedor;
    this->contenedorAnterior = this->contenedor;

    this->cruz = cruz;
    this->cruzAnterior = this->cruz;

    this->silueta = silueta;
    this->siluetaAnterior = this->silueta;

    this->caja = caja;
    this->cajaAnterior = this->caja;

    this->tiempoInicial = tiempoInicial;
    this->tiempoVida = tiempoVida;
    this->id = id;
    this->visible = visible;
    this->muerto = muerto;
}

float Sujeto_::movimiento(){
    float movimientoPromedio=0;

    movimientoPromedio+= caja.getTopLeft().distance(cajaAnterior.getTopLeft());
    movimientoPromedio+= caja.getBottomRight().distance(cajaAnterior.getBottomRight());
    int j, t = 10000;
    for(int i = 0; i < 4; i++){
        if(cruz.getVertices()[i].y < t){
            t = cruz.getVertices()[i].y;
            j=i;
        }
    }

    movimientoPromedio+= cruz.getVertices()[j].distance(cruz.getClosestPoint(cruz.getVertices()[j]))*2;
    movimientoPromedio/= tiempoUltimo-tiempoAnterior;
    if(movimientoPromedio>2)movimientoPromedio=2;
    return movimientoPromedio;

}



/*******************************************************************************
*                                                                              *
* Contructor:                                                                  *
* Se predefinen los valoes iniciales en el momento de creación del objeto     *
*                                                                              *
*                                                                              *
*******************************************************************************/


Seguidor_::Seguidor_(){
    //datos internos
    tiempoOlvido = 120;
    distanciaOlvido = 30;

    //datos para el buscador de contornos
    buscadorContornos.setMinAreaRadius(20);
    buscadorContornos.setMaxAreaRadius(400);
    buscadorContornos.setThreshold(15);
    buscadorContornos.setFindHoles(true);

    ultimoId = 0;
}

void Seguidor_::procesarImagen(ofImage externo){
    //guardamos la imagen y aplicamos algoritmos de busqueda
    buscadorContornos.findContours(externo.getPixels());
    setSeguidos();
}

int Seguidor_::nuevoIdSujeto(){
    ultimoId++;
    return ultimoId;
}

void Seguidor_::setSeguidos(){

    //obtnemos los conseguidos y precargamos para trabajo
    vector<Sujeto>actual;
    //acutal.resize(buscadorContornos.size());

    for(int i = 0; i < buscadorContornos.size(); i++){
        Sujeto temp;
        temp.iniciar(toOf(buscadorContornos.getCentroid(i)),
                     toOf(buscadorContornos.getConvexHull(i)),
                     toOf(buscadorContornos.getFitQuad(i)),
                     toOf(buscadorContornos.getContour(i)),
                     toOf(buscadorContornos.getBoundingRect(i)),
                     ofGetElapsedTimeMillis(),
                     tiempoOlvido,
                     -1,
                     true,
                     false
                    );
        actual.push_back(temp);
    }

    //actualizamos
    for(int i = 0; i < Seguidos.size(); i++){
        if(Seguidos[i].muerto) continue;//si está muerto, vamos con el siguiente en la lista
        Seguidos[i].update();//actualizamos las condiciones

        for(int j = 0; j < actual.size(); j++){//recorremos los actuales
            if(actual[j].muerto) continue;//si ya fue cargado, saltamos

            //si la distancia es igual o menor a la establecida
            if(Seguidos[i].centro.squareDistance(actual[j].centro)<= distanciaOlvido*distanciaOlvido){
                Seguidos[i].update(actual[j].centro,
                                   actual[j].contenedor,
                                   actual[j].cruz,
                                   actual[j].silueta,
                                   actual[j].caja
                                   );
                actual[j].muerto = true; //control de carga
            }
        }


    }

    for(int i = 0; i < actual.size(); i++){//agregamos los no reconocidos
        if(actual[i].muerto) continue;
        actual[i].id = nuevoIdSujeto();
        Seguidos.push_back(actual[i]);
        actual[i].muerto = true;
    }

}

vector<Sujeto> Seguidor_::getPresencias(){
    vector<Sujeto> temp;
    for(int i = 0; i < Seguidos.size(); i++){
        if(Seguidos[i].muerto) continue;//si está muerto, vamos con el siguiente en la lista
        temp.push_back(Seguidos[i]);
    }
    return temp;
}






