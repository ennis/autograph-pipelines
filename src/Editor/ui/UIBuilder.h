#pragma once
#include "UICanvas.h"
#include "Layout.h"
#include <type_traits>
#include <autograph/Core/Support/Utils.h>

namespace ag {
	namespace ui {
		namespace builder
		{
			///////////////////////////////////////////////////////////////////////////////
			auto autoFit(bool enabled)
			{
				return [=](Text& t) { t.autoFit = true; };
			}

			auto textSize(float size)
			{
				return [=](Text& t) { t.textSize = size; };
			}

			auto horizontalSpacing(float size) {
				return [=](auto& layout) { layout.horizontalSpacing = size; };
			}
			
			auto verticalSpacing(float size) {
				return [=](auto& layout) { layout.verticalSpacing = size; };
			}

			auto spacing(float size) {
				return [=](auto& layout) { layout.spacing = size; };
			}

			template <typename F>
			auto onClick(Subscription& sub, F&& f)
			{
				return[&sub, f{ std::move(f) }](Button& b) { b.onClick.subscribe(sub, std::move(f)); };
			}

			///////////////////////////////////////////////////////////////////////////////
			template <typename WidgetType, typename Arg>
			auto parseBuilderArg(overload_choice<0>, WidgetType& w, std::unique_ptr<Arg>&& a) -> std::enable_if_t<std::is_base_of<Widget::Layout, std::decay_t<Arg>>::value>
			{
				w.layout = std::move(a);
			}
			
			template <typename WidgetType, typename Arg>
			auto parseBuilderArg(overload_choice<1>, WidgetType& w, Arg&& a) -> decltype(a(w), void())
			{
				a(w);
			}
			
			template <typename WidgetType, typename Arg>
			auto parseBuilderArg(overload_otherwise, WidgetType& w, Arg&& a) -> void
			{
				w.add(std::forward<Arg>(a));
			}

			template <typename WidgetType> void parseBuilderArgs(WidgetType& w) {
			}

			template <typename WidgetType, typename... Args> void parseBuilderArgs(WidgetType& w, Args &&... args) {
				auto dummy = { (parseBuilderArg(select_overload{}, w, std::forward<Args>(args)), 0)... };
			}

			///////////////////////////////////////////////////////////////////////////////
			template <typename LayoutType, typename Arg>
			auto parseBuilderLayoutArg(LayoutType& l, Arg&& a) -> decltype(a(l), void())
			{
				a(l);
			}

			template <typename LayoutType> void parseBuilderLayoutArgs(LayoutType& l) {
			}

			template <typename LayoutType, typename... Args> void parseBuilderLayoutArgs(LayoutType& l, Args &&... args) {
				auto dummy = { (parseBuilderLayoutArg(l, std::forward<Args>(args)), 0)... };
			}

			///////////////////////////////////////////////////////////////////////////////
			template <typename... Args>
			std::unique_ptr<Panel> verticalPanel(Args&&... args)
			{
				auto panel = std::make_unique<Panel>();
				panel->layout = std::make_unique<VerticalLayout>();
				parseBuilderArgs(*panel, std::forward<Args>(args)...);
				return panel;
			}
			
			template <typename... Args>
			std::unique_ptr<Panel> panel(Args&&... args)
			{
				auto panel = std::make_unique<Panel>();
				parseBuilderArgs(*panel, std::forward<Args>(args)...);
				return panel;
			}

			template <typename... Args>
			std::unique_ptr<GridLayout> gridLayout(unsigned rows, unsigned columns, Args&&... args)
			{
				auto layout = std::make_unique<GridLayout>(rows, columns);
				parseBuilderLayoutArgs(*layout, std::forward<Args>(args)...);
				return layout;
			}
			
			template <typename... Args>
			std::unique_ptr<VerticalLayout> verticalLayout(Args&&... args)
			{
				auto layout = std::make_unique<VerticalLayout>();
				parseBuilderLayoutArgs(*layout, std::forward<Args>(args)...);
				return layout;
			}

			template <typename... Args>
			std::unique_ptr<Panel> grid(unsigned rows, unsigned columns, Args&&... args)
			{
				auto panel = std::make_unique<Panel>();
				panel->layout = std::make_unique<GridLayout>(rows, columns);
				parseBuilderArgs(*panel, std::forward<Args>(args)...);
				return panel;
			}

			template <typename... Args>
			std::unique_ptr<Button> button(const char* label, Args&&... args)
			{
				auto button = std::make_unique<Button>();
				button->add(std::make_unique<Text>(label));
				parseBuilderArgs(*button, std::forward<Args>(args)...);
				return button;
			}

			template <typename... Args>
			std::unique_ptr<Text> text(const char* textString, Args&&... args) {
				auto text = std::make_unique<Text>(textString);
				parseBuilderArgs(*text, std::forward<Args>(args)...);
				return text;
			}

			template <typename... Args>
			std::unique_ptr<Slider> slider(Args&&... args) {
				auto slider = std::make_unique<Slider>();
				parseBuilderArgs(*slider, std::forward<Args>(args)...);
				return slider;
			}

			std::unique_ptr<Dummy> dummy() {
				return std::make_unique<Dummy>();
			}
		}
	}
}
