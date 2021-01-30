#pragma once
#include <vector>

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

class VertletBody
{
public:
	VertletBody(std::vector<VertletPoint*>& points, std::vector<VertletStick*>& sticks) :
		m_points(points),
		m_sticks(sticks)
	{}

	virtual ~VertletBody();

	const float m_pointradius = 5.f;
	std::vector<VertletPoint*> m_points;
	std::vector<VertletStick*> m_sticks;
	
	void Update(const int32_t screen_width, const int32_t screen_height);	

private:
	/* velocity reduction on collision */
	const float m_bounce = 0.9f;
	/* downwards force added to velocity each update */
	const float m_gravity = 0.1f;
	/* amount to reduce velocity each update */
	const float m_friction = 0.999f;
	/* number of times to run the constrain logic each update, prevents wobbling of bodies */
	const int m_constrain_loops = 3; 

	void UpdatePoints(const int32_t screen_width, const int32_t screen_height);		
	void UpdateSticks(const int32_t screen_width, const int32_t screen_height);
	void ConstrainPoints(const int32_t screen_width, const int32_t screen_height);
};