#include "VertletBody.h"

VertletBody::~VertletBody()
{
	for (auto& p : m_points)
	{
		delete p;
	}

	for (auto& s : m_sticks)
	{
		delete s;
	}
}

void VertletBody::Update(const int32_t screen_width, const int32_t screen_height, const olc::vf2d mouse_dir, const olc::vf2d mouse_pos)
{
	UpdatePoints(screen_width, screen_height, mouse_dir, mouse_pos);

	for (size_t i = 0; i <= m_constrain_loops; ++i)
	{
		UpdateSticks(screen_width, screen_height);
		ConstrainPoints(screen_width, screen_height);
	}
}

void VertletBody::UpdatePoints(const int32_t screen_width, const int32_t screen_height, const olc::vf2d mouse_dir, const olc::vf2d mouse_pos)
{
	for (auto& p : m_points)
	{	
		if (!p->m_pinned)
		{
			p->m_touched = false;
			
			// check if mouse is within the point
			float dx = mouse_pos.x - p->m_x;
			float dy = mouse_pos.y - p->m_y;

			bool within = abs(dx) <= m_pointradius * 2 && abs(dy) <= m_pointradius * 2; // TODO radius detect tolerance!

			float mouse_mod_x = 0;
			float mouse_mod_y = 0;

			if (within)
			{
				mouse_mod_x = mouse_dir.x * 5.f; // TODO mouse move amount!
				mouse_mod_y = mouse_dir.y * 5.f;

				p->m_touched = true;
			}
				
			// calc velocity
			const auto vx = (p->m_x - p->m_oldx - mouse_mod_x) * m_friction;
			const auto vy = (p->m_y - p->m_oldy - mouse_mod_y) * m_friction;

			// update old pos for next frame
			p->m_oldx = p->m_x;
			p->m_oldy = p->m_y;

			// apply velocity
			p->m_x += vx;
			p->m_y += vy;
			// apply gravity
			p->m_y += m_gravity;
		}
	}
}

void VertletBody::UpdateSticks(const int32_t screen_width, const int32_t screen_height)
{
	for (auto& s : m_sticks)
	{
		const auto dx = s->m_pb->m_x - s->m_pa->m_x; // x distance
		const auto dy = s->m_pb->m_y - s->m_pa->m_y; // y distance
		const auto distance = sqrt(dx * dx + dy * dy); // distance between points
		const auto difference = s->m_length - distance; // how displaced the points are from stick length
		const auto percent = difference / distance / 2; // percent each point must move to align with stick len
		const auto offset_x = dx * percent;
		const auto offset_y = dy * percent;

		// update points positions to be stick length apart
		if (!s->m_pa->m_pinned)
		{
			s->m_pa->m_x -= offset_x;
			s->m_pa->m_y -= offset_y;
		}

		if (!s->m_pb->m_pinned)
		{
			s->m_pb->m_x += offset_x;
			s->m_pb->m_y += offset_y;
		}
	}
}

void VertletBody::ConstrainPoints(const int32_t screen_width, const int32_t screen_height)
{
	for (auto& p : m_points)
	{
		if (!p->m_pinned)
		{
			const auto vx = (p->m_x - p->m_oldx) * m_friction;
			const auto vy = (p->m_y - p->m_oldy) * m_friction;

			// confine x to screen bounds
			if (p->m_x >= screen_width - m_pointradius)
			{
				p->m_x = screen_width - m_pointradius;
				// invert x velocity, apply bounce speed reduction
				p->m_oldx = p->m_x + vx * m_bounce;
			}
			else if (p->m_x < 0 + m_pointradius)
			{
				p->m_x = 0 + m_pointradius;
				p->m_oldx = p->m_x + vx * m_bounce;
			}

			// confine y to screen bounds
			if (p->m_y >= screen_height - m_pointradius)
			{
				p->m_y = screen_height - m_pointradius;
				// invert y velocity, apply bounce speed reduction
				p->m_oldy = p->m_y + vy * m_bounce;
			}
			else if (p->m_y < 0 + m_pointradius)
			{
				p->m_y = 0 + m_pointradius;
				p->m_oldy = p->m_y + vy * m_bounce;
			}
		}
	}
}
