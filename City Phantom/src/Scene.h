#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "Boss.h"
#include "Vector2D.h"
#include <vector>
#include "GuiControlButton.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LoadState();

	void SaveState();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	// Return the player position
	Vector2D GetPlayerPosition();

	float Slower(float ogPos, float goalPos, float time);

	void Create();
	MapLayer* layerNav;
	Map* map;
	int enemyCounter = 0;
	std::vector<Enemy*> enemyList;
	bool in = false;
	
	SDL_Texture* mouseTileTex = nullptr;

	std::vector<Item*> ItemList;
	Boss* Boss_;
	int CoinCounter = 0;
	int HeartCounter = 0;

	GuiControlButton* startbt;
	GuiControlButton* exitbt;
	GuiControlButton* settingsbt;
	
	GuiControlButton* layout;

	std::vector<GuiControlButton*> guiButtons;
	GuiControlButton* menuLayout;


	GuiControlButton* playBt;
	GuiControlButton* continueBt;
	GuiControlButton* quitgameMMBT;
	GuiControlButton* creditsBt;
	GuiControlButton* exitBt;

	std::vector<GuiControlButton*> guiButtonsMM;

	GuiControlButton* returnSTBT;

	std::vector<GuiControlButton*> guiButtonsSettings;

	bool gameStarted = false;



private:

	SDL_Texture* img;
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	int enemySky = 560;
	int enemyGround = 561;
	int coin = 570;
	int Slime = 571;
	int heart = 572;
	bool enemyesIn = false;

	int WWidth;
	int WHeight;



};