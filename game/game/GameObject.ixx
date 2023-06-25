export module GameObject;
import Enums;
export import ResourceHolder;
export import <list>;

export import <SFML/Graphics.hpp>;

export class GameObject
{
protected:
	sf::Texture textures;
	sf::Sprite sprite;
	sf::Vector2f velocity;
	sf::IntRect hitbox;

	ObjectType objectType;

public:
	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&) = 0;
	virtual void collision(GameObject*, CollisionSide) = 0;
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
	virtual sf::FloatRect getHitbox()
	{
		return sprite.getGlobalBounds();
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

export void MyFunc();