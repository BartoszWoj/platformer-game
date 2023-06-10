export module Entity;
export import GameObject;

export class Entity : public GameObject
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
	bool attackAnimationPlaying = 0;

	int HP;

	virtual void collisionWithStaticBlock(GameObject& block, CollisionSide side)
	{
		sf::FloatRect thisBounds = this->getSprite().getGlobalBounds();
		sf::FloatRect blockBounds = block.getSprite().getGlobalBounds();
		switch (side)
		{
		case CollisionSide::left:
			this->velocity.x = 0;
			this->sprite.setPosition(blockBounds.left + blockBounds.width, thisBounds.top);
			break;

		case CollisionSide::right:
			this->velocity.x = 0;
			this->sprite.setPosition(blockBounds.left - thisBounds.width, thisBounds.top);
			break;

		case CollisionSide::top:
			this->velocity.y = 0;
			this->sprite.setPosition(thisBounds.left, blockBounds.top + blockBounds.height);
			break;

		case CollisionSide::bottom:
			this->velocity.y = 0;
			this->sprite.setPosition(thisBounds.left, blockBounds.top - thisBounds.height);
			jump();
			break;
		}

	}
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
	virtual void move() = 0;
	virtual void jump() = 0;
	virtual void animate() = 0;
	virtual void receiveDamage(CollisionSide side)
	{
		this->HP -= 10;
		this->velocity.y = -jumpHeight / 1.5;
		switch (side)
		{
		case CollisionSide::left:
			this->velocity.x = maxXVelocity;
			break;
		case CollisionSide::right:
			this->velocity.x = -maxXVelocity;
			break;
		case CollisionSide::bottom:
		case CollisionSide::top:
			if (this->facingLeft)
			{
				this->velocity.x = maxXVelocity;
			}
			else
				this->velocity.x = -maxXVelocity;
			break;
		}
		if (this->HP < 0)
			this->HP = 0;
	}
public:
	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&) = 0;
	virtual int getHP()
	{
		return HP;
	}
	bool toRemoval() override
	{
		if (this->HP <= 0)
			return 1;
		return 0;
	}
};