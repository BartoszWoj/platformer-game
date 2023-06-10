export module State;

export import <SFML/Graphics.hpp>;

export class State
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