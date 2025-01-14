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
#include "tracy/Tracy.hpp"
#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"

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

	// Create a enemy using the entity manager 
	//the num of enemyes in tyhe level

	SDL_Rect startbutton = { 520, 300, 120,20 };
	SDL_Rect settingbutton = { 520, 350, 120,20 };
	SDL_Rect exitbutton = { 520, 400, 120,20 };
	
	SDL_Rect layoutBounds2 = { 0, -90, WWidth, WHeight };
	menuLayout = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Layout", layoutBounds2, this);
	menuLayout->isLayout = true;
	menuLayout->isMenu = true;

	startbt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "START GAME", startbutton, this);
	startbt->Isvisible = true;
	guiButtons.push_back(startbt);

	settingsbt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "SETTINGS", settingbutton, this);
	settingsbt->Isvisible = true;
	guiButtons.push_back(settingsbt);

	exitbt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "EXIT", exitbutton, this);
	exitbt->Isvisible = true;
	guiButtons.push_back(exitbt);

	





	return ret;


}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 

	Engine::GetInstance().map->Load("Assets/Maps/", "Map_.tmx");

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/background-music.wav", 0.0f);
	WWidth = Engine::GetInstance().window.get()->width;
	WHeight = Engine::GetInstance().window.get()->height;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	map = Engine::GetInstance().map.get();
	layerNav = map->GetNavigationLayer();
	if (enemyesIn == false)
	{
		Create();
		enemyesIn = true;
	}

	return true;
}

void Scene::Create()
{
	for (int x = 0; x < map->GetWidth(); x++)
	{
		for (int y = 0; y < map->GetHeight(); y++)
		{
			if (layerNav != nullptr) {
				if (x >= 0 && y >= 0 && x < map->GetWidth() && y < map->GetHeight()) {

					int gid = layerNav->Get(x, y);
					if (gid == enemyGround)
					{
							pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
							
								Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::WALKING_ENEMY);
								enemy->SetParameters(enemyNode);
								enemy->Start();
								enemy->OGposition = Vector2D(map->MapToWorld(x, y));
								enemy->SetPosition(enemy->OGposition);
								enemyList.push_back(enemy);
								enemyCounter++;
							
					}
					else if (gid == enemySky)
					{
						pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
						
							Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FYING_ENEMY);
							enemy->SetParameters(enemyNode);
							enemy->Start();
							enemy->OGposition = Vector2D(map->MapToWorld(x, y));
							enemy->SetPosition(enemy->OGposition);
							enemyList.push_back(enemy);
							enemyCounter++;
							
					}
					else if (gid == coin)
					{
						//L08 Create a new item using the entity manager and set the position to (200, 672) to test
						pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item");
						
						Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
						item->SetParameters(itemNode,ItemType::COIN);
						item->SetPosition(map->MapToWorld(x, y));
						item->Start();
						ItemList.push_back(item);
						CoinCounter++;
					}
					else if (gid == Slime)
					{
						pugi::xml_node BossNode = configParameters.child("entities").child("boss").child("slime");

						Boss_ = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::SLIME);
						Boss_->SetParameters(BossNode);
						Boss_->Start();
						Boss_->OGposition = Vector2D(map->MapToWorld(x, y));
						Boss_->SetPosition(Boss_->OGposition);

						enemyCounter++;
					}
					else if (gid == heart)
					{
						//L08 Create a new item using the entity manager and set the position to (200, 672) to test
						pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item");

						Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
						item->SetParameters(itemNode, ItemType::HEART);
						item->SetPosition(map->MapToWorld(x, y));
						item->Start();
						ItemList.push_back(item);
						HeartCounter++;
					}
				}
			}
		}
	}

	pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item");

	Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->SetParameters(itemNode, ItemType::DASH);
	item->SetPosition(map->MapToWorld(7, 46));
	item->Start();
	ItemList.push_back(item);
}

float Scene::Slower(float ogPos, float goalPos, float time)
{
	float acceleration = goalPos - ogPos;
	float speed = ogPos + time * acceleration;
	return speed;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	ZoneScoped;
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 2;
	
	//Camera movement
	Vector2D mapLimit = map->MapToWorld(map->GetWidth(),map->GetHeight());

	if (player->position.getY() > WHeight / (camSpeed*2) &&
		player->position.getY() < mapLimit.getY() - WHeight / (camSpeed * 2))
	{
		Engine::GetInstance().render.get()->camera.y = (-player->position.getY() * camSpeed) + WHeight / 2;
	}
	if (player->position.getX() > WWidth / (camSpeed * 2) &&
		player->position.getX() < mapLimit.getX() - WWidth / (camSpeed * 2))
	{
		Engine::GetInstance().render.get()->camera.x = Slower(Engine::GetInstance().render.get()->camera.x, (- player->position.getX() * camSpeed) + WWidth / 2, 0.2f);
	}

	int buildingEndgeX = map->MapToWorld(54, 47).getX();
	int buildingEndgeY = map->MapToWorld(48, 48).getY();

	if ((player->position.getX() > buildingEndgeX || player->position.getY() > buildingEndgeY)) {
		
		if (map->Building == false)
		{
			SaveState();
		}

		map->Building = true;

	}
	else
	{
		map->Building = false;
	}

	return true;

	if (startbt->isClicked==true) {

		startbt->Isvisible = false;

	}

}


// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

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



void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");
	if (!enemiesNode) {
		enemiesNode = sceneNode.child("entities").append_child("enemies");
	}
	

	for (auto& enemy : enemyList) {
		pugi::xml_node enemyNode = enemiesNode.append_child("enemy");
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(enemy->GetPosition().getX());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(enemy->GetPosition().getY());

		//enemyNode.append_attribute("alive") = enemy->IsAlive();
		if (enemy->IsAlive() == false) {

			enemyNode.append_attribute("alive") = false;

		}

	}

	pugi::xml_node ItemsNode = sceneNode.child("entities").child("items");
	if (!ItemsNode) {
		ItemsNode = sceneNode.child("entities").append_child("items");
	}


	for (auto& item : ItemList) {
		pugi::xml_node ItemNode = enemiesNode.append_child("enemy");
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(item->GetPosition().getX());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(item->GetPosition().getY());

		if (item->isPicked == true) {

			ItemNode.append_attribute("alive") = false;

		}

	}

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");

}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		(sceneNode.child("entities").child("player").attribute("y").as_int())-32);
	player->SetPosition(playerPos);

	pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");
	if (enemiesNode) {
		for (pugi::xml_node enemyNode = enemiesNode.child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy")) {
			float enemyX = enemyNode.child("position").attribute("x").as_float();
			float enemyY = enemyNode.child("position").attribute("y").as_float();

			bool isAlive = enemyNode.attribute("alive").as_bool(true);

			// Find the enemy by its ID and set the position
			for (auto& enemy : enemyList) {
				enemy->SetPosition(Vector2D(enemyX, enemyY));

				if (isAlive) {
					enemy->state = States::IDLE_R;;  // Enemy is alive
				}
				
				break;

			}
		}

	}
}
bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}
