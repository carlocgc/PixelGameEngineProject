#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "VertletPhysics.h"

/**
 * \brief Print with cout
 * \param value Value to print
 */
#define LOG(VALUE) std::cout << (VALUE) << std::endl

 /* alive objects */
int g_objects = 0;

void* operator new(size_t size)
{
	void* ptr = malloc(size);

	++g_objects;

	//LOG(g_objects);

	return ptr;
}

void operator delete(void* p)
{
	free(p);

	--g_objects;

	//LOG(g_objects);	
}

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

		CreateNet(m_bodies, 75, 10, 10, 25, 25, true);
		CreateNet(m_bodies, 1620, 10, 10, 25, 25);
		
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

		last_mouse_pos = current_mouse_pos;

		// Create bodies

		if (GetMouse(0).bPressed)
		{
			const auto pos = GetMousePos();
			CreateChain(pos.x, pos.y, m_bodies);
		}

		// Render

		Clear(olc::VERY_DARK_CYAN);

		for (auto& body : m_bodies)
		{
			body->Update(ScreenWidth(), ScreenHeight(), mouse_direction_norm, current_mouse_pos);

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
	if (demo.Construct(1920, 1080, 1, 1, false))
		demo.Start();
	return 0;
}