// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods
/***************************************************************************

    ui/selmenu.h

    MAME system/software selection menu.

***************************************************************************/

#ifndef MAME_FRONTEND_UI_SELMENU_H
#define MAME_FRONTEND_UI_SELMENU_H

#pragma once

#include "ui/menu.h"

#include <map>
#include <memory>
#include <mutex>
#include <vector>


struct ui_software_info;

namespace ui {
class menu_select_launch : public menu
{
public:

	virtual ~menu_select_launch() override;

protected:

	// tab navigation
	enum class focused_menu
	{
		main,
		left,
		righttop,
		rightbottom
	};

	menu_select_launch(mame_ui_manager &mui, render_container &container, bool is_swlist);

	focused_menu get_focus() const { return m_focus; }
	void set_focus(focused_menu focus) { m_focus = focus; }

	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;

	// hanlders
	void inkey_navigation();

	// draw arrow
	void draw_common_arrow(float origx1, float origy1, float origx2, float origy2, int current, int dmin, int dmax, float title);
	void draw_info_arrow(int ub, float origx1, float origx2, float oy1, float line_height, float text_size, float ud_arrow_width);

	int     visible_items;
	void    *m_prev_selected;
	int     m_total_lines;
	int     m_topline_datsview;   // right box top line
	bool    m_ui_error;

private:
	using bitmap_ptr_vector = std::vector<bitmap_ptr>;
	using texture_ptr_vector = std::vector<texture_ptr>;

	class cache
	{
	public:
		cache(running_machine &machine);
		~cache();

		bitmap_argb32 &snapx_bitmap() { return *m_snapx_bitmap; }
		render_texture *snapx_texture() { return m_snapx_texture.get(); }
		bool snapx_driver_is(game_driver const *value) const { return m_snapx_driver == value; }
		bool snapx_software_is(ui_software_info const *software) const { return m_snapx_software == software; }
		void set_snapx_driver(game_driver const *value) { m_snapx_driver = value; }
		void set_snapx_software(ui_software_info const *software) { m_snapx_software = software; }

		bitmap_argb32 &no_avail_bitmap() { return *m_no_avail_bitmap; }
		render_texture *star_texture() { return m_star_texture.get(); }

		bitmap_ptr_vector const &toolbar_bitmap() { return m_toolbar_bitmap; }
		bitmap_ptr_vector const &sw_toolbar_bitmap() { return m_sw_toolbar_bitmap; }
		texture_ptr_vector const &toolbar_texture() { return m_toolbar_texture; }
		texture_ptr_vector const &sw_toolbar_texture() { return m_sw_toolbar_texture; }

	private:
		bitmap_ptr              m_snapx_bitmap;
		texture_ptr             m_snapx_texture;
		game_driver const       *m_snapx_driver;
		ui_software_info const  *m_snapx_software;

		bitmap_ptr              m_no_avail_bitmap;
		bitmap_ptr              m_star_bitmap;
		texture_ptr             m_star_texture;

		bitmap_ptr_vector       m_toolbar_bitmap;
		bitmap_ptr_vector       m_sw_toolbar_bitmap;
		texture_ptr_vector      m_toolbar_texture;
		texture_ptr_vector      m_sw_toolbar_texture;
	};
	using cache_ptr = std::shared_ptr<cache>;
	using cache_ptr_map = std::map<running_machine *, cache_ptr>;

	static constexpr std::size_t MAX_ICONS_RENDER = 40;

	void reset_pressed() { m_pressed = false; m_repeat = 0; }
	bool mouse_pressed() const { return (osd_ticks() >= m_repeat); }
	void set_pressed();

	bool snapx_valid() const { return m_cache->snapx_bitmap().valid(); }

	// draw left panel
	virtual float draw_left_panel(float x1, float y1, float x2, float y2) = 0;

	game_driver const       *m_info_driver;
	ui_software_info const  *m_info_software;
	int                      m_info_view;
	std::vector<std::string> m_items_list;
	std::string              m_info_buffer;

	// draw infos
	void infos_render(float x1, float y1, float x2, float y2);
	virtual void general_info(const game_driver *driver, std::string &buffer) = 0;

	// get selected software and/or driver
	virtual void get_selection(ui_software_info const *&software, game_driver const *&driver) const = 0;
	void select_prev()
	{
		if (!m_prev_selected)
		{
			selected = 0;
		}
		else
		{
			for (int x = 0; x < item.size(); ++x)
			{
				if (item[x].ref == m_prev_selected)
				{
					selected = x;
					break;
				}
			}
		}
	}

	void draw_toolbar(float x1, float y1, float x2, float y2);
	void draw_star(float x0, float y0);
	float draw_icon(int linenum, void *selectedref, float x1, float y1);

	void get_title_search(std::string &title, std::string &search);

	// handle keys
	virtual void handle_keys(uint32_t flags, int &iptkey) override;

	// handle mouse
	virtual void handle_events(uint32_t flags, event &ev) override;

	// draw game list
	virtual void draw(uint32_t flags) override;

	// draw right panel
	void draw_right_panel(float origx1, float origy1, float origx2, float origy2);
	float draw_right_box_title(float x1, float y1, float x2, float y2);

	// images render
	void arts_render(float origx1, float origy1, float origx2, float origy2);
	std::string arts_render_common(float origx1, float origy1, float origx2, float origy2);
	void arts_render_images(bitmap_argb32 *bitmap, float origx1, float origy1, float origx2, float origy2);
	void draw_snapx(float origx1, float origy1, float origx2, float origy2);

	// text for main top/bottom panels
	virtual void make_topbox_text(std::string &line0, std::string &line1, std::string &line2) const = 0;
	virtual std::string make_driver_description(game_driver const &driver) const = 0;
	virtual std::string make_software_description(ui_software_info const &software) const = 0;

	// cleanup function
	static void exit(running_machine &machine);

	cache_ptr               m_cache;
	bool                    m_is_swlist;
	focused_menu            m_focus;
	bool                    m_pressed;          // mouse button held down
	osd_ticks_t             m_repeat;

	render_texture          *m_icons_texture[MAX_ICONS_RENDER];
	bitmap_ptr              m_icons_bitmap[MAX_ICONS_RENDER];
	game_driver const       *m_old_icons[MAX_ICONS_RENDER];

	static std::mutex       s_cache_guard;
	static cache_ptr_map    s_caches;
};

} // namespace ui

#endif // MAME_FRONTEND_UI_SELMENU_H
