#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;

}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(itemNode);
	}

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}

	return ret;


}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "Map.tmx");
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	map = Engine::GetInstance().map.get();
	layerNav = map->GetNavigationLayer();
	if (enemyesIn == false)
	{
		CreateEnemyes();
		enemyesIn = true;
	}

	return true;
}

void Scene::CreateEnemyes()
{
	for (int x = 0; x < map->GetWidth(); x++)
	{
		for (int y = 0; y < map->GetHeight(); y++)
		{
			if (layerNav != nullptr) {
				if (x >= 0 && y >= 0 && x < map->GetWidth() && y < map->GetHeight()) {

					int gid = layerNav->Get(x, y);
					if (gid == enemySet)
					{
						enemyList[enemyCounter]->SetPosition(map->MapToWorld(x,y));
						enemyList[enemyCounter]->ResetPath();

						enemyCounter++;
					}
				}
			}
		}
	}

}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 2;
	
	//Camera movement
	int mapLimitX = 3328;
	int mapLimitY = 1184;

	if (player->position.getY() > Engine::GetInstance().window.get()->height / (camSpeed*2) &&
		player->position.getY() < mapLimitY - Engine::GetInstance().window.get()->height / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.y = (-player->position.getY() * camSpeed) + Engine::GetInstance().window.get()->height / 2;
	}
	if (player->position.getX() > Engine::GetInstance().window.get()->width / (camSpeed * 2) &&
		player->position.getX() < mapLimitX - Engine::GetInstance().window.get()->width / (camSpeed*2))
	{
		Engine::GetInstance().render.get()->camera.x = (-player->position.getX() * camSpeed) + Engine::GetInstance().window.get()->width / 2;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {

		LOG("%d", Engine::GetInstance().window.get()->width);
		LOG("%d", Engine::GetInstance().window.get()->height);

	}

	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
		mousePos.getY() - Engine::GetInstance().render.get()->camera.y);


	//Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(), mouseTile.getY());
	SDL_Rect rect = { 0,0,32,32 };
	Engine::GetInstance().render.get()->DrawTexture(mouseTileTex,
		highlightTile.getX(),
		highlightTile.getY(),
		&rect);

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	//If mouse button is pressed modify enemy position
	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
		enemyList[0]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
		enemyList[0]->ResetPath();
	}

	return true;



}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	SDL_DestroyTexture(img);
	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}
