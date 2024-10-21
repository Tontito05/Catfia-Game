#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(100, 100);
	return true;
}

bool Player::Start() {
	
	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/gato de pie first bocetos.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	return true;
}

bool Player::Update(float dt)
{
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);


	if (isJumping == true)
	{
		LOG("%f", pbody->body->GetLinearVelocity().y);
	}
		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.4 * dt;

			//Set the dash so the player can use it on the LEFT
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && isDashingL == false) {
				// Apply an initial Left force
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);
				isDashingL = true;
			}
		}

		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.4 * dt;

			//Set the dash so the player can use it on the RIGHT
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && isDashingR == false) {
				// Apply an initial Right force
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);
				isDashingR = true;
			}
		}

		//Jump
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
			// Apply an initial upward force
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			isJumping = true;
		}

		// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
		if (isJumping == true)
		{
			velocity = pbody->body->GetLinearVelocity();
			//We insert this here so the player camn move during the jump, so we dont limit the movement
			//Move Left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = -0.3 * dt;
			}
			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = 0.3 * dt;
			}
		}

		//Glovals to add --> DashForce / DashSlower / PlayerVelocity 

		//Right Dash
		if (CanDash == true)
		{
			if (isDashingR == true)
			{
				//The parameter that creates the slowing sensation of the dash
				DashSlower -= 0.01;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
				velocity = pbody->body->GetLinearVelocity();

				//where we look if the dash has finished or not
				DashForce += DashSlower;
				if (DashForce <= 0)
				{
					ResetDash();
				}
			}
			//Left Dash
			else if (isDashingL == true)
			{
				//Same as on top
				DashSlower += 0.01;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
				velocity = pbody->body->GetLinearVelocity();
				DashForce -= DashSlower;
				if (DashForce <= 0)
				{
					ResetDash();
				}
			}
		}

		// Apply the velocity to the player
		pbody->body->SetLinearVelocity(velocity);

		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground

		isJumping = false;			
		JumpMinus = 1;
		isDashingL = false;
		isDashingR = false;
		CanDash = true;

		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::WALL:
		LOG("End Collision WALL");
		break;
	default:
		break;
	}
}

void Player::ResetDash()
{
	DashForce = 4;
	DashSlower = 0;
	CanDash = false;
}