
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <SFML/Graphics.hpp>
#include "pac_man_character.h"
#include "maze.h"
#include "Point.h"
#include "Ghost.h"
#include "PowerPallet.h"
#include <cmath>
#include <time.h>
using namespace std;
using namespace sf;

#include <SFML/Graphics.hpp>



void displayGameOverWindow(RenderWindow &window)
{

    sf::Font font;
    if (!font.loadFromFile("PAC-FONT.TTF"))
    {

        return;
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setPosition(300, 200);

    while (true)
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();

        window.draw(gameOverText);

        window.display();
    }
}

const int GhostSpritespeedincrease = 2, GhostGesture = 5, ThreadNumber = 5;
int incrementation[GhostGesture], decrementation[ThreadNumber];
sem_t ghost_unlocking_key, booster_unlocking_key;
pthread_t threadsBoost[GhostGesture], threadsG[GhostGesture], threads[ThreadNumber];
pthread_mutex_t GhostsThreadBusy[GhostGesture], ThreadBusy[ThreadNumber];

void locker_structure1(int num)
{
    pthread_mutex_lock(&GhostsThreadBusy[num]);
    int maxe = -1;
    for (int i = 0; i < GhostGesture; i++)
    {
        if (incrementation[i] > maxe)
        {
            maxe = incrementation[i];
        }
    }
    maxe++;
    incrementation[num] = maxe;
    pthread_mutex_unlock(&GhostsThreadBusy[num]);

    for (int i = 0; i < GhostGesture; i++)
    {

        pthread_mutex_lock(&GhostsThreadBusy[i]);
        pthread_mutex_unlock(&GhostsThreadBusy[i]);

        while ((incrementation[i] != 0) && (incrementation[i] < incrementation[num]))
        {
        }
    }
}
void unlocker_structure1(int num)
{

    incrementation[num] = 0;
    usleep(10000);
}
void locker_structure2(int num)
{
    pthread_mutex_lock(&ThreadBusy[num]);
    int maxe = -1;
    for (int i = 0; i < ThreadNumber; i++)
    {
        if (decrementation[i] > maxe)
        {
            maxe = decrementation[i];
        }
    }
    maxe++;
    decrementation[num] = maxe;
    pthread_mutex_unlock(&ThreadBusy[num]);

    for (int i = 0; i < ThreadNumber; i++)
    {

        pthread_mutex_lock(&ThreadBusy[i]);
        pthread_mutex_unlock(&ThreadBusy[i]);

        while ((decrementation[i] != 0) && (decrementation[i] < decrementation[num]))
        {
        }
    }
}
void unlocker_structure2(int num)
{
    decrementation[num] = 0;
    usleep(10000);
}

class Game
{
public:
    RenderWindow window;
    Clock mouth_open, automoving, boostT, lifeT, ghostselfmove, movement, moveout, pallettimer, eatghosts, ghostkeyT;
    Pacman player;
    Maze maze;
    Food food;
    Event keypress;
    Ghostlist ghosts;
    PowerPallet pallet;
    Font font;
    Text lifedisplay, scores;
    int previousmove = 0, score = 0, validitycount = 0, targetx = player.pacman_S.getPosition().x, targety = player.pacman_S.getPosition().y, lives = 1;
    bool newEvent, activepallet, firstappearance = true, ghostlaunched = false, teleported = false;

    Game() : window(sf::VideoMode(1420, 800), "Pac-Man 22i-0921, 22i-1203")
    {
        font.loadFromFile("PAC-FONT.TTF");
        for (int i = 0; i < GhostGesture; i++)
        {
            ghosts.create(i);
        }
        pallet.Visible = false;
        newEvent = 0;
    }

    bool walldetector(const Sprite &pacman, const Sprite *walls, int wallcounts)
    {
        FloatRect pacmanBounds = pacman.getGlobalBounds();
        int i = 0;
        while (i < wallcounts)
        {
            FloatRect wallBounds = walls[i].getGlobalBounds();
            if (pacmanBounds.intersects(wallBounds))
            {
                return true;
            }
            i++;
        }
        return false;
    }

    void activatePower()
    {
        FloatRect pacmanBounds = player.pacman_S.getGlobalBounds(), wallBounds = pallet.pallet_s.getGlobalBounds();
        if (pacmanBounds.intersects(wallBounds) && pallet.Visible)
        {
            pallet.Visible = false;
            score += 10;
            pallettimer.restart();
            eatghosts.restart();
            ghosts.eatable();
            activepallet = true;
        }
    }

    void PacmanMovement(Event &event)
    {
        int PosY = player.pacman_S.getPosition().y, PosX = player.pacman_S.getPosition().x, moveAmount = 5;
        if (event.key.code == sf::Keyboard::Up)
        {
            //
            player.pacman_T.loadFromFile("pac-manu.png");
            //
            player.pacman_S.setPosition(PosX, PosY - 10);
            if (!walldetector(player.pacman_S, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(player.pacman_S, maze.SpritevWalls, maze.n_vertical))
            {
                previousmove = player.direction;
                player.direction = 2;
            }
            else
            {
                player.pacman_S.setPosition(PosX, PosY);
            }
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            //
            player.pacman_T.loadFromFile("pac-mand.png");
            //
            player.pacman_S.setPosition(PosX, PosY + 10);
            if (!walldetector(player.pacman_S, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(player.pacman_S, maze.SpritevWalls, maze.n_vertical))
            {

                previousmove = player.direction;
                player.direction = 4;
            }
            else
            {
                player.pacman_S.setPosition(PosX, PosY);
            }
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            //
            player.pacman_T.loadFromFile("pac-manl.png");
            //
            player.pacman_S.setPosition(PosX - 10, PosY);
            if (!walldetector(player.pacman_S, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(player.pacman_S, maze.SpritevWalls, maze.n_vertical))
            {

                previousmove = player.direction;
                player.direction = 3;
            }
            else
            {
                player.pacman_S.setPosition(PosX, PosY);
            }
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            //
            player.pacman_T.loadFromFile("pac-manr.png");
            //
            player.pacman_S.setPosition(PosX + 10, PosY);
            if (!walldetector(player.pacman_S, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(player.pacman_S, maze.SpritevWalls, maze.n_vertical))
            {

                previousmove = player.direction;
                player.direction = 1;
            }
            else
            {
                player.pacman_S.setPosition(PosX, PosY);
            }
        }
    }

    void PacmanMovementautomated()
    {
        int PosY = player.pacman_S.getPosition().y;
        int PosX = player.pacman_S.getPosition().x;
        int moveAmount = 10;

        switch (player.direction)
        {
        case 1:
            player.pacman_S.setPosition(PosX + moveAmount, PosY);
            break;

        case 2:
            player.pacman_S.setPosition(PosX, PosY - moveAmount);
            break;

        case 3:
            player.pacman_S.setPosition(PosX - moveAmount, PosY);
            break;

        case 4:
            player.pacman_S.setPosition(PosX, PosY + moveAmount);
            break;

        default:
            break;
        }

        if (walldetector(player.pacman_S, maze.SpritehWalls, maze.n_horizontal) ||
            walldetector(player.pacman_S, maze.SpritevWalls, maze.n_vertical))
        {
            player.pacman_S.setPosition(PosX, PosY);
        }
        else
        {
            previousmove = player.direction;
        }
    }

    void checkfood()
    {
        int index = IncrementScore(player.pacman_S, food.spritefood, food.n_food);
        if (index != -1)
        {
            food.valid[index] = 0;
            score += 50;
            validitycount++;

            while (validitycount == food.n_food)
            {
                int i = 0;
                while (i < food.n_food)
                {
                    food.valid[i] = 1;
                    i++;
                }
                validitycount = 0;
            }
        }
    }

    int IncrementScore(const Sprite &pacman, const Sprite *walls, int wallcounts)
    {
        FloatRect pacmanBounds = pacman.getGlobalBounds();
        int i = 0;
        while (i < wallcounts)
        {
            FloatRect wallBounds = walls[i].getGlobalBounds();
            if (pacmanBounds.intersects(wallBounds) && food.valid[i] != 0)
            {
                if (!firstappearance)
                {
                    firstappearance = true;
                    pallettimer.restart();
                }
                return i;
            }
            i++;
        }
        return -1;
    }
   
    bool Ghostwall_detection(Ghost *org)
    {
        if (ghosts.initial)
        {
            Ghost *curr = ghosts.initial;
            FloatRect pacmanBounds = org->SpriteGhost.getGlobalBounds();
            while (curr)
            {
                if (org != curr)
                {
                    FloatRect wallBounds = curr->SpriteGhost.getGlobalBounds();
                    if (pacmanBounds.intersects(wallBounds))
                    {
                        return true;
                    }
                }
                curr = curr->next;
            }
        }
        if (org->SpriteGhost.getPosition().x > maze.centerx - 100 && org->SpriteGhost.getPosition().x < maze.centerx + 100 && org->SpriteGhost.getPosition().y < maze.centery + 80 && org->SpriteGhost.getPosition().y > maze.centery + 20 && !org->home)
        {
            return true;
        }
        return false;
    }

    void Predict_hit()
    {
        int PosY = player.pacman_S.getPosition().y, PosX = player.pacman_S.getPosition().x;
        Pacman temp;
        temp.pacman_S.setPosition(PosX, PosY);
        while (true)
        {
            switch (player.direction)
            {
            case 1: // Right
                temp.pacman_S.setPosition(temp.pacman_S.getPosition().x + 1, PosY);
                break;
            case 2: // Up
                temp.pacman_S.setPosition(PosX, temp.pacman_S.getPosition().y - 1);
                break;
            case 3: // Left
                temp.pacman_S.setPosition(temp.pacman_S.getPosition().x - 1, PosY);
                break;
            case 4: // Down
                temp.pacman_S.setPosition(PosX, temp.pacman_S.getPosition().y + 1);
                break;
            default:
                break;
            }

            if (walldetector(temp.pacman_S, maze.SpritehWalls, maze.n_horizontal) ||
                walldetector(temp.pacman_S, maze.SpritevWalls, maze.n_vertical))
            {
                targetx = temp.pacman_S.getPosition().x;
                targety = temp.pacman_S.getPosition().y;
                return;
            }
        }
    }

    void MovementGhost(Ghost *curr)
    {
        X_axis_search(curr);
        Y_axis_search(curr);
    }

    void Ghostmovementautomated(int i)
    {
        if (ghosts.initial && ghosts.shootdown > 0)
        {
            ghosts.shootdown -= 1;
            Ghost *curr = &ghosts.initial[i];
            curr->prevx = curr->SpriteGhost.getPosition().x;
            curr->prevy = curr->SpriteGhost.getPosition().y;
            MovementGhost(curr);
        }
    }
 
    int sign(int x)
    {
        return (x > 0) - (x < 0);
    }

    bool X_axis_search(Ghost *curr)
    {
        int PosY = curr->SpriteGhost.getPosition().y;
        int PosX = curr->SpriteGhost.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;

        switch (sign(targetx - PosX))
        {
        case 1: // Target is to the right
            PosX += 2;
            if (curr->fastmove)
            {
                PosX += GhostSpritespeedincrease;
            }
            break;
        case -1: // Target is to the left
            PosX -= 2;
            if (curr->fastmove)
            {
                PosX -= GhostSpritespeedincrease;
            }
            break;
        default: // Target is on the same vertical line
            switch (sign(targety - PosY))
            {
            case 1: // Target is below
                PosY += 2;
                if (curr->fastmove)
                {
                    PosY += GhostSpritespeedincrease;
                }
                break;
            case -1: // Target is above
                PosY -= 2;
                if (curr->fastmove)
                {
                    PosY -= GhostSpritespeedincrease;
                }
                break;
            }

            curr->SpriteGhost.setPosition(orgposX, PosY);
            if (!walldetector(curr->SpriteGhost, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(curr->SpriteGhost, maze.SpritevWalls, maze.n_vertical))
            {
                curr->SpriteGhost.setPosition(PosX, PosY);
                return true;
            }
            curr->SpriteGhost.setPosition(orgposX, orgposY);
            return false;
        }

        curr->SpriteGhost.setPosition(PosX, orgposY);
        if (!walldetector(curr->SpriteGhost, maze.SpritehWalls, maze.n_horizontal) &&
            !walldetector(curr->SpriteGhost, maze.SpritevWalls, maze.n_vertical))
        {
            curr->SpriteGhost.setPosition(PosX, PosY);
            return true;
        }
        curr->SpriteGhost.setPosition(orgposX, orgposY);
        return false;
    }

    bool Y_axis_search(Ghost *curr)
    {
        int PosY = curr->SpriteGhost.getPosition().y;
        int PosX = curr->SpriteGhost.getPosition().x;
        int orgposY = PosY;
        int orgposX = PosX;

        switch (sign(targety - PosY))
        {
        case 1: // Target is below
            PosY += 2;
            break;
        case -1: // Target is above
            PosY -= 2;
            break;
        default: // Target is on the same horizontal line
            switch (sign(targetx - PosX))
            {
            case 1: // Target is to the right
                PosX += 2;
                break;
            case -1: // Target is to the left
                PosX -= 2;
                break;
            }

            // Check if moving horizontally hits a wall or ghost
            curr->SpriteGhost.setPosition(PosX, orgposY);
            if (!walldetector(curr->SpriteGhost, maze.SpritehWalls, maze.n_horizontal) &&
                !walldetector(curr->SpriteGhost, maze.SpritevWalls, maze.n_vertical) &&
                !Ghostwall_detection(curr))
            {
                curr->SpriteGhost.setPosition(PosX, PosY);
                return true;
            }
            curr->SpriteGhost.setPosition(orgposX, orgposY);
            return false;
        }

        // Check if moving vertically hits a wall or ghost
        curr->SpriteGhost.setPosition(orgposX, PosY);
        if (!walldetector(curr->SpriteGhost, maze.SpritehWalls, maze.n_horizontal) &&
            !walldetector(curr->SpriteGhost, maze.SpritevWalls, maze.n_vertical) &&
            !Ghostwall_detection(curr))
        {
            curr->SpriteGhost.setPosition(PosX, PosY);
            return true;
        }
        curr->SpriteGhost.setPosition(orgposX, orgposY);
        return false;
    }

    void Initially_homeexpulsion(int i)
    {
        int key_c = 0;
        switch (key_c)
        {
        case 0:
        case 1:
            while (key_c < 2)
            {
                sem_wait(&ghost_unlocking_key);
                key_c++;
                usleep(10000);
            }
            break;
        }

        ghosts.outofhome += 1;
        if (ghosts.initial)
        {
            Ghost *curr = &(ghosts.initial[i]);
            curr->SpriteGhost.setPosition(maze.centerx + 20 - 30 * i, maze.centery - 5 * (rand() % 5));
            curr->dir = rand() % 4 + 1;
            curr->home = false;
        }
    }

    void GameHomemanagement(int i)
    {
        if (ghosts.initial)
        {
            Ghost *curr = &(ghosts.initial[i]);
            switch (curr->home)
            {
            case true:
                curr->SpriteGhost.setPosition(maze.centerx + 20 - 30 * i, maze.centery - 5 * (rand() % 5));
                curr->dir = rand() % 4 + 1;
                curr->home = false;
                break;
            }
        }
    }

    void plantpallet()
    {
        do
        {
            for (int i = 0; i < food.n_food; i++)
            {
                if (rand() % food.n_food == 0)
                {
                    pallet.Visible = true;
                    pallet.pallet_s.setPosition(food.food_coords[i][0] - 15, food.food_coords[i][1] - 15);
                    return;
                }
            }
        } while (!pallet.Visible);
    }

    bool pacmanhitsghost(Ghost *curr)
    {
        FloatRect pacmanBounds = player.pacman_S.getGlobalBounds();
        FloatRect wallBounds = curr->SpriteGhost.getGlobalBounds();
        return pacmanBounds.intersects(wallBounds) ? true : false;
    }

    void pacmanhitdetectionghost(int i)
    {
        Ghost *curr = &ghosts.initial[i];
        if (pacmanhitsghost(curr))
        {
            if (activepallet)
            {
                curr->TextureGhost.loadFromFile("ghosts.png");
                curr->SpriteGhost.setTexture(curr->TextureGhost);
                curr->SpriteGhost.setPosition(curr->mytype * 80 + maze.centerx - 100, curr->myytype * 60 + maze.centery + 100);
                curr->home = true;
                score += 150;
            }
            else
            {
                switch (lifeT.getElapsedTime().asSeconds() >= 5)
                {
                case true:
                    Ghost *newcurr = &ghosts.initial[i];
                    newcurr->SpriteGhost.setPosition(newcurr->mytype * 80 + maze.centerx - 100, newcurr->myytype * 60 + maze.centery + 100);
                    newcurr->home = true;
                    lives--;
                    lifeT.restart();
                    break;
                }
                return;
            }
        }
    }

    void Graphicaldisplay(int Tnum)
    {
        while (window.isOpen())
        {
            window.clear();
            window.draw(player.pacman_S);

            float time_left = pallettimer.getElapsedTime().asSeconds();
            switch (!firstappearance)
            {
            case true:
                time_left = 15.0;
                break;
            }

            float val = (time_left) / 15.0;
            switch (val >= 1 && !pallet.Visible)
            {
            case true:
                val = 0;
                break;
            }

            switch (val >= 1 && pallet.Visible)
            {
            case true:
                val = 1;
                break;
            }

            lifedisplay.setFont(font);
            scores.setFont(font);
            lifedisplay.setCharacterSize(34);
            scores.setCharacterSize(34);
            lifedisplay.setColor(Color::White);
            scores.setColor(Color::White);
            lifedisplay.setString("Life: " + to_string(lives));
            scores.setString("Score: " + to_string(score));
            lifedisplay.setPosition(1250, 200);
            scores.setPosition(1250, 400);
            ghosts.display(window);
            pallet.display(window);
            maze.display(window, validitycount, food.n_food, val, lives);
            food.display(window);
            window.draw(scores);
            window.draw(lifedisplay);
            window.display();
            usleep(10000);
        }
    }

    void IOhandling(int Tnum)
    {
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                locker_structure2(Tnum);
                keypress = event;
                newEvent = 1;
                unlocker_structure2(Tnum);
            }
        }
    }

    void game_processing(int Tnum)
    {
        while (window.isOpen())
        {
            locker_structure2(Tnum);

            switch (newEvent)
            {
            case 1:
                newEvent = 0;
                switch (keypress.type)
                {
                case Event::Closed:
                    window.close();
                    break;
                case Event::KeyPressed:
                    PacmanMovement(keypress);
                    break;
                }
                break;
            }

            if (ghosts.outofhome < GhostGesture)
            {
                int i = 0;
                while (i < GhostGesture)
                {
                    if (ghosts.initial[i].home == 1)
                    {
                        ghosts.selfmovementremind(i);
                        ghosts.movementremind(i);
                    }
                    i++;
                }
            }

            if (ghosts.outofhome > 0)
            {
                ghostlaunched = true;
            }

            if (ghosts.outofhome >= GhostGesture)
            {
                int i = 0;
                while (i < GhostGesture)
                {
                    GameHomemanagement(i);
                    i++;
                }
            }

            if (boostT.getElapsedTime().asSeconds() >= 10)
            {
                ghosts.normalize();
                boostT.restart();
            }

            if (automoving.getElapsedTime().asSeconds() >= 0.03)
            {
                PacmanMovementautomated();
                automoving.restart();
            }

            if (ghostselfmove.getElapsedTime().asSeconds() >= 0.06)
            {
                ghosts.modifystate();
                ghostselfmove.restart();
            }

            int value;
            sem_getvalue(&ghost_unlocking_key, &value);
            if (ghostkeyT.getElapsedTime().asSeconds() >= 1.5 && value == 0)
            {
                sem_post(&ghost_unlocking_key);
                ghostkeyT.restart();
            }

            if (movement.getElapsedTime().asSeconds() > 0.0405f)
            {
                movement.restart();
            }

            if (movement.getElapsedTime().asSeconds() > 0.010f && !teleported && ghostlaunched)
            {
                if (previousmove != player.direction)
                {
                    Predict_hit();
                }
                ghosts.focustarget();
            }

            if (lives <= 0)
            {
                displayGameOverWindow(window);
                cout << "Game over" << endl;
            }

            unlocker_structure2(Tnum);
        }
    }

    void food_checker(int Tnum)
    {
        while (window.isOpen())
        {
            locker_structure2(Tnum);
            checkfood();
            unlocker_structure2(Tnum);
        }
    }

    void Ghost_engine(int Tnum)
    {
        Initially_homeexpulsion(Tnum);
        while (window.isOpen())
        {
            locker_structure1(Tnum);
            //ghosts.initial[Tnum].SpriteGhost.setPosition(300,240);
            ghosts.movement(Tnum);
            Ghostmovementautomated(Tnum);
            pacmanhitdetectionghost(Tnum);
            unlocker_structure1(Tnum);
        }
    }

    void Ghost_Boost_engine(int Tnum)
    {
        while (window.isOpen())
        {
            sem_wait(&booster_unlocking_key);
            ghosts.initial[Tnum].speed();
            while (ghosts.initial[Tnum].fastmove)
            {
                sleep(1);
            }
            sem_post(&booster_unlocking_key);
            ghosts.initial[Tnum].normal();
            usleep(10000);
        }
    }

    void Pallet_confirmation(int Tnum)
    {
        while (window.isOpen())
        {
            locker_structure2(Tnum);
            activatePower();

            switch (pallettimer.getElapsedTime().asSeconds() >= 15.0f && !pallet.Visible)
            {
            case true:
                plantpallet();
                break;
            }

            float eatghostsTime = eatghosts.getElapsedTime().asSeconds();
            switch (eatghostsTime > 9.0f && eatghostsTime < 9.3f)
            {
            case true:
                ghosts.uneatable();
                activepallet = false;
                break;
            }

            unlocker_structure2(Tnum);
        }
    }
};


