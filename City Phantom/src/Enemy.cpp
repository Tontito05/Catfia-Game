#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Vector2D.h"
#include "Map.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name = "enemy";
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, texW / 1.5, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Enemy::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);
	// L13: TODO 3:	Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared)

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		SetPosition(OGposition);
	}

		if (SightDistance <= 5)
		{
			pathfinding->PropagateAStar(MANHATTAN);
			SightDistance++;
		}
		else
		{
			Vector2D pos = GetPosition();
			Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
			pathfinding->ResetPath(tilePos);
			SightDistance = 0;
		}

		Vector2D PosInMap = Engine::GetInstance().map->WorldToMap(position.getX(),position.getY());

		if ((pathfinding->IsWalkable(PosInMap.getX() + 1, PosInMap.getY()) == true)
			&& (pathfinding->foundDestination != true)
			&& (stat == States::WALKING_R))
		{
			velocity.x = 0.4 * 16;
		}
		else if ((pathfinding->IsWalkable(PosInMap.getX() + 1, PosInMap.getY()) == false)
				&& (pathfinding->foundDestination != true)
				&& (stat == States::WALKING_R))
		{
			stat = States::WALKING_L;
		}

		if ((pathfinding->IsWalkable(PosInMap.getX(), PosInMap.getY()) == true)
			&& (pathfinding->foundDestination != true)
			&& (stat == States::WALKING_L))
		{
			velocity.x = -0.4 * 16;
		}
		else if ((pathfinding->IsWalkable(PosInMap.getX() , PosInMap.getY()) == false)
			&& (pathfinding->foundDestination != true)
			&& (stat == States::WALKING_L))
		{
			stat = States::WALKING_R;
		}

		pbody->body->SetLinearVelocity(velocity);

	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	// Draw pathfinding 
	pathfinding->DrawPath();

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}