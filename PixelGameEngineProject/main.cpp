#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//#include "Utility.h"
#include "VertletPhysics.h"

using namespace VertletPhysics;

// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		// Name your application
		sAppName = "Game";
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here		
			
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Determine mouse move direction	
		const olc::vf2d current_mouse_pos = GetWindowMouse();
		const olc::vf2d mouse_direction = last_mouse_pos - current_mouse_pos;
		olc::vf2d mouse_direction_norm{ 0, 0 };

		if (mouse_direction.x != 0 || mouse_direction.y != 0)
		{
			mouse_direction_norm = mouse_direction.norm();
		}

		// store last pos for next update
		last_mouse_pos = current_mouse_pos;

		// check should cut
		const bool should_cut = GetMouse(0).bHeld;

		if (GetKey(olc::Q).bPressed)
		{
			DestroyBodies();
		}
		else if (GetKey(olc::R).bPressed)
		{
			int x = rand() % 1000;
			
			CreateNet(m_bodies, x, 10, 80, 80, 4);
		}
		
		// Render
		Clear(olc::VERY_DARK_CYAN);

		for (auto& body : m_bodies)
		{
			body->Update(ScreenWidth(), ScreenHeight(), mouse_direction_norm, current_mouse_pos, should_cut);

			body->Render(this);
		}

		return true;
	}

private:

	std::vector<VertletBody*> m_bodies;

	olc::vi2d last_mouse_pos{ 0, 0 };

	void DestroyBodies()
	{
		for (auto&& body : m_bodies)
		{
			delete body;
		}

		m_bodies.clear();
	}
};

int main()
{
	Game demo;
	if (demo.Construct(1280, 720, 1, 1, false))
		demo.Start();
	return 0;
}