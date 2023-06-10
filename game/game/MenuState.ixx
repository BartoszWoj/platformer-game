export module MenuState;
import State;
import Button;
import Background;

export class MenuState : public State
{
protected:
	int currentlySelected = 0;

	bool Zreleased = 0;
	bool UPreleased = 0;
	bool DOWNreleased = 0;

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

	sf::Text title;
	sf::Text credit;
	Button mainMenuButton[2];

public:
	MenuState(ResourceHolder& resources)
	{
		MyText myTitle(-70, -100, "Platformer Game", resources, 64);
		this->title = myTitle.getText();
		MyText myCredit(-20, -80, "By Milosz Cebula", resources, 32);
		this->credit = myCredit.getText();
		mainMenuButton[0] = Button(-50, -35, "Start Game", resources);
		mainMenuButton[1] = Button(-50, 0, "Exit", resources);
		this->stateID = 0;
		this->mainMenuButton[0].select();
	}

	void update() override
	{
		navigate(this->mainMenuButton, 2);
		if (choose())
		{
			switch (this->currentlySelected)
			{
			case 0:
				stateID = 1;
				this->level = this->currentlySelected + 1;
				break;
			case 1:
				this->exiting = 1;
				break;
			}
		}
		updateButtons(this->mainMenuButton, 2);
	}

	void render(sf::RenderTarget& target)override
	{
		drawBackground(target);
		target.draw(this->title);
		target.draw(this->credit);
		for (int i = 0; i < 2; i++)
			this->mainMenuButton[i].draw(target);

		sf::View view(sf::Vector2f(0, -32), sf::Vector2f(target.getSize().x / 4, target.getSize().y / 4));
		target.setView(view);
	}
};