export module ResourceHolder;
import <iostream>;
import <fstream>;
import <filesystem>;
import <regex>;
import <SFML/Graphics.hpp>;

export class ResourceHolder final
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
				if (std::regex_match(fileName, reg))				//regex - veryfying files names and extracting textures names
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
		std::filesystem::path fontsPath = std::filesystem::current_path() /= "resources\\fonts";
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