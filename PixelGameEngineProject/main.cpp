#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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

		m_points.emplace_back(VertletPoint(100, 100, 95, 95));


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_BLUE);
		
		UpdatePoints(m_points, fElapsedTime);
		
		RenderPoints(m_points);
			
		return true;
	}


private:

	const float m_PointSize = 10.f;
	
	std::vector<VertletPoint> m_points;

	void UpdatePoints(std::vector<VertletPoint>& points, float fElapsedTime)
	{
		for (auto& p : points)
		{
			// calc velocity
			const float vx = p.x - p.oldx;
			const float vy = p.y - p.oldy;
			
			// apply velocity
			p.x += vx * fElapsedTime;
			p.y += vy * fElapsedTime;
			
			// confine x to screen bounds
			if (p.x >= ScreenWidth())
			{
				p.x = ScreenWidth();
				// invert x velocity
				p.oldx = p.x + vx;		
			}
			else if (p.x  < 0)
			{
				p.x = 0;
				p.oldx = p.x + vx;
			}

			// confine y to screen bounds
			if (p.y >= ScreenHeight())
			{
				p.y = ScreenHeight();
				// invert y velocity
				p.oldy = p.y + vy;
			}
			else if (p.y < 0)
			{
				p.y = 0;
				p.oldy = p.y + vx;
			}
		}
	}

	void RenderPoints(std::vector<VertletPoint>& points)
	{
		for (auto& p : points)
		{
			DrawRect(p.x, p.y, m_PointSize, m_PointSize);
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