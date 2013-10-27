/*
	This is the "Lego Rush" game-wannabe created by Krzysztof 'hun7er' Marciniak (@hun7err) for the 0h game jam.
	Want to use and/or modify the code? Of course you can, just one condition: please let me know (on twitter maybe?),
	hope you will be able to create something better than I did ;-)
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#define PI 3.1415

#define DegToRad(x) (x * PI / 180.0)

bool collide(const sf::Sprite& player,  sf::RectangleShape& obj)
{
	if(obj.getPosition().x + obj.getSize().x > player.getPosition().x && obj.getPosition().x < player.getPosition().x + 60)
	{
		if(obj.getPosition().y + obj.getSize().y > player.getPosition().y + player.getGlobalBounds().height - 70 && obj.getPosition().y < player.getPosition().y + player.getGlobalBounds().height)
		{
			return false;
		}
	}

	if(obj.getPosition().x + obj.getSize().x > player.getPosition().x && obj.getPosition().x < player.getPosition().x + player.getGlobalBounds().width-40)
	{
		if(obj.getPosition().y + obj.getSize().y > player.getPosition().y && obj.getPosition().y  < player.getPosition().y + player.getGlobalBounds().height)
		{
			return true;
		}
	}
	return false;
}

bool loadMap(std::vector<sf::RectangleShape*>& map, std::string filename)
{
	std::ifstream in(filename);
	if(in.is_open())
	{
		char ch;
		sf::RectangleShape* rect;

		sf::Color colors[] = {sf::Color::Red, sf::Color::Yellow, sf::Color::White};

		while(!in.eof())
		{
			int i = 0;
			while(in >> std::noskipws >> ch)
			{
				if(ch == '.' || ch == 'b')
				{
					if(ch == 'b')
					{
						rect = new sf::RectangleShape(sf::Vector2f(15, 6));
						rect->setPosition(100 + i*30, 424);
						rect->setFillColor(colors[rand() % 3]);
						map.push_back(rect);
					}
					i++;
				}
			}
		}
		return true;
	}
	return false;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 500), "Lego rush - don't step on lego!");
	sf::RectangleShape background(sf::Vector2f(800, 500)), floor(sf::Vector2f(800, 70));
	background.setFillColor(sf::Color::Black);
	sf::Font bitFont;
	if(!bitFont.loadFromFile("res/8-bit Madness.ttf"))
	{
		return -1;
	}
	
	sf::Text text;
	text.setFont(bitFont);
	text.setString("LOADING RESOURCES...");
	text.setCharacterSize(50);
	text.setColor(sf::Color::White);
	text.setPosition((window.getSize().x - text.getGlobalBounds().width)/2, (window.getSize().y - 2*text.getGlobalBounds().height)/2);
	
	window.draw(text);

	window.display();
	sf::Color wall(0, 105, 236);
	background.setFillColor(wall);

	sf::Texture texture;
	if(!texture.loadFromFile("res/player.png", sf::IntRect(0, 0, 15, 150)))
	{
		return -1;
	}
	texture.setSmooth(false);
	texture.setRepeated(false);

	sf::Sprite player;
	player.setTexture(texture);
	player.setTextureRect(sf::IntRect(0, 0, 15, 25));
	player.setScale(10.0f, 10.0f);
	player.setPosition(20, window.getSize().y - player.getGlobalBounds().height - floor.getSize().y);
	floor.setPosition(0, window.getSize().y - floor.getSize().y);
	floor.setFillColor(sf::Color::Black);
	sf::SoundBuffer buffer, painbuffer;
	
	if(!buffer.loadFromFile("res/what is love 8 bit.ogg") || !painbuffer.loadFromFile("res/the_pain.ogg"))
	{
		return -1;
	}
	
	sf::Sound music, pain;
	music.setBuffer(buffer);
	music.setVolume(50);
	music.play();

	pain.setBuffer(painbuffer);
	pain.setVolume(50);

	std::vector<sf::RectangleShape*> blocks;
	loadMap(blocks, "res/basic_map.txt");

	sf::Clock clock;

	double x = 80, y = window.getSize().y - player.getGlobalBounds().height - floor.getSize().y, alpha = DegToRad(60.0), v = 530.0, g = 1200;
	bool jumping = false;

	window.setFramerateLimit(60);
	
	int i = 0;
	sf::Clock currentTime;

	bool playing = false, dead = false;

	text.setCharacterSize(30);
	text.setString("Welcome to LEGO RUSH, a 0h game jam game\nby Krzysztof 'hun7er' Marciniak (hun7err on twitter)\n\nLong story short:\nYou are Tom, a burglar who nearly got caught\nand now you need to escape not stepping\non the lego blocks laying everywhere in\nthe corridor. The only key used is space (to jump)\nSources:\n\nasdf movie: http://youtu.be/IYnsfV5N2n8\nwhat is love 8-bit: http://youtu.be/CT8t_1JXWn8\n\n\n[press any key to start]");
	text.setPosition((window.getSize().x - text.getGlobalBounds().width)/2, (window.getSize().y - text.getGlobalBounds().height)/2);
	window.draw(text);
	window.display();

	sf::RectangleShape block(sf::Vector2f(15, 6));
	block.setFillColor(sf::Color::Red);

	std::vector<sf::RectangleShape*>::iterator lastBlockPos = blocks.end()-2;
	sf::RectangleShape* lastBlock = *lastBlockPos;

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();

			else if(event.type == sf::Event::KeyPressed && !dead)
			{
				playing = true;
				if(event.key.code == sf::Keyboard::Space && !jumping && playing)
				{
					jumping = true;
					clock.restart();
				}

				else if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
			}
			
		} 

		window.clear();

		if(player.getPosition().x > lastBlock->getPosition().x + lastBlock->getSize().x + 50)
		{
			playing = false;
			sf::Color c(0, 0, 0, 200);
			background.setFillColor(c);
			text.setString("YOU WIN!");
			text.setCharacterSize(70);
			text.setPosition((window.getSize().x - text.getGlobalBounds().width)/2, (window.getSize().y - 4*text.getGlobalBounds().height)/2);
			window.draw(background);
			window.draw(text);
			window.display();
		}

		if(playing && !dead)
		{
			double t = clock.getElapsedTime().asSeconds();
		
			double x2 = x, y2 = y;
			if(jumping)
			{
				x2 += v * t * cos(alpha);
				y2 += -v * t * sin(alpha) + 0.5 * g * t*t;
			}
			if(y2 >= y && jumping)
			{
				jumping = false;
			}

			player.setPosition(x, y2);
			if(collide(player, block))
			{
				block.setFillColor(sf::Color::Magenta);
			}
			else
			{
				block.setFillColor(sf::Color::Red);
			}

			window.draw(background);
			window.draw(floor);
			window.draw(player);
			block.setPosition(50+player.getGlobalBounds().width+10, y+player.getGlobalBounds().height - block.getSize().y);
			for(std::vector<sf::RectangleShape*>::iterator it = blocks.begin(); it != blocks.end(); ++it)
			{
				window.draw(**it);
			}
			window.display();

			if(currentTime.getElapsedTime().asMilliseconds() > 50)
			{
				sf::Vector2f pos;
				for(std::vector<sf::RectangleShape*>::iterator it = blocks.begin(); it != blocks.end(); ++it)
				{
					pos = (*it)->getPosition();
					pos.x -= 7;
					(*it)->setPosition(pos);
					if(collide(player, **it) && playing && !dead)
					{
						playing = false;
						jumping = false;
						dead = true;
						music.stop();
						pain.play();
						sf::Color c(0, 0, 0, 200);
						background.setFillColor(c);
						text.setString("GAME OVER");
						text.setCharacterSize(70);
						text.setPosition((window.getSize().x - text.getGlobalBounds().width)/2, (window.getSize().y - 4*text.getGlobalBounds().height)/2);
						window.draw(background);
						window.draw(text);
						window.display();
					}
				}

				if(currentTime.getElapsedTime().asMilliseconds() > 100)
				{
					currentTime.restart();
					i++;
				}
			}
			if(!jumping)
			{
				player.setTextureRect(sf::IntRect(0, (((i % 4) + 2) * 25), 15, 25));
			}
			else
			{
				player.setTextureRect(sf::IntRect(0, 25, 15, 25));
			}
		}
	}

	for(std::vector<sf::RectangleShape*>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		delete *it;

	return 0;
}