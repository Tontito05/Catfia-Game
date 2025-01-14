#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(int id, SDL_Rect bounds, const char* text);
	virtual ~GuiControlButton();

	// Called each loop iteration
	
	bool isLayout = false;
	bool isLayoutMenu = false;
	bool isClicked = false;
	bool Isvisible = true;
	bool isMenu = false;
	// Called each loop iteration
	bool Update(float dt);

private:

	bool canClick = true;
	bool drawBasic = false;
	bool isLayoutSet = false;
	SDL_Texture* layoutUI;
	SDL_Texture* menuUI;
};

