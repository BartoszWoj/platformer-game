#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <fstream>
#include <sstream>
#include <random>

#include <filesystem>
#include <regex>

#include <SFML/Graphics.hpp>

class ResourceHolder final
{
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Font> fonts;

	void loadTextures()
	{
		std::filesystem::path texturesPath = std::filesystem::current_path() /= "resources\\textures"; //filesystem - finding texture files to read
		for (auto file : std::filesystem::recursive_directory_iterator{ texturesPath })
		{
			if (file.path().extension() == ".png")
			{
				std::string fileName(file.path().stem().string());
				std::regex reg(".+Textures?");
				if (std::regex_match(fileName, reg))		//regex - veryfying files names and extracting textures names
				{
					std::regex ToRemove("Textures?");
					std::string objectName = std::regex_replace(fileName, ToRemove, "");
					sf::Texture texture;
					texture.loadFromFile(file.path().string());

					this->textures[objectName] = texture;
				}
				else
					std::cout << "invalid file in resources\\textures\n";
			}
		}
	}

	void loadFonts()
	{
		std::filesystem::path fontsPath = std::filesystem::current_path() /= "resources\\fonts"; //filesystem - finding font files to read
		for (auto entry : std::filesystem::recursive_directory_iterator{ fontsPath })
		{
			if (entry.path().extension() == ".ttf")
			{
				std::string fontName(entry.path().stem().string());
				sf::Font font;
				font.loadFromFile(entry.path().string());

				this->fonts[fontName] = font;
			}
			else
				std::cout << "invalid file in resources\\fonts\n";
		}
	}

public:
	ResourceHolder()
	{
		loadTextures();
		loadFonts();
	}

	sf::Texture& getTexture(std::string name)
	{
		return this->textures[name];
	}

	sf::Font& getFont(std::string name)
	{
		return this->fonts[name];
	}
};

enum ObjectType {
	nonPhysical,
	staticBlock,
	player,
	playerAttack,
	enemy,
	enemyAttack,
	levelFinish
};

enum class collisionSide
{
	left,
	right,
	top,
	bottom,
};

class Object
{
protected:
	sf::Texture textures;
	sf::Sprite sprite;
	sf::Vector2f velocity;

	ObjectType objectType;

	int HP;
	int ATK;
	int DEF;
	bool dead;

public:
	virtual void updateObject(std::list<std::shared_ptr<Object>>&) = 0;
	virtual void collision(std::shared_ptr<Object>, collisionSide) = 0;
	virtual void receiveDamage(int dmg, collisionSide side) = 0;
	virtual sf::Vector2f getVelocity()
	{
		return this->velocity;
	}
	virtual void setVelocity(int xVelocity, int yVelocity)
	{
		this->velocity.x = xVelocity;
		this->velocity.y = yVelocity;
	}
	virtual ObjectType getObjectType()
	{
		return objectType;
	}
	virtual sf::Sprite getSprite()
	{
		return this->sprite;
	}
	virtual void updateSprite()
	{
		this->sprite.move(this->velocity.x, this->velocity.y);
	}
	virtual void draw(sf::RenderTarget& target)
	{
		target.draw(this->sprite);
	}
	virtual bool toRemoval()
	{
		return 0;
	}
};

class MyText final : public Object
{
	sf::Text text;
	sf::Font* font;

public:
	MyText()
	{
		objectType = nonPhysical;
		text.setScale(0.25, 0.25);
	}
	MyText(int x, int y, std::string _text, ResourceHolder& resources, int size = 50, sf::Color colour = sf::Color::Black, int rotation = 0)
	{
		objectType = nonPhysical;
		font = &resources.getFont("arial");
		text.setFont(*font);
		text.setPosition(x, y);
		text.setString(_text);
		text.setScale(0.25, 0.25);
		text.setCharacterSize(size);
		text.setFillColor(colour);
		text.setRotation(rotation);
	}
	void setText(int x, int y, std::string _text, ResourceHolder& resources, int size = 50, sf::Color colour = sf::Color::Black, int rotation = 0)
	{
		font = &resources.getFont("arial");
		text.setFont(*font);
		text.setPosition(x, y);
		text.setString(_text);
		text.setCharacterSize(size);
		text.setFillColor(colour);
		text.setRotation(rotation);
	}
	sf::Text getText()
	{
		return text;
	}
	void draw(sf::RenderTarget& target) override
	{
		target.draw(this->sprite);
	}
	virtual void updateObject(std::list<std::shared_ptr<Object>>&)override{}
	virtual void collision(std::shared_ptr<Object>, collisionSide)override {}
	virtual void receiveDamage(int dmg, collisionSide side)override{}
};

class Button final
{
	sf::RectangleShape box;
	sf::Text text;
	float boxX;
	float boxY;
	float textX;

public:
	Button()
	{

	}
	Button(float _boxX, float _boxY, std::string _text, ResourceHolder& resources) :
		boxX(_boxX), boxY(_boxY)
	{
		MyText myText(boxX + 2, boxY + 1, _text, resources);
		this->text = myText.getText();
		this->box.setFillColor(sf::Color(128, 128, 128));
		unselect();
	}
	void select()
	{
		this->box.setPosition(this->boxX - 4, this->boxY - 1);
		this->box.setSize(sf::Vector2f(88, 22));
		this->box.setOutlineThickness(4);
		this->box.setOutlineColor(sf::Color::Red);
		this->box.setRotation(3.f);
		this->text.setRotation(3.f);
	}
	void unselect()
	{
		this->box.setPosition(this->boxX, this->boxY);
		this->box.setSize(sf::Vector2f(80, 20));
		this->box.setOutlineThickness(2);
		this->box.setOutlineColor(sf::Color::White);
		this->box.setRotation(0.f);
		this->text.setRotation(0.f);
	}
	void draw(sf::RenderTarget& target)
	{
		target.draw(this->box);
		target.draw(this->text);
	}
};

class FinishFlag final : public Object
{
	sf::Clock animationTimer;
	sf::IntRect currentFrame;
	bool wasReached = 0;
	sf::Text congratulations;

public:

	FinishFlag(double x, double y, ResourceHolder& resources)
	{
		this->objectType = levelFinish;
		this->wasReached = 0;
		MyText myCongratulations;
		myCongratulations.setText(x - 45, y - 30, "CONGRATULATIONS", resources);
		congratulations = myCongratulations.getText();
		this->textures = resources.getTexture("finishFlag");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
	}
	void animate()
	{
		if (this->animationTimer.getElapsedTime().asSeconds() > 0.25)
		{
			if (this->currentFrame.left != 32)
				this->currentFrame.left += 32;
			else
				currentFrame.left = 0;
			this->animationTimer.restart();
		}
		this->sprite.setTextureRect(this->currentFrame);
	}
	void updateObject(std::list<std::shared_ptr<Object>>&)
	{
		animate();
	}
	void collision(std::shared_ptr<Object> other, collisionSide side)override
	{
		if (other->getObjectType() == player)
			wasReached = 1;
	}
	bool finished()
	{
		return wasReached;
	}
	void draw(sf::RenderTarget& target) override
	{
		target.draw(this->sprite);
		if (wasReached)
			target.draw(congratulations);
	}
	virtual void receiveDamage(int dmg, collisionSide side)override{}

};

class Entity : public Object
{
protected:

	sf::Clock animationTimer;
	sf::IntRect currentFrame;

	double xAccelleration;
	double maxXVelocity;
	double jumpHeight;

	bool isWalking = 0;
	bool isAttacking = 0;
	bool facingLeft = 0;

	virtual void collisionWithStaticBlock(Object& block, collisionSide side)
	{
		sf::FloatRect thisBounds = this->getSprite().getGlobalBounds();
		sf::FloatRect blockBounds = block.getSprite().getGlobalBounds();
		switch (side)
		{
		case collisionSide::left:
			this->velocity.x = 0;
			this->sprite.setPosition(blockBounds.left + blockBounds.width, thisBounds.top);
			break;

		case collisionSide::right:
			this->velocity.x = 0;
			this->sprite.setPosition(blockBounds.left - thisBounds.width, thisBounds.top);
			break;

		case collisionSide::top:
			this->velocity.y = 0;
			this->sprite.setPosition(thisBounds.left, blockBounds.top + blockBounds.height);
			break;

		case collisionSide::bottom:
			this->velocity.y = 0;
			this->sprite.setPosition(thisBounds.left, blockBounds.top - thisBounds.height);
			jump();
			break;
		}

	}

public:
	virtual void updateObject(std::list<std::shared_ptr<Object>>&) = 0;
	virtual void move() = 0;
	virtual void jump() = 0;
	virtual void gravity()
	{
		if (this->velocity.y < 15)
			this->velocity.y += 0.5;
		if (this->sprite.getGlobalBounds().top + this->sprite.getGlobalBounds().height >= 0)
		{
			this->velocity.y = 0;
			this->sprite.setPosition(this->sprite.getGlobalBounds().left, -this->sprite.getGlobalBounds().height);
			jump();
		}
	}
	virtual void animate() = 0;
	virtual void receiveDamage(int dmg, collisionSide side)
	{
		this->HP -= dmg - this->DEF;
		this->velocity.y = -jumpHeight / 2;
		switch (side)
		{
		case collisionSide::left:
			this->velocity.x = -maxXVelocity;
			break;
		case collisionSide::right:
			this->velocity.x = maxXVelocity;
			break;
		case collisionSide::bottom:
		case collisionSide::top:
			if (this->facingLeft)
			{
				this->velocity.x = maxXVelocity;
			}
			else
				this->velocity.x = maxXVelocity;
			break;
		}
	}
	bool toRemoval() override
	{
		if (this->HP <= 0)
			return 1;
		return 0;
	}
};

class PlayerAttack : public Object
{
	std::vector<std::shared_ptr<Object>> alreadyHit;
public:

	PlayerAttack(sf::FloatRect playerPosition,bool playerFacingLeft, int dmg)
	{
		this->objectType = playerAttack;
		this->ATK = dmg;
		this->HP = 1;
		if(playerFacingLeft)
		this->sprite.setPosition(playerPosition.left-16, playerPosition.top);
		else
			this->sprite.setPosition(playerPosition.left + 32, playerPosition.top);
		this->sprite.setTextureRect(sf::IntRect(0,0,16,32));
	}

	virtual void updateObject(std::list<std::shared_ptr<Object>>&)override{}
	virtual void receiveDamage(int dmg, collisionSide side)override{}

	void collision(std::shared_ptr<Object> other, collisionSide side)override
	{
		switch (other->getObjectType())
		{
		case enemy:
			bool wasAttacked = 0;
				for (std::shared_ptr<Object> enemy : alreadyHit)
					if (other == enemy)
						wasAttacked = 1;
				if (!wasAttacked)
				{
					other->receiveDamage(this->ATK, side);
					this->alreadyHit.push_back(other);
				}
			break;
		}
	}
	bool toRemoval() override
	{
		if (this->HP <= 0)
			return 1;
		HP--;
		return 0;
	}
};

class Player final : public Entity
{
	sf::Text gameOver;
	bool Zreleased;

	void move()override
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && this->velocity.x > -this->maxXVelocity)
		{
			this->velocity.x -= xAccelleration;
			this->facingLeft = 1;
			this->isWalking = 1;

		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && this->velocity.x < this->maxXVelocity)
		{
			this->velocity.x += this->xAccelleration;
			this->facingLeft = 0;
			this->isWalking = 1;
		}
		else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if (this->velocity.x < -0.2)
				this->velocity.x += this->xAccelleration;
			else if (this->velocity.x > 0.2)
				this->velocity.x -= this->xAccelleration;
			else
			{
				this->velocity.x = 0;
				this->isWalking = 0;
			}
		}
	}
	void jump()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			this->velocity.y = -this->jumpHeight;
	}
	void attack(int dmg, std::list<std::shared_ptr<Object>>& list)
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			this->Zreleased = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && this->Zreleased)
		{
			this->Zreleased = 0;
			this->isAttacking = 1;
			list.push_back(std::make_shared<PlayerAttack>(this->sprite.getGlobalBounds(), this->facingLeft, this->ATK));
		}
	}
	void animate()override
	{
		int animation;
		if (this->isAttacking)
			animation = 2;
		else if (this->isWalking)
		{
			animation = 1;
		}
		else
			animation = 0;
		if (this->currentFrame.top != animation * 64 + this->facingLeft * 32)
		{
			this->currentFrame.top = animation * 64 + this->facingLeft * 32;
			this->currentFrame.left = 0;
			this->animationTimer.restart();
		}
		else if (this->animationTimer.getElapsedTime().asSeconds() > 0.25)
		{
			if (this->currentFrame.left < 96)
				this->currentFrame.left += 32;
			else
			{
				currentFrame.left = 0;
				if (isAttacking == 1)
					isAttacking = 0;
			}
			this->animationTimer.restart();
		}
		this->sprite.setTextureRect(this->currentFrame);
	}
public:
	Player(double x, double y, ResourceHolder& resources)
	{
		this->objectType = player;
		this->HP = 100;
		this->ATK = 500;
		this->DEF = 10;
		MyText myGameOver;
		myGameOver.setText(x, y, "GAME OVER", resources);
		gameOver = myGameOver.getText();
		this->textures = resources.getTexture("player");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
		this->xAccelleration = 0.35;
		this->maxXVelocity = 3.5;
		this->jumpHeight = 8;

	}
	void updateObject(std::list<std::shared_ptr<Object>>& list) override
	{
		gravity();
		move();
		attack(this->ATK, list);
		animate();
	}
	void collision(std::shared_ptr<Object> other, collisionSide side)override
	{
		switch (other->getObjectType())
		{
		case staticBlock:
			collisionWithStaticBlock(*other, side);
			break;
		}
	}
	void draw(sf::RenderTarget& target) override
	{
		if (this->HP > 0)
		target.draw(this->sprite);
		else
		{
			gameOver.setPosition(this->sprite.getGlobalBounds().left-30, this->sprite.getGlobalBounds().top-20);
			target.draw(gameOver);
		}
	}
};

class Enemy : public Entity
{
protected:

	std::shared_ptr<Object> playerPtr;

	void collisionWithPlayer(std::shared_ptr<Object> other, collisionSide side)
	{
		other->receiveDamage(this->ATK, side);
	}
	void move()override
	{
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(1, 100);
		int random = uniform_dist(e1);
		if (random % 100 < 1)
		{
			if (this->isWalking)
				this->isWalking = 0;
			else
			{
				if (std::rand() % 100 < 50)
					this->facingLeft = 1;
				else
					this->facingLeft = 0;
				this->isWalking = 1;
			}
		}

		if (this->isWalking && this->facingLeft && this->velocity.x > -this->maxXVelocity)
		{
			this->velocity.x -= xAccelleration;
		}
		else if (this->isWalking && !this->facingLeft && this->velocity.x < this->maxXVelocity)
		{
			this->velocity.x += this->xAccelleration;
		}
		else if (!this->isWalking)
		{
			if (this->velocity.x < -0.2)
				this->velocity.x += this->xAccelleration;
			else if (this->velocity.x > 0.2)
				this->velocity.x -= this->xAccelleration;
			else
			{
				this->velocity.x = 0;
			}
		}
	}
	virtual void detectPlayer() = 0;
public:
	void updateObject(std::list<std::shared_ptr<Object>>&)override
	{
		gravity();
		detectPlayer();
		move();
		animate();
	}
	void collision(std::shared_ptr<Object> other, collisionSide side)override
	{
		switch (other->getObjectType())
		{
		case staticBlock:
			collisionWithStaticBlock(*other, side);
			break;
		case player:
			collisionWithPlayer(other, side);
			break;
		}
	}
};

class Slime : public Enemy
{
public:
	Slime(double x, double y, ResourceHolder& resources, std::list<std::shared_ptr<Object>>& objectsList)
	{
		this->objectType = enemy;
		this->HP = 1000;
		this->ATK = 20;
		this->DEF = 300;
		for (std::shared_ptr<Object> o : objectsList)
			if (o->getObjectType() == player)
				this->playerPtr = o;
		this->textures = resources.getTexture("slime");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
		this->xAccelleration = 0.20;
		this->maxXVelocity = 1;
		this->jumpHeight = 8;
	}
	void animate()override
	{
		int animation;
		if (this->isWalking)
		{
			animation = 1;
		}
		else
			animation = 0;
		if (this->currentFrame.top != animation * 64 + this->facingLeft * 32)
		{
			this->currentFrame.top = animation * 64 + this->facingLeft * 32;
			this->currentFrame.left = 0;
			this->animationTimer.restart();
		}
		else if (this->animationTimer.getElapsedTime().asSeconds() > 0.25)
		{
			if (this->currentFrame.left != 96)
				this->currentFrame.left += 32;
			else
				currentFrame.left = 0;
			this->animationTimer.restart();
		}
		this->sprite.setTextureRect(this->currentFrame);
	}
	void detectPlayer() override
	{
		sf::FloatRect playerPosition = playerPtr->getSprite().getGlobalBounds();
		if (playerPtr->getSprite().getGlobalBounds().top > this->sprite.getGlobalBounds().top - 100
			&& playerPtr->getSprite().getGlobalBounds().top < this->sprite.getGlobalBounds().top + 100)
		{
			if (playerPtr->getSprite().getGlobalBounds().left > this->sprite.getGlobalBounds().left - 100
				&& playerPtr->getSprite().getGlobalBounds().left < this->sprite.getGlobalBounds().left)
			{
				this->facingLeft = 1;
				this->isWalking = 1;
			}
			if (playerPtr->getSprite().getGlobalBounds().left < this->sprite.getGlobalBounds().left + 100
				&& playerPtr->getSprite().getGlobalBounds().left > this->sprite.getGlobalBounds().left)
			{
				this->facingLeft = 0;
				this->isWalking = 1;
			}
		}
	}
	virtual void jump(){}
	virtual void attack() {}
};

class BrickBlock final : public Object
{
public:

	BrickBlock(double x, double y, ResourceHolder& resources)
	{
		this->objectType = staticBlock;
		this->textures = resources.getTexture("brickBlock");

		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
		this->sprite.setPosition(sf::Vector2f(x, y));
	}
	virtual void updateObject(std::list<std::shared_ptr<Object>>&)override {}
	virtual void collision(std::shared_ptr<Object>, collisionSide)override {}
	virtual void receiveDamage(int dmg, collisionSide side)override {}
};

void drawBackground(sf::RenderTarget& target)
{
	sf::VertexArray sky;
	sf::VertexArray ground;
	int leftBorder = -500;
	int rightBorder = 2000;
	int upperBorder = 2000;
	int bottomBorder = -200;

	sky = sf::VertexArray(sf::Quads, 4);
	sky[0].position = sf::Vector2f(leftBorder, -upperBorder);
	sky[0].color = sf::Color::Blue;
	sky[1].position = sf::Vector2f(rightBorder, -upperBorder);
	sky[1].color = sf::Color::Blue;
	sky[2].position = sf::Vector2f(rightBorder, 0);
	sky[2].color = sf::Color::Cyan;
	sky[3].position = sf::Vector2f(leftBorder, 0);
	sky[3].color = sf::Color::Cyan;
	ground = sf::VertexArray(sf::Quads, 4);
	ground[0].position = sf::Vector2f(leftBorder, 0);
	ground[0].color = sf::Color::Green;
	ground[1].position = sf::Vector2f(rightBorder, 0);
	ground[1].color = sf::Color::Green;
	ground[2].position = sf::Vector2f(rightBorder, -bottomBorder);
	ground[2].color = sf::Color(0, 64, 0);
	ground[3].position = sf::Vector2f(leftBorder, -bottomBorder);
	ground[3].color = sf::Color(0, 64, 0);

	target.draw(sky);
	target.draw(ground);
}

class State
{
protected:
	bool exiting = 0;
	int level = 0;

	int stateID;

public:
	virtual void update() = 0;
	virtual void render(sf::RenderTarget& target) = 0;
	virtual bool closing()
	{
		return this->exiting;
	}
	virtual int getStateID()
	{
		return this->stateID;
	}
	virtual int getLevel()
	{
		return this->level;
	}
};

class MenuState : public State
{
protected:
	int currentlySelected;

	bool Zreleased;
	bool Xreleased;
	bool UPreleased;
	bool DOWNreleased;

	virtual void updateButtons(Button arr[], int lenght)
	{
		for (int i = 0; i < lenght; i++)
			if (&arr[i] == &arr[this->currentlySelected])
				arr[this->currentlySelected].select();
			else
				arr[i].unselect();
	}
	virtual void navigate(Button arr[], int lenght)
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			this->DOWNreleased = 1;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			this->UPreleased = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && this->DOWNreleased && this->currentlySelected < lenght - 1)
		{
			arr[this->currentlySelected].unselect();
			this->currentlySelected++;
			arr[this->currentlySelected].select();
			this->DOWNreleased = 0;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && this->UPreleased && this->currentlySelected > 0)
		{
			arr[this->currentlySelected].unselect();
			this->currentlySelected--;
			arr[this->currentlySelected].select();
			this->UPreleased = 0;
		}
	}
	virtual bool choose()
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			this->Zreleased = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && this->Zreleased)
		{
			this->Zreleased = 0;
			return 1;
		}
		return 0;
	}
	virtual bool cancel()
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			this->Xreleased = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && this->Xreleased)
		{
			this->Xreleased = 0;
			return 1;
		}
		return 0;
	}
public:

	virtual void update() = 0;////////////////////////////////////////usun to potem
	virtual void render(sf::RenderTarget& target) = 0;////////////////
};

class TitleScreenState final : public MenuState
{
	sf::Font font;//////////////////////////////////////////
	sf::Text title;
	sf::Text credit;
	Button mainMenuButton[2];
	Button levelSelectButton[4];

	enum MenuStates
	{
		mainMenu, levelSelection
	};
	MenuStates state;

public:
	TitleScreenState(ResourceHolder& resources)
	{
		MyText myTitle(-70, -100, "Platformer Game", resources, 64);
		this->title = myTitle.getText();
		MyText myCredit(-20, -80, "By Milosz Cebula", resources, 32);
		this->credit = myCredit.getText();
		mainMenuButton[0] = Button(-50, -35, "Start Game", resources);
		mainMenuButton[1] = Button(-50, 0, "Exit", resources);
		levelSelectButton[0] = Button(-50, -85, "Level 1", resources);
		levelSelectButton[1] = Button(-50, -50, "Level 2", resources);
		levelSelectButton[2] = Button(-50, -15, "Level 3", resources);
		levelSelectButton[3] = Button(-50, 20, "Return", resources);
		this->stateID = 0;
		this->currentlySelected = 0;
		this->Zreleased = 1;
		this->Xreleased = 1;
		this->UPreleased = 1;
		this->DOWNreleased = 1;
		this->mainMenuButton[0].select();
		state = mainMenu;
	}

	void update() override
	{
		switch (state)
		{
		case mainMenu:
			navigate(this->mainMenuButton, 2);
			if (choose())
			{
				switch (this->currentlySelected)
				{
				case 0:
					stateID = 1;
					this->level = this->currentlySelected + 1;
					/*state = levelSelection;
					this->levelSelectButton[0].select();*/
					break;
				case 1:
					this->exiting = 1;
					break;
				}
			}
			else if (cancel())
			{
				this->currentlySelected = 1;
			}
			updateButtons(this->mainMenuButton, 2);
			break;
		case levelSelection:
			navigate(this->levelSelectButton, 4);
			if (choose())
			{
				switch (this->currentlySelected)
				{
				case 0:
				case 1:
				case 2:
					stateID = 1;
					this->level = this->currentlySelected + 1;
					break;
				case 3:
					state = mainMenu;
					this->currentlySelected = 0;
					break;
				}
			}
			if (cancel())
			{
				state = mainMenu;
				this->currentlySelected = 0;
			}
			updateButtons(this->levelSelectButton, 4);
			break;
		}
	}
	void render(sf::RenderTarget& target)override
	{
		drawBackground(target);
		switch (state)
		{
		case mainMenu:
			target.draw(this->title);
			target.draw(this->credit);
			for (int i = 0; i < 2; i++)
				this->mainMenuButton[i].draw(target);
			break;
		case levelSelection:
			for (int i = 0; i < 4; i++)
				this->levelSelectButton[i].draw(target);
			break;
		}

		sf::View view(sf::Vector2f(0, -32), sf::Vector2f(target.getSize().x / 4, target.getSize().y / 4));
		target.setView(view);
	}
};

void updateCollision(std::shared_ptr<Object> o1, std::shared_ptr<Object> o2)
{
	sf::FloatRect position1 = o1->getSprite().getGlobalBounds();
	sf::FloatRect nextPosition1 = position1;
	nextPosition1.left += o1->getVelocity().x;
	nextPosition1.top += o1->getVelocity().y;
	sf::FloatRect position2 = o2->getSprite().getGlobalBounds();
	sf::FloatRect nextPosition2 = position2;
	nextPosition1.left += o2->getVelocity().x;
	nextPosition1.top += o2->getVelocity().y;
	if (nextPosition1.intersects(nextPosition2))
	{
		if (position1.top < position2.top
			&& position1.top + position1.height < position2.top + position2.height
			&& position1.left < position2.left + position2.width
			&& position1.left + position1.width > position2.left)
		{
			o1->collision(o2, collisionSide::bottom);
			o2->collision(o1, collisionSide::top);
		}
		if (position1.top > position2.top
			&& position1.top + position1.height > position2.top + position2.height
			&& position1.left < position2.left + position2.width
			&& position1.left + position1.width > position2.left)
		{
			o1->collision(o2, collisionSide::top);
			o2->collision(o1, collisionSide::bottom);
		}
		if (position1.left < position2.left
			&& position1.left + position1.width < position2.left + position2.width
			&& position1.top < position2.top + position2.height
			&& position1.top + position1.height > position2.top)
		{
			o1->collision(o2, collisionSide::right);
			o2->collision(o1, collisionSide::left);
		}
		else if (position1.left > position2.left
			&& position1.left + position1.width > position2.left + position2.width
			&& position1.top < position2.top + position2.height
			&& position1.top + position1.height > position2.top)
		{
			o1->collision(o2, collisionSide::left);
			o2->collision(o1, collisionSide::right);
		}
	}
}
class GameState final : public State
{
	std::list<std::shared_ptr<Object>> objectsList;
	std::shared_ptr<FinishFlag> finishFlag;
	bool playerDied;
	bool levelEnding;
	sf::Clock levelFinishDelay;

	void addToList(int id, double x, double y, ResourceHolder& resources)
	{
		switch (id)
		{
		case 0:
			this->objectsList.push_back(std::make_shared<Player>(x, y, resources));
			break;
		case 1:
			this->objectsList.push_back(std::make_shared<BrickBlock>(x, y, resources));
			break;
		case 2:
			this->objectsList.push_back(std::make_shared<Slime>(x, y, resources, objectsList));
			break;
		case 9:
			this->finishFlag = std::make_shared<FinishFlag>(x, y, resources);
			this->objectsList.push_back(finishFlag);
			break;
		}
	}

public:
	GameState(int level, ResourceHolder& resources)
	{
		playerDied = 0;
		levelEnding = 0;
		stateID = 1;
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
		int i = 0;
		for (std::list<std::shared_ptr<Object>>::iterator it1 = objectsList.begin(); it1 != objectsList.end(); it1++)
		{
			i++;
			it1->get()->updateObject(objectsList);
			for (std::list<std::shared_ptr<Object>>::iterator it2 = objectsList.begin(); it2 != objectsList.end(); it2++)
			{
				updateCollision(*it1, *it2);
			}
			if (it1->get()->toRemoval())
			{
				if (it1->get()->getObjectType() != player)
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
		if (!levelEnding && (finishFlag->finished() || playerDied))
		{
			levelFinishDelay.restart();
			levelEnding = 1;
		}
		if (levelEnding && levelFinishDelay.getElapsedTime().asSeconds() > 3)
		{
			this->stateID = 0;
		}
	}
	void render(sf::RenderTarget& target)override
	{
		sf::View view;
		view.setSize(sf::Vector2f(target.getSize().x / 4, target.getSize().y / 4));
		drawBackground(target);
		for (std::shared_ptr<Object> o : this->objectsList)
		{
			o->draw(target);
			if (o->getObjectType() == player)
			{
				view.setCenter(o->getSprite().getPosition());
			}
		}
		target.setView(view);
	}
};

class Game final
{
	const int windowWidth = 1280;
	const int windowHeight = 720;
	sf::RenderWindow window;
	std::unique_ptr<State> state;
	ResourceHolder resources;

	int stateID;

	bool closing()
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
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
				this->state = std::make_unique<TitleScreenState>(resources);
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
		this->state = std::make_unique<TitleScreenState>(resources);
		// run the program as long as the window is open
		while (window.isOpen())
		{
			// check all the window's events that were triggered since the last iteration of the loop
			if (this->closing())
				window.close();
			state->update();
			if (state->closing())
				window.close();
			changeState();
			// clear the window with black color
			window.clear();

			// draw everything here...
			state->render(window);
			// end the current frame
			window.display();

		}
	}
};

int main()
{
	Game game;
	game.run();

	return 0;
}