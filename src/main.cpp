#include "ofMain.h"
#include "ofApp.h"


//========================================================================
int main(int argc, char *argv[]){

    int width = 1024;
    int height = 768;
    bool fullscreen = false;
    ofVec2f position = ofVec2f(0,0);
    bool vertSync = true;
    int targetFPS = 30;

    for(int i = 1; i < argc; i++){

        if(ofToString(argv[i]) == "-h"){
            cout << "Proyecto cosmos uso: " << endl;
            cout << "programa [-res x y] [-pos x y] ";
            cout << "[-full] [-noVertSync]" << endl;
            cout << "\t [-targetFPS n] [-h]" << endl << endl;

            cout << "-res\t\tajusta la resolución de la ventana a x y" << endl;
            cout << "-pos\t\tajusta la posición de la ventana a x y" << endl;
            cout << "-full\t\tlanza el programa en pantalla completa (ignorará -res)" << endl;
            cout << "-noVertSync\tdesactiva la sincronización vertical" << endl;
            cout << "-targetFPS\tajusta el FPS objetivo a n cuadros por segundo" << endl;
            cout << "-h\t\tmuestra esta información y finaliza el programa" << endl;
            return 0;

        }

        if(ofToString(argv[i]) == "-res"){
            width = ofToInt(argv[i+1]);
            height = ofToInt(argv[i+2]);
            i+=2;
        }

        if(ofToString(argv[i]) == "-full")
            fullscreen = true;

        if(ofToString(argv[i]) == "-pos"){
            int x = ofToInt(argv[i+1]);
            int y = ofToInt(argv[i+2]);
            position = ofVec2f(x,y);
            i+=2;
        }

        if(ofToString(argv[i]) == "-noVertSync")
            vertSync = false;

        if(ofToString(argv[i]) == "-targetFPS"){
            targetFPS = ofToInt(argv[i+1]);
            i+=1;
        }


    }



    ofGLWindowSettings settings;
    settings.setGLVersion(3,2);
    settings.width = width;
    settings.height = height;
    settings.setPosition(position);
    settings.windowMode = OF_WINDOW;
    if(fullscreen)
        settings.windowMode = OF_FULLSCREEN;

    ofCreateWindow(settings);

    ofApp *app = new ofApp();
    app->args = vector<string>(argv,argv+argc);
    app->sW = settings.width;
    app->sH = settings.height;
    app->fullscreen = fullscreen;
    app->vertSync = vertSync;
    app->targetFPS = targetFPS;
    ofRunApp(app);



}


