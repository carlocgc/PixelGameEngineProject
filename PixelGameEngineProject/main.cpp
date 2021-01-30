#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

/**
 * \brief Print with cout
 * \param value Value to print
 */
#define LOG(value) std::cout << (value) << std::endl

struct VertletPoint
{
	float x;
	float y;
	float oldx;
	float oldy;

	VertletPoint(const float _x, const float _y, const float _oldx, const float _oldy) :
		x(_x),
		y(_y),
		oldx(_oldx),
		oldy(_oldy)
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

		m_Points.emplace_back(VertletPoint(100, 100, 95, 95));


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_BLUE);

		UpdatePoints(m_Points, fElapsedTime);

		RenderPoints(m_Points);

		return true;
	}


private:
	
	const float m_PointRadius = 5.f;
	
	const float m_Bounce = 0.9f;
	const float m_Gravity = 0.5f;
	const float m_Friction = 0.999f;

	std::vector<VertletPoint> m_Points;

	void UpdatePoints(std::vector<VertletPoint>& points, const float fElapsedTime) const
	{
		// TODO incorporate fElapsedTime

		for (auto& p : points)
		{
			// calc velocity
			float vx = (p.x - p.oldx) * m_Friction;
			float vy = (p.y - p.oldy) * m_Friction;

			// update old pos for next frame
			p.oldx = p.x;
			p.oldy = p.y;			
			
			// apply velocity
			p.x += vx;
			p.y += vy;
			// apply gravity
			p.y += m_Gravity;

			// confine x to screen bounds
			if (p.x >= ScreenWidth() - m_PointRadius)
			{
				p.x = ScreenWidth() - m_PointRadius;
				// invert x velocity, apply bounce speed reduction
				p.oldx = p.x + vx * m_Bounce;
			}
			else if (p.x < 0 + m_PointRadius)
			{
				p.x = 0 + m_PointRadius;
				p.oldx = p.x + vx * m_Bounce;
			}

			// confine y to screen bounds
			if (p.y >= ScreenHeight() - m_PointRadius)
			{
				p.y = ScreenHeight() - m_PointRadius;
				// invert y velocity, apply bounce speed reduction
				p.oldy = p.y + vy * m_Bounce;
			}
			else if (p.y < 0 + m_PointRadius)
			{
				p.y = 0 + m_PointRadius;
				p.oldy = p.y + vy * m_Bounce;
			}
		}
	}

	void RenderPoints(std::vector<VertletPoint>& points)
	{
		for (auto& p : points)
		{
			FillCircle(p.x, p.y, m_PointRadius, olc::WHITE);			
		}
	}
};

int main()
{
	Game demo;
	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();
	return 0;
}