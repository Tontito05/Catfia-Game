#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Engine.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
	PS = PlayerStates::IDLE;
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(100, 300);
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	menu = Engine::GetInstance().textures.get()->Load("Assets/Textures/Controlls.png");
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	jumpingright.LoadAnimations(parameters.child("animations").child("jumpingright"));
	jumpingleft.LoadAnimations(parameters.child("animations").child("jumpingleft"));
	walkingleft.LoadAnimations(parameters.child("animations").child("walkingleft"));
	walkingright.LoadAnimations(parameters.child("animations").child("walkingright"));
	dying.LoadAnimations(parameters.child("animations").child("dying"));
	currentAnimation = &idle;

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW/1.5,texH/1.5, bodyType::DYNAMIC);
	pbody->body->GetFixtureList()[0].SetFriction(0);
	pbody->body->SetFixedRotation(0);
	
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

	//God Mode Controll
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode==false)
	{

		Godmode = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode == true)
	{
		Godmode = false;
	}

	//Controlls Menu Controll
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN && inMenu == false)
	{
		inMenu = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN && inMenu == true)
	{
		inMenu = false;
	}


	if (inMenu == false && PS != PlayerStates::DEAD)
	{
		if (Godmode == false)
		{

			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && (PS == PlayerStates::IDLE || PS == PlayerStates::WALKING)) {

				PS = PlayerStates::WALKING;
				Player_POV = POV::LEFT;
			}
			// Move right
			else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && (PS == PlayerStates::IDLE || PS == PlayerStates::WALKING)) {

				PS = PlayerStates::WALKING;
				Player_POV = POV::RIGHT;
			}
			else if (PS == PlayerStates::WALKING)
			{
				PS = PlayerStates::IDLE;
			}

			//Jump
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && PS != PlayerStates::JUMPING) {
				// Apply an initial upward force
				PS = PlayerStates::JUMPING;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			}


			switch (PS)
			{
			case PlayerStates::DEAD:
				break;
			case PlayerStates::IDLE:
				break;

			case PlayerStates::WALKING:

				if (Player_POV == POV::LEFT)
				{
					velocity.x = -0.4 * 16;

					if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash == true) {
						// Apply an initial Left force
						pbody->body->SetLinearVelocity({ 0,0 });
						pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);
						CanDash = false;
						PS = PlayerStates::DASH;
						velocity = pbody->body->GetLinearVelocity();
					}
				}
				else if (Player_POV == POV::RIGHT)
				{
					velocity.x = 0.4 * 16;

					if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash == true) {
						// Apply an initial Right force
						pbody->body->SetLinearVelocity({ 0,0 });
						pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);
						CanDash = false;
						PS = PlayerStates::DASH;
						velocity = pbody->body->GetLinearVelocity();
					}
				}

				break;
			case PlayerStates::DASH:


				if (Player_POV == POV::RIGHT)
				{
					//The parameter that creates the slowing sensation of the dash
					DashSlower -= 0.01f;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					velocity.y = -1;
					//where we look if the dash has finished or not
					DashForce += DashSlower;
					if (DashForce <= 0)
					{
						PS = PlayerStates::IDLE;
						ResetDash();
					}
				}
				//Left Dash
				else if (Player_POV == POV::LEFT)
				{
					//Same as on top
					DashSlower += 0.01f;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					velocity.y = -1;
					DashForce -= DashSlower;
					if (DashForce <= 0)
					{
						PS = PlayerStates::IDLE;
						ResetDash();
					}
				}

				break;
			case PlayerStates::JUMPING:


				velocity = pbody->body->GetLinearVelocity();

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && pbody->body->GetLinearVelocity().x > -5 && isDashingR == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-0.05, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					Player_POV = POV::LEFT;

				}
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && pbody->body->GetLinearVelocity().x < 5 && isDashingL == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0.05, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					Player_POV = POV::RIGHT;

				}
				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_UP) && (JumpMinus > 0))
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, JumpMinus), true);
					velocity = pbody->body->GetLinearVelocity();
				}
				else
				{
					JumpMinus -= 0.05;

				}

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash==true)
				{
					pbody->body->SetLinearVelocity({ 0,0 });
					if (Player_POV == POV::LEFT)
					{
						pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);
					}
					else if (Player_POV == POV::RIGHT)
					{
						pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);
					}
					CanDash = false;
					PS = PlayerStates::DASH;
					velocity = pbody->body->GetLinearVelocity();
				}

				break;
			case PlayerStates::FALLING:

				if (pbody->body->GetLinearVelocity().y < 0.01)
				{
					PS = PlayerStates::IDLE;
				}

				break;
			default:
				break;
			}





			

		if (pbody->body->GetLinearVelocity().y > 10)
		{
			velocity.y = TerminalVelocity.y;
		}
		pbody->body->SetLinearVelocity(velocity);

		}
		else // GOD MODE 
		{
			b2Vec2 velocityGodMode = b2Vec2(0, 0);
			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocityGodMode.x = -0.5 * 16;
			}

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocityGodMode.x = 0.5 * 16;
			}
			//Fly
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {

				velocityGodMode.y = -0.5 * 16;
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

				velocityGodMode.y = 0.5 * 16;
			}

			pbody->body->SetLinearVelocity(velocityGodMode);
			
		}

	}


	//Reset
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		ResetPlayer();
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	
	if (pbody->body->GetLinearVelocity().y < 0 && Jumping==true&& PS == PlayerStates::JUMPING && Player_POV == POV::LEFT) {
		// Use jump animation
		currentAnimation = &jumpingleft;

	}
	else if (Jumping == true && PS == PlayerStates::JUMPING && Player_POV == POV::RIGHT) {

		currentAnimation = &jumpingright;
	}
	else if (Jumping == true) {

		currentAnimation = &jumpingright;
	}

	/*else if (pbody->body->GetLinearVelocity().y > 5) {

		currentAnimation = &jumpingright;
	}*/

	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT&& PS == PlayerStates::WALKING && Player_POV == POV::LEFT) {
		currentAnimation = &walkingleft;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT&& PS == PlayerStates::WALKING && Player_POV == POV::RIGHT) {
		currentAnimation = &walkingright;
	}
	else if (isDead == true) {


		currentAnimation = &dying;
	}
	else {
		currentAnimation = &idle;  // Only set to idle if no other state is active
	}

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

	//Menu Controll
	if (inMenu == true)
	{
			Engine::GetInstance().render.get()->DrawTexture(menu,(- Engine::GetInstance().render.get()->camera.x / 2)+20, (- Engine::GetInstance().render.get()->camera.y / 2)+20);
	}


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

			PS = PlayerStates::IDLE;
			JumpMinus = 1;
			CanDash = true;

			break;
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::WALL:
			ResetDash();
			LOG("Collision WALL");
			
			break;
		case ColliderType::DEATH:
			LOG("Collision DEATH");
			
			if (Godmode == false)
			{
				isDead = true;
			}

			break;
		}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

		switch (physB->ctype)
		{
		case ColliderType::PLATFORM:
			LOG("End Collision PLATFORM");
			CanDash = true;
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
		case ColliderType::DEATH:
			LOG("End Collision DEATH");
			break;
		default:
			break;
		}
}

void Player::ResetDash()
{
	DashForce = 3;
	DashSlower = 0;

	if (pbody->body->GetLinearVelocity().y > 0.01)
	{
		PS = PlayerStates::FALLING;
	}
	if (pbody->body->GetLinearVelocity().y < 0.01)
	{
		PS = PlayerStates::JUMPING;
	}

}

void Player::ResetPlayer()
{
	CleanUp();
	pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
	Awake();
	Start();
	isDead = false;
}