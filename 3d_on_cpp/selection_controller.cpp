#include <algorithm>
#include <ranges>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "basic_functions.h"
#include "selection_controller.h"
#include "logic_controller.h"

SelectionController* SelectionController::instance_ = nullptr;

SelectionController::SelectionController()
{
}

SelectionController::~SelectionController()
{
}

void SelectionController::update_transform_data()
{
}

void SelectionController::update_transform_points()
{
}

void SelectionController::remove_shape_from_selection(Shape* figure)
{
	if (!figure) return;
	selected_shapes_.remove(figure);
	is_selection_active_ = !(selected_shapes_.empty());
	if (is_selection_active_) {
		update_transform_data();
		update_transform_points();
	}
	else clear_selection();
}

void SelectionController::drag_rotation(sf::Vector2f mouse_position)
{
	//auto center = selection_transform_data.position;
	//auto vector_to_mouse = get_vector_from_points(center, mouse_position);
	//auto vector_to_rotation_point = get_vector_from_points(center, transform_points_[TransformMode::Angle]->getPosition());
	//float angle = angle_between_vectors_2d(vector_to_rotation_point, vector_to_mouse);
	//selection_transform_data.rotation += sf::radians(angle);
	//update_transform_to_selected_shapes();
}

void SelectionController::drag_movement(sf::Vector2f mouse_position)
{
	//auto delta = get_vector_from_points(selection_transform_data.position, mouse_position);
	//selection_transform_data.position += delta;
	update_transform_to_selected_shapes();
}

void SelectionController::update_transform_to_selected_shapes()
{
	//auto lastCenter = last_transform_data.position;
	//auto newCenter = selection_transform_data.position;
	//auto lastSize = last_transform_data.size;
	//auto newSize = selection_transform_data.size;

	//auto deltaRotation = selection_transform_data.rotation - last_transform_data.rotation;
	//if (selected_shapes_.size() == 1) {
	//	auto* shape = selected_shapes_.front();
	//	TransformData newTransform;
	//	newTransform.position = selection_transform_data.position;
	//	newTransform.size = selection_transform_data.size;
	//	newTransform.rotation = selection_transform_data.rotation;
	//	shape->set_transform(newTransform);
	//	update_transform_points();
	//	return;
	//}

	//float scaleX = lastSize.x != 0.f ? (newSize.x / lastSize.x) : 1.f;
	//float scaleY = lastSize.y != 0.f ? (newSize.y / lastSize.y) : 1.f;
	for (auto* shape : selected_shapes_)
	{
		//auto shape_transform = shape->get_transform();
		//sf::Vector2f offset = shape_transform.position - lastCenter;
		//sf::Vector2f scaledOffset{ offset.x * scaleX, offset.y * scaleY };
		//sf::Vector2f scaledSize{ shape_transform.size.x * scaleX, shape_transform.size.y * scaleY };

		//shape_transform.position = newCenter + scaledOffset;
		//shape_transform.size = scaledSize;
		//shape_transform.rotation += deltaRotation;

		//shape->set_transform(shape_transform);
	}
	update_transform_points();
}

AABB SelectionController::get_all_shapes_bounds_transform(std::list<Shape*> figures)
{
	if (figures.empty()) return AABB::Default;
	AABB bounding_box;
	bounding_box.min = figures.front()->get_box().min;
	bounding_box.max = figures.front()->get_box().max;
	for (const auto& f : figures)
	{
		bounding_box.min = glm::min(bounding_box.min, f->get_box().min);
		bounding_box.max = glm::max(bounding_box.max, f->get_box().max);
	}
}

SelectionController* SelectionController::get_instance()
{
	return (instance_ == nullptr) ? instance_ = new SelectionController() : instance_;
}

std::list<Shape*> SelectionController::get_selected_shapes() { return selected_shapes_; }

bool SelectionController::is_point_on_selection(sf::Vector2f point)
{
	if (!is_selection_active_) return false;
	if (selection_rect_.getGlobalBounds().contains(point)) return true;
	return false;
}

bool SelectionController::try_select_shape(Shape* shape, sf::Vector2f point, bool is_union)
{
	if (!shape->contains(point)) return false;
	try_add_shape_to_selection(shape, is_union);
	return true;
}

void SelectionController::try_add_shape_to_selection(Shape* shape, bool is_union)
{
	if (is_transforming_) return;

	if (!is_union) clear_selection();
	bool is_selected = std::find(selected_shapes_.begin(), selected_shapes_.end(), shape) != selected_shapes_.end();
	if (!is_selected) selected_shapes_.push_back(shape);
	else {
		selected_shapes_.remove(shape);
		shape->set_outline(false);
	}
	is_selection_active_ = !(selected_shapes_.size() == 0);
	if (is_selection_active_) {
		update_transform_data();
		update_transform_points();
	}
	for (auto& shape : selected_shapes_)
		shape->set_outline(is_selection_active_);
}

void SelectionController::delete_selected_shapes()
{
	std::vector<Shape*> toDelete(selected_shapes_.begin(), selected_shapes_.end());
	for (auto* shape : toDelete)
		if (shape) LogicController::get_instance()->delete_shape(shape);
	clear_selection();
}

void SelectionController::clear_selection()
{
	for (auto& shape : selected_shapes_)
		if (shape) shape->set_outline(false);
	selected_shapes_.clear();
	is_selection_active_ = false;
}

void SelectionController::try_begin_drag_transform_mode(sf::Vector2f mouse_position)
{
	if (!is_selection_active_) return;
	if (selection_rect_.getGlobalBounds().contains(mouse_position))
		is_transforming_ = true;
}

void SelectionController::end_drag_transform_mode() { is_transforming_ = false; }

void SelectionController::update_transform(sf::Vector2f mouse_position)
{
	if (!is_selection_active_ || !is_transforming_) return;
	last_transform_data = selection_transform_data;
	switch (transform_mode)
	{
	case TransformMode::Rotate: drag_rotation(mouse_position); break;
	case TransformMode::Move: drag_movement(mouse_position); break;
	}
}

void SelectionController::try_copy_shapes()
{
	if (!is_selection_active_) return;
	for (auto shape : copy_buffer) delete shape;
	copy_buffer.clear();

	for (auto shape : selected_shapes_)
		if (shape) copy_buffer.push_back(shape->clone());
}

void SelectionController::try_paste_shapes(sf::Vector2f mouse_position)
{
	if (copy_buffer.empty()) return;
	clear_selection();
	auto sel_box = get_all_shapes_bounds_transform(copy_buffer);
	auto copy_center = get_vec_mid(sel_box.min, sel_box.max);
	std::vector<Shape*> pasted;
	for (auto& proto : copy_buffer) {
		if (!proto) continue;
		auto new_shape = proto->clone();
		auto shape_transform = new_shape->get_transform();
		//auto offset = shape_transform.position - copy_center;
		//new_shape->set_transform(mouse_position + offset, shape_transform.size, shape_transform.rotation);
		//LogicController::get_instance()->add_shape(new_shape);
		//pasted.push_back(new_shape);
	}

	if (!pasted.empty()) {
		clear_selection();
		for (auto* s : pasted) try_add_shape_to_selection(s, true);
	}
}

void SelectionController::draw_selection(sf::RenderWindow& window)
{
	if (is_selection_active_) {
		window.draw(selection_rect_);
	}
}