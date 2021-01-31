#pragma once
#include <vector>

#include "olcPixelGameEngine.h"
namespace VertletPhysics
{
	struct VertletStick;
	class VertletBody;
	
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
		bool m_detached;
		bool m_touched;

		std::vector<VertletStick*> m_attached_sticks;

		VertletPoint(const float _x, const float _y, const float _oldx, const float _oldy, const bool pinned = false, const float radius = 5.f, const bool should_draw = false);
	};

	/**
	 * \brief Two points and a length, used to constrain two points
	 */
	struct VertletStick
	{
		VertletPoint* m_pa;
		VertletPoint* m_pb;
		VertletBody* m_owning_body;
		float m_length;
		bool m_hidden;

		VertletStick(VertletPoint* pa, VertletPoint* pb, const float length, const bool hidden = false);

		bool ReplacePoint(const VertletPoint* old_point);
	};

	/**
	 * \brief Structure comprised of some arrangement of VertletPoints & VertletSticks, update and render functions
	 */
	class VertletBody
	{
	public:
		VertletBody(std::vector<VertletPoint*>& points, std::vector<VertletStick*>& sticks, bool _draw_points = false);

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
		void Update(const int32_t screen_width, const int32_t screen_height, const olc::vf2d mouse_dir = { 0, 0 }, const olc::vf2d mouse_pos = { 0, 0 });

		/**
		 * \brief Draws the physics bodies to the screen
		 * \param renderer PixelGameEngine game pointer
		 */
		void Render(olc::PixelGameEngine* renderer);

		bool ReplacePoint(VertletPoint* old_point, VertletPoint* new_point);

	private:

		/**
		 * \brief Update the points velocity
		 * \param mouse_dir Direction the mouse is moving since last frame
		 * \param mouse_pos Current position of the mouse
		 */
		void UpdatePoints(const olc::vf2d mouse_dir = { 0, 0 }, const olc::vf2d mouse_pos = { 0, 0 });

		/**
		 * \brief Adjusts the points to be stick length apart
		 */
		void UpdateSticks();

		/**
		 * \brief Handles point screen bounds check and applies bounce
		 * \param screen_width
		 * \param screen_height
		 */
		void ConstrainPoints(const int32_t screen_width, const int32_t screen_height);
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