#pragma once
#include <string>
#include <Engine/OGLGame.h>

#include "GameObject.h"
#include "Rect.h"


/**
*  An OpenGL Game based on ASGE.
*/
class BreakoutGame :
	public ASGE::OGLGame
{
public:
	BreakoutGame();
	~BreakoutGame();
	virtual bool init() override;

private:
	void keyHandler(const ASGE::SharedEventData data);
	void clickHandler(const ASGE::SharedEventData data);
	void setupResolution();

	virtual void update(const ASGE::GameTime &) override;
	virtual void render(const ASGE::GameTime &) override;

	int  key_callback_id = -1;	        /**< Key Input Callback ID. */
	int  mouse_callback_id = -1;        /**< Mouse Input Callback ID. */

	int lives = 3;
	int win = 0;
	bool blocks_dead = false;

	//Add your GameObjects

	GameObject* block[48] = { nullptr };
	GameObject* ball = nullptr;
	GameObject* paddle = nullptr;

	ASGE::Sprite* block_sprite[48] = { nullptr };
	ASGE::Sprite* ball_sprite = nullptr;
	ASGE::Sprite* paddle_sprite = nullptr;

	int paddle_move = 0;

	bool load = true;

	bool in_menu = true;
};