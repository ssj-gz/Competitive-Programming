from manimlib.imports import *

def create_down_arrow(arrow_tip, arrow_total_length, arrow_stem_thickness, arrow_tip_length, arrow_tip_base_width):
    arrow_points = []
    point = arrow_tip
    arrow_points.append(point)
    point = point + (arrow_tip_base_width / 2) * RIGHT + arrow_tip_length * UP
    arrow_points.append(point)
    point = point - (arrow_stem_thickness - arrow_tip_base_width) / 2 * LEFT
    arrow_points.append(point)
    point = point + (arrow_total_length - arrow_tip_length) * UP
    arrow_points.append(point)
    point = point + (arrow_stem_thickness) * LEFT
    arrow_points.append(point)
    point = point + (arrow_total_length - arrow_tip_length) * DOWN
    arrow_points.append(point)
    point = point - (arrow_stem_thickness - arrow_tip_base_width) / 2 * LEFT
    arrow_points.append(point)
    point = point + arrow_tip_base_width / 2 * RIGHT + arrow_tip_length * UP
    arrow_points.append(arrow_tip)

    return Polygon(*arrow_points, color = BLACK, fill_opacity = 1, fill_color = YELLOW)


