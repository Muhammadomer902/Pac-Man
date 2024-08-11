
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>

using namespace sf;
using namespace std;

class Ghost
{
public:
    Texture TextureGhost;
    Sprite SpriteGhost;
    Ghost *next = nullptr;
    int prevx, prevy, currstate = 1, resident = 0, dir = (rand() % 2 == 0) ? 1 : 3, mytype, myytype;
    bool criticalstate = false, home = true, fastmove;
    Ghost()
    {
    }
    Ghost(int val)
    {
        int y = val / 4;
        fastmove = 0;
        srand(time(0));
        TextureGhost.loadFromFile("ghost.png");
        SpriteGhost.setTexture(TextureGhost);
        SpriteGhost.setPosition( 300,  300);
        SpriteGhost.setScale(0.05, 0.05);
    }
    void speed()
    {
        fastmove = true;
        TextureGhost.loadFromFile("ghost.png");
        SpriteGhost.setTexture(TextureGhost);
    }
    void normal()
    {
        fastmove = false;
        TextureGhost.loadFromFile("ghost.png");
        SpriteGhost.setTexture(TextureGhost);
    }
};

class Ghostlist
{
public:
    Ghost *initial = nullptr;
    int listsize = 0, x = 0, y = 0, change_in_position = 0, change_in_sprite = 0, change_in_position1 = 0, change_in_sprite1 = 0, outofhome = 0, shootdown = 0;

    Ghostlist()
    {
    }
    void create(int i)
    {

        Ghost *create = new Ghost(i);
        if (initial == nullptr)
        {
            initial = create;
        }
        else
        {
            Ghost *temp;
            temp = new Ghost[listsize + 1];
            int i = 0;
            while (i < listsize)
            {
                temp[i] = initial[i];
                i++;
            }
            temp[listsize] = *create;
            initial = temp;
        }
        listsize++;
    }


    void normalize()
    {
        int i = 0;
        while (i < listsize)
            initial[i++].fastmove = 0;
    }

    void focustarget()
    {
        shootdown = listsize;
    }

    void modifystate()
    {
        change_in_sprite = change_in_sprite1 = listsize;
    }

    void movement(int i)
    {
        if (initial && initial[i].home)
        {
            initial[i].SpriteGhost.setPosition(300, 240);
            initial[i].home = false;
        }
    }

    void movementremind(int i)
    {
        if (initial && change_in_position1-- > outofhome)
            initial[i].SpriteGhost.setPosition(300, 240);
    }
    void selfmovementremind(int i)
    {
        if (initial && change_in_sprite1-- > outofhome)
        {
            if (initial[i].currstate == 1)
            {
                initial[i].SpriteGhost.setPosition(initial[i].SpriteGhost.getPosition().x, initial[i].SpriteGhost.getPosition().y - 1);
                initial[i].currstate = 2;
            }
            else
            {
                initial[i].SpriteGhost.setPosition(initial[i].SpriteGhost.getPosition().x, initial[i].SpriteGhost.getPosition().y + 1);
                initial[i].currstate = 1;
            }
        }
    }
    void display(RenderWindow &window)
    {
        if (initial)
        {
            int i = 0;
            while (i < listsize)
            {
                window.draw(initial[i++].SpriteGhost);
            }
        }
    }
    void eatable()
    {
        if (initial)
        {
            int i = 0;
            while (i < listsize)
            {
                initial[i].TextureGhost.loadFromFile("ghostdying.png");
                initial[i].SpriteGhost.setTexture(initial[i++].TextureGhost);
            }
        }
    }
    void uneatable()
    {
        if (initial)
        {
            int i = 0;
            while (i < listsize)
            {
                initial[i].TextureGhost.loadFromFile("ghost.png");
                initial[i].SpriteGhost.setTexture(initial[i++].TextureGhost);
            }
        }
    }
};
