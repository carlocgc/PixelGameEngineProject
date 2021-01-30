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

	LOG(g_objects);

	return ptr;
}

void operator delete(void* p)
{
	free(p);

	--g_objects;

	LOG(g_objects);	
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

	virtual ~Game()
	{
		for (const auto& body : m_bodies)
		{
			delete body;
		}
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		
		CreateBodies();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		m_elapsedtime += fElapsedTime;

		//LOG(m_elapsedtime);

		if (m_elapsedtime >= m_duration)
		{
			DestroyBodies();

			CreateBodies();

			m_elapsedtime = 0;
		}
		
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

	const float m_duration = 5.f;
	
	float m_elapsedtime = 0.f;

	void CreateBodies()
	{
		// create some points
		VertletPoint* p0 = new VertletPoint(100, 100, 95, 95);
		VertletPoint* p1 = new VertletPoint(200, 100, 200, 100);
		std::vector<VertletPoint*> point_vec{ p0, p1 };

		// Create some sticks
		VertletStick* stick_a = new VertletStick(p0, p1, Distance(p0, p1));
		std::vector<VertletStick*> stick_vec{ stick_a };

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