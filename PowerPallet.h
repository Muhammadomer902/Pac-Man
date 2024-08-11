
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>
using namespace std;
using namespace sf;
class PowerPallet
{
public:
    Texture pallet_t;
    Sprite pallet_s;
    bool Visible=false;
    PowerPallet(){
        pallet_t.loadFromFile("cherry.png");
        pallet_s.setTexture(pallet_t);
        pallet_s.setScale(0.15,0.15);
    }
    void display(RenderWindow& window){
        if(Visible){
            window.draw(pallet_s);
        }
    }

};
