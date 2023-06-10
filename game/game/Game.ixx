export module Game;
import MenuState;
import GameState;
import ResourceHolder;
import <iostream>;

export class Game final
{
	const int windowWidth = 1280;
	const int windowHeight = 720;
	sf::RenderWindow window;
	std::unique_ptr<State> state;
	ResourceHolder resources;
	sf::Clock fpsCounter;
	int frames;

	int stateID;

	bool closing()
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				return 1;
		}
		return 0;
	}

	void changeState()
	{
		if (this->stateID != state->getStateID())
		{
			this->stateID = state->getStateID();
			switch (state->getStateID())
			{
			case 0:
				this->state = std::make_unique<MenuState>(resources);
				break;
			case 1:
				this->state = std::make_unique<GameState>(state->getLevel(), resources);
				break;
			}
		}
	}

public:
	Game()
	{
		window.create(sf::VideoMode(windowWidth, windowHeight), "Platformer Game");
		window.setFramerateLimit(30);
	}
	void run()
	{
		this->stateID = 0;
		this->state = std::make_unique<MenuState>(resources);
		while (window.isOpen())
		{
			if (this->closing())
				window.close();

			state->update();
			if (state->closing())
				window.close();
			changeState();

			window.clear();
			state->render(window);
			window.display();

			/*frames++;
			float time = fpsCounter.getElapsedTime().asSeconds();
			if (time >= 1)
			{
				fpsCounter.restart();
				std::cout << frames << '\n';
				frames = 0;
			}*/
		}
	}
};