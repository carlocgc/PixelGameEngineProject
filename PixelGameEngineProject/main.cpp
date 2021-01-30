#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

/**
 * \brief Print with cout
 * \param value Value to print
 */
#define LOG(value) std::cout << (value) << std::endl

struct VertletPoint
{
	float m_x;
	float m_y;
	float m_oldx;
	float m_oldy;

	VertletPoint(const float _x, const float _y, const float _oldx, const float _oldy) :
		m_x(_x),
		m_y(_y),
		m_oldx(_oldx),
		m_oldy(_oldy)
	{}
};

struct VertletStick
{
	VertletPoint* m_pa;
	VertletPoint* m_pb;
	float m_length;

	VertletStick(VertletPoint* pa, VertletPoint* pb, const float length) :
		m_pa(pa),
		m_pb(pb),
		m_length(length)
	{}
};

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

		// Create some points
		m_points.emplace_back(VertletPoint(100, 100, 95, 95));
		m_points.emplace_back(VertletPoint(200, 100, 200, 100));

		// Create some sticks
		m_sticks.emplace_back(&m_points[0], &m_points[1], Distance(&m_points[0], &m_points[1]));

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_BLUE);

		UpdatePoints(m_points, fElapsedTime);

		UpdateSticks(m_sticks, fElapsedTime);

		RenderPoints(m_points);

		RenderSticks(m_sticks);

		return true;
	}

private:

	const float m_pointradius = 5.f;

	const float m_bounce = 0.9f;
	const float m_gravity = 0.5f;
	const float m_friction = 0.999f;

	std::vector<VertletPoint> m_points;
	std::vector<VertletStick> m_sticks;

	void UpdatePoints(std::vector<VertletPoint>& points, const float fElapsedTime) const
	{
		// TODO incorporate fElapsedTime

		for (auto& p : points)
		{
			// calc velocity
			float vx = (p.m_x - p.m_oldx) * m_friction;
			float vy = (p.m_y - p.m_oldy) * m_friction;

			// update old pos for next frame
			p.m_oldx = p.m_x;
			p.m_oldy = p.m_y;

			// apply velocity
			p.m_x += vx;
			p.m_y += vy;
			// apply gravity
			p.m_y += m_gravity;

			// confine x to screen bounds
			if (p.m_x >= ScreenWidth() - m_pointradius)
			{
				p.m_x = ScreenWidth() - m_pointradius;
				// invert x velocity, apply bounce speed reduction
				p.m_oldx = p.m_x + vx * m_bounce;
			}
			else if (p.m_x < 0 + m_pointradius)
			{
				p.m_x = 0 + m_pointradius;
				p.m_oldx = p.m_x + vx * m_bounce;
			}

			// confine y to screen bounds
			if (p.m_y >= ScreenHeight() - m_pointradius)
			{
				p.m_y = ScreenHeight() - m_pointradius;
				// invert y velocity, apply bounce speed reduction
				p.m_oldy = p.m_y + vy * m_bounce;
			}
			else if (p.m_y < 0 + m_pointradius)
			{
				p.m_y = 0 + m_pointradius;
				p.m_oldy = p.m_y + vy * m_bounce;
			}
		}
	}

	void UpdateSticks(std::vector<VertletStick>& sticks, const float fElapsedTime)
	{
		for (auto& s : sticks)
		{
			const float dx = s.m_pb->m_x - s.m_pa->m_x; // x distance
			const float dy = s.m_pb->m_y - s.m_pa->m_y; // y distance
			const float distance = sqrt(dx * dx + dy * dy); // distance between points
			const float difference = s.m_length - distance; // how displaced the points are from stick length
			const float percent = difference / distance / 2; // percent each point must move to align with stick len
			float offset_x = dx * percent;
			float offset_y = dy * percent;

			// update points positions to be stick length apart
			s.m_pa->m_x -= offset_x;
			s.m_pa->m_y -= offset_y;

			s.m_pb->m_x += offset_x;
			s.m_pb->m_y += offset_y;
		}
	}
	
	void RenderPoints(std::vector<VertletPoint>& points)
	{
		for (auto& p : points)
		{
			FillCircle(p.m_x, p.m_y, m_pointradius, olc::WHITE);
		}
	}


	void RenderSticks(std::vector<VertletStick>& sticks)
	{
		for (auto& s : sticks)
		{
			DrawLine(s.m_pa->m_x, s.m_pa->m_y, s.m_pb->m_x, s.m_pb->m_y);
		}
	}
	
	float Distance(const VertletPoint* pa, const VertletPoint* pb)
	{
		const float dx = pb->m_x - pa->m_x;
		const float dy = pb->m_y - pa->m_y;

		const float distance = sqrt(dx * dx + dy * dy);

		//float d1 = dx * dx + dy * dy;
		
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