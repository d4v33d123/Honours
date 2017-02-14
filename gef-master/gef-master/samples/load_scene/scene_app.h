#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <Box2D/Box2D.h>
#include "Car.h"
#include "build/vs2015/AICar.h"
#include "build/vs2015/Track.h"


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Renderer3D;
	class Font;
	class Mesh;
	class InputManager;
	class Scene;
}

enum GState
{
	STARTUP,
	MENU,
	GAME,
	GAMEOVER,
	EXIT
};


class MyDestructionListener : public b2DestructionListener
{
	void SayGoodbye(b2Fixture* fixture)
	{
		if (FixtureUserData* fud = (FixtureUserData*)fixture->GetUserData())
			delete fud;
	}

	//(unused but must implement all pure virtual functions)
	void SayGoodbye(b2Joint* joint) {}
};


class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupCamera();
	void SetupLights();
	void HandleInput();
	void LoadMap();

	gef::InputManager* input_manager_;

	gef::SpriteRenderer* sprite_renderer_;
	class gef::Renderer3D* renderer_3d_;

	// Gamestate Related functions and variables
	void StartInit();
	void StartUpdate();
	void StartRender();
	void StartInput();
	bool startupinited;
	
	void MenuInit();
	void MenuUpdate();
	void MenuRender();
	void MenuInput();
	bool menuinited;

	void GameInit();
	void GameUpdate();
	void GameRender();
	void GameInput();
	bool gameinited;

	void GameOverUpdate();
	void GameOverRender();
	void GameOverInput();

	GState game_state;

	gef::Font* font_;

	float fps_;

	gef::Vector4 camera_eye;
	gef::Vector4 camera_lookat;
	gef::Vector4 camera_up;
	float camera_fov;
	float near_plane;
	float far_plane;

	// menu related variables
	Net net_type;


	// ----- Game Related Variables -----
	// Track
	Track* level_;
	int trackNum_;

	// box2d related functions

	void Keyboard(unsigned char key);
	void KeyboardUp(unsigned char key);

	void handleContact(b2Contact* contact, bool began);
	void tire_vs_groundArea(b2Fixture* tireFixture, b2Fixture* groundAreaFixture, bool began);

	void BeginContact(b2Contact* contact) { handleContact(contact, true); }
	void EndContact(b2Contact* contact) { handleContact(contact, false); }
	
	
	// box2d related vairables
	b2World* world;
	int controlState;
	MyDestructionListener destructionListener;
	b2Body* groundBody;
	Car* car;
	AICar* _aiCar;

	// time trial related variables
	double AiTime;
	double PlayerTime;
	int dataSize;


};



#endif // _SCENE_APP_H
