
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
class Pacman
{

public:
    Texture pacman_T;
    Sprite pacman_S;
    int direction = 1; 
    Pacman()
    {
        pacman_T.loadFromFile("pac-manr.png");
        pacman_S.setTexture(pacman_T);
        pacman_S.setScale(0.1f, 0.1f);
        pacman_S.setPosition(300, 485);
        
    }
};
