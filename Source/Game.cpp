#include <string>

#include <Engine/Keys.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Sprite.h>

#include "Game.h"

/**
*   @brief   Default Constructor.
*   @details Consider setting the game's width and height
             and even seeding the random number generator.
*/
BreakoutGame::BreakoutGame()
{
}

/**
*   @brief   Destructor.
*   @details Remove any non-managed memory and callbacks.
*/
BreakoutGame::~BreakoutGame()
{
	this->inputs->unregisterCallback(key_callback_id);
	this->inputs->unregisterCallback(mouse_callback_id);


}

/**
*   @brief   Initialises the game.
*   @details The game window is created and all assets required to
			 run the game are loaded. The keyHandler and clickHandler
			 callback should also be set in the initialise function.
*   @return  True if the game initialised correctly.
*/
bool BreakoutGame::init()
{
	setupResolution();
	if (!initAPI())
	{
		return false;
	}

	toggleFPS();
	renderer->setWindowTitle("Breakout!");
	renderer->setClearColour(ASGE::COLOURS::BLACK);

	// input handling functions
	inputs->use_threads = false;

	key_callback_id = inputs->addCallbackFnc(
		ASGE::E_KEY, &BreakoutGame::keyHandler, this);
	
	mouse_callback_id =inputs->addCallbackFnc(
		ASGE::E_MOUSE_CLICK, &BreakoutGame::clickHandler, this);

	for (int i = 0; i < 48; i++)
	{
		block[i] = new GameObject;
		if (block[i]->addSpriteComponent(renderer.get(), ".\\Resources\\Textures\\puzzlepack\\png\\element_purple_rectangle.png") == false)
		{
			load = false;
		}
		block_sprite[i] = block[i]->spriteComponent()->getSprite();
		block_sprite[i]->xPos(38 + ((i % 6) * 100));
		block_sprite[i]->yPos(30 + (floor(i / 6) * 50));
	}

	for (int i = 0; i < 3; i++)
	{
		gem[i] = new GameObject;
		if (gem[i]->addSpriteComponent(renderer.get(), ".\\Resources\\Textures\\puzzlepack\\png\\element_green_diamond.png") == false)
		{
			load = false;
		}
		gem_sprite[i] = gem[i]->spriteComponent()->getSprite();
		gem[i]->direction = new vector2(0, 1);
		gem[i]->visibility = false;
	}

	ball = new GameObject;
	if(ball->addSpriteComponent(renderer.get(), ".\\Resources\\Textures\\puzzlepack\\png\\ballGrey.png") ==  false);
	{
		load = false;
	}
	ball_sprite = ball->spriteComponent()->getSprite();
	ball_sprite->xPos(300);
	ball_sprite->yPos(700);
	ball->direction->y = 1;
	ball->direction->x = 1;

	paddle = new GameObject;
	if (paddle->addSpriteComponent(renderer.get(), ".\\Resources\\Textures\\puzzlepack\\png\\paddleBlue.png") == false);
	{
		load = false;
	}
	paddle_sprite = paddle->spriteComponent()->getSprite();
	paddle_sprite->xPos(250);
	paddle_sprite->yPos(850);

	srand(0);
	nextGem = (rand() % 1000) + 1000;


	return true;
}

/**
*   @brief   Sets the game window resolution
*   @details This function is designed to create the window size, any 
             aspect ratio scaling factors and safe zones to ensure the 
			 game frames when resolutions are changed in size.
*   @return  void
*/
void BreakoutGame::setupResolution()
{
	// how will you calculate the game's resolution?
	// will it scale correctly in full screen? what AR will you use?
	// how will the game be framed in native 16:9 resolutions?
	// here are some abritrary values for you to adjust as you see fit
	// https://www.gamasutra.com/blogs/KenanBolukbasi/20171002/306822/Scaling_and_MultiResolution_in_2D_Games.php
	game_width = 640;
	game_height = 920;
}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
			 keyboard input. For this game, calls to this function
			 are thread safe, so you may alter the game's state as you
			 see fit.
*   @param   data The event data relating to key input.
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::keyHandler(const ASGE::SharedEventData data)
{
	auto key = static_cast<const ASGE::KeyEvent*>(data.get());
	
	if (key->key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}

	if (key->key == ASGE::KEYS::KEY_ENTER &&
		key->action == ASGE::KEYS::KEY_PRESSED)
	{
		in_menu = false;
	}

	if (key->key == ASGE::KEYS::KEY_A &&
		key->action == ASGE::KEYS::KEY_PRESSED)
	{
		paddle_move = -1;
	}

	if (key->key == ASGE::KEYS::KEY_D &&
		key->action == ASGE::KEYS::KEY_PRESSED)
	{
		paddle_move = 1;
	}

	if (key->key == ASGE::KEYS::KEY_A &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		paddle_move = 0;
	}

	if (key->key == ASGE::KEYS::KEY_D &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		paddle_move = 0;
	}

}

/**
*   @brief   Processes any click inputs
*   @details This function is added as a callback to handle the game's
		     mouse button input. For this game, calls to this function
             are thread safe, so you may alter the game's state as you
             see fit.
*   @param   data The event data relating to key input.
*   @see     ClickEvent
*   @return  void
*/
void BreakoutGame::clickHandler(const ASGE::SharedEventData data)
{
	auto click = static_cast<const ASGE::ClickEvent*>(data.get());

	double x_pos, y_pos;
	inputs->getCursorPos(x_pos, y_pos);
}


/**
*   @brief   Updates the scene
*   @details Prepares the renderer subsystem before drawing the
		     current frame. Once the current frame is has finished
		     the buffers are swapped accordingly and the image shown.
*   @return  void
*/
void BreakoutGame::update(const ASGE::GameTime& us)
{
	auto paddle_x = paddle_sprite->xPos();
	auto ball_x = ball_sprite->xPos();
	auto ball_y = ball_sprite->yPos();
	for (int i = 0; i < 3; i++)
	{
		 gem_y[i] = gem_sprite[i]->yPos();
	}
	auto dt_sec = us.delta_time.count() / 1000.0;

	rect ball_box = ball->spriteComponent()->getBoundingBox();
	rect paddle_box = paddle->spriteComponent()->getBoundingBox();
	rect block_box[48] = { NULL };
	rect gem_box[3] = { NULL };
	for (int i = 0; i < 48; i++)
	{
		block_box[i] = block[i]->spriteComponent()->getBoundingBox();
		if (block_box[i].isInside(ball_box) && block[i]->visibility == true)
		{
			ball->direction->y *= -1;
			block[i]->visibility = false;
			score += 2;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		gem_box[i] = gem[i]->spriteComponent()->getBoundingBox();
		if (gem_box[i].isInside(paddle_box) && gem[i]->visibility == true)
		{
			gem[i]->visibility = false;
			gemOnScreen--;
			score += 5;
		}
	}

	if (paddle_box.isInside(ball_box))
	{
		ball->direction->y *= -1;
	}

	if (ball_sprite->xPos() + ball_sprite->width() > game_width ||
		ball_sprite->xPos() < 0)
	{
		ball->direction->x *= -1;
	}

	if (ball_sprite->yPos() < 0)
	{
		ball->direction->y *= -1;
	}

	if (ball_sprite->yPos() > game_height + 100)
	{
		if (win == 0)
		{
			lives--;
			if (lives > 0)
			{
				ball_x = 300;
				ball_y = 700;
			}
			else
			{
				win = -1;
			}
		}
	}

	blocks_dead = true;
	for (int i = 0; i < 48; i++)
	{
		if (block[i]->visibility == true)
		{
			blocks_dead = false;
		}
	}
	if (blocks_dead)
	{
		win = 1;
	}

	if (!in_menu)
	{
		paddle_x += 500 * dt_sec * paddle_move;
		ball_x += 400 * ball->speed * dt_sec * ball->direction->x;
		ball_y += 400 * ball->speed * dt_sec * ball->direction->y;
		for (int i = 0; i < 3; i++)
		{
			if (gem[i]->visibility)
			{
				gem_y[i] += 300 * dt_sec * gem[i]->direction->y;
				if (gem_sprite[i]->yPos() > game_height)
				{
					gemOnScreen--;
					gem[i]->visibility = false;
				}
			}
		}
		
	}

	paddle_sprite->xPos(paddle_x);
	ball_sprite->xPos(ball_x);
	ball_sprite->yPos(ball_y);
	for (int i = 0; i < 3; i++)
	{
		gem_sprite[i]->yPos(gem_y[i]);
	}
	
	if (!in_menu && win == 0)
	{
		timeCount += us.delta_time.count();
		if (timeCount > nextGem)
		{
			dropGem();
			nextGem = (rand() % 5000) + 2000;
			timeCount = 0;
		}
	}
	//make sure you use delta time in any movement calculations!

	

}

/**
*   @brief   Renders the scene
*   @details Renders all the game objects to the current frame.
	         Once the current frame is has finished the buffers are
			 swapped accordingly and the image shown.
*   @return  void
*/
void BreakoutGame::render(const ASGE::GameTime &)
{
	renderer->setFont(0);

	if (in_menu)
	{
		renderer->renderText("breakout" , 200, 200);
	}
	else
	{
		for (int i = 0; i < 48; i++)
		{
			if (block[i]->visibility)
			{
				renderer->renderSprite(*block_sprite[i]);
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (gem[i]->visibility)
			{
				renderer->renderSprite(*gem_sprite[i]);
			}
		}
		renderer->renderSprite(*ball_sprite);
		renderer->renderSprite(*paddle_sprite);
		renderer->renderText(std::to_string(lives), 40, 20);
		renderer->renderText(std::to_string(score), 200, 20);
		if (win == -1)
		{
			renderer->renderText("you lose!!!!", 300, 500);
		}
		if (win == 1)
		{
			renderer->renderText("you win!!!!", 300, 500);
		}
	}


}

void BreakoutGame::dropGem()
{
	if (gemOnScreen != 3);
	{
		gemOnScreen++;
		gem_sprite[gemNum]->xPos(rand() % game_width - gem_sprite[gemNum]->width());
		gem_sprite[gemNum]->yPos(gem_sprite[gemNum]->height() * -1);
		gem[gemNum]->visibility = true;
		gemNum++;
		if (gemNum >= 3)
		{
			gemNum = 0;
		}
	}
}
