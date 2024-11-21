#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Vector2D.h"
#include <vector>

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

	// Return the player position
	Vector2D GetPlayerPosition();

	void CreateEnemyes();
	MapLayer* layerNav;
	Map* map;
	int enemyCounter = 0;

private:

	SDL_Texture* img;

	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;
	int enemySet = 146;
	bool enemyesIn = false;

};