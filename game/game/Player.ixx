export module Player;
export import Entity;
import PlayerAttack;

export class Player final : public Entity
{

	sf::Clock invicibilityTimer;

	bool Zreleased = 0;

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
	void attack(std::list<std::shared_ptr<GameObject>>& list)
	{
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			this->Zreleased = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && this->Zreleased)
		{
			this->Zreleased = 0;
			this->isAttacking = 1;
			list.push_back(std::make_shared<PlayerAttack>(this->getHitbox(), this->facingLeft));
		}
	}
	void receiveDamage(CollisionSide side)override
	{
		if (invicibilityTimer.getElapsedTime().asSeconds() > 0.75)
		{
			invicibilityTimer.restart();
			Entity::receiveDamage(side);
		}
	}
	void animate()override
	{
		enum
		{
			idle = 0,
			walk = 1,
			attack = 2
		}animation;
		if (this->isAttacking)
			animation = attack;
		else if (this->isWalking)
		{
			animation = walk;
		}
		else
			animation = idle;
		if (this->currentFrame.top != animation * 64 + this->facingLeft * 32)
		{
			switch (animation)
			{
			case idle:
			case walk:
				this->currentFrame.top = animation * 64 + this->facingLeft * 32;
				this->currentFrame.left = 0;
				break;
			case attack:
				if (!attackAnimationPlaying)
				{
					currentFrame.left = 0;
					attackAnimationPlaying = 1;
				}
				currentFrame.top = 128 + this->facingLeft * 32;
				break;
			}
		}
		if (this->animationTimer.getElapsedTime().asSeconds() > 0.25)
		{
			if (this->currentFrame.left < 96)
				this->currentFrame.left += 32;
			else
			{
				currentFrame.left = 0;
				if (isAttacking == 1)
				{
					isAttacking = 0;
					attackAnimationPlaying = 0;
				}
			}
			this->animationTimer.restart();
		}
		this->sprite.setTextureRect(this->currentFrame);
	}
public:
	Player(double x, double y, ResourceHolder& resources)
	{
		this->objectType = ObjectType::player;
		this->HP = 100;
		this->textures = resources.getTexture("player");
		this->sprite.setTexture(this->textures);
		this->sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
		this->sprite.setPosition(sf::Vector2f(x, y));
		this->currentFrame = sf::IntRect(0, 0, 32, 32);
		this->xAccelleration = 0.5;
		this->maxXVelocity = 5;
		this->jumpHeight = 8;

	}
	sf::FloatRect getHitbox() override
	{
		sf::FloatRect hitbox = sprite.getGlobalBounds();
		hitbox.left += 12;
		hitbox.width -= 24;
		return hitbox;
	}
	void updateObject(std::list<std::shared_ptr<GameObject>>& list) override
	{
		if (this->HP > 0)
		{
			gravity();
			move();
			attack(list);
			animate();
		}
	}
	void collision(GameObject* other, CollisionSide side)override
	{
		switch (other->getObjectType())
		{
		case ObjectType::staticBlock:
			collisionWithStaticBlock(*other, side);
			break;
		case ObjectType::enemy:
			receiveDamage(side);
			break;
		}
	}
	void setPosition(float left, float top)override
	{
		sprite.setPosition(left - 12, top);
	}
	void draw(sf::RenderTarget& target) override
	{
		if (this->HP > 0)
			target.draw(this->sprite);
	}
};