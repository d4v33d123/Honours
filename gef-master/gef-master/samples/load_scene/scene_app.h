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

	gef::InputManager* input_manager_;

	gef::SpriteRenderer* sprite_renderer_;
	class gef::Renderer3D* renderer_3d_;

	gef::Font* font_;

	float fps_;

	gef::Vector4 camera_eye;
	gef::Vector4 camera_lookat;
	gef::Vector4 camera_up;
	float camera_fov;
	float near_plane;
	float far_plane;

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



};



#endif // _SCENE_APP_H
