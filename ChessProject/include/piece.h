#pragma once
#include "board.h"



class piece{
    PieceType type;
    PieceColor color;
    int x;
    int y;

    public:

    //constructores
    piece(int X,int Y,PieceType t,PieceColor c):x(X),y(Y),type(t),color(c){}

    piece(const piece& other):x(other.x),y(other.y),type(other.type),color(other.color){}

    ~piece(){delete this;}


    PieceType getType(){return this->type;}
    PieceColor getColor(){return this->color;}
    int getX(){return this->x;}
    int getY(){return this->y;}

    void setPos(int X, int Y){
        this->x=X;
        this->y=Y;
    }

    void draw();//imprime en pantalla el objeto dependiendo del tipo y color

};