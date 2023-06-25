export module GameState;
export import State;
import Background;
import Player;
import BrickBlock;
import Slime;
import FinishFlag;
import HealthBar;
import MyText;
import Enums;
import Player;

import <iostream>;
import <fstream>;
import <sstream>;
import <thread>;

export class GameState final : public State
{
	sf::View view;
	std::list<std::shared_ptr<GameObject>> objectsList;
	std::list<std::shared_ptr<GameObject>> newlyAdded;
	std::shared_ptr<Player> playerPtr;
	std::shared_ptr<FinishFlag> finishPtr;
	HealthBar healthBar;
	sf::Text gameOver;
	bool playerDied = 0;
	bool levelEnding = 0;
	sf::Clock levelEndingDelay;

	void addToList(int id, double x, double y, ResourceHolder& resources)
	{
		switch (id)
		{
		case 0:
			this->playerPtr = std::make_shared<Player>(x, y, resources);
			this->objectsList.push_back(playerPtr);
			break;
		case 1:
			this->objectsList.push_back(std::make_shared<BrickBlock>(x, y, resources));
			break;
		case 2:
			this->objectsList.push_back(std::make_shared<Slime>(x, y, resources, playerPtr));
			break;
		case 9:
			this->finishPtr = std::make_shared<FinishFlag>(x, y, resources);
			this->objectsList.push_back(finishPtr);
			break;
		}
	}

	void determineCollision(GameObject* o1, GameObject* o2)
	{
		sf::FloatRect position1 = o1->getHitbox();
		sf::FloatRect nextPosition1 = position1;
		nextPosition1.left += o1->getVelocity().x;
		nextPosition1.top += o1->getVelocity().y;
		sf::FloatRect position2 = o2->getHitbox();
		sf::FloatRect nextPosition2 = position2;
		nextPosition2.left += o2->getVelocity().x;
		nextPosition2.top += o2->getVelocity().y;
		if (nextPosition1.intersects(nextPosition2))
		{
			if (position1.top <= position2.top
				&& position1.top + position1.height <= position2.top + position2.height
				&& position1.left < position2.left + position2.width
				&& position1.left + position1.width > position2.left)
			{
				o1->collision(o2, CollisionSide::bottom);
				o2->collision(o1, CollisionSide::top);
			}
			else if (position1.top > position2.top
				&& position1.top + position1.height > position2.top + position2.height
				&& position1.left < position2.left + position2.width
				&& position1.left + position1.width > position2.left)
			{
				o1->collision(o2, CollisionSide::top);
				o2->collision(o1, CollisionSide::bottom);
			}
			else if (position1.left <= position2.left
				&& position1.left + position1.width <= position2.left + position2.width
				&& position1.top < position2.top + position2.height
				&& position1.top + position1.height > position2.top)
			{
				o1->collision(o2, CollisionSide::right);
				o2->collision(o1, CollisionSide::left);
			}
			else if (position1.left > position2.left
				&& position1.left + position1.width > position2.left + position2.width
				&& position1.top < position2.top + position2.height
				&& position1.top + position1.height > position2.top)
			{
				o1->collision(o2, CollisionSide::left);
				o2->collision(o1, CollisionSide::right);
			}
		}
	}
	void updateElement(std::list<std::shared_ptr<GameObject>>::iterator it1)
	{
		it1->get()->updateObject(newlyAdded);
		for (std::list<std::shared_ptr<GameObject>>::iterator it2 = it1; it2 != objectsList.end(); it2++)
		{
			determineCollision(it1->get(), it2->get());
		}
		if (it1->get()->toRemoval())
		{
			if (*it1 != playerPtr)
			{
				objectsList.erase(it1);
			}
			else
			{
				playerDied = 1;
			}
		}
		else
			it1->get()->updateSprite();
	}

public:
	GameState(int level, ResourceHolder& resources)
	{
		stateID = 1;
		MyText myGameOver(0, 0, "GAME OVER", resources, 64);
		gameOver = myGameOver.getText();
		std::ifstream levelFile("resources/levels/level" + std::to_string(level) + ".csv");
		if (levelFile)
		{
			std::string dataString;
			std::string objectIDstring, Xstring, Ystring;
			while (getline(levelFile, dataString))
			{
				std::stringstream dataSS(dataString);

				if (getline(dataSS, objectIDstring, ',') &&
					getline(dataSS, Xstring, ',') &&
					getline(dataSS, Ystring, ','))
				{
					std::stringstream IDss(objectIDstring);
					std::stringstream Xss(Xstring);
					std::stringstream Yss(Ystring);
					int id;
					double x, y;
					if (IDss >> id && Xss >> x && Yss >> y)
					{
						addToList(id, x * 16, -y * 16, resources);
					}
					else
						std::cout << "invalid line in level" + std::to_string(level) + ".csv\n";
				}
				else
					std::cout << "invalid line in level" + std::to_string(level) + ".csv\n";
			}
			levelFile.close();
		}
		else
			std::cout << "couldn't open level" + std::to_string(level) + ".csv";
	}

	void update() override
	{
		std::vector<std::thread> threads;	//threads - updating objects
		for (std::list<std::shared_ptr<GameObject>>::iterator it1 = objectsList.begin(); it1 != objectsList.end(); it1++)
		{
			threads.push_back(std::thread([this, it1] { this->updateElement(it1); }));
		}
		for (int j = 0; j < threads.size(); j++)
		{
			threads[j].join();
		}
		if (!levelEnding && (finishPtr->finished() || playerDied))
		{
			levelEndingDelay.restart();
			levelEnding = 1;
		}
		if (levelEnding && levelEndingDelay.getElapsedTime().asSeconds() > 3)
		{
			this->stateID = 0;
		}
		objectsList.splice(objectsList.end(), newlyAdded);
		healthBar.updateHealthBar(playerPtr);
	}
	void render(sf::RenderTarget& target)override
	{
		view.setSize(sf::Vector2f(target.getSize().x / 4, target.getSize().y / 4));
		drawBackground(target);
		for (std::shared_ptr<GameObject> o : this->objectsList)
		{
			o->draw(target);
			if (o == playerPtr)
			{
				view.setCenter(o->getSprite().getPosition());
			}
		}
		target.setView(view);
		healthBar.drawHealthBar(target);
		if (playerDied)
		{
			gameOver.setPosition(view.getCenter());
			gameOver.setPosition(gameOver.getGlobalBounds().left - 50, gameOver.getGlobalBounds().top - 25);
			target.draw(gameOver);
		}
	}
};