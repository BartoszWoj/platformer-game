export module Enemy;
import Entity;
import <random>;

export class Enemy : public Entity
{
protected:

	std::shared_ptr<GameObject> playerPtr;

	virtual void move()override
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
	virtual void detectPlayer()
	{
		sf::FloatRect playerPosition = playerPtr->getHitbox();
		if (playerPtr->getHitbox().top > this->getHitbox().top - 100
			&& playerPtr->getHitbox().top < this->getHitbox().top + 100)
		{
			if (playerPtr->getHitbox().left > this->getHitbox().left - 100
				&& playerPtr->getHitbox().left < this->getHitbox().left)
			{
				this->facingLeft = 1;
				this->isWalking = 1;
			}
			if (playerPtr->getHitbox().left < this->getHitbox().left + 100
				&& playerPtr->getHitbox().left > this->getHitbox().left)
			{
				this->facingLeft = 0;
				this->isWalking = 1;
			}
		}
	}
public:
	virtual void updateObject(std::list<std::shared_ptr<GameObject>>&)override
	{
		gravity();
		detectPlayer();
		move();
		animate();
	}
	virtual void collision(GameObject* other, CollisionSide side)override
	{
		switch (other->getObjectType())
		{
		case ObjectType::staticBlock:
			collisionWithStaticBlock(*other, side);
			break;
		case ObjectType::playerAttack:
			receiveDamage(side);
			break;
		}
	}
};