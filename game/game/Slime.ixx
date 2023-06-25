export module Slime;
import Enemy;
import Player;

export class Slime final : public Enemy
{
public:
	Slime(double x, double y, ResourceHolder& resources, std::shared_ptr<Player>& _playerPtr)
	{
		this->objectType = ObjectType::enemy;
		this->HP = 30;
		this->playerPtr = _playerPtr;
		this->textures = resources.getTexture("slime");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
		this->xAccelleration = 0.05;
		this->maxXVelocity = 1;
		this->jumpHeight = 5;
	}
	sf::FloatRect getHitbox() override
	{
		sf::FloatRect hitbox = sprite.getGlobalBounds();
		hitbox.top += 14;
		hitbox.height -= 14;
		return hitbox;
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
	void setPosition(float left, float top)override
	{
		sprite.setPosition(left, top - 14);
	}
	virtual void jump() {}
	virtual void attack() {}
};