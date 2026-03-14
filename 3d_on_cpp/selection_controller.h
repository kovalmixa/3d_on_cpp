#pragma once
#include <list>

#include "complex_shape.h"
#include "transform_data.h"

enum class TransformMode {
    Move,
    Rotate
};

class SelectionController {
private:
    static SelectionController* instance_;
    std::list<ComplexShape*> selected_shapes_;
    std::list<ComplexShape*> copy_buffer;
    sf::RectangleShape selection_rect_;
    TransformData selection_transform_data;
    TransformData last_transform_data;

    const float TRANSFORM_POINTS_SIZE = 5.f;
    const float TRANSFORM_POINTS_DRAG_RADIUS_OFFSET = 10.f;
    bool is_transforming_ = false;
    bool is_selection_active_ = false;

    SelectionController();
    ~SelectionController();

    void update_transform_data();
    void update_transform_points();

    bool is_point_near_transform_point(sf::Vector2f point, sf::Shape* transform_point);
    void drag_rotation(sf::Vector2f mouse_position);
    void drag_movement(sf::Vector2f mouse_position);

    void update_transform_to_selected_shapes();
    TransformData get_all_shapes_bounds_transform(std::list<ComplexShape*> shapes);
public:
    TransformMode transform_mode = TransformMode::Move;
    static SelectionController* get_instance();

    std::list<ComplexShape*> get_selected_shapes();
    bool is_point_on_selection(sf::Vector2f mouse_position);
    bool try_select_shape(ComplexShape* shape, sf::Vector2f point, bool is_union);
    void try_add_shape_to_selection(ComplexShape* shape, bool is_union);
    void remove_shape_from_selection(ComplexShape* shape);
    void delete_selected_shapes();
    void clear_selection();

    void try_begin_drag_transform_mode(sf::Vector2f mouse_position);
    void end_drag_transform_mode();
    void update_transform(sf::Vector2f mouse_position);

    void try_copy_shapes();
    void try_paste_shapes(sf::Vector2f mouse_position);

    void draw_selection(sf::RenderWindow& window);
};