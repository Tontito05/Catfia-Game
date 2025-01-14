#include "Boss.h"
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
#include "tracy/Tracy.hpp"

Boss::Boss(EntityType type_) : Entity(type_)
{
	name = "enemy";
}

Boss::~Boss() {}

bool Boss::Awake() {
	return true;
}

bool Boss::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.child("position").attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();


	//Load animations

	walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
	walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
	attackleft.LoadAnimations(parameters.child("animations").child("attackleft"));
	attackright.LoadAnimations(parameters.child("animations").child("attackright"));
	dying.LoadAnimations(parameters.child("animations").child("dying"));
	stunned.LoadAnimations(parameters.child("animations").child("stuned"));
	damage.LoadAnimations(parameters.child("animations").child("damage"));
	currentAnimation = &walkingright;

	enemyMovementSfx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/enemy_sfx.ogg");
	enemydamage = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/damage_sfx.ogg");

	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 1.2, texW / 2.1, bodyType::DYNAMIC);


	//Assign collider type
	pbody->ctype = ColliderType::BOSS;
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;

	//Assign a layer only for the enemyes
	b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics->ENEMY_LAYER;
	pbody->body->GetFixtureList()[0].SetFilterData(filter);

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	attTimer.Start();

	return true;
}

bool Boss::Update(float dt)
{
	ZoneScoped;

	if ((state != States::DYING) && (state != States::DAMAGE))
	{
		CheckIfAttack();
	}

	if (isDead == false)
	{
		switch (state)
		{
		case States::WALKING_L:

			Vel.x = -0.5 * 16;
			Vel.y = -GRAVITY_Y;
			pbody->body->SetLinearVelocity(Vel);
			currentAnimation = &walkingleft;

			break;
		case States::WALKING_R:

			Vel.x = 0.5 * 16;
			Vel.y = -GRAVITY_Y;
			pbody->body->SetLinearVelocity(Vel);
			currentAnimation = &walkingright;

			break;
		case States::ATTACK_L:

			Vel.x = -0.8 * 16;
			Vel.y = -GRAVITY_Y;
			pbody->body->SetLinearVelocity(Vel);
			currentAnimation = &attackleft;

			break;
		case States::ATTACK_R:

			Vel.x = 0.8 * 16;
			Vel.y = -GRAVITY_Y;
			pbody->body->SetLinearVelocity(Vel);
			currentAnimation = &attackright;

			break;
		case States::DAMAGE:
			currentAnimation = &damage;
			pbody->body->SetLinearVelocity({ 0,-GRAVITY_Y });
			if(currentAnimation->HasFinished())
			{
				state = States::WALKING_L;
			}
			break;
		case States::DYING:
			isDead = true;
			currentAnimation = &dying;
			break;
		case States::STUNNED:
			pbody->body->SetLinearVelocity({0,-GRAVITY_Y});
			currentAnimation = &stunned;
			break;
		default:
			break;
		}

	}
	else
	{
		state = States::DYING;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool Boss::CleanUp()
{
	return true;
}

void Boss::SetPosition(Vector2D pos) {

	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Boss::GetPosition() {

	if (pbody == nullptr || pbody->body == nullptr) {
		// Handle the error, e.g., return a default position or log an error
		return Vector2D(0, 0);
	}
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Boss::Reset() {

	isDead = false;
	state = States::IDLE_L;
	CleanUp();
	pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	Awake();
	Start();
	SetPosition(OGposition);
}

bool Boss::CheckIfAttack()
{
	if (attTimer.ReadSec() < attDuration)
	{
		//Attacking
		if (state == States::WALKING_R)
		{
			state = States::ATTACK_R;
		}
		else if (state == States::WALKING_L)
		{
			state = States::ATTACK_L;
		}
		else if(state == States::STUNNED)
		{
			state = States::WALKING_L;
		}

		return true;
	}
	else if (attTimer.ReadSec() > attDuration && attTimer.ReadSec() < (attDuration * 3))
	{
		//No attaking
		if (loops <= 0)
		{
			state = States::STUNNED;
		}
		else if (state == States::ATTACK_L)
		{
			state = States::WALKING_L;
		}
		else if (state == States::ATTACK_R)
		{
			state = States::WALKING_R;
		}

		return false;
	}
	else if (attTimer.ReadSec() > (attDuration*3))
	{
		if (loops <= 0)
		{
			loops = 3;
		}

		attTimer.Start();
		loops--;
	}

	return true;

}

void Boss::Damaged()
{
	if (health > 0)
	{
		health--;
		state = States::DAMAGE;
		loops = 3;
		Engine::GetInstance().audio.get()->PlayFx(enemydamage);
	}
	else
	{
		b2Filter filter;
		// CatBits tipo de layer, maskBits con que layers colisiona
		filter.categoryBits = 0;
		filter.maskBits = 0;
		//Ponidendo nuevo filtro
		pbody->body->GetFixtureList()[0].SetFilterData(filter);
		pbody->body->SetGravityScale(0);

		state = States::DYING;
	}
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::WALL:

		switch (state)
		{

		case States::WALKING_L:
			state = States::WALKING_R;
			break;
		case States::WALKING_R:
			state = States::WALKING_L;
			break;
		case States::ATTACK_L:
			state = States::ATTACK_R;
			break;
		case States::ATTACK_R:
			state = States::ATTACK_L;
			break;
		}

		break;

	}
}