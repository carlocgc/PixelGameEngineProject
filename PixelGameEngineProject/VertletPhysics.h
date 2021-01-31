#pragma once
#include <vector>

#include "olcPixelGameEngine.h"
namespace VertletPhysics
{
	/* Velocity reduction on collision */
	const float g_bounce = 0.9f;
	/* Downwards force added to velocity each update */
	const float g_gravity = 0.1f;
	/* Amount to reduce velocity each update */
	const float g_friction = 0.999f;
	/* number of times to run the constrain logic each update, prevents wobbling of bodies */
	const int g_constrain_loops = 3;

	/**
	 * \brief Point that has physics forces applied to it
	 */
	struct VertletPoint
	{
		float m_x;
		float m_y;
		float m_oldx;
		float m_oldy;
		float m_radius;

		bool m_should_draw;
		bool m_pinned;
		bool m_touched;

		VertletPoint(const float _x, const float _y, const float _oldx, const float _oldy, const bool pinned = false, const float radius = 5.f, const bool should_draw = false) :
			m_x(_x),
			m_y(_y),
			m_oldx(_oldx),
			m_oldy(_oldy),
			m_radius(radius),
			m_should_draw(should_draw),
			m_pinned(pinned),
			m_touched(false)
		{}
	};

	/**
	 * \brief Two points and a length, used to constrain two points
	 */
	struct VertletStick
	{
		VertletPoint* m_pa;
		VertletPoint* m_pb;
		float m_length;
		bool m_hidden;

		VertletStick(VertletPoint* pa, VertletPoint* pb, const float length, const bool hidden = false) :
			m_pa(pa),
			m_pb(pb),
			m_length(length),
			m_hidden(hidden)
		{}
	};

	/**
	 * \brief Structure comprised of some arrangement of VertletPoints & VertletSticks, update and render functions
	 */
	class VertletBody
	{
	public:
		VertletBody(std::vector<VertletPoint*>& points, std::vector<VertletStick*>& sticks, bool _draw_points = false) :
			draw_points(_draw_points),
			m_points(points),
			m_sticks(sticks)
		{}

		// TODO rule of three

		virtual ~VertletBody()
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

		const bool draw_points;

		std::vector<VertletPoint*> m_points;
		std::vector<VertletStick*> m_sticks;

		/**
		 * \brief Updates the points and sticks
		 * \param screen_width Width of the game screen
		 * \param screen_height Height of the game screen
		 * \param mouse_dir Direction the mouse is moving since last frame
		 * \param mouse_pos Current position of the mouse
		 */
		void Update(const int32_t screen_width, const int32_t screen_height, const olc::vf2d mouse_dir = {0, 0}, const olc::vf2d mouse_pos = {0, 0})
		{
			UpdatePoints(mouse_dir, mouse_pos);

			for (size_t i = 0; i <= g_constrain_loops; ++i)
			{
				UpdateSticks();
				ConstrainPoints(screen_width, screen_height);
			}
		}

		/**
		 * \brief Draws the physics bodies to the screen
		 * \param renderer PixelGameEngine game pointer
		 */
		void Render(olc::PixelGameEngine* renderer)
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

	private:

		/**
		 * \brief Update the points velocity
		 * \param mouse_dir Direction the mouse is moving since last frame
		 * \param mouse_pos Current position of the mouse
		 */
		void UpdatePoints(const olc::vf2d mouse_dir = { 0, 0 }, const olc::vf2d mouse_pos = {0, 0})
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

		/**
		 * \brief Adjusts the points to be stick length apart
		 */
		void UpdateSticks()
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

		/**
		 * \brief Handles point screen bounds check and applies bounce
		 * \param screen_width
		 * \param screen_height
		 */
		void ConstrainPoints(const int32_t screen_width, const int32_t screen_height)
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
	};

	/**
	 * \brief Get the distance between two points
	 * \param pa First point
	 * \param pb Second point
	 * \return Distance between
	 */
	static float Distance(const VertletPoint* pa, const VertletPoint* pb)
	{
		const float dx = pb->m_x - pa->m_x;
		const float dy = pb->m_y - pa->m_y;

		const float distance = sqrt(dx * dx + dy * dy);

		return distance;
	}

	/**
	 * \brief Creates a net of points and sticks
	 * \param out_bodies Vec of vertlet body pointers
	 * \param start_x Top left x position of the net
	 * \param start_y Top left y position of the net
	 * \param len_x Number of points in the x axis
	 * \param len_y Number of points in th y axis
	 * \param point_dist Distance between points
	 * \param draw_points Whether points are drawn, drawn if true
	 * \return True if successfully created
	 */
	static bool CreateNet(std::vector<VertletBody*>& out_bodies, const float start_x, const float start_y, const int32_t len_x, const int32_t len_y, const float point_dist, const bool draw_points = false)
	{
		std::vector<VertletPoint*> all_points;
		std::vector<VertletStick*> all_sticks;

		// create points
		for (auto y = 0; y < len_y; y++)
		{
			for (auto x = 0; x < len_x; x++)
			{
				const bool pinned = y == 0;
				const float pos_x = start_x + point_dist * x;
				const float pos_y = start_y + point_dist * y;

				auto* point = new VertletPoint(pos_x, pos_y, pos_x, pos_y, pinned, 5.f, draw_points);

				all_points.emplace_back(point);
			}
		}

		// create sticks	
		for (int32_t y = 0; y < len_y; y++)
		{
			for (int32_t x = 0; x < len_x; x++)
			{
				const bool right_needed = x != len_x - 1;
				const bool btm_needed = y != len_y - 1;

				if (right_needed)
				{
					// 1d - 2d index conversion
					const int32_t start_point = x + len_x * y;
					const int32_t end_point = (x + 1) + len_x * y;

					all_sticks.emplace_back(new VertletStick(all_points[start_point], all_points[end_point], point_dist));
				}

				if (btm_needed)
				{
					// 1d - 2d index conversion
					const int32_t start_point = x + len_x * y;
					const int32_t end_point = x + len_x * (y + 1);

					all_sticks.emplace_back(new VertletStick(all_points[start_point], all_points[end_point], point_dist));
				}
			}
		}

		// create body
		auto* body = new VertletBody(all_points, all_sticks, draw_points);
		out_bodies.emplace_back(body);

		return true;
	}

	/**
	 * \brief Create a box and chain physics body
	 * \param x Pin x position
	 * \param y Pin y position
	 * \param out_bodies Vec of physics body pointers
	 * \return True if successfully created
	 */
	static bool CreateChain(const float x, const float y, std::vector<VertletBody*>& out_bodies)
	{
		// create box points
		auto* const p0 = new VertletPoint(x + 100, y + 100, x + 85, y + 95);
		auto* const p1 = new VertletPoint(x + 200, y + 200, x + 200, y + 200);
		auto* const p2 = new VertletPoint(x + 100, y + 200, x + 100, y + 200);
		auto* const p3 = new VertletPoint(x + 200, y + 100, x + 200, y + 100);
		std::vector<VertletPoint*> box_points{ p0, p1, p2, p3 };

		// Create box sticks
		auto* const s0 = new VertletStick(p0, p2, Distance(p0, p2));
		auto* const s1 = new VertletStick(p2, p1, Distance(p2, p1));
		auto* const s2 = new VertletStick(p1, p3, Distance(p1, p3));
		auto* const s3 = new VertletStick(p3, p0, Distance(p3, p0));
		auto* const s4 = new VertletStick(p3, p2, Distance(p3, p2), true); // support stick	
		std::vector<VertletStick*> box_sticks{ s0, s1, s2, s3, s4 };

		// Create box body
		auto* box = new VertletBody(box_points, box_sticks);
		out_bodies.emplace_back(box);

		// Create chain points
		auto* const p4 = new VertletPoint(x + 200, y, x + 200, y);
		auto* const p5 = new VertletPoint(x + 100, y, x + 100, y);
		auto* const p6 = new VertletPoint(x, y, x, y, true);
		std::vector<VertletPoint*> chain_points{ p4, p5, p6 };

		// Create box sticks
		auto* const s5 = new VertletStick(p3, p4, Distance(p3, p4));
		auto* const s6 = new VertletStick(p4, p5, Distance(p4, p5));
		auto* const s7 = new VertletStick(p5, p6, Distance(p5, p6));
		std::vector<VertletStick*> chain_sticks{ s5, s6, s7 };

		// Create chain
		auto* const chain = new VertletBody(chain_points, chain_sticks);
		out_bodies.emplace_back(chain);

		return true;
	}
}