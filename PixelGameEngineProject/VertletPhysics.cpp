#include "VertletPhysics.h"

namespace VertletPhysics
{
	VertletPoint::VertletPoint(const float _x, const float _y, const float _oldx, const float _oldy, const bool pinned, const float radius, const bool should_draw) :
		m_x(_x),
		m_y(_y),
		m_oldx(_oldx),
		m_oldy(_oldy),
		m_radius(radius),
		m_should_draw(should_draw),
		m_pinned(pinned),
		m_touched(false),
		m_detached(false)
	{}
	
	VertletStick::VertletStick(VertletPoint* pa, VertletPoint* pb, const float length, const bool hidden) :
		m_pa(pa),
		m_pb(pb),
		m_length(length),
		m_hidden(hidden),
		m_owning_body(nullptr)
	{
		m_pa->m_attached_sticks.emplace_back(this);
		m_pb->m_attached_sticks.emplace_back(this);
	}

	bool VertletStick::ReplacePoint(const VertletPoint* old_point)
	{
		// Create new point
		auto* const new_point = new VertletPoint(old_point->m_x, old_point->m_y, old_point->m_oldx, old_point->m_y, old_point->m_pinned, old_point->m_radius, old_point->m_should_draw);

		// Set attached stick to this
		new_point->m_attached_sticks.emplace_back(this);

		VertletPoint* retired_point = nullptr;

		// set replaced point to new point
		if (m_pa == old_point)
		{
			retired_point = m_pa;
			m_pa = new_point;
		}
		else if (m_pb == old_point)
		{
			retired_point = m_pa;
			m_pa = new_point;
		}
		else
		{
			return false;
		}

		//m_owning_body->ReplacePoint();

		// TODO Finish

		return true;

	}

	VertletBody::VertletBody(std::vector<VertletPoint*>& points, std::vector<VertletStick*>& sticks, bool _draw_points) :
		draw_points(_draw_points),
		m_points(points),
		m_sticks(sticks)
	{
		for (auto& stick : sticks)
		{
			stick->m_owning_body = this;
		}
	}

	void VertletBody::Update(const int32_t screen_width, const int32_t screen_height, const olc::vf2d mouse_dir, const olc::vf2d mouse_pos)
	{
		UpdatePoints(mouse_dir, mouse_pos);

		for (size_t i = 0; i <= g_constrain_loops; ++i)
		{
			UpdateSticks();
			ConstrainPoints(screen_width, screen_height);
		}
	}

	void VertletBody::Render(olc::PixelGameEngine* renderer)
	{
		// render points
		for (const auto& p : m_points)
		{
			if (p->m_should_draw && draw_points)
			{
				const auto colour = p->m_touched ? olc::RED : olc::WHITE;
				const auto radius = p->m_touched ? p->m_radius * 3 : p->m_radius;
				renderer->FillCircle(p->m_x, p->m_y, radius, colour);
			}
		}

		// render sticks
		for (const auto& s : m_sticks)
		{
			if (!s->m_hidden)
			{
				renderer->DrawLine(s->m_pa->m_x, s->m_pa->m_y, s->m_pb->m_x, s->m_pb->m_y);
			}
		}
	}

	bool VertletBody::ReplacePoint(VertletPoint* old_point, VertletPoint* new_point)
	{
		auto i = std::find(m_points.begin(), m_points.end(), old_point);

		if (i != m_points.end())
		{
			// delete and remove old point

			m_points.erase(i);

			delete old_point;

			// add new point

			m_points.emplace_back(new_point);

			return true;
		}

		return false;
	}

	void VertletBody::UpdatePoints(const olc::vf2d mouse_dir, const olc::vf2d mouse_pos)
	{
		for (auto& p : m_points)
		{
			if (!p->m_pinned)
			{
				// reset mouse touched flag
				p->m_touched = false;

				// check if mouse is within the point
				const float dx = mouse_pos.x - p->m_x;
				const float dy = mouse_pos.y - p->m_y;

				const bool within = abs(dx) <= p->m_radius * 2 && abs(dy) <= p->m_radius * 2; // TODO radius detect tolerance!

				float mouse_mod_x = 0;
				float mouse_mod_y = 0;

				// calculate mouse effect to apply to the point vel
				if (within)
				{
					mouse_mod_x = mouse_dir.x * 5.f; // TODO mouse move amount! maybe have point just follow mouse while inside its radius?
					mouse_mod_y = mouse_dir.y * 5.f;

					p->m_touched = true;

					// TODO REMOVE DEBUG CODE
					p->m_detached = true;
				}

				// calc velocity, apply mouse effect
				const auto vx = (p->m_x - p->m_oldx - mouse_mod_x) * g_friction;
				const auto vy = (p->m_y - p->m_oldy - mouse_mod_y) * g_friction;

				// update old pos for next frame
				p->m_oldx = p->m_x;
				p->m_oldy = p->m_y;

				// apply velocity
				p->m_x += vx;
				p->m_y += vy;
				// apply gravity
				p->m_y += g_gravity;
			}
		}
	}

	void VertletBody::UpdateSticks()
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
			if (!s->m_pa->m_pinned && !s->m_pa->m_detached)
			{
				s->m_pa->m_x -= offset_x;
				s->m_pa->m_y -= offset_y;
			}

			if (!s->m_pb->m_pinned && !s->m_pb->m_detached)
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
				const auto vx = (p->m_x - p->m_oldx) * g_friction;
				const auto vy = (p->m_y - p->m_oldy) * g_friction;

				// confine x to screen bounds
				if (p->m_x >= screen_width - p->m_radius)
				{
					p->m_x = screen_width - p->m_radius;
					// invert x velocity, apply bounce speed reduction
					p->m_oldx = p->m_x + vx * g_bounce;
				}
				else if (p->m_x < 0 + p->m_radius)
				{
					p->m_x = 0 + p->m_radius;
					p->m_oldx = p->m_x + vx * g_bounce;
				}

				// confine y to screen bounds
				if (p->m_y >= screen_height - p->m_radius)
				{
					p->m_y = screen_height - p->m_radius;
					// invert y velocity, apply bounce speed reduction
					p->m_oldy = p->m_y + vy * g_bounce;
				}
				else if (p->m_y < 0 + p->m_radius)
				{
					p->m_y = 0 + p->m_radius;
					p->m_oldy = p->m_y + vy * g_bounce;
				}
			}
		}
	}
}