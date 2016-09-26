#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    //se establecen condiciones de muestra en pantalla
    ofSetVerticalSync(vertSync);
    ofSetFrameRate(targetFPS);
    if(fullscreen){
        sW = ofGetScreenWidth();
        sH = ofGetScreenHeight();
    }

    //inicia kinect y reserva la memoria necesaria
    if(!iniciarKinect()){
        //si falló en iniciarse
        ofLog() << "Finalizando programa";
        ofExit();
        return;
    }
    ofSleepMillis(500);

    //variables que usa el sistema
    cout << "screen res: " << sW << "x" << sH << endl;
    cout << "vertical Sync: " << (vertSync?"true":"false") << endl;
    cout << "target FPS: " << targetFPS << endl;

    //se crea el espacio inicial que contiene la escena vacia
    crearEspacioInicial();

    //se cargan los videos para trabajar

    //indice al video actual


    //lee la carpeta de videos
    videos.listDir("videos/");
    indiceVideoActual = ofRandom(videos.size());

    //se fuerza el inicio del video
    fadeout = 0;
    cambioVideo(false);

    //carga los shader que vamos a usar
    fusion.load("shaders/fusion");
    estela.load("shaders/estela");

    //cargamos y limpiamos los buffer del GPU
    for(int i = 0; i < 4; i++){
        //se aloja el espacio en memoria
        fbo[i].allocate(sW,sH);

        //se abre el buffer para trabajar
        fbo[i].begin();

            //se limpian los bytes del buffer
            ofClear(0,0,0);

        //se cierra el buffer
        fbo[i].end(); 
    }

    //cargamos los sonidos
    ofDirectory sonidos;
    sonidos.listDir("sonidos/");
    sonido.load(sonidos.getPath(0));
    sonido.play();
    sonido.setVolume(1);
    cout << "sound selected: " << sonidos.getPath(0) << endl;

    //variables de captura de silueta
    siluetas.tiempoOlvido = 500; //en milisegundos
    siluetas.distanciaOlvido = 100; //en pixeles base el kinect

    //oculta la información técnica
    showFPS = false;

    //variables para el control de cambio de video
    ultimoCambioVideo = 0;//cuando cambió el video
    totalidadMascara = 0.0;//cuanta mascara será aplicada
    indiceMovimiento = 0;//posición del buffer de movimiento
    movimientoTotal = 0.0;//cuanto movimiento hay

    //llenamos los buffer de movimiento con ceros
    for(int i = 0; i < 30; i++)
        movimientoActual.push_back(0.0);

    //variables para el control de cambio de sonido en base a distancia
    indiceDistancias = 0;//posición del buffer de distancias
    //llenamos los buffer de distancias con ceros
    for(int i = 0; i < 30; i++)
        distancias.push_back(0.0);



}

//--------------------------------------------------------------
void ofApp::update(){

    leerKinect();

    siluetas.procesarImagen(fondoRemovido);//se se buscan las siluetas en la imagen
    video.update();//se actualiza el frame del video

    //se usan 4 buffer de video para realizar el proceso de siluetas e interpolación con el video

    //se dibujarán las siluetas sobre negro
    fbo[0].begin();//se abre el buffer
    ofBackground(0);//se coloca el fondo a negro

    //se obtienen las precencias activas
    vector<Sujeto> presencias = siluetas.getPresencias();

    haySujetos = presencias.size()==0? false:true;

    //se obtienen las distancias promediadas para calcular variaciones de sonido
    float distanciaPromedio = imgProc.profundidadPromedio(
                fondoRemovido,
                ofRectangle(0,0,fondoRemovido.getWidth(), fondoRemovido.getHeight())
                );

    if(presencias.size() == 0) distanciaPromedio = 0;//si no hay presencias, el promedio es 0
    distanciaPromedio /= 256.0; //se ajusta el primedio a una escala de 0 a 1


    //se ajusta el sonido en base a la nueva distancia
    alteracionSonido(distanciaPromedio);

    //se usará para calcular el movmiento total promedio
    float totalMovimiento = 0;

    //se dibujan las siluetas una por una

    //por cada presencia conseguida
    for(int i = 0; i < presencias.size(); i++){
    
        //se analiza si alguien entró y se calcula si debe cambiar el video
        cambioVideo(presencias[i].tiempoUltimo-presencias[i].tiempoInicial, 2000);
    
        //se carga el movimiento de la presencia actual
        totalMovimiento += presencias[i].movimiento();

        //se establece un color en relacion al id de la precencia
        int h = (presencias[i].id*15)%255;
        ofColor color;
        color.setHsb(h,32,255);
        ofSetColor(color);
    
        ofBeginShape();//se inicia el dibujo de la silueta
    
        //por cada punto conseguido
        for(int j = 0; j < presencias[i].silueta.getVertices().size(); j++){
            //se ajusta la posición a la pantalla
            ofPoint temp = remapPuntoKtoS(presencias[i].silueta.getVertices().at(j));
            ofVertex(temp); //se dibuja el vertice
        }
        ofEndShape();//se finaliza la silueta
    }

    //se promedia el movimiento

    totalMovimiento /= presencias.size();


    if(presencias.size() == 0) totalMovimiento = 0;
    
    //se calcula el cambio de opacidad de la mascara en relación al movimiento
    calculoMovimiento(totalMovimiento);

    fbo[0].end();//se cierra el buffer

    //se usa un buffer de intercambio para dibujar las estelas
    fbo[1].begin();//se abre el buffer

        //se almacena el buffer que contiene las estelas del frame anterior
        fbo[2].draw(0,0);

    fbo[1].end();//se cierra el buffer

    //se crea la interpolacion de suavizado de las siluetas
    fbo[2].begin();//se abre el buffer
        estela.begin();//se abre un hilo de procesamiento en GPU

            //se dibuja el buffer de intercambio
            fbo[0].draw(0,0);
            //se pasa como parametro el buffer de intercambio
            estela.setUniformTexture("fbo", fbo[1].getTexture(),1);

        estela.end();//se cierra el hilo de procesamiento en GPU
    fbo[2].end();//se cierra el buffer

    //se realiza la interpolación entre las siluetas y el video
    fbo[3].begin();//se abre el buffer
        fusion.begin();//se abre un hilo de procesamiento en GPU

            //se dibuja las siluetas ya procesadas con la estela
            fbo[2].draw(0,0);

            //se pasa el video como variable
            fusion.setUniformTexture("video", video.getTexture(), 2);

            //se establecen las variables de cambio de mascara
            fusion.setUniform1f("totalidadMascara",totalidadMascara);
            fusion.setUniform1f("fadeout",abs(fadeout));
            fusion.setUniform2f("scrRes",sW,sH);

        fusion.end();//se cierra el hilo de procesamiento en GPU
    fbo[3].end();//se cierra el buffer
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);//se coloca el fondo a negro

    //se dibuja a pantalla el buffer con el video interpolado

    fbo[3].draw(0,0);

    //si se solicita la infomación técnica
    if(showFPS){
        ofSetColor(10);
        ofDrawRectangle(0,0,0,220,20);
        ofSetColor(255);
        ofDrawBitmapString("FPS: "+ofToString(ofGetFrameRate()) , 20, 20);
    }

    //se actualiza el estado de cambio de video

    cambioVideo(false);
    volumenSonido();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==OF_KEY_RETURN){
        cambioVideo(2100, 2000);
    }

    if(key=='m'){
        crearEspacioInicial();
    }

    if(key=='f'){
        fullscreen = !fullscreen;
        ofSetFullscreen(fullscreen);
        ofSleepMillis(100);
        sW = ofGetWindowWidth();
        sH = ofGetWindowHeight();
        for(int i = 0; i < 4; i++){
            //se aloja el espacio en memoria
            fbo[i].clear();
            fbo[i].allocate(sW,sH);

            //se abre el buffer para trabajar
            fbo[i].begin();

                //se limpian los bytes del buffer
                ofClear(0,0,0);

            //se cierra el buffer
            fbo[i].end();
        }
        cout << "screen res: " << sW << "x" << sH << endl;

    }

    if(key=='d'){
        showFPS = !showFPS;
    }

}


//--------------------------------------------------------------
void ofApp::exit(){
    //retornar kinect a su posición original
    kinect.setCameraTiltAngle(0);
    //colocar el led parpadeante
    kinect.setLed(ofxKinect::LED_BLINK_GREEN);
    ofSleepMillis(100);
    //desactivar kinect
    kinect.close();
    sonido.stop();
    sonido.unload();
    video.close();

}

/*****************************************************************/
bool ofApp::iniciarKinect(){
    //se activa el kinect en modo profundidad sin video
    kinect.init(false);
    kinect.open();

    //se asegura que haya un kinect conectado
    if(!kinect.isConnected()) {
        ofLogNotice() << "Kinect no conectado";
        return false;
    }

    //cambia el modo del led
    kinect.setLed(ofxKinect::LED_GREEN);

    //aloja el espacio en memoria para las imagenes usadas
    imagenKinect.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
    espacioInicial.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
    fondoRemovido.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

    //se apunta el kinect hacia el angulo adecuado
    kinect.setCameraTiltAngle(-20);
    while(kinect.getCurrentCameraTiltAngle() > -20){
        ofSleepMillis(500);
    }

    return true;
}

/*****************************************************************/
bool ofApp::leerKinect(){

    //se pide actualización al kinect
    kinect.update();

    //si hay nueva data se continúa
    if(kinect.isFrameNew()){

        //se coloca la información obtenida del kinect en las variables necesarias
        imagenKinect.setFromPixels(kinect.getDepthPixels());
        fondoRemovido.setFromPixels(imagenKinect.getPixels());

        //se remueve el fondo en base al espacioInicial
        imgProc.reconocimientoUsuarios(fondoRemovido,espacioInicial,10);
        return true;
    }
    return false;
}

/*****************************************************************/
void ofApp::crearEspacioInicial(){


    ofLogNotice()<<"calibrando...";

    //se aloja la memoria para el espacioInicial
    if(!espacioInicial.isAllocated())
        espacioInicial.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);


    //variables de trabajo
    int i = 0;
    int pasos = 60;

    //se crean 60 imagenes para promediar
    ofImage proceso[pasos];

    //se leen 60 frames
    while(i < pasos){
        //si hay nueva data desde el kinect
        if(leerKinect()){
            //se aloja el espacio en memoria para las variables de proceso
            proceso[i].allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
            //se coloca la data obtenida del kinect
            proceso[i].setFromPixels(imagenKinect.getPixels());
            i++;
        }
    }

    //se reestablece el espacio inicial para procesarse
    espacioInicial = proceso[0];

    //se sacan los máximos de las imagenes obtenidas y se colocan en espacio inciial
    for(i = 1; i < pasos; i++)
       imgProc.maximos(espacioInicial, proceso[i]);

    ofLogNotice()<<"calibrado";
}

/*****************************************************************/
void ofApp::cambioVideo(bool cambiar){

    //si se solicitó cambiar y no estamos a medio cambio
    if(cambiar && fadeout == 1.0){
        fadeout = 1.0 - 1.0/30.0;
    }

    //si se difuminó completamente la pantalla, iniciamos el siguiente video
    if(fadeout < 1.0/30.0 && fadeout >= 0.0){
        video.stop();

        //apuntamos al siguiente video en la lista
        indiceVideoActual = ofRandom(videos.size());

        //cargamos el video, corremos y lo dejamos en mute
        video.load(videos.getPath(indiceVideoActual));
        video.play();
        video.setVolume(0);

        //guardamos cuando fue cambiado para evitar saltos erraticos
        ultimoCambioVideo = ofGetElapsedTimeMillis();
    }

    //si fue inciado el intercambio, continuar
    if(fadeout < 1.0){
        fadeout -= 1.0/30.0;
    }


    //si finalizó reestablecer, reiniciamos
    if(fadeout < -1.0){
        fadeout = 1.0;
    }


}

/*****************************************************************/
void ofApp::cambioVideo(int tiempoVivo, int tiempoMilis){
    //tiempo de bloqueo para que los videos no salten erraticamente
    int tiempoCambio = ofGetElapsedTimeMillis() - ultimoCambioVideo + 1000;

    //si un sujeto está desde hace "tiempoMilis" y ya salimos del bloqueo de saltos cambiamos video
    if(tiempoVivo > tiempoMilis && tiempoVivo < tiempoMilis+500 && tiempoCambio > tiempoMilis){
        cambioVideo(true);
    }

}

/*****************************************************************/
void ofApp::calculoMovimiento(float movimiento){
    //guardamos la distancia recibida en una posición del buffer de movimiento

    movimientoActual[indiceMovimiento] = movimiento;
    
    //apuntamos a la siguiente posición del buffer para una proxima ocasión
    indiceMovimiento = (indiceMovimiento+1) % movimientoActual.size();

    //el movimiento total será el promedio de lso movimientos almacenadas
    //esto es para generar un cambio suave
    movimientoTotal = 0;
    for(int i = 0; i < movimientoActual.size(); i++)
        movimientoTotal += movimientoActual[i];
    //se promedia

    movimientoTotal /= movimientoActual.size();

    //si es mayor que 0.4 se considera movimiento máximo
    if(movimientoTotal > 0.4 && inicioMovimientoMaximo == 0)
        inicioMovimientoMaximo = ofGetElapsedTimeMillis();
    //si es menor que 0.08 se considera sin movimiento 
    if(movimientoTotal < 0.08 && inicioMovimientoMaximo == 0)
        inicioMovimientoMaximo = ofGetElapsedTimeMillis();
    
    //no cumple las condiciones para cambiar la opacidad
    if(movimientoTotal <= 0.4 && movimientoTotal >= 0.08 || !haySujetos)
        inicioMovimientoMaximo = 0;

    //se calcula el cambio
    calculoMovimiento();
}

/*****************************************************************/
void ofApp::calculoMovimiento(){
    
    //tiempo del movimiento
    int tiempoMovimiento = 0;
    
    //condicion para cambiar
    if(inicioMovimientoMaximo > 0){
        
        
        tiempoMovimiento = ofGetElapsedTimeMillis() - inicioMovimientoMaximo;
        
        //5000ms para iniciar el cambio
        if(tiempoMovimiento > 5000 || totalidadMascara > 0){
            totalidadMascara += 1.0/30.0;
            if(totalidadMascara > 1) totalidadMascara = 1;
        }
    
    }else{
        
        //deshacer el cambio
        if(totalidadMascara == 0) return;
        totalidadMascara -= 1.0/30.0;
        if(totalidadMascara < 0) totalidadMascara = 0;
    }
}

/*****************************************************************/
void ofApp::alteracionSonido(float distancia){

    //guardamos la distancia recibida en una posición del buffer de distancias
    distancias[indiceDistancias] = distancia;

    //apuntamos a la siguiente posición del buffer para una proxima ocasión
    indiceDistancias = (indiceDistancias+1) % distancias.size();

    //la distancia total será el promedio de las distancias almacenadas
    //esto es para generar un cambio suave de sonido
    distanciaTotal = 0;

    //calculo de promedios
    for(int i = 0; i < distancias.size(); i++)
        distanciaTotal += distancias[i];
    distanciaTotal /= distancias.size();

    //el grado de alteración se refleja como 0.5 para una octava más bajo
    //y la mitad de velocidad, y 2 para una octava mas alto t doble velocidad
    //usamos un valor más bajo para una alteración más delicada
    float alteracion = 1.0/6.0;

    //calculamos un pivote de alteración
    alteracion = (1.0-alteracion/2.0)+(distanciaTotal*alteracion);

    //alteramos el sonido
    sonido.setSpeed(alteracion);
}

/*****************************************************************/
void ofApp::volumenSonido(){
    //obtener volumen
    float volumen = sonido.getVolume();

    //si se consiguieron sujetos
    //cambio suave de volumen
    if(!haySujetos){
        volumen -= 1.0/30.0;
        if(volumen < 0) volumen = 0;
    }else{
        volumen += 1.0/30.0;
        if(volumen > 1) volumen = 1;
    }
    //se establece el volumen calculado
    sonido.setVolume(volumen);

}



/*****************************************************************/
ofPoint ofApp::remapPunto(ofPoint in, ofPoint min, ofPoint max, ofPoint newMin, ofPoint newMax){
    ofPoint salida;
    salida.x = ofMap(in.x, min.x, max.x, newMin.x, newMax.x);
    salida.y = ofMap(in.y, min.y, max.y, newMin.y, newMax.y);
    salida.z = ofMap(in.z, min.z, max.z, newMin.z, newMax.z);
    return salida;

}

/*****************************************************************/
ofPoint ofApp::remapPuntoKtoS(ofPoint in){
    return this->remapPunto(in, ofPoint(0,0,0),ofPoint(kinect.width,kinect.height,255),ofPoint(0,0,0),ofPoint(sW,sH,255));
}











