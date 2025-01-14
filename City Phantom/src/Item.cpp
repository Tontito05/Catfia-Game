#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "tracy/Tracy.hpp"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() 
{

}

bool Item::Awake() {


	return true;
}

bool Item::Start() {


	h = 32;
	w = 32;
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + w / 2, (int)position.getY() + h / 2, w, h, bodyType::STATIC);

	b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics->ITEM_LAYER;
	pbody->body->GetFixtureList()[0].SetFilterData(filter);

	pbody->body->GetFixtureList()[0].SetSensor(true);

	//initilize textures
	switch (type)
	{
	case ItemType::COIN:
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Coin.png");
		pbody->ctype = ColliderType::COIN;
		break;
	case ItemType::HEART:
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Life.png");
		pbody->ctype = ColliderType::HEART;
		break;
	case ItemType::DASH:
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Power.png");
		pbody->ctype = ColliderType::DASH;
		break;
	case ItemType::KEY:
		break;
	default:
		break;
	}

	idleAnim.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnim = &idleAnim;

	pbody->listener = this;

	return true;
}

bool Item::Update(float dt)
{
	ZoneScoped;

	if (isPicked)
	{
		Disable();
		pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	}
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - w / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - h / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnim->GetCurrentFrame());
	currentAnim->Update();



	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:

		isPicked = true;

		break;
	}
}