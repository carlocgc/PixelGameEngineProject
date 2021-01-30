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
		
		CreateBodies();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		Clear(olc::DARK_BLUE);

		for (auto& body : m_bodies)
		{
			body->Update(ScreenWidth(), ScreenHeight());

			RenderBody(body);
		}

		return true;
	}

private:

	std::vector<VertletBody*> m_bodies;

	void CreateBodies()
	{
		// create some points
		VertletPoint* p0 = new VertletPoint(100, 100, 85, 95);
		VertletPoint* p1 = new VertletPoint(200, 100, 200, 100);
		VertletPoint* p2 = new VertletPoint(100, 200, 100, 200);
		VertletPoint* p3 = new VertletPoint(200, 200, 200, 200);
		std::vector<VertletPoint*> point_vec{ p0, p1, p2, p3 };

		// Create some sticks
		VertletStick* s0 = new VertletStick(p0, p1, Distance(p0, p1));
		VertletStick* s1 = new VertletStick(p1, p3, Distance(p1, p3));
		VertletStick* s2 = new VertletStick(p3, p2, Distance(p3, p2));
		VertletStick* s3 = new VertletStick(p2, p0, Distance(p2, p0));
		VertletStick* s4 = new VertletStick(p1, p2, Distance(p1, p2)); // support stick
		
		std::vector<VertletStick*> stick_vec{ s0, s1, s2, s3, s4};

		VertletBody* v1 = new VertletBody(point_vec, stick_vec);
		m_bodies.emplace_back(v1);
	}

	void DestroyBodies()
	{
		for (auto && body : m_bodies)
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
			FillCircle(p->m_x, p->m_y, body->m_pointradius, olc::WHITE);
		}

		// render sticks
		for (const auto& s : body->m_sticks)
		{
			DrawLine(s->m_pa->m_x, s->m_pa->m_y, s->m_pb->m_x, s->m_pb->m_y);
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