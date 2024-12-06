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
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	menu = Engine::GetInstance().textures.get()->Load("Assets/Textures/controllsotherversion.png");
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
	dash.LoadAnimations(parameters.child("animations").child("dash"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	damage.LoadAnimations(parameters.child("animations").child("damage"));

	currentAnimation = &idle;

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW / 1.5, texH / 1.5, bodyType::DYNAMIC);

	//Set the rotation of the player
	pbody->body->GetFixtureList()[0].SetFriction(0);
	pbody->body->SetFixedRotation(true);

	//Set a layer for the player so that when enemies die they can't push the player
	//IMPORTANT --> Adria helped me with this, i undesrtand how it works but i whanna give him  credit for it
	b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics->PLAYER_LAYER;
	pbody->body->GetFixtureList()[0].SetFilterData(filter);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	damageTimer.RsetTimer();
	damaged == false;

	walkingplayer = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/walking_sfx.ogg");
	dashPlayer = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/dash_sfx.ogg");
	damagePlayer = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/damage_sfx.ogg");
	jumpPlayer = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/jump_sfx.ogg");

	return true;
}

bool Player::Update(float dt)
{

	LOG("%d", counter);

	// L08 TODO 5: Add physics to the  player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);


	fxTimer += dt;
	if (walksoundTimer.ReadMSec() > 300) // El sonido durará 300 ms (ajusta este valor según tus necesidades)
	{
		Fxplayed = false; // Reseteamos el flag para poder reproducir el sonido la próxima vez que el jugador camine
		walksoundTimer.RsetTimer(); // Reiniciar el temporizador del sonido de caminar
	}

	//God Mode Controll
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode == false)
	{
		Godmode = true;
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN && Godmode == true)
	{
		Godmode = false;
	}

	//Debug Controll for the levels (only one level so its just a reset at this point)
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		ResetPlayer();
	}
	else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		ResetPlayer();
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

	//Check if in menu
	if (inMenu == false && isDead == false)
	{
		//Check godmode
		if (Godmode == false)
		{

			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = -0.8 * 4;
				if (!Fxplayed && !Jumping)
				{
					Engine::GetInstance().audio.get()->PlayFx(walkingplayer); // Suponiendo que tienes un sistema de audio para reproducir sonidos
					walksoundTimer.Start(); // Iniciar el temporizador para el sonido de caminar
					Fxplayed = true; // Marcar que el sonido de caminar ya fue reproducido
				}


				//Set the dash so the player can use it on the LEFT
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash == true) {
					// Apply an initial Left force

					Engine::GetInstance().audio.get()->PlayFx(dashPlayer, 0);


					pbody->body->SetLinearVelocity({ 0,0 });
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, 0), true);


					// booleans for dashing
					isDashingL = true;
					CanDash = false;
				}
				state = States::WALKING_L;

			}


			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = 0.8 * 8;
				if (!Fxplayed && !Jumping)
				{
					Engine::GetInstance().audio.get()->PlayFx(walkingplayer); // Suponiendo que tienes un sistema de audio para reproducir sonidos
					walksoundTimer.Start(); // Iniciar el temporizador para el sonido de caminar
					Fxplayed = true; // Marcar que el sonido de caminar ya fue reproducido
				}
				//Set the dash so the player can use it on the RIGHT
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_DOWN && CanDash == true) {
					// Apply an initial Right force

					Engine::GetInstance().audio.get()->PlayFx(dashPlayer, 0);

					pbody->body->SetLinearVelocity({ 0,0 });
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, 0), true);

					// booleans for dashing
					isDashingR = true;
					CanDash = false;
				}

				state = States::WALKING_R;

			}

			//Reset the dash and jump when killing a flying enemie --> its a mechanic i implemented to make the game more fun
			//and mavey at some point i will use it for a puzzle
			if (KillReset == true)
			{
				CanDash = true;

				//set a jump when it kills the flying enemye
				pbody->body->SetLinearVelocity({ 0,0 });
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
				Jumping = false;
				JumpingReset = true;
				falling = true;
				ResetDash();

				//Stop the reset
				KillReset = false;
			}

			//Jump
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Jumping == false) {
				// Apply an initial upward force

				Engine::GetInstance().audio.get()->PlayFx(jumpPlayer, 0);

				pbody->body->SetLinearVelocity({ 0,0 });
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
				Jumping = true;
			}

			//when the player bounces beacouse of damage
			if (damaged == true)
			{
				//basically the same as the enemy reset
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
				Jumping = true;
				damaged = false;
			}

			// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
			if (Jumping == true || JumpingReset == true)
			{
				falling = false;
				velocity = pbody->body->GetLinearVelocity();
				//We insert this here so the player camn move during the jump, so we dont limit the movement
				//Move Left
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && pbody->body->GetLinearVelocity().x > -5 && isDashingR == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-0.75, 0), true);
					velocity = pbody->body->GetLinearVelocity();
					state = States::JUMPING_L;
					JumpingLeft = true;

				}

				// Move right
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && pbody->body->GetLinearVelocity().x < 5 && isDashingL == false)
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0.75, 0), true);
					velocity = pbody->body->GetLinearVelocity();

					state = States::JUMPING_R;
					JumpingRight = true;

				}

				// a way to make the jump increment if the button is pressed more or less --> we use the jump minus variable. the bigger the variable the harder it is to jump
				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_UP) && (JumpMinus > 0))
				{
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, JumpMinus), true);
					velocity = pbody->body->GetLinearVelocity();
				}
				else
				{
					JumpMinus -= 0.1;

				}

			}

			//Set a terminal velocity --> so that the player doesnt fall at an infinite speed, beacouse the gravity on y is det to 20
			//Why?? --> its the gravity most used in games
			if (falling == true && Jumping == false)
			{
				//The terminal velocity is the variable we edit, not the max velocity of falling, that one is 7
				velocity.y = Engine::GetInstance().scene->Slower(TerminalVelocity, 7, 0.01);
				if (TerminalVelocity <= 7)
				{
					TerminalVelocity += velocity.y;
				}

			}

			//Dash --> its complicated, but it works, it applyes an impulse to the player, that is lowly being reversed by another variable
			//to the point that hits 0 and the dash stops

			if (isDashingR == true)
			{
				//The parameter that creates the slowing sensation of the dash
				DashSlower -= 0.01f;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
				velocity = pbody->body->GetLinearVelocity();
				velocity.y = -0.5;
				//where we look if the dash has finished or not
				DashForce += DashSlower;
				if (DashForce <= 0)
				{
					ResetDash();
				}

				state = States::DASH_L;
			}
			//Left Dash
			else if (isDashingL == true)
			{
				//Same as on top
				DashSlower += 0.01f;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashSlower, 0), true);
				velocity = pbody->body->GetLinearVelocity();
				velocity.y = -0.5;
				DashForce -= DashSlower;
				if (DashForce <= 0)
				{
					ResetDash();
				}
				state = States::DASH_R;
			}


			pbody->body->SetLinearVelocity(velocity);
		}
		else // GOD MODE 
		{
			b2Vec2 velocityGodMode = b2Vec2(0, 0);
			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocityGodMode.x = -0.5 * 16;
				state = States::WALKING_L;
			}

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocityGodMode.x = 0.5 * 16;
				state = States::WALKING_R;
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


		//if not doing all this actions, just idle
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) != KEY_REPEAT && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) != KEY_REPEAT && Jumping == false && state != States::DAMAGE) {

			state = States::IDLE_R;
		}

	}


	//Reset
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		ResetPlayer();
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	//A way to tell the sistem that it needs time to do the animation, when the time has passed the animation ends.
	//its also used for other things like damage taking
	if (damageTimer.active == true)
	{
		if (damageTimer.ReadMSec() < 200)
		{
			currentAnimation = &damage;
		}
		else
		{
			damageTimer.RsetTimer();
		}
	}

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	//Menu Controll
	if (inMenu == true)
	{
		Engine::GetInstance().render.get()->DrawTexture(menu, (-Engine::GetInstance().render.get()->camera.x / 2) + 20, (-Engine::GetInstance().render.get()->camera.y / 2) + 20);
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

		//reset the jump and dash when touching the ground
		Jumping = false;
		JumpingReset = false;
		JumpMinus = 1;
		CanDash = true;

		//also set that the player isnt falling enymore
		falling = false;
		TerminalVelocity = 0;

		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		Engine::GetInstance().audio.get()->PlayFx(damagePlayer, 0);

		// this is my way of the player to interact with the enemyes.
		for (int i = 0; i < Engine::GetInstance().scene->enemyList.size(); i++)
		{
			//get the enemy that the player is touching
			if (physB == Engine::GetInstance().scene->enemyList[i]->pbody)
			{
				//check if the player takes damage
				if ((Godmode == false) && (state != States::DYING) && (state != States::DASH_L) && (state != States::DASH_R) && (Engine::GetInstance().scene->enemyList[i]->isDead == false))
				{
					damageTimer.Start();
					state = States::DAMAGE;
					checkLife();
				}
				//check if the player kills the enemy
				else if ((Godmode == false) && (state != States::DYING) && ((state == States::DASH_L) || (state == States::DASH_R)) && (Engine::GetInstance().scene->enemyList[i]->isDead == false))
				{
					//Reset the dash and jump if it kills the flying enemy
					if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::FYING_ENEMY)
					{
						KillReset = true;
					}


					//Chek the way of the dash and apply the force to the enemy
					//the enemy kill it here
					if (state == States::DASH_L)
					{
						if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::FYING_ENEMY)
						{
							Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, -DashForce), true);
							counter++;
						}
						if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::WALKING_ENEMY)
						{
							counter++;
							Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(DashForce, -DashForce), true);
						}
						Engine::GetInstance().audio.get()->PlayFx(damagePlayer, 0);
					}
					else if (state == States::DASH_R)
					{
						if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::FYING_ENEMY)
						{
							Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, -DashForce), true);
							counter++;
						}
						if (Engine::GetInstance().scene->enemyList[i]->type == EntityType::WALKING_ENEMY)
						{
							Engine::GetInstance().scene->enemyList[i]->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-DashForce, -DashForce), true);
							counter++;

						}
						Engine::GetInstance().audio.get()->PlayFx(damagePlayer, 0);
					}
					ResetDash();
					
					Engine::GetInstance().scene->enemyList[i]->isDead = true;
				}
			}
		}

		break;

	case ColliderType::WALL:
		//we reset the dash if we hit a wall
		ResetDash();

		LOG("Collision WALL");

		break;
	case ColliderType::DEATH:
		LOG("Collision DEATH");

		//check if godmode is on
		if (Godmode == false)
		{
			//Apply damage and states

			Engine::GetInstance().audio.get()->PlayFx(damagePlayer, 0);

			//beacouse of the spikes the player bounces back so its not falling anymore
			falling = false;
			TerminalVelocity = 0;

			//We start the damage timer so the player can do the damage animation
			damageTimer.Start();
			state = States::DAMAGE;
			checkLife();
			damaged = true;
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

		//When we exit a collision we are falling (if we are not jumping) and we can dash
		CanDash = true;
		if (Jumping == false)
		{
			falling = true;
		}

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

//Reset the parameters of the dash and if its a kill reset we dont reset the velocity of the player
void Player::ResetDash()
{
	DashForce = 4;
	DashSlower = 0;
	isDashingL = false;
	isDashingR = false;
	if (KillReset == false)
	{
		pbody->body->SetLinearVelocity({ 0,0 });
	}

}

//Reset the player parameters
void Player::ResetPlayer()
{
	CleanUp();
	pbody->body->DestroyFixture(&pbody->body->GetFixtureList()[0]);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
	Awake();
	Start();
	state = States::IDLE_R;
	life = 3;
	damaged = false;
	isDead = false;
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

//Life manageing
void Player::checkLife()
{
	if (life <= 0)
	{
		isDead = true;
		state = States::DYING;
	}
	else
	{
		life--;
		state = States::DAMAGE;
	}
}
