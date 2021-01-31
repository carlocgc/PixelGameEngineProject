#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "VertletBody.h"

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


// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		// Name your application
		sAppName = "Example";
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here		

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Determine mouse move direction
		const olc::vi2d current_mouse_pos = GetWindowMouse();
		const olc::vf2d mouse_direction = last_mouse_pos - current_mouse_pos;		
		
		olc::vf2d mouse_direction_norm{ 0, 0 };
		
		if (mouse_direction.x != 0 || mouse_direction.y != 0)
		{
			mouse_direction_norm = mouse_direction.norm();
			LOG(mouse_direction_norm.str());
		}
		 
		last_mouse_pos = current_mouse_pos;

		// Create bodies
		
		if (GetMouse(0).bPressed)
		{
			const auto pos = GetWindowMouse();
			CreateChain(pos.x, pos.y);
		}

		// Render

		Clear(olc::DARK_GREY);

		for (auto& body : m_bodies)
		{
			body->Update(ScreenWidth(), ScreenHeight(), mouse_direction_norm, current_mouse_pos);

			RenderBody(body);
		}

		return true;
	}

private:

	std::vector<VertletBody*> m_bodies;

	olc::vi2d last_mouse_pos{ 0, 0 };

	void CreateChain(float _x, float _y)
	{
		// create box points
		VertletPoint* p0 = new VertletPoint(_x + 100, _y + 100, _x + 85, _y + 95);
		VertletPoint* p1 = new VertletPoint(_x + 200, _y + 200, _x + 200, _y + 200);
		VertletPoint* p2 = new VertletPoint(_x + 100, _y + 200, _x + 100, _y + 200);
		VertletPoint* p3 = new VertletPoint(_x + 200, _y + 100, _x + 200, _y + 100);
		std::vector<VertletPoint*> box_points{ p0, p1, p2, p3 };

		// Create box sticks
		VertletStick* s0 = new VertletStick(p0, p2, Distance(p0, p2));
		VertletStick* s1 = new VertletStick(p2, p1, Distance(p2, p1));
		VertletStick* s2 = new VertletStick(p1, p3, Distance(p1, p3));
		VertletStick* s3 = new VertletStick(p3, p0, Distance(p3, p0));
		VertletStick* s4 = new VertletStick(p3, p2, Distance(p3, p2), true); // support stick	
		std::vector<VertletStick*> box_sticks{ s0, s1, s2, s3, s4 };

		// Create box body
		VertletBody* box = new VertletBody(box_points, box_sticks);
		m_bodies.emplace_back(box);

		// Create chain points
		VertletPoint* p4 = new VertletPoint(_x + 200, _y, _x + 200, _y);
		VertletPoint* p5 = new VertletPoint(_x + 100, _y, _x + 100, _y);
		VertletPoint* p6 = new VertletPoint(_x, _y, _x, _y, false, true);
		std::vector<VertletPoint*> chain_points{ p4, p5, p6 };

		// Create box sticks
		VertletStick* s5 = new VertletStick(p3, p4, Distance(p3, p4));
		VertletStick* s6 = new VertletStick(p4, p5, Distance(p4, p5));
		VertletStick* s7 = new VertletStick(p5, p6, Distance(p5, p6));
		std::vector<VertletStick*> chain_sticks{ s5, s6, s7 };

		// Create chain
		VertletBody* chain = new VertletBody(chain_points, chain_sticks);
		m_bodies.emplace_back(chain);
	}

	void DestroyBodies()
	{
		for (auto&& body : m_bodies)
		{
			delete body;
		}

		m_bodies.clear();
	}

	void RenderBody(const VertletBody* body)
	{
		// render points
		for (const auto& p : body->m_points)
		{
			if (!p->m_hidden)
			{
				const auto colour = p->m_touched ? olc::RED : olc::WHITE;
				const auto radius = p->m_touched ? body->m_pointradius * 3 : body->m_pointradius;
				FillCircle(p->m_x, p->m_y, radius, colour);
			}
		}

		// render sticks
		for (const auto& s : body->m_sticks)
		{
			if (!s->m_hidden)
			{
				DrawLine(s->m_pa->m_x, s->m_pa->m_y, s->m_pb->m_x, s->m_pb->m_y);
			}
		}
	}

	float Distance(const VertletPoint* pa, const VertletPoint* pb)
	{
		const float dx = pb->m_x - pa->m_x;
		const float dy = pb->m_y - pa->m_y;

		const float distance = sqrt(dx * dx + dy * dy);

		return distance;
	}
};

int main()
{
	Game demo;
	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();
	return 0;
}