#include "ImageProcess.h"


void imageProcess::suma(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, '+');
}

void imageProcess::resta(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, '-');
}

void imageProcess::mult(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, '*');
}

void imageProcess::maximos(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, '>');
}

void imageProcess::minimos(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, '<');
}

void imageProcess::promedio(ofImage &interno, ofImage externo){
    this->operacion(interno, externo, 'p');
}
//------------------------------------------------------------------
void imageProcess::set(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '=');
}

void imageProcess::suma(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '+');
}

void imageProcess::resta(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '-');
}

void imageProcess::mult(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '*');
}

void imageProcess::div(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '/');
}

void imageProcess::maximos(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '>');
}

void imageProcess::minimos(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, '<');
}

void imageProcess::promedio(ofImage &interno, unsigned char scalar){
    this->operacion(interno, scalar, 'p');
}


//--------------------------------------------------------------------
void imageProcess::aplanar(ofImage &interno, unsigned char umbral){
    this->operacion(interno, umbral, 'a');
}

void imageProcess::seccionar(ofImage &interno, unsigned char lejos, unsigned char cerca){
    int x,y,p,l,v[2], c;
    x = interno.getWidth();
    y = interno.getHeight();
    p = interno.getPixels().getBytesPerPixel();
    l = p*x*y;
    c = 0;
    if(lejos < cerca){
        unsigned char i = cerca;
        cerca = lejos;
        lejos = i;
    }
    for(c=0;c<l;c++){
        v[0] = interno.getPixels()[c];
        if(v[0]<cerca || v[0]>lejos){
            interno.getPixels()[c] = 0;
        }
    }
    interno.update();

}

void imageProcess::reconocimientoUsuarios(ofImage &interno, ofImage externo, int tolerancia){
    int x[3],y[3],p[3],px,py,pp,v[3],pos[2];

    x[1] = interno.getWidth();
    x[2] = externo.getWidth();
    y[1] = interno.getHeight();
    y[2] = externo.getHeight();
    p[1] = interno.getPixels().getBytesPerPixel();
    p[2] = externo.getPixels().getBytesPerPixel();


    x[0] = x[1]<x[2]?x[1]:x[2];
    y[0] = y[1]<y[2]?y[1]:y[2];
    p[0] = p[1]<p[2]?p[1]:p[2];

    for(py=0;py<y[0];py++){
        for(px=0;px<x[0];px++){
            for(pp=0;pp<p[0];pp++){
                pos[0] = py*x[1]*p[1]+px*p[1]+pp;
                pos[1] = py*x[2]*p[2]+px*p[2]+pp;
                v[0]=interno.getPixels()[pos[0]];
                v[1]=externo.getPixels()[pos[1]];
                if(v[1]+tolerancia > v[0]){
                    interno.getPixels()[pos[0]] = 0;
                }
            }
        }
    }
    interno.update();

}

void imageProcess::mapSeccion(ofImage &interno, unsigned char min, unsigned char max, unsigned char newMin, unsigned char newMax){
    int x,y,p,l,v[2], c;
    x = interno.getWidth();
    y = interno.getHeight();
    p = interno.getPixels().getBytesPerPixel();
    l = p*x*y;
    c = 0;
    for(c=0;c<l;c++){
        interno.getPixels()[c] = ofMap(interno.getPixels()[c],min,max,newMin,newMax);
    }
    interno.update();
}

unsigned char imageProcess::profundidadPromedio(ofImage interno, ofRectangle area){
    int x,y,p,px,py,pp,c,pos,v;
    x = interno.getWidth();
    y = interno.getHeight();
    p = interno.getPixels().getBytesPerPixel();
    vector<unsigned int> suma;
    suma.push_back(0);
    suma.push_back(0);
    c = 0;
    for(py= area.getTop() ;py < area.getBottom() ;py++){
        for(px= area.getLeft() ;px< area.getRight() ;px++){
            for(pp=0;pp<p;pp++){
                pos = py*x*p+px*p+pp;
                v = interno.getPixels()[pos];
                if(v==0)continue;
                if(suma[suma.size()-2] >= (unsigned int)-256) {
                    suma.push_back(0);
                    suma.push_back(0);
                }
                suma[suma.size()-2] += v;
                suma[suma.size()-1] ++;
            }
        }
    }
    if(suma[1]==0)return 0;
    for(c = 0; c < suma.size(); c+=2){
        suma[c] /= suma[c+1];
        suma[c+1] = 1;
        if(c > 1){
            suma[0]+=suma[c];
            suma[1]++;
        }
    }
    suma[0] /= suma[1];
    return suma[0];
}


//-------------------------------------------------------------------------------
void imageProcess::operacion(ofImage &interno, ofImage externo, char operador){
    int x[3],y[3],p[3],px,py,pp,v[3],pos[2];

    x[1] = interno.getWidth();
    x[2] = externo.getWidth();
    y[1] = interno.getHeight();
    y[2] = externo.getHeight();
    p[1] = interno.getPixels().getBytesPerPixel();
    p[2] = externo.getPixels().getBytesPerPixel();


    x[0] = x[1]<x[2]?x[1]:x[2];
    y[0] = y[1]<y[2]?y[1]:y[2];
    p[0] = p[1]<p[2]?p[1]:p[2];

    for(py=0;py<y[0];py++){
        for(px=0;px<x[0];px++){
            for(pp=0;pp<p[0];pp++){
                pos[0] = py*x[1]*p[1]+px*p[1]+pp;
                pos[1] = py*x[2]*p[2]+px*p[2]+pp;
                v[0]=interno.getPixels()[pos[0]];
                v[1]=externo.getPixels()[pos[1]];
                switch(operador){
                    case '+':
                    v[2] = v[0]+v[1];
                    v[2] = v[2] > 255? 255: v[2];
                    break;

                    case '-':
                    v[2] = v[0]-v[1];
                    v[2] = v[2] < 0? 0: v[2];
                    break;

                    case '*':
                    v[2] = (v[0]*v[1])>>8;
                    v[2] = v[2] > 255? 255: v[2];
                    break;

                    case '>':
                    v[2] = v[0] > v[1]? v[0] : v[1];
                    break;

                    case '<':
                    v[2] = v[0] < v[1]? v[0] : v[1];
                    break;

                    case 'p':
                    v[2] = (v[0]+v[1])/2;
                    break;
                }

                interno.getPixels()[pos[0]] = v[2];
            }
        }
    }
    interno.update();

}

void imageProcess::operacion(ofImage &interno, unsigned char scalar, char operador){
    int x,y,p,l,v[2], c;
    x = interno.getWidth();
    y = interno.getHeight();
    p = interno.getPixels().getBytesPerPixel();
    l = p*x*y;
    c = 0;

    for(c=0;c<l;c++){
        v[0] = v[0]=interno.getPixels()[c];
        switch(operador){
            case '=':
            v[1] = scalar;
            break;

            case '+':
            v[1] = v[0]+scalar;
            v[1] = v[1] > 255? 255: v[1];
            break;

            case '-':
            v[1] = v[0]-scalar;
            v[1] = v[1] < 0? 0: v[1];
            break;

            case '*':
            v[1] = (v[0]*scalar)>>8;
            v[1] = v[1] > 255? 255: v[1];
            break;

            case '/':
            v[1] = v[0]/scalar;
            break;

            case '>':
            v[1] = v[0] > scalar? v[0] : scalar;
            break;

            case '<':
            v[1] = v[0] < scalar? v[0] : scalar;
            break;

            case 'p':
            v[1] = (v[0]+scalar)/2;
            break;

            case 'a':
            v[1] = v[0] > scalar? 1 : 0 ;
            break;
        }
        interno.getPixels()[c] = v[1];

    }

    interno.update();
}

