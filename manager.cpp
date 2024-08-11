
#include "game.h"
Game G;

void *engine_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.game_processing(Tnum);
    pthread_exit(NULL);
}

void *interface_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.Graphicaldisplay(Tnum);
    pthread_exit(NULL);
}

void *input_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.IOhandling(Tnum);
    pthread_exit(NULL);
}

void *food_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.food_checker(Tnum);
    pthread_exit(NULL);
}

void *pallet_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.Pallet_confirmation(Tnum);
    pthread_exit(NULL);
}

void *TextureGhost(void *arg)
{
    int Tnum = *((int *)arg);
    G.Ghost_engine(Tnum);
    pthread_exit(NULL);
}

void *boost_t(void *arg)
{
    int Tnum = *((int *)arg);
    G.Ghost_Boost_engine(Tnum);
    pthread_exit(NULL);
}

void play()
{

    for (int i = 0; i < ThreadNumber; i++)
    {
        pthread_mutex_init(&ThreadBusy[i], NULL);
        decrementation[i] = 0;
    }

    for (int i = 0; i < GhostGesture; i++)
    {
        pthread_mutex_init(&GhostsThreadBusy[i], NULL);
        incrementation[i] = 0;
    }

    sem_init(&ghost_unlocking_key, 0, 1);
    sem_init(&booster_unlocking_key, 0, 2);

    srand(time(0));
    pthread_create(&threads[1], NULL, input_t, (void *)(new int(1)));
    pthread_create(&threads[2], NULL, engine_t, (void *)(new int(2)));
    pthread_create(&threads[3], NULL, food_t, (void *)(new int(3)));
    pthread_create(&threads[4], NULL, pallet_t, (void *)(new int(4)));

    for (int i = 0; i < GhostGesture; i++)
    {
        pthread_create(&threadsG[i], NULL, TextureGhost, (void *)(new int(i)));
        pthread_create(&threadsBoost[i], NULL, boost_t, (void *)(new int(i)));
    }
    G.Graphicaldisplay(0);
}

class Menu {
private:
public:
	sf::RenderWindow& window2;
	sf::Font font;
	sf::Text titleText;
	sf::Text menuTexts[2];
	int selectedItemIndex;
	int menuCount;

	sf::Text returnText;
	bool showInstructions;

	Menu(sf::RenderWindow& win) : window2(win), selectedItemIndex(0), menuCount(2), showInstructions(false) {
		if (!font.loadFromFile("PAC-FONT.TTF")) {
			std::cerr << "Failed to load font!" << std::endl;
			
		}

		titleText.setFont(font);
		titleText.setString("PacMan");
		titleText.setCharacterSize(100);
		titleText.setFillColor(sf::Color::White);
		titleText.setPosition(window2.getSize().x / 2 - titleText.getGlobalBounds().width / 2, 60);

		std::string menuStrings[4] = { "Play",  "Exit" };

		for (int i = 0; i < menuCount; ++i) {
			menuTexts[i].setFont(font);
			menuTexts[i].setString(menuStrings[i]);
			menuTexts[i].setCharacterSize(45);
			menuTexts[i].setFillColor(sf::Color::White);
			menuTexts[i].setPosition(window2.getSize().x / 2 - menuTexts[i].getGlobalBounds().width / 2,
				window2.getSize().y / 2 + i * 50);
		}
		menuTexts[0].setFillColor(sf::Color::Green);

		returnText.setFont(font);
		returnText.setString("Back");
		returnText.setCharacterSize(30);
		returnText.setFillColor(sf::Color::White);
		returnText.setPosition(window2.getSize().x / 2 - returnText.getGlobalBounds().width / 2,
			window2.getSize().y - 100); 
	}

	void draw() {
		window2.draw(titleText);
		if (!showInstructions) {
			for (int i = 0; i < menuCount; ++i) {
				window2.draw(menuTexts[i]);
			}
		}
		else {

			window2.draw(returnText);
		}
	}

	void MoveUp() {
		if (selectedItemIndex - 1 >= 0) {
			menuTexts[selectedItemIndex].setFillColor(sf::Color::White);
			selectedItemIndex--;
			menuTexts[selectedItemIndex].setFillColor(sf::Color::Green);
		}
	}

	void MoveDown() {
		if (selectedItemIndex + 1 < menuCount) {
			menuTexts[selectedItemIndex].setFillColor(sf::Color::White);
			selectedItemIndex++;
			menuTexts[selectedItemIndex].setFillColor(sf::Color::Green);
		}
	}

	int getSelectedItemIndex() { return selectedItemIndex; }

	void ShowInstructions(bool show) {
		showInstructions = show;
	}

	bool IsInstructionsShown() {
		return showInstructions;
	}

	bool HandleReturnButton(sf::Vector2f mousePosition) {
		if (returnText.getGlobalBounds().contains(mousePosition)) {
			showInstructions = false;
			return true;
		}
		return false;
	}
};




int startgame() {

	sf::RenderWindow window2(sf::VideoMode(1200, 800), "PacMan");
	Menu menu(window2);

	while (window2.isOpen()) {
		sf::Event event;
		while (window2.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window2.close();
				break;
			case sf::Event::KeyPressed:
				if (!menu.IsInstructionsShown()) {
					if (event.key.code == sf::Keyboard::Up) {
						menu.MoveUp();
					}
					else if (event.key.code == sf::Keyboard::Down) {
						menu.MoveDown();
					}
					else if (event.key.code == sf::Keyboard::Return) {
				
						switch (menu.getSelectedItemIndex()) {
						case 0:
							window2.close();
                            G.window.isOpen();
                            play();    
							break;
						case 1:
							window2.close();
							break;
						}
					}
				}
				else {
					if (event.key.code == sf::Keyboard::Return) {
						menu.ShowInstructions(false);
					}
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (menu.IsInstructionsShown()) {

					if (menu.HandleReturnButton(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
						menu.ShowInstructions(false);
					}
				}
				break;
			}
		}

		window2.clear();
		menu.draw();
		window2.display();
	}

	return 0;
}
int main() {
	startgame();
}
