#pragma once

#ifndef LEMONADE_ENGINE_SRC_SNOWY_UI_H
#define LEMONADE_ENGINE_SRC_SNOWY_UI_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include "transforms.h"
#include "color.h"

namespace ui
{
	using UICoord = int32_t;
	using UIUnit = uint32_t;

	using UIPosition = Vec2D<UICoord>;
	using UISize = Vec2D<UIUnit>;

	using UINode_ID = const char*;

	class UIElement;

	class UINode : public std::enable_shared_from_this<UINode>
	{
	private:
		UINode_ID m_ID;

		UINode* m_parent{ nullptr };
		std::vector<std::unique_ptr<UINode>> m_children;

		bool m_queued_for_deletion = false;

	public:

		constexpr UINode(const UINode_ID& ID) :
			m_ID(ID)
		{
		}

		virtual ~UINode()
		{
			IF_DEBUG(std::cout << "UINode \"" << m_ID << "\" freed." << std::endl;)
		}

		// -- Getters -- //

		constexpr UINode_ID ID() const { return m_ID; }
		constexpr UINode* get_parent() const { return m_parent; }

		constexpr bool has_id(const UINode_ID& ID) const { return m_ID == ID; }
		constexpr bool is_queued_for_deletion() const { return m_queued_for_deletion; }

		virtual constexpr bool is_ui_element() const { return false; }
		virtual constexpr UIElement* as_ui_element() { return nullptr; }

		constexpr bool has_children() const
		{
			if (m_children.empty())
				return false;

			// Check if any aren't null
			for (const auto& child : m_children)
				if (child) return true;

			return false;
		}

		constexpr bool has_child(UINode* child) const
		{
			if (!child)
			{
				return false;
			}

			if (!has_children())
			{
				return false;
			}

			for (const auto& other_child : m_children)
				if (other_child.get() == child)
					return true;

			return false;
		}

		constexpr bool has_child(const UINode_ID& child_ID) const
		{
			if (!has_children())
			{
				return false;
			}

			for (const auto& other_child : m_children)
				if (other_child->ID() == child_ID)
				{
					return true;
				}

			return false;
		}

		bool has_parent() const
		{
			return m_parent != nullptr;
		}

		bool is_parent(const UINode_ID& parent_ID) const
		{
			if (!has_parent())
			{
				return false;
			}

			return m_parent->ID() == parent_ID;
		}
		constexpr bool is_parent(UINode* parent) const
		{
			if (!parent)
			{
				return false;
			}

			if (!has_parent())
			{
				return false;
			}

			return m_parent == parent;
		}

		// -- Indexing -- //

		constexpr size_t find_child(UINode* child)
		{
			for (size_t i = 0; i < m_children.size(); i++)
				if (m_children[i].get() == child) return i;

			return m_children.size();
		}

		constexpr size_t find_child(const UINode_ID& child_ID)
		{
			for (size_t i = 0; i < m_children.size(); i++)
			{
				if (m_children[i]->ID() == child_ID)
				{
					return i;
				}
			}

			return m_children.size();
		}

		UINode* get_child(const UINode_ID& child_ID)
		{
			for (size_t i = 0; i < m_children.size(); i++)
			{
				if (m_children[i]->ID() == child_ID)
				{
					return m_children[i].get();
				}
			}

			return nullptr;
		}

		// -- Setters -- //

		UINode* set_parent(UINode* parent)
		{
			m_parent = parent;
			return m_parent;
		}

		UINode* add_child(UINode* child)
		{
			if (!child)
				return nullptr;

			if (has_child(child))
				return child;

			child->set_parent(this);
			m_children.push_back(std::unique_ptr<UINode>(child));

			return m_children.back().get();
		}

		constexpr void remove_child(UINode* child)
		{
			if (!child || !has_child(child))
			{
				return;
			}

			if (!child->is_queued_for_deletion())
			{
				child->queue_free();
				return;
			}

			if (child->is_parent(this))
			{
				child->set_parent(nullptr);
			}

			m_children.erase(m_children.begin() + find_child(child));
		}

		void remove_child(const UINode_ID& child_ID)
		{
			if (!has_child(child_ID))
			{
				return;
			}

			auto child = get_child(child_ID);

			if (!child) return;

			if (!child->is_queued_for_deletion())
			{
				child->queue_free();
				return;
			}

			if (child->is_parent(this))
				child->set_parent(nullptr);

			m_children.erase(m_children.begin() + find_child(child_ID));
		}

		// -- Misc -- //

		void queue_free_child(UINode* child) const
		{
			if (!child || !has_child(child)) return;
			child->queue_free();
		}

		void queue_free_child(const UINode_ID& child_ID)
		{
			if (!has_child(child_ID)) return;
			auto child = get_child(child_ID);
			if (child) child->queue_free();
		}

		void queue_free()
		{
			if (m_queued_for_deletion) return;
			m_queued_for_deletion = true;

			if (has_children())
			{
				for (const auto& child : m_children)
					if (child) child->queue_free();
			}

			if (!has_parent()) return;

			if (auto parent = m_parent)
			{
				parent->remove_child(this);
			}
		}

		void print(size_t depth = 0)
		{
			for (size_t i = 0; i < depth; i++)
				std::cout << "    ";

			std::cout << m_ID << std::endl;
			if (has_children())
			{
				for (const auto& child : m_children)
					if (child) child->print(depth + 1);
			}
		}

	public:
		// -- Loop -- //

		// Update

		void update_children()
		{
			if (!has_children()) return;

			for (const auto& child : m_children)
				if (child) child->update();
		}

		virtual void update() { update_children(); }

		// Render

		void render_children()
		{
			if (!has_children()) return;

			for (const auto& child : m_children)
				if (child) child->render();
		}

		virtual void render()
		{
			render_children();
		}
	};

	class UIElement : public UINode
	{
	protected:
		UIPosition m_rel_position{ 0, 0 };
		UISize m_rel_size{ 1, 1 };

		color_t m_fg{ DEFAULT_FG_COLOR };
		color_t m_bg{ DEFAULT_BG_COLOR };
	public:
		UIElement() = default;
		UIElement(const UINode_ID& ID, const UIPosition& position, const UISize& size, const color_t& fg, const color_t& bg) :
			UINode(ID), m_rel_position(position), m_rel_size(size), m_fg(fg), m_bg(bg)
		{
			update();
		}

		// -- Getters -- //

		virtual constexpr bool is_ui_element() const override { return true; }
		virtual constexpr UIElement* as_ui_element() override { return this; }

		virtual UIElement* get_parent_ui_element() const
		{
			if (auto parent = get_parent())
			{
				if (parent->is_ui_element())
					return parent->as_ui_element();
			}

			return nullptr;
		}

		virtual UIPosition parent_offset() const
		{
			if (auto parent = get_parent_ui_element())
				return parent->pos();
			return { 0, 0 };
		}

		virtual UISize parent_bounding_box() const
		{
			if (auto parent = get_parent_ui_element())
				return parent->size();
			return m_rel_size;
		}

		UICoord parent_offset_x() const { return parent_offset().x; }
		UICoord parent_offset_y() const { return parent_offset().y; }

		UIUnit parent_bounding_width() const { return parent_bounding_box().x; }
		UIUnit parent_bounding_height() const { return parent_bounding_box().y; }

		virtual UIPosition offset() const
		{
			return pos();
		}

		virtual UISize bounding_box() const
		{
			return size();
		}

		UICoord offset_x() const { return offset().x; }
		UICoord offset_y() const { return offset().y; }

		UIUnit bounding_width() const { return bounding_box().x; }
		UIUnit bounding_height() const { return bounding_box().y; }

		UIPosition pos() const { return parent_offset() + m_rel_position; }
		UICoord posx() const { return pos().x; }
		UICoord posy() const { return pos().y; }

		UISize size() const { return { std::min(m_rel_size.x, parent_bounding_width()), std::min(m_rel_size.y, parent_bounding_height()) }; }
		UIUnit width() const { return size().x; }
		UIUnit height() const { return size().y; }

		UICoord pos_x1() const { return pos().x; }
		UICoord pos_y1() const { return pos().y; }
		UICoord pos_x2() const { return pos().x + size().x; }
		UICoord pos_y2() const { return pos().y + size().y; }

		UIPosition top_left() const { return { pos_x1(), pos_y1() }; }
		UIPosition top_right() const { return { pos_x2(), pos_y1() }; }
		UIPosition bottom_left() const { return { pos_x1(), pos_y2() }; }
		UIPosition bottom_right() const { return { pos_x2(), pos_y2() }; }

		virtual void update_transform() const
		{
		}

		virtual void update() override
		{
			update_transform();
			update_children();
		}
	};

	struct UILabel : public UIElement
	{
		struct initializer
		{
			UINode_ID ID;

			UIPosition position;
			UISize size;

			std::string text;

			color_t fg{ color::get("white") };
			color_t bg{ color::get("black") };
			TCOD_alignment_t alignment{ TCOD_alignment_t::TCOD_LEFT };
		};

		std::string text;
		TCOD_alignment_t alignment{ TCOD_alignment_t::TCOD_LEFT };

		UILabel() = default;

		UILabel(initializer init) :
			UIElement(init.ID, init.position, init.size, init.fg, init.bg),
			text(init.text), alignment(init.alignment)
		{
		}

		virtual void render() override
		{
			render_children();

			output::console_printf_rgb_rect_ex(
				posx(), posy(), width(), height(),
				m_fg, m_bg, DEFAULT_BKGND_FLAG, alignment,
				text
			);
		}
	};

	struct UIFrame : public UIElement
	{
		struct initializer
		{
			UINode_ID ID;

			UIPosition position;
			UISize size;

			std::string title = "";

			color_t fg{ color::get("white") };
			color_t bg{ color::get("black") };

			bool clear = true;

			std::array<int, 9> decoration
			{
				218, 196, 191,
				179, 0, 179,
				192, 196, 217
			};

			bool draw_frame = true;
		};

		std::string title;
		bool clear;
		std::array<int, 9> decoration;
		bool draw_frame;

		UIFrame(const initializer& init) :
			UIElement(init.ID, init.position, init.size, init.fg, init.bg), clear(init.clear),
			title(init.title), draw_frame(init.draw_frame), decoration(init.decoration)
		{
		}

		virtual UIPosition offset() const override
		{
			return pos() + UIPosition(1, 1);
		}

		virtual UISize bounding_box() const override
		{
			return size() - UISize(2, 2);
		}

		virtual void render() override
		{
			render_children();

			output::draw_frame_rgb(
				posx(), posy(), width(), height(), m_fg, m_bg, decoration, clear, DEFAULT_BKGND_FLAG
			);
		}
	};
}

#endif // !LEMONADE_ENGINE_SRC_SNOWY_UI_H