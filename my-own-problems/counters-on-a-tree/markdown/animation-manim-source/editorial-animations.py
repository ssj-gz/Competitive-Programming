from manimlib.imports import *

from ssjgz_scene import *
from downarrow import *
from digit_scroll_animation import *
from graph import *
from itertools import chain

class DistTracker():
    def __init__(self):
        self.distances = []
        self.numTimesAdjusted = 0

    def insertDist(self, newDist):
        self.distances.append(newDist)

    def adjustAllDistances(self, toAdd):
        self.numTimesAdjusted = self.numTimesAdjusted + 1
        for i,dist in enumerate(self.distances):
            self.distances[i] = self.distances[i] + toAdd

    def grundyNumber(self):
        result = 0
        for dist in self.distances:
            result = result ^ dist
        return result

    def getNumTimesAdjusted(self):
        return self.numTimesAdjusted
        

    def clear(self):
        self.distances = []
        self.numTimesAdjusted = 0

grundy_node_tex_colour = "#2600ff"

def align_objects_sequentially(mobjects, y, centre_horizontally_around_x = None, x_gap_between_elements = DEFAULT_MOBJECT_TO_MOBJECT_BUFFER):
    for i,mobject in enumerate(mobjects):
        if i >= 1:
            mobjects[i].next_to(mobjects[i - 1], RIGHT, buff = x_gap_between_elements)
        mobjects[i].set_y(y)

    if centre_horizontally_around_x is not None:
        left_edge = mobjects[0].get_x() - mobjects[0].get_width() / 2
        total_width = (mobjects[-1].get_x() + mobjects[-1].get_width() / 2) - left_edge
        desired_left_edge = -total_width / 2 + centre_horizontally_around_x
        for mobject in mobjects:
            mobject.set_x(mobject.get_x() + (desired_left_edge - left_edge))

def do_collect_and_propagate_along_node_chain_naive(scene, dist_tracker_implementation = 'naive', right_to_left = False):
        # Graph, coins, and the Grundy numbers beneath the coins.
        num_nodes = 8
        frame_width = scene.camera.get_frame_width()
        frame_height = scene.camera.get_frame_height()

        node_diameter_to_gap_ratio = 1.3
        proportion_of_frame_width_to_use = 0.9
        proportion_of_frame_height_to_use = 0.9
        node_diameter = (frame_width * proportion_of_frame_width_to_use) / (num_nodes + (num_nodes - 1) / node_diameter_to_gap_ratio)
        gap_between_nodes = node_diameter / node_diameter_to_gap_ratio
        node_radius = node_diameter / 2

        arrow_total_length = node_diameter
        arrow_dist_above_node = node_diameter / 5

        node_left = -frame_width * proportion_of_frame_width_to_use  / 2
        g = Graph(scene, globals()["Node"], globals()["NodeMObject"])
        previous_node = None
        nodes = []
        for i in range(0, num_nodes):
            newNode = g.create_node(node_left + node_radius, frame_height * proportion_of_frame_height_to_use / 2 - (arrow_total_length + arrow_dist_above_node), { 'radius' : node_radius})
            newNode.config['coin_mobject'] = None
            newNode.config['grundy_number'] = 0
            if previous_node:
                g.create_edge(previous_node, newNode, {})
            node_left = node_left + node_diameter + gap_between_nodes
            previous_node = newNode
            nodes.append(newNode)


        coin_radius = node_radius / 2

        def create_coin_for_node(node, coin_colour):
            coin_mobject = Circle(color=BLACK, fill_opacity = 1, fill_color = coin_colour, radius = coin_radius)
            node.config['coin_mobject'] = coin_mobject
            coin_mobject.move_to([node.config['center_x'], node.config['center_y'], 0])
            return coin_mobject

        coin_mobjects = []
        coin_mobjects.append(create_coin_for_node(nodes[0], ORANGE))
        coin_mobjects.append(create_coin_for_node(nodes[1], GREEN))
        coin_mobjects.append(create_coin_for_node(nodes[4], PURPLE))

        if right_to_left:
            # Simulate the left-to-right, so we have the correct starting
            # grundy numbers.
            distTracker = DistTracker()
            for node_index,node in enumerate(nodes):
                node.config['grundy_number'] = node.config['grundy_number'] ^ distTracker.grundyNumber()
                if node.config['coin_mobject']:
                    distTracker.insertDist(0)
                distTracker.adjustAllDistances(1)

            nodes.reverse()

        def arrow_tip_position_for_node(node):
            arrow_tip = [node.config['center_x'], node.config['center_y'], 0] + (node_diameter * 2 / 3) * UP
            return arrow_tip

        arrow = create_down_arrow(
                arrow_tip = arrow_tip_position_for_node(nodes[0]),
                arrow_total_length = arrow_total_length,
                arrow_stem_thickness = arrow_total_length / 4,
                arrow_tip_base_width = node_diameter * 2 / 3,
                arrow_tip_length = node_diameter / 2
                )
        scene.play(GrowFromCenter(arrow), g.create_animation(), *map(GrowFromCenter, coin_mobjects))

        grundy_node_tex_colour = "#2600ff"
        
        grundy_tex_mobjects = []
        for node in nodes:
            node_mobject = g.mobject_for_node[node]
            node_grundy_tex = TexMobject('grundy', color = grundy_node_tex_colour, fill_opacity = 1, fill_color = grundy_node_tex_colour)
            node_grundy_tex.next_to(node_mobject, DOWN)
            node_grundy_tex.text_scale_factor = 1
            grundy_tex_mobjects.append(node_grundy_tex)
            node.config['grundy_text'] = node_grundy_tex

        grundy_tex_mobjects_appear_anims = list(map(lambda n: GrowFromCenter(n), grundy_tex_mobjects))

        scene.play(*grundy_tex_mobjects_appear_anims)

        grundy_tex_mobjects_change_to_value_anims = []
        for node in nodes:
            grundy_text = node.config['grundy_text']
            target_value_grundy_tex = TexMobject(node.config['grundy_number'], color = grundy_node_tex_colour)
            target_value_grundy_tex.move_to(grundy_text)
            grundy_tex_mobjects_change_to_value_anims.append(Transform(grundy_text, target_value_grundy_tex))

        scene.play(*grundy_tex_mobjects_change_to_value_anims)

        # Set up DistTracker display.
        grundy_value_mobject = None
        disttracker_grundy_color = BLUE

        disttracker_top_y = 0
        disttracker_text_scale = 1.5
        disttracker_title_display = TexMobject(r'\textit{DistTracker}^\text{TM}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        disttracker_title_display.scale(disttracker_text_scale)
        disttracker_title_display.align_on_border(LEFT)
        disttracker_title_display.set_y(disttracker_top_y + disttracker_title_display.get_height() / 2)

        intro_anims = [Write(disttracker_title_display)]

        partial_grid = None
        NUM_BITS = 3


        if dist_tracker_implementation == 'naive':

            # Grundy number display.
            grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE)
            grundy_value_mobject.digitValue = 0
            grundy_value_mobject.scale(disttracker_text_scale)
            grundy_value_mobject.text_scale_factor = disttracker_text_scale

            grundy_value_mobject.set_y(grundy_value_mobject.get_height() / 2 - scene.camera.get_frame_height() / 2 + DEFAULT_MOBJECT_TO_EDGE_BUFFER)

            grundy_number_label = TexMobject(r'\textit{grundy number} =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            grundy_number_label.scale(disttracker_text_scale)
            grundy_number_label.set_x(0)
            grundy_number_label.next_to(grundy_value_mobject, UP * 2)

            grundy_xor_elements = [grundy_value_mobject]
            grundy_xor_digits = []

            grundy_number_second_equals = None

            scene.play(AnimationGroup(*intro_anims, Write(grundy_number_label), Write(grundy_value_mobject)))

        elif dist_tracker_implementation == 'partial_grid' or dist_tracker_implementation == 'optimised':
            def create_rectangle_aligned_to_cells(self, cell_col, cell_row, cell_horizontal_span, colour = "#ff0000"):
                first_cell = partial_grid.item_at[cell_row][0]
                cell_top_left_x = first_cell.get_x() - CELL_WIDTH / 2 + cell_col * CELL_WIDTH
                cell_top_left_y = first_cell.get_y() + CELL_HEIGHT / 2
                print("create_rectangle_aligned_to_cells: cell_col:", cell_col, " cell_row:", cell_row, " cell_horizontal_span:" , cell_horizontal_span)

                rectangle = Rectangle(color="#ff0000",fill_opacity=1, fill_color=colour, width = CELL_WIDTH * cell_horizontal_span, height = CELL_HEIGHT)
                rectangle_width = CELL_WIDTH * cell_horizontal_span
                print("num_in_row:", num_in_row)
                # Reposition so that top-left is at origin.
                rectangle.shift([+rectangle_width / 2, -CELL_HEIGHT / 2, 0])
                # Place in the correct place.
                rectangle.shift([cell_top_left_x, cell_top_left_y, 0])

                return rectangle


            grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE) # TODO
            grundy_value_mobject.digitValue = 0
            CELL_HEIGHT = 0.75
            CELL_WIDTH = 0.75
            partial_grid = VGroup()
            num_in_row = 2
            partial_grid.item_at = []
            red_one_zone_for_row = []
            partial_grid.addition_coins_for_row = []
            partial_grid.num_in_row = []
            partial_grid.create_rectangle_aligned_to_cells = create_rectangle_aligned_to_cells

            grid_topleft_x = -scene.camera.get_frame_width() / 2 + MED_LARGE_BUFF
            grid_topleft_y = disttracker_title_display.get_y() - 3 * disttracker_title_display.get_height()

            for row in range(0, NUM_BITS):
                partial_grid.item_at.append([])
                partial_grid.addition_coins_for_row.append([])
                for col in range(0, num_in_row):
                    square = Square(color=BLACK,fill_opacity=0, side_length = CELL_WIDTH)
                    # Reposition so that top left is at origin.
                    square.move_to([grid_topleft_x +CELL_WIDTH / 2, grid_topleft_y +CELL_HEIGHT / 2, 0])
                    # Place in the correct place.
                    square.shift([col * CELL_WIDTH, -row * CELL_HEIGHT, 0])
                    print("square: get_center:", square.get_center())

                    partial_grid.add(square)
                    partial_grid.item_at[row].append(square)


                red_one_zone_num_cells = num_in_row / 2
                red_one_zone = create_rectangle_aligned_to_cells(None, num_in_row // 2, row, num_in_row / 2)
                red_one_zone_for_row.append(red_one_zone)

                partial_grid.num_in_row.append(num_in_row)

                num_in_row = num_in_row * 2

            partial_grid.red_one_zone_for_row = red_one_zone_for_row

            for red_one_zone in red_one_zone_for_row:
                # Draw the red one zones *behind* the partial_grid.
                scene.bring_to_back(red_one_zone)


            # Powers of 2, to the right of the grid, one for each row/ bitNum.
            powers_of_two_mobjects = []
            to_add_mobjects = []
            plus_sign_mobjects = []
            power_of_2 = 1
            text_scale_factor = None
            for bitNum in range(0, NUM_BITS):
                power_of_two_mobject = TexMobject('2^'+str(bitNum) + "=" + str(power_of_2), colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                text_scale_factor = CELL_HEIGHT / power_of_two_mobject.get_height()
                power_of_two_mobject.scale(text_scale_factor)
                power_of_two_mobject.next_to(partial_grid.item_at[NUM_BITS - 1][-1])
                power_of_two_mobject.set_y(partial_grid.item_at[bitNum][0].get_y())
                powers_of_two_mobjects.append(power_of_two_mobject)
                power_of_2 = power_of_2 * 2

                is_last = (bitNum == NUM_BITS - 1)
                to_add_mobject = TexMobject('0', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                plus_object = TexMobject('+', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                to_add_mobject.scale(text_scale_factor)
                to_add_mobject.text_scale_factor = text_scale_factor
                plus_object.align_on_border(RIGHT)
                plus_object.set_y(partial_grid.item_at[bitNum][0].get_y())
                to_add_mobject.next_to(plus_object, LEFT)
                if is_last:
                    plus_object.set_opacity(0)

                to_add_mobject.digitValue = 0
                to_add_mobjects.append(to_add_mobject)
                plus_sign_mobjects.append(plus_object)


            addition_line_y = to_add_mobject[-1].get_y() - to_add_mobject[-1].get_height() / 2 - MED_SMALL_BUFF
            addition_line_left = to_add_mobject[0].get_x() - to_add_mobject[0].get_width()
            addition_line_right = to_add_mobject[0].get_x() + to_add_mobject[0].get_width()
            addition_line_mobject = Line([addition_line_left, addition_line_y, 0], [addition_line_right, addition_line_y, 0], color = BLACK)

            grundy_value_mobject.scale(text_scale_factor)
            grundy_value_mobject.text_scale_factor = text_scale_factor
            grundy_value_mobject.next_to(addition_line_mobject, DOWN)

            grundy_number_label = TexMobject(r'\textit{grundy number} =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            grundy_number_label.scale(text_scale_factor)
            grundy_number_label.next_to(grundy_value_mobject, LEFT)

            partial_grid.to_add_mobjects = to_add_mobjects
            partial_grid.powers_of_two_mobjects = powers_of_two_mobjects

            partial_grid.coin_mobjects_for_row = []
            partial_grid.position_for_new_coins = []
            for bitNum in range(0, NUM_BITS):
                partial_grid.coin_mobjects_for_row.append([])
                partial_grid.position_for_new_coins.append(0)

            scene.play(*intro_anims, Write(partial_grid), *map(Write, powers_of_two_mobjects), *map(Write, red_one_zone_for_row), *map(Write, to_add_mobjects), *map(Write, plus_sign_mobjects), Write(addition_line_mobject), Write(grundy_number_label), Write(grundy_value_mobject))
            #self.play(ApplyMethod(thing.shift, LEFT * CELL_WIDTH),
            #          Transform(grid, grid.copy()))
            #self.play(ApplyMethod(thing.shift, RIGHT * CELL_WIDTH),
            #          Transform(grid, grid.copy()))
            ##self.play(WiggleOutThenIn(grid))


        # Ok - move through the node chain!
        distTracker = DistTracker()
        for node_index,node in enumerate(nodes):

            if dist_tracker_implementation == 'optimised' and node_index == 7:
                scene.save_thumbnail() # This state makes for a nice thumbnail :)
            if dist_tracker_implementation != 'optimised' and node_index == 4:
                scene.save_thumbnail()

            # Propagate.
            propagate_text = TexMobject(r'\textit{Propagate}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            propagate_text.scale(disttracker_text_scale)
            propagate_text.align_on_border(RIGHT)
            propagate_text.set_y(disttracker_title_display.get_y())
            scene.play(FadeInFrom(propagate_text, DOWN))

            create_equation_anims = []
            new_grundy_number_mobjects = []
            remove_equation_anims = []
            for node in [node]:
                equation_scale_factor = 1.4
                equation_drop_y_amount = node.config['grundy_text'].get_height()
                node_mobject = g.mobject_for_node[node]
                old_grundy_number_mobject = node.config['grundy_text'].copy()
                grundy_number_under_node_x = old_grundy_number_mobject.get_x()
                grundy_number_under_node_y = old_grundy_number_mobject.get_y()
                old_grundy_number_mobject.shift(equation_drop_y_amount * DOWN)
                old_grundy_number_mobject.scale(equation_scale_factor)
                xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                xor_symbol.scale(equation_scale_factor)
                grundy_from_disttracker_target = grundy_value_mobject.copy()
                grundy_from_disttracker = grundy_value_mobject.copy()
                grundy_from_disttracker_target.scale(old_grundy_number_mobject.get_height() / grundy_from_disttracker.get_height())
                print("blah scale:", old_grundy_number_mobject.get_height() / grundy_from_disttracker.get_height())
                equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                equal_symbol.scale(equation_scale_factor)
                new_grundy_number = node.config['grundy_number'] ^ distTracker.grundyNumber()
                new_grundy_number_mobject = TexMobject(str(new_grundy_number), colour = BLACK, fill_opacity = 1, fill_color = old_grundy_number_mobject.get_fill_color())
                new_grundy_number_mobject.scale(equation_scale_factor)
                new_grundy_number_mobjects.append(new_grundy_number_mobject)

                grundy_update_equation = [old_grundy_number_mobject, xor_symbol, grundy_from_disttracker_target, equal_symbol, new_grundy_number_mobject]
                align_objects_sequentially(grundy_update_equation, old_grundy_number_mobject.get_y(), centre_horizontally_around_x = node.config['center_x'], x_gap_between_elements = SMALL_BUFF)
                for i in grundy_update_equation:
                    scene.remove(i)

                create_equation_anims.extend([ 
                                            Transform(node.config['grundy_text'], old_grundy_number_mobject),
                                            FadeIn(xor_symbol),
                                            Transform(grundy_from_disttracker, grundy_from_disttracker_target),
                                            FadeIn(equal_symbol)
                                        ])


                new_grundy_number_mobject_under = new_grundy_number_mobject.copy()
                new_grundy_number_mobject_under.scale(1 / equation_scale_factor)
                new_grundy_number_mobject_under.move_to([grundy_number_under_node_x, grundy_number_under_node_y, 0])


            scene.play(*create_equation_anims)
            scene.play(*map(FadeIn, new_grundy_number_mobjects))

            for i,node in enumerate([node]):
                node_mobject = g.mobject_for_node[node]
                #node.config['grundy_text'].become(new_grundy_number_mobjects[i])
                #scene.remove(new_grundy_number_mobjects[i])
                # Ideally, we'd now Transform node.config['grundy_text'] into new_grundy_number_mobject_under,
                # but for some reason, this results in a slightly glitch in the Transform animation,
                # and I have no idea why.
                scene.remove(node.config['grundy_text'])

            remove_equation_anims.extend([ 
                                        FadeOut(old_grundy_number_mobject),
                                        FadeOut(xor_symbol),
                                        FadeOut(grundy_from_disttracker),
                                        FadeOut(equal_symbol),
                                        Transform(new_grundy_number_mobject, new_grundy_number_mobject_under)
                                    ])

            scene.play(*remove_equation_anims, FadeOutAndShift(propagate_text, UP))

            #grundy_value_to_move = grundy_value_mobject.copy()
            #node_grundy = node.config['grundy_text']
            #node_grundy_target = node_grundy.copy()
            #node_grundy_target.shift([-node_grundy_target.get_width() * 2, -1, 0])
            #xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            #xor_symbol.next_to(node_grundy_target, RIGHT, buff = SMALL_BUFF)
            #grundy_value_destination = TexMobject(str(distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = disttracker_grundy_color)
            #grundy_value_destination.next_to(xor_symbol, RIGHT, buff = SMALL_BUFF)
            #equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            #equal_symbol.next_to(grundy_value_destination, buff = SMALL_BUFF)

            #scene.play(
            #          Transform(node_grundy, node_grundy_target),
            #          Transform(grundy_value_to_move, grundy_value_destination),
            #          FadeIn(xor_symbol),
            #          FadeIn(equal_symbol))

            #new_grundy_tex = TexMobject(str(node.config['grundy_number'] ^ distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = grundy_node_tex_colour)
            #new_grundy_tex.next_to(equal_symbol, RIGHT, buff = SMALL_BUFF)
            #scene.play(FadeIn(new_grundy_tex))

            #node_grundy_to_fade_out = node_grundy.copy()
            #node_grundy.become(new_grundy_tex)
            #scene.remove(new_grundy_tex)
            #node_grundy_target = node_grundy.copy()
            #node_grundy_target.next_to(g.mobject_for_node[node], DOWN)

            #scene.play(FadeOutAndShift(propagate_text, UP),
            #          FadeOut(node_grundy_to_fade_out),
            #          FadeOut(grundy_value_to_move),
            #          FadeOut(xor_symbol),
            #          FadeOut(equal_symbol),
            #          Transform(node_grundy, node_grundy_target))

            # Collect.
            collect_text = TexMobject(r'\textit{Collect}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            collect_text.scale(disttracker_text_scale)
            collect_text.align_on_border(RIGHT)
            collect_text.set_y(disttracker_title_display.get_y())

            coin_mobject_for_node = node.config['coin_mobject']

            outtro_anims = [FadeOutAndShift(collect_text, UP)]

            if coin_mobject_for_node:
                scene.play(FadeInFrom(collect_text, DOWN),
                           WiggleOutThenIn(coin_mobject_for_node, scale_value = 1.4))
            else:
                scene.play(FadeInFrom(collect_text, DOWN))

            if not coin_mobject_for_node:
                cross_out = Cross(collect_text)
                scene.play(Write(cross_out))
                scene.play(FadeOutAndShift(collect_text, UP),
                          FadeOutAndShift(cross_out, UP))
            else:
                if dist_tracker_implementation == 'naive':
                    coin_colour = coin_mobject_for_node.get_fill_color()
                    print("coin_colour:", coin_colour, " coin_mobject:", hasattr(node, 'coin_mobject'), " node: ", repr(node))
                    is_first_coin = (len(grundy_xor_elements) == 1)
                    new_objects = []
                    coin_digit_mobject = TexMobject(str(0), colour = BLACK, fill_opacity = 1, fill_color = coin_colour)
                    coin_digit_mobject.scale(disttracker_text_scale)
                    grundy_xor_elements_targets = []
                    anims = []
                    grundy_xor_y = grundy_xor_elements[-1].get_y()

                    coin_copy = coin_mobject_for_node.copy()

                    for i in grundy_xor_elements:
                        target = i.copy()
                        grundy_xor_elements_targets.append(target)
                        anims.append(Transform(i, target))
                    if is_first_coin:
                        equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        equal_symbol.scale(disttracker_text_scale)
                        equal_symbol.equal_symbol = True
                        print("equal_symbol:", repr(equal_symbol))
                        new_objects.append(equal_symbol)
                        grundy_xor_elements_targets.insert(0, equal_symbol)
                        grundy_xor_elements_targets.insert(0, coin_digit_mobject)
                        grundy_xor_elements.insert(0, equal_symbol)
                        grundy_xor_elements.insert(0, coin_copy)
                    else:
                        equals_index = -1
                        print("enumerating grundy_xor_elements:")
                        for i,obj in enumerate(grundy_xor_elements):
                            print(" ", repr(i))
                            if hasattr(obj, 'equal_symbol'):
                                equals_index = i
                                break
                        assert(equals_index != -1)
                        xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        xor_symbol.scale(disttracker_text_scale)
                        new_objects.append(xor_symbol)
                        grundy_xor_elements_targets.insert(equals_index, coin_digit_mobject)
                        grundy_xor_elements_targets.insert(equals_index, xor_symbol)
                        grundy_xor_elements.insert(equals_index, coin_copy)
                        grundy_xor_elements.insert(equals_index, xor_symbol)


                    align_objects_sequentially(grundy_xor_elements_targets, grundy_xor_y, centre_horizontally_around_x = 0)
                    anims.append(Transform(coin_copy, coin_digit_mobject))
                    for i in new_objects:
                        anims.append(FadeIn(i))

                    insertDist_text = TexMobject(r'\textit{insertDist}(', str(0), ')', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                    insertDist_text[1].set_color(coin_colour)
                    insertDist_text.scale(disttracker_text_scale)
                    insertDist_text.next_to(collect_text.get_center(), 2 * DOWN)
                    insertDist_text.align_on_border(RIGHT)
                    anims.append(FadeInFrom(insertDist_text, DOWN))

                    scene.play(*anims)

                    coin_copy.text_scale_factor = disttracker_text_scale
                    coin_copy.digitValue = 0
                    grundy_xor_digits.append(coin_copy)

                    scene.play(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor),
                               FadeOutAndShift(insertDist_text, UP), *outtro_anims)
                    grundy_value_mobject.digitValue = distTracker.grundyNumber()
                    #shift_elements_to_right_by = 0
                    #new_tracked_distance_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = coin_mobject_for_node.get_fill_color())
                    #new_tracked_distance_mobject.scale(disttracker_text_scale)
                    #new_tracked_distance_mobject.digit = 0
                    #shift_pairs = []
                    #new_element_to_add = None
                    #if not tracked_distance_mobjects:
                    #    new_tracked_distance_mobject.next_to(grundy_number_label, RIGHT)

                    #    assert(not grundy_number_second_equals)
                    #    grundy_number_second_equals = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                    #    grundy_number_second_equals.scale(disttracker_text_scale)
                    #    grundy_number_second_equals.next_to(new_tracked_distance_mobject, RIGHT)

                    #    grundy_value_mobject_target = grundy_value_mobject.copy()
                    #    grundy_value_mobject_target.next_to(grundy_number_second_equals, RIGHT)
                    #    shift_pairs.append([grundy_value_mobject, grundy_value_mobject_target])

                    #    new_element_to_add = grundy_number_second_equals
                    #else:
                    #    xor_text = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                    #    xor_text.scale(disttracker_text_scale)
                    #    xor_text.next_to(tracked_distance_mobjects[-1], RIGHT)
                    #    new_tracked_distance_mobject.next_to(xor_text, RIGHT)

                    #    grundy_number_second_equals_target = grundy_number_second_equals.copy()
                    #    grundy_number_second_equals_target.next_to(new_tracked_distance_mobject)

                    #    grundy_value_mobject_target = grundy_value_mobject.copy()
                    #    grundy_value_mobject_target.next_to(grundy_number_second_equals_target, RIGHT)

                    #    shift_pairs.append([grundy_value_mobject, grundy_value_mobject_target])
                    #    shift_pairs.append([grundy_number_second_equals, grundy_number_second_equals_target])

                    #    new_element_to_add = xor_text


                    #coin_copy = coin_mobject_for_node.copy()

                    #animations = [Transform(coin_copy, new_tracked_distance_mobject),
                    #              Transform(grundy_value_mobject, grundy_value_mobject_target),
                    #              FadeIn(new_element_to_add),
                    #              FadeOutAndShift(collect_text, UP)]
                    #for shift_pair in shift_pairs:
                    #    animations.append(Transform(shift_pair[0], shift_pair[1]))

                    #scene.play(*animations, *outtro_anims)

                    #scene.remove(coin_copy)
                    #scene.add(new_tracked_distance_mobject)
                    #tracked_distance_mobjects.append(new_tracked_distance_mobject)

                elif dist_tracker_implementation == 'partial_grid' or dist_tracker_implementation == 'optimised':
                    coin_mobjects_to_transform = []

                    for bitNum in range(0, NUM_BITS):
                        coin_copy = coin_mobject_for_node.copy()
                        coin_target_mobject = coin_mobject_for_node.copy()
                        coin_target_mobject.move_to(partial_grid.item_at[bitNum][partial_grid.position_for_new_coins[bitNum]])
                        coin_copy.target = coin_target_mobject

                        coin_copy.addition_representative = coin_copy.copy()
                        coin_copy.addition_representative.set_opacity(0)
                        coin_copy.addition_representative.cross = Cross(coin_copy.addition_representative)
                        coin_copy.addition_representative.is_crossed = False

                    
                        coin_mobjects_to_transform.append(coin_copy)
                        # TODO - handle the case where there are more than one coins
                        # at the left of the row.
                        for existing_coin in partial_grid.coin_mobjects_for_row[bitNum]:
                            if existing_coin.pos_in_row == partial_grid.position_for_new_coins[bitNum]:
                                reduced_coin_scale = 0.5
                                reduced_coin_dx = -coin_target_mobject.get_width() / 4 * reduced_coin_scale
                                reduced_coin_dy = -coin_target_mobject.get_height() / 4 * reduced_coin_scale
                                coin_target_mobject.scale(reduced_coin_scale)
                                coin_target_mobject.shift(-reduced_coin_dx, -reduced_coin_dy)
                                existing_coin_target = existing_coin.copy()
                                existing_coin_target.scale(reduced_coin_scale)
                                existing_coin_target.shift(+reduced_coin_dx, +reduced_coin_dy)
                                existing_coin.target = existing_coin_target
                                coin_mobjects_to_transform.append(existing_coin)

                        partial_grid.coin_mobjects_for_row[bitNum].append(coin_copy)
                        coin_copy.pos_in_row = partial_grid.position_for_new_coins[bitNum]

                    coin_anims = []
                    for coin_mobject in coin_mobjects_to_transform:
                        coin_anims.append(Transform(coin_mobject, coin_mobject.target))

                    scene.play(*coin_anims, *outtro_anims)


                distTracker.insertDist(0)

            # Move to next node.    
            if node_index != num_nodes - 1:

                print("adjustAllDistances")
                addToAllDists_text = TexMobject(r'\textit{addToAllDists}(1)', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                addToAllDists_text.scale(disttracker_text_scale)
                addToAllDists_text.align_on_border(RIGHT)
                addToAllDists_text.set_y(disttracker_title_display.get_y())

                intro_animations = [FadeInFrom(addToAllDists_text, DOWN)]
                outtro_animations = [FadeOutAndShift(addToAllDists_text, UP), ApplyMethod(arrow.shift, [nodes[node_index + 1].config['center_x'] - node.config['center_x'], 0, 0])]

                current_grundy_value = distTracker.grundyNumber()
                distTracker.adjustAllDistances(1)
                new_grundy_value = distTracker.grundyNumber()

                if dist_tracker_implementation == 'naive':
                    def add_plus_one(digitMObject):
                        plusOne = TexMobject("+1")
                        plusOne.set_color(RED)
                        plusOne.scale(disttracker_text_scale / 3)
                        plusOne.next_to(digitMObject, TOP / 5)
                        plusOne.shift((digitMObject.get_width() / 2, 0, 0))
                        scene.add(plusOne)
                        return plusOne

                    addPlusOneAnims = []
                    fadePlusOneAnims = []
                    for digit_mobject in grundy_xor_digits:
                        plusOne = add_plus_one(digit_mobject)

                        fadedMovedPlusOne = plusOne.copy()
                        fadedMovedPlusOne.set_opacity(0)
                        fadedMovedPlusOne.shift((0, 3 * plusOne.get_height(), 0))

                        addPlusOneAnims.append(FadeInFrom(plusOne, DOWN))

                        fadePlusOneAnims.append(Transform(plusOne, fadedMovedPlusOne))

                    scene.play(*intro_animations,
                              LaggedStart(*addPlusOneAnims))

                    animations = []
                    for digit_mobject in grundy_xor_digits:
                        new_digit_value = digit_mobject.digitValue + 1
                        animations.append(create_scroll_digit_to_animation(digit_mobject, digit_mobject.digitValue, new_digit_value, digitMObjectScale = disttracker_text_scale))

                    print("current_grundy_value:", current_grundy_value, " new_grundy_value:", new_grundy_value)

                    animations.append(create_scroll_digit_to_animation(grundy_value_mobject, current_grundy_value, new_grundy_value, digitMObjectScale = disttracker_text_scale))
                    grundy_value_mobject.digitValue = new_grundy_value
                    animations.extend(outtro_animations)

                    for digit_mobject in grundy_xor_digits:
                        digit_mobject.digitValue = digit_mobject.digitValue + 1
                    
                    scene.play(LaggedStart(*animations),
                              LaggedStart(*fadePlusOneAnims))
                elif dist_tracker_implementation == 'partial_grid' or dist_tracker_implementation == 'optimised':
                    scene.play(*intro_animations)

                    coin_advance_anims = []
                    red_one_zone_advance_anims = []
                    coin_addition_representative_transforms = []
                    addition_representatives_to_hide = []
                    addition_representatives_to_show = []
                    for bitNum in range(0, NUM_BITS):
                        print("row:", bitNum, " num coins in row:", len(partial_grid.coin_mobjects_for_row[bitNum]))
                        num_in_row = len(partial_grid.item_at[bitNum])
                        
                        if dist_tracker_implementation == 'optimised':

                            # Scroll the red-one-zones.
                            if len(partial_grid.red_one_zone_for_row[bitNum]) == 1:
                                # First time scrolling; let's set things up the way we want them.
                                red_one_zone = partial_grid.red_one_zone_for_row[bitNum]
                                empty_red_one_zone = partial_grid.create_rectangle_aligned_to_cells(None, 0, 0, 0, GREEN)

                                partial_grid.red_one_zone_for_row[bitNum] = [[red_one_zone, [num_in_row // 2, num_in_row - 1]],
                                                                             [empty_red_one_zone, [-1, -1]]]
                                red_one_zone.id = "R1"
                                empty_red_one_zone.id = "R2"

                            red_zone_info_for_row = partial_grid.red_one_zone_for_row[bitNum]

                            print("Advancing red one zones: bitNum: ", bitNum)
                            print("Before:")
                            for i in range(0, 2):
                                print(red_zone_info_for_row[i][0].id, " [", red_zone_info_for_row[i][1][0], ", ", red_zone_info_for_row[i][1][1], "]")


                            for i in range(0, 2):
                                if red_zone_info_for_row[i][1][0] != -1:
                                    red_zone_info_for_row[i][1][0] = red_zone_info_for_row[i][1][0] - 1
                                if red_zone_info_for_row[i][1][1] != -1:
                                    red_zone_info_for_row[i][1][1] = red_zone_info_for_row[i][1][1] - 1

                            if red_zone_info_for_row[0][1][0] == -1:
                                print("left is -1, right is:", red_zone_info_for_row[0][1][1])
                                if red_zone_info_for_row[0][1][1] != -1:
                                    print("narrowing")
                                    # Ok - just narrowing, not disappeared yet.
                                    # Set the left cell back to 0.
                                    red_zone_info_for_row[0][1][0] = 0
                                    # Activate other Rect?
                                    if red_zone_info_for_row[1][1][0] == -1:
                                        # Yes.
                                        print("activating other rect")
                                        red_zone_info_for_row[1][1][0] = num_in_row - 1
                                        red_zone_info_for_row[1][1][1] = num_in_row - 1
                                        blah = partial_grid.create_rectangle_aligned_to_cells(None, num_in_row, bitNum, 0)
                                        red_zone_info_for_row[1][0].become(blah) 
                                    else:
                                        print("not activating other rect (", red_zone_info_for_row[1][1][0], ")")



                                else:
                                    # Disappeared!
                                    print("disappearing")
                                    blah = red_zone_info_for_row.pop(0)
                                    red_zone_info_for_row.append(blah)
                                    red_zone_info_for_row[1][1][0] = -1
                                    red_zone_info_for_row[1][1][1] = -1
                                    red_one_zone_advance_anims.append(Transform(red_zone_info_for_row[1][0], partial_grid.create_rectangle_aligned_to_cells(None, 0, bitNum, 0)))

                                    # Activate other Rect?
                                    if red_zone_info_for_row[0][1][0] == -1:
                                        # Yes.
                                        print("activating other rect")
                                        red_zone_info_for_row[0][1][0] = num_in_row - 1
                                        red_zone_info_for_row[0][1][1] = num_in_row - 1
                                        blah = partial_grid.create_rectangle_aligned_to_cells(None, num_in_row, bitNum, 0)
                                        red_zone_info_for_row[0][0].become(blah) 
                                    else:
                                        print("not activating other rect (", red_zone_info_for_row[0][1][0], ")")

                            for i in range(0, 2):
                                if red_zone_info_for_row[i][1][0] > 0 and (1 + red_zone_info_for_row[i][1][1] - red_zone_info_for_row[i][1][0]) < num_in_row // 2:
                                    # The rightmost rectangle needs to keep its right-hand edge on the right of the grid until it fills num_in_row // 2 cells.
                                    red_zone_info_for_row[i][1][1] = num_in_row - 1


                            print("After:")
                            for i in range(0, 2):
                                print(red_zone_info_for_row[i][0].id, " [", red_zone_info_for_row[i][1][0], ", ", red_zone_info_for_row[i][1][1], "]")

                            for i in range(0, 2):
                                scene.bring_to_back(red_zone_info_for_row[i][0])
                                if red_zone_info_for_row[i][1][0] != -1:
                                    red_one_zone_advance_anims.append(Transform(red_zone_info_for_row[i][0], partial_grid.create_rectangle_aligned_to_cells(None, red_zone_info_for_row[i][1][0], bitNum, 1 + red_zone_info_for_row[i][1][1] - red_zone_info_for_row[i][1][0])))

                        for coin in partial_grid.coin_mobjects_for_row[bitNum]:
                            if dist_tracker_implementation == 'partial_grid':
                                was_in_red = coin.pos_in_row >= num_in_row / 2
                            else:
                                adjusted_coin_pos = (coin.pos_in_row + distTracker.getNumTimesAdjusted() - 1 + num_in_row) % num_in_row
                                was_in_red = adjusted_coin_pos >= num_in_row / 2

                            x_offset_to_new_pos = 0

                            if dist_tracker_implementation == 'partial_grid':
                                # Scroll the coins.
                                if coin.pos_in_row == num_in_row - 1:
                                    # Wrap back to beginning.
                                    target_coin_at_row_begin = coin.copy()
                                    target_coin_at_row_begin.shift([-CELL_WIDTH * (num_in_row - 1), 0, 0])

                                    coin_advance_anims.append(CounterclockwiseTransform(coin, target_coin_at_row_begin))
                                else:
                                    coin_advance_anims.append(ApplyMethod(coin.shift, [CELL_WIDTH, 0, 0]))

                                previous_pos = coin.pos_in_row
                                coin.pos_in_row = (coin.pos_in_row + 1) % num_in_row
                                x_offset_to_new_pos = (coin.pos_in_row - previous_pos) * CELL_WIDTH

                            # Do we need an addition representative in the right place?
                            if dist_tracker_implementation == 'partial_grid':
                                in_red = coin.pos_in_row >= num_in_row / 2
                            else:
                                adjusted_coin_pos = (coin.pos_in_row + distTracker.getNumTimesAdjusted()) % num_in_row
                                in_red = adjusted_coin_pos >= num_in_row / 2

                            
                            print("bitNum:", bitNum, " coin:", repr(coin), " was_in_red:", was_in_red, " in_red:", in_red, " numTimesAdjusted:", distTracker.getNumTimesAdjusted())
                            if not was_in_red and in_red:
                                coin.addition_representative.become(coin.copy())
                                coin.addition_representative.set_opacity(0)
                                coin.addition_representative.cross.set_opacity(0)
                                coin.addition_representative.is_crossed = False
                                addition_representatives_to_show.append(coin.addition_representative)
                                coin.addition_representative.shift([x_offset_to_new_pos, 0, 0])
                                print("bitNum:", bitNum, " coin gets added to additions:", repr(coin.addition_representative))
                                partial_grid.addition_coins_for_row[bitNum].append(coin.addition_representative)

                            elif was_in_red and not in_red:
                                print("bitNum:", bitNum, " coin gets removed from additions:", repr(coin.addition_representative))
                                partial_grid.addition_coins_for_row[bitNum].remove(coin.addition_representative)
                                addition_representatives_to_hide.append(coin.addition_representative)

                                # Move the addition representative back to the source node in the partial grid.
                                new = coin.copy()
                                new.shift([x_offset_to_new_pos, 0, 0])
                                coin_addition_representative_transforms.append(Transform(coin.addition_representative, new))

                                target_cross = coin.addition_representative.cross.copy()
                                target_cross.move_to(new.get_center())
                                target_cross.set_opacity(0)
                                coin_addition_representative_transforms.append(Transform(coin.addition_representative.cross, target_cross))

                        x = partial_grid.powers_of_two_mobjects[bitNum].get_x() + powers_of_two_mobjects[bitNum].get_width()
                        y = partial_grid.item_at[bitNum][0].get_y()
                        print("bitNum:", bitNum, " partial_grid.addition_coins_for_row: ", partial_grid.addition_coins_for_row[bitNum])
                        for coin_addition_representative in partial_grid.addition_coins_for_row[bitNum]:
                            new = Circle(radius = coin_radius, color = BLACK, fill_color = coin_addition_representative.get_fill_color())
                            new.set_opacity(1)
                            new.move_to([x, y, 0])
                            coin_addition_representative.cross.move_to(coin_addition_representative.get_center())
                            target_cross = coin_addition_representative.cross.copy()
                            target_cross.move_to(new.get_center())
                            coin_addition_representative_transforms.append(Transform(coin_addition_representative, new))
                            coin_addition_representative_transforms.append(Transform(coin_addition_representative.cross, target_cross))
                            x = x + coin_radius * 2 + SMALL_BUFF
                            print(" new at:", new.get_x())

                    # Advance the coins.
                    if dist_tracker_implementation == 'partial_grid':
                        scene.play(*coin_advance_anims)
                    else:
                        scene.play(*red_one_zone_advance_anims)

                    # Update the addition representative coins.
                    for m in addition_representatives_to_show:
                        m.set_opacity(1)
                    if coin_addition_representative_transforms:
                        scene.play(*coin_addition_representative_transforms)

                    for m in addition_representatives_to_hide:
                        m.set_opacity(0)

                    # Add crosses and update all numbers.
                    number_update_anims = []
                    power_of_2 = 1
                    for bitNum in range(0, NUM_BITS):
                        addition_coins_for_row = partial_grid.addition_coins_for_row[bitNum].copy()
                        while len(addition_coins_for_row) >= 2:
                            for i in range(0,2):
                                if not addition_coins_for_row[0].is_crossed:
                                    scene.remove(addition_coins_for_row[0].cross)
                                    addition_coins_for_row[0].cross = Cross(addition_coins_for_row[0])
                                    number_update_anims.append(Write(addition_coins_for_row[0].cross))
                                    addition_coins_for_row[0].is_crossed = True
                                addition_coins_for_row.pop(0)

                        if addition_coins_for_row:
                            target_cross = addition_coins_for_row[0].cross
                            target_cross.set_opacity(0)
                            number_update_anims.append(Transform(addition_coins_for_row[0].cross,target_cross))
                            addition_coins_for_row[0].is_crossed = False

                        to_add_mobject = partial_grid.to_add_mobjects[bitNum]
                        new_value = 0
                        if len(partial_grid.addition_coins_for_row[bitNum]) % 2 == 1:
                            new_value = power_of_2
                            number_update_anims.append(WiggleOutThenIn(partial_grid.powers_of_two_mobjects[bitNum]))

                        number_update_anims.append(create_scroll_digit_to_animation(to_add_mobject, to_add_mobject.digitValue, new_value, digitMObjectScale = to_add_mobject.text_scale_factor))
                        to_add_mobject.digitValue = new_value

                        power_of_2 = power_of_2 * 2

                    number_update_anims.append(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor))
                    grundy_value_mobject.digitValue = distTracker.grundyNumber()

                    if number_update_anims:
                        scene.play(*number_update_anims)


                    scene.play(*outtro_animations)

                    if dist_tracker_implementation == 'optimised':
                        for bitNum in range(0, NUM_BITS):
                            partial_grid.position_for_new_coins[bitNum] = (partial_grid.position_for_new_coins[bitNum] + partial_grid.num_in_row[bitNum] - 1) % partial_grid.num_in_row[bitNum]

class RotateGraph(Animation):
    def __init__(self, graph, node_to_rotate_about, angle):
        dummyMObject = Circle()
        dummyMObject.set_opacity(0)

        self.graph = graph
        self.node_to_rotate_about = node_to_rotate_about
        self.angle = angle

        self.original_node_pos = {}

        dummyGroup = VGroup()
        for node in graph.nodes:
            self.original_node_pos[node] = [node.config['center_x'], node.config['center_y']]
            dummyGroup.add(graph.mobject_for_node[node])

        for edge in graph.edges:
            dummyGroup.add(graph.mobject_for_edge[edge])

        Animation.__init__(self, dummyGroup)

    def interpolate_mobject(self, alpha):
        for node in self.graph.nodes:
            if node == self.node_to_rotate_about:
                continue
            node_to_move = node
            node_mobject_to_move = self.graph.mobject_for_node[node]
            start_x = self.original_node_pos[node][0]
            start_y = self.original_node_pos[node][1]

            rotation_center_x = self.node_to_rotate_about.config['center_x']
            rotation_center_y = self.node_to_rotate_about.config['center_y']

            dx = start_x - rotation_center_x
            dy = start_y - rotation_center_y

            angle = self.angle * alpha
            cos_angle = math.cos(angle)
            sin_angle = math.sin(angle)

            new_x = dx * cos_angle - dy * sin_angle + rotation_center_x
            new_y = dx * sin_angle + dy * cos_angle + rotation_center_y

            if node.node_id == 3:
                print("node: ", node_to_move, " angle: ", angle, " start_x: ", start_x, " start_y: ", start_y, " rotation_center_x: ", rotation_center_x, " rotation_center_y: ", rotation_center_y, " new_x: ", new_x, " new_y: ", new_y)

            node_to_move.config['center_x'] = new_x
            node_to_move.config['center_y'] = new_y

            #print("newx/y for node:", node_to_move.config['value'], new_x, new_y, " alpha:", alpha, " self: ", repr(self))
            node_mobject_to_move.move_to([new_x, new_y, 0])
            node_mobject_to_move.config['center_x'] = new_x
            node_mobject_to_move.config['center_y'] = new_y
            #print("Moved mobject ", repr(node_mobject_to_move), " for node:", node_to_move.config['value'], " to ", node_mobject_to_move.get_center())

            for edge in self.graph.edges:
                if edge.start_node == node_to_move or edge.end_node == node_to_move:
                    start_and_end_point = edge.get_start_and_end_point(alpha)
                    edge_object = self.graph.mobject_for_edge[edge]
                    edge_object.set_start_and_end_attrs([start_and_end_point[0][0], start_and_end_point[0][1], 0], [start_and_end_point[1][0], start_and_end_point[1][1], 0])
                    edge_object.generate_points()








class MoveCoins2Editorial_1_collect_and_propagate_along_node_chain_left_to_right_naive(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, right_to_left = False)
        
class MoveCoins2Editorial_2_collect_and_propagate_along_node_chain_right_to_left_naive(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, right_to_left = True)

class NodeCoinMObject(VMobject):
    def __init__(self, node_id, config):
        VMobject.__init__(self)
        self.node_id = node_id
        self.config = config

        border_colour = BLACK
        if 'border_colour' in config:
            border_colour = config['border_colour']

        circle_radius = config['radius']
        circle = Circle(radius=circle_radius,color=border_colour,fill_opacity=1, fill_color=WHITE)
        self.add(circle)

        if 'coin_colour' in config:
            coin_radius = config['coin_radius']
            coin_mobject = Circle(color=border_colour, fill_opacity = 1, fill_color = config['coin_colour'], radius = coin_radius)
            self.coin_mobject = coin_mobject
            self.add(coin_mobject)
            print("added coin_mobject:", coin_mobject, " to ", repr(self))

        if 'value' in config:
            self.value = config['value']
            value_mobject = TexMobject(str(self.value), colour = BLACK, fill_opacity = 1, fill_color = config['value_colour'])
            value_mobject.scale(0.8)
            value_mobject.text_scale = 0.8
            self.value_mobject = value_mobject
            self.add(value_mobject)

    def copy(self):
        return self.deepcopy()

def create_rectangle_around_nodes(nodes, gap_size = SMALL_BUFF, stroke_width = 1):
    rect_top = -1000
    rect_bottom = +1000
    rect_left = 1000
    rect_right = -1000
    for node in nodes:
        node_x = node.config['center_x']
        node_y = node.config['center_y']
        node_radius = node.config['radius']
        rect_left = min(rect_left, node_x - node_radius)
        rect_right = max(rect_right, node_x + node_radius)
        rect_top = max(rect_top, node_y + node_radius)
        rect_bottom = min(rect_bottom, node_y - node_radius)

    rect_left = rect_left - gap_size
    rect_right = rect_right + gap_size
    rect_top = rect_top + gap_size
    rect_bottom = rect_bottom - gap_size

    rectangle = Rectangle(color=RED, width = rect_right - rect_left, height = rect_top - rect_bottom, stroke_width = stroke_width)
    # Shift so that top-left is at origin.
    rectangle.shift(rectangle.get_width() / 2 * RIGHT, rectangle.get_height() / 2 * DOWN)
    # Shift to correct position.
    rectangle.shift(rect_left * RIGHT + rect_top * UP)

    return rectangle


def create_centroid_and_branches_tree(scene):
    g = Graph(scene, globals()["Node"], globals()["NodeCoinMObject"])
    node_radius = 0.3

    MAGENTA = "#ff00ff"
    RED = "#ff0000"
    BROWN = "#8B4513"
    PURPLE = "#800080"
    ORANGE = "#FF8C00"

    def create_node(parent, dx, dy, grundy_number, coin_colour):
        config = {'radius': node_radius, 'value' : grundy_number, 'coin_radius': node_radius * 3 / 5, 'value_colour': grundy_node_tex_colour}
        if coin_colour:
            config['coin_colour'] = coin_colour

        parent_x = 0
        parent_y = 0
        if parent:
            parent_x = parent.config['center_x']
            parent_y = parent.config['center_y']

        new_node = g.create_node(parent_x + dx, parent_y + dy, config)
        new_node.grundy_number = grundy_number
        if parent:
            g.create_edge(new_node, parent, {'colour' : BLACK})

        return new_node

    centre_node = create_node(None, 1, 1, 7, None)
    centre_node.config['border_colour'] = RED

    def rotate_tree_90_degrees_counter_clockwise(graph):
        center_x = centre_node.config['center_x']
        center_y = centre_node.config['center_y']

        for node in graph.nodes:
            dx = node.config['center_x'] - center_x
            dy = node.config['center_y'] - center_y

            node.config['center_x'] = center_x - dy
            node.config['center_y'] = center_y + dx

    branch_roots = []

    b1 = create_node(centre_node, 0.2, -0.8, random.randint(0, 7), BROWN)
    b1.branch_root_number = 1
    b1_nodeA = create_node(b1, 0.9, -0.9, random.randint(0, 7), ORANGE)
    b1_nodeB = create_node(b1, 0.1, -1.1, random.randint(0, 7), None)
    b1_nodeC = create_node(b1, -0.6, -0.8, random.randint(0, 7), None)
    b1_nodeD = create_node(b1_nodeA, -0.2, -0.7, random.randint(0, 7), None)
    branch_roots.append(b1)

    rotate_tree_90_degrees_counter_clockwise(g)

    b2 = create_node(centre_node, -0.2, -0.7, random.randint(0, 7), None)
    b2.branch_root_number = 2
    b2_nodeA = create_node(b2, 0.9, -0.9, random.randint(0, 7), YELLOW)
    b2_nodeB = create_node(b2, 0.1, -1.1, random.randint(0, 7), None)
    b2_nodeC = create_node(b2, -0.6, -0.8, random.randint(0, 7), None)
    b2_nodeD = create_node(b2_nodeB, -0.2, -0.7, random.randint(0, 7), GREEN)
    b2_nodeE = create_node(b2_nodeA, -0.2, -0.7, random.randint(0, 7), None)
    branch_roots.append(b2)

    rotate_tree_90_degrees_counter_clockwise(g)

    b3 = create_node(centre_node, -0.2, -0.7, random.randint(0, 7), MAGENTA)
    b3.branch_root_number = 3
    b3_nodeA = create_node(b3, 0.6, -0.9, random.randint(0, 7), None)
    b3_nodeB = create_node(b3, -0.1, -1.1, random.randint(0, 7), None)
    b3_nodeC = create_node(b3_nodeA, 0.1, -0.9, random.randint(0, 7), None)
    b4_nodeD = create_node(b3_nodeC, -0.9, 0.0, random.randint(0, 7), RED)
    branch_roots.append(b3)

    rotate_tree_90_degrees_counter_clockwise(g)

    b4 = create_node(centre_node, 0.3, -0.8, random.randint(0, 7), PURPLE)
    b4.branch_root_number = 4
    b4_nodeA = create_node(b4, 0.6, -0.8, random.randint(0, 7), None)
    b4_nodeB = create_node(b4_nodeA, 0.1, -0.7, random.randint(0, 7), GREY)
    branch_roots.append(b4)

    # Rotate 180 degrees, so that branch 1 is at the right.
    rotate_tree_90_degrees_counter_clockwise(g)
    rotate_tree_90_degrees_counter_clockwise(g)

    furthest_point_from_centre = 0.0
    for node in g.nodes:
        dx = node.config['center_x'] - centre_node.config['center_x']
        dy = node.config['center_y'] - centre_node.config['center_y']
        furthest_point_from_centre = max(furthest_point_from_centre, math.sqrt(dx * dx + dy * dy))

    print("furthest_point_from_centre:", furthest_point_from_centre)

    required_center_x = -scene.camera.get_frame_width() / 2 +  DEFAULT_MOBJECT_TO_EDGE_BUFFER + furthest_point_from_centre
    required_center_y = scene.camera.get_frame_height() / 2 -  DEFAULT_MOBJECT_TO_EDGE_BUFFER - furthest_point_from_centre
    dx = required_center_x - centre_node.config['center_x']
    dy = required_center_y - centre_node.config['center_y']
    print("dx: ", dx, " dy: ", dy)
    for node in g.nodes:
        node.config['center_x'] = node.config['center_x'] + dx
        node.config['center_y'] = node.config['center_y'] + dy

    return { 'graph' : g, 'branch_roots' : branch_roots, 'centre_node' : centre_node, 'furthest_point_from_centre' : furthest_point_from_centre }


class MoveCoins2Editorial_3_show_branches(SSJGZScene):
    def construct(self):
        super().construct()

        centroid_graph_info = create_centroid_and_branches_tree(self)
        g = centroid_graph_info['graph']
        centre_node = centroid_graph_info['centre_node']
        branch_roots = centroid_graph_info['branch_roots']
        node_radius = centre_node.config['radius']

        centre_node_x = centre_node.config['center_x']
        centre_node_y = centre_node.config['center_y']
        # Make more space around the centre node by moving all nodes
        # away from it by a constant amount.
        extra_space_from_center = 1
        for node in g.nodes:
            if node is centre_node:
                continue
            dx = node.config['center_x'] - centre_node_x
            dy = node.config['center_y'] - centre_node_y
            distance = math.sqrt(dx * dx + dy * dy)
            new_x = centre_node_x + dx * (distance + extra_space_from_center) / distance
            new_y = centre_node_y + dy * (distance + extra_space_from_center) / distance
            node.config['center_x'] = new_x
            node.config['center_y'] = new_y

        scale_factor = 1.2

        for node in g.nodes:
            dx = node.config['center_x'] - centre_node_x
            dy = node.config['center_y'] - centre_node_y
            distance = math.sqrt(dx * dx + dy * dy)
            new_x = centre_node_x + dx * scale_factor
            new_y = centre_node_y + dy * scale_factor
            node.config['center_x'] = new_x
            node.config['center_y'] = new_y

        for node in g.nodes:
            node.config['center_x'] = node.config['center_x'] - centre_node_x
            node.config['center_y'] = node.config['center_y'] - centre_node_y
            node.config['radius'] = node.config['radius'] * scale_factor


        branch_root_label_mobjects = []
        for branch_index in range(0, len(branch_roots)):
            branch_label = TexMobject(r'b_' + str(branch_index + 1), colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            branch_root_label_mobjects.append(branch_label)

        center_label_mobject = TexMobject(r'C', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        center_label_mobject.next_to(g.create_mobject_for_node(centre_node), UP, buff = SMALL_BUFF)
        center_label_mobject.shift([0.1, 0, 0])
        del g.mobject_for_node[centre_node]

        self.play(g.create_animation(), Write(center_label_mobject))

        branch_root_label_mobjects[0].next_to(g.mobject_for_node[branch_roots[0]], UP, buff = SMALL_BUFF)
        branch_root_label_mobjects[1].next_to(g.mobject_for_node[branch_roots[1]], LEFT, buff = SMALL_BUFF)
        branch_root_label_mobjects[2].next_to(g.mobject_for_node[branch_roots[2]], DOWN, buff = SMALL_BUFF)
        branch_root_label_mobjects[3].next_to(g.mobject_for_node[branch_roots[3]], UP, buff = SMALL_BUFF)

        self.play(LaggedStart(*map(FadeIn, branch_root_label_mobjects), lag_ratio = 3 * DEFAULT_LAGGED_START_LAG_RATIO))

        new_objects = []
        for branch_index in range(0, len(branch_roots)):
            other_branches = branch_roots.copy()
            other_branches.remove(branch_roots[branch_index])
            branch_nodes = g.find_descendents_at_height(centre_node, ignore_node_list = other_branches)
            branch_nodes = list(chain(*branch_nodes))
            branch_nodes.remove(centre_node)
            branch_rect = create_rectangle_around_nodes(branch_nodes, gap_size = SMALL_BUFF, stroke_width = 8)
            new_objects.append(branch_rect)
            branch_label = TexMobject(r'B_' + str(branch_index + 1), colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            branch_label.scale(2)
            if branch_index != 0:
                branch_label.next_to(branch_rect, LEFT, buff = SMALL_BUFF)
            else:
                branch_label.next_to(branch_rect, RIGHT, buff = SMALL_BUFF)
            new_objects.append(branch_label)

        self.play(LaggedStart(*map(FadeIn, new_objects), lag_ratio = 3 * DEFAULT_LAGGED_START_LAG_RATIO))

        self.save_thumbnail()

def do_collect_and_propagate_branches_naive(scene, reverse_branch_order = False):
    distTracker = DistTracker()

    centroid_graph_info = create_centroid_and_branches_tree(scene)
    g = centroid_graph_info['graph']
    centre_node = centroid_graph_info['centre_node']
    branch_roots = centroid_graph_info['branch_roots']
    node_radius = centre_node.config['radius']
    furthest_point_from_centre = centroid_graph_info['furthest_point_from_centre']

    if reverse_branch_order:
        # "Pretend" that we did the Propagate/Collect dance with branch order
        # b1, b2, b3 and b4 prior to starting this i.e. that we did do_collect_and_propagate_branches_naive
        # first.  We just need to update the grundy numbers.
        distTracker = DistTracker()

        for i in range(0, len(branch_roots)):
            print("branch:", i + 1)
            branch_to_straighten = branch_roots[i]
            other_branches = branch_roots.copy()
            other_branches.remove(branch_to_straighten)

            descendents_by_height = g.find_descendents_at_height(centre_node, ignore_node_list = other_branches)
            descendents_by_height.pop(0) # Ditch the root

            for layer in descendents_by_height:
                print("adjustAllDistances +1")
                distTracker.adjustAllDistances(1)
                for node in layer:
                    new_grundy_value =  node.grundy_number ^ distTracker.grundyNumber()
                    print(" change grundy from ", node.grundy_number, " to: ", new_grundy_value)
                    node.grundy_number = new_grundy_value
                    node.config['value'] = new_grundy_value
            distTracker.adjustAllDistances(-len(descendents_by_height))

            for dist_from_branch_root,layer in enumerate(descendents_by_height):
                for node in layer:
                    if 'coin_colour' in node.config:
                        print("insertDist:", dist_from_branch_root + 1)
                        distTracker.insertDist(dist_from_branch_root + 1)

        distTracker.clear()

    scene.play(g.create_animation())

    if reverse_branch_order:
        scene.play(RotateGraph(g, centre_node, 3 * PI / 2))

    # Graph is displayed; now the DistTracker.
    graph_bottom = centre_node.config['center_y'] - furthest_point_from_centre

    disttracker_top_y = graph_bottom
    disttracker_text_scale = 1.5
    disttracker_title_display = TexMobject(r'\textit{DistTracker}^\text{TM}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
    disttracker_title_display.scale(disttracker_text_scale)
    disttracker_title_display.align_on_border(LEFT)
    disttracker_title_display.set_y(disttracker_top_y - disttracker_title_display.get_height() / 2 - MED_LARGE_BUFF)

    # Grundy number display.
    grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE)
    grundy_value_mobject.digitValue = 0
    grundy_value_mobject.scale(disttracker_text_scale)
    grundy_value_mobject.text_scale_factor = disttracker_text_scale

    grundy_value_mobject.set_y(grundy_value_mobject.get_height() / 2 - scene.camera.get_frame_height() / 2 + DEFAULT_MOBJECT_TO_EDGE_BUFFER)

    grundy_number_label = TexMobject(r'\textit{grundy number} =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
    grundy_number_label.scale(disttracker_text_scale)
    grundy_number_label.set_x(0)
    grundy_number_label.next_to(grundy_value_mobject, UP * 2)


    grundy_xor_elements = [grundy_value_mobject]
    grundy_xor_digits = []

    scene.play(Write(disttracker_title_display), Write(grundy_number_label), Write(grundy_value_mobject))

    enlarged_node_radius = node_radius * 2
    horizontal_gap_between_nodes = enlarged_node_radius * 1.4 # i.e. between right edge of one and left edge of next
    x_dist_between_node_centres = 2 * enlarged_node_radius + horizontal_gap_between_nodes
    vertical_gap_between_nodes = enlarged_node_radius * 1.3
    if not reverse_branch_order:
        branch_to_straighten_index = 0
    else: 
        branch_to_straighten_index = len(branch_roots) - 1

    def add_amount_to_change_by(digitMObject, amount_to_change_by):
        text = str(amount_to_change_by)
        if amount_to_change_by > 0:
            text = "+" + text
        plusAmount = TexMobject(text)
        plusAmount.set_color(RED)
        plusAmount.scale(disttracker_text_scale / 3)
        plusAmount.next_to(digitMObject, TOP / 5)
        plusAmount.shift((digitMObject.get_width() / 2, 0, 0))
        scene.add(plusAmount)
        return plusAmount

    # Iterate through the branches.
    for i in range(0, len(branch_roots)):
        print("branch_to_straighten_index:", branch_to_straighten_index)

        previous_graph_state = g.get_restorable_state()
        # Move the center node out of the way of the nodes from the other branches
        centre_node.config['center_x'] = centre_node.config['center_x'] + 3 * enlarged_node_radius
        centre_node.config['center_y'] = centre_node.config['center_y'] + enlarged_node_radius * 1.5
        centre_node.config['radius'] = enlarged_node_radius

        branch_to_straighten = branch_roots[branch_to_straighten_index]
        rightmost_branch = branch_to_straighten
        other_branches = branch_roots.copy()
        other_branches.remove(branch_to_straighten)

        branch_label = TexMobject(r'\textit{Branch }b_' + str(branch_to_straighten.branch_root_number), colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        branch_label.align_on_border(RIGHT)
        branch_label.set_y(scene.camera.get_frame_height() / 2 - branch_label.get_height())

        descendents_by_height = g.find_descendents_at_height(centre_node, ignore_node_list = other_branches)
        descendents_by_height.pop(0) # Ditch the root
        for layer in descendents_by_height:
            layer.sort(key = lambda m: -m.config['center_y'])

        for edge in g.edges:
            for layer in descendents_by_height:
                for node in layer:
                    if edge.start_node == node or edge.end_node == node:
                        edge.config['colour'] = '#eeeeee'

        x = centre_node.config['center_x'] + node_radius + horizontal_gap_between_nodes + enlarged_node_radius
        for layer in descendents_by_height:
            y = centre_node.config['center_y']
            for node in layer:
                node.config['center_x'] = x
                node.config['center_y'] = y
                node.config['radius'] = enlarged_node_radius
                y = y - 2 * enlarged_node_radius - vertical_gap_between_nodes
            x = x + x_dist_between_node_centres

        # Propagate.
        propagate_text = TexMobject(r'\textit{Propagate}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        propagate_text.scale(disttracker_text_scale)
        propagate_text.align_on_border(RIGHT)
        propagate_text.set_y(disttracker_title_display.get_y())

        scene.play(g.create_animation(), FadeInFrom(propagate_text, DOWN), FadeInFrom(branch_label, DOWN))

        nodes_at_current_dist_rect = create_rectangle_around_nodes([centre_node], gap_size = SMALL_BUFF)
        scene.play(Write(nodes_at_current_dist_rect))
        for layer in descendents_by_height:
            nodes_at_current_dist_rect_target = create_rectangle_around_nodes(layer, gap_size = SMALL_BUFF)

            # Adjust all the distances + grundy (with instructions), and shift highlighting rect to this layer.
            addToAllDists_anims = []
            addToAllDists_text = TexMobject(r'\textit{addToAllDists}(1)', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            addToAllDists_text.scale(disttracker_text_scale)
            addToAllDists_text.next_to(propagate_text.get_center(), 2 * DOWN)
            addToAllDists_text.align_on_border(RIGHT)
            addToAllDists_anims.append(FadeInFrom(addToAllDists_text, DOWN))

            addPlusOneAnims = []
            fadePlusOneAnims = []
            for digit_mobject in grundy_xor_digits:
                plusOne = add_amount_to_change_by(digit_mobject, 1)

                fadedMovedPlusOne = plusOne.copy()
                fadedMovedPlusOne.set_opacity(0)
                fadedMovedPlusOne.shift((0, 3 * plusOne.get_height(), 0))

                addPlusOneAnims.append(FadeInFrom(plusOne, DOWN))

                fadePlusOneAnims.append(Transform(plusOne, fadedMovedPlusOne))

            scene.play(FadeInFrom(addToAllDists_text, DOWN), LaggedStart(*addPlusOneAnims))

            increment_digits_and_change_grundy_anims = []
            distTracker.adjustAllDistances(1)
            for digit_mobject in grundy_xor_digits:
                increment_digits_and_change_grundy_anims.append(create_scroll_digit_to_animation(digit_mobject, digit_mobject.digitValue, digit_mobject.digitValue + 1, digitMObjectScale = digit_mobject.text_scale_factor))
                digit_mobject.digitValue = digit_mobject.digitValue + 1

            increment_digits_and_change_grundy_anims.append(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor))
            grundy_value_mobject.digitValue = distTracker.grundyNumber()

            scene.play(Transform(nodes_at_current_dist_rect, nodes_at_current_dist_rect_target), FadeOutAndShift(addToAllDists_text, UP), *increment_digits_and_change_grundy_anims, LaggedStart(*fadePlusOneAnims))

            # Propagate the grundy numbers to all the nodes in this layer.
            # The animations are done in parallel, which makes things a little tricky.
            for edge in g.edges: # Workaround manim's odd z-ordering that makes the edges appear
                                 # in front of the equations, for some reason.
                scene.bring_to_back(g.mobject_for_edge[edge])

            create_equation_anims = []
            new_grundy_number_mobjects = []
            new_grundy_number_mobjects_in_nodes = []
            remove_equation_anims = []
            for node in layer:
                node_mobject = g.mobject_for_node[node]
                old_grundy_number_mobject = node_mobject.value_mobject.copy()
                old_grundy_number_mobject.next_to(node_mobject, DOWN, buff = SMALL_BUFF)
                xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                grundy_from_disttracker_target = grundy_value_mobject.copy()
                grundy_from_disttracker = grundy_value_mobject.copy()
                equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                new_grundy_number = node.grundy_number ^ distTracker.grundyNumber()
                new_grundy_number_mobject = TexMobject(str(new_grundy_number), colour = BLACK, fill_opacity = 1, fill_color = old_grundy_number_mobject.get_fill_color())
                new_grundy_number_mobject.scale(old_grundy_number_mobject.get_height() / new_grundy_number_mobject.get_height())
                new_grundy_number_mobjects.append(new_grundy_number_mobject)

                grundy_update_equation = [old_grundy_number_mobject, xor_symbol, grundy_from_disttracker_target, equal_symbol, new_grundy_number_mobject]
                align_objects_sequentially(grundy_update_equation, old_grundy_number_mobject.get_y(), centre_horizontally_around_x = node.config['center_x'])
                for i in grundy_update_equation:
                    scene.remove(i)

                create_equation_anims.extend([ 
                                            Transform(node_mobject.value_mobject, old_grundy_number_mobject),
                                            FadeIn(xor_symbol),
                                            Transform(grundy_from_disttracker, grundy_from_disttracker_target),
                                            FadeIn(equal_symbol)
                                        ])


                new_grundy_number_mobject_in_node = new_grundy_number_mobject.copy()
                new_grundy_number_mobject_in_node.move_to([node.config['center_x'], node.config['center_y'], 0])
                new_grundy_number_mobjects_in_nodes.append(new_grundy_number_mobject_in_node)

                remove_equation_anims.extend([ 
                                            FadeOut(old_grundy_number_mobject),
                                            FadeOut(xor_symbol),
                                            FadeOut(grundy_from_disttracker),
                                            FadeOut(equal_symbol),
                                        ])

            scene.play(*create_equation_anims)
            scene.play(*map(FadeIn, new_grundy_number_mobjects))

            for i,node in enumerate(layer):
                node_mobject = g.mobject_for_node[node]
                node_mobject.value_mobject.become(new_grundy_number_mobjects_in_nodes[i].copy())
                # Ideally, we'd Transform node_mobject.value_mobject into new_grundy_number_mobject_in_node,
                # but this results in a strange glitch and I don't know why.
                # Instead, Transform(new_grundy_number_mobject_in_node[i]), hide node_mobject.value_mobject,
                # and restore it later.
                node_mobject.value_mobject.set_opacity(0)
                remove_equation_anims.append(Transform(new_grundy_number_mobjects[i], new_grundy_number_mobjects_in_nodes[i]))


            scene.play(*remove_equation_anims)

            for i,node in enumerate(layer):
                node_mobject = g.mobject_for_node[node]
                node_mobject.set_opacity(1)
                scene.remove(new_grundy_number_mobjects[i])

            print("distTracker:", distTracker.distances)

        # Reset DistTracker.
        amount_to_subtract_from_disttracker = len(descendents_by_height)
        print("amount_to_subtract_from_disttracker:", amount_to_subtract_from_disttracker, " DistTracker:", distTracker.distances)
        addToAllDists_text = TexMobject(r'\textit{addToAllDists}(' + str(-amount_to_subtract_from_disttracker) + ')', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        addToAllDists_text.scale(disttracker_text_scale)
        addToAllDists_text.next_to(propagate_text, 2 * DOWN)
        addToAllDists_text.align_on_border(RIGHT)
        addToAllDists_anims.append(FadeInFrom(addToAllDists_text, DOWN))

        distTracker.adjustAllDistances(-amount_to_subtract_from_disttracker)
        subtractAnims = []
        fadeSubtractAnims = []
        for digit_mobject in grundy_xor_digits:
            subtractFromDistTracker = add_amount_to_change_by(digit_mobject, -amount_to_subtract_from_disttracker)

            fadedMovedSubtractFromDistTracker = subtractFromDistTracker.copy()
            fadedMovedSubtractFromDistTracker.set_opacity(0)
            fadedMovedSubtractFromDistTracker.shift((0, 3 * subtractFromDistTracker.get_height(), 0))

            subtractAnims.append(FadeInFrom(subtractFromDistTracker, DOWN))

            fadeSubtractAnims.append(Transform(subtractFromDistTracker, fadedMovedSubtractFromDistTracker))

        scene.play(FadeInFrom(addToAllDists_text, DOWN), LaggedStart(*subtractAnims))

        increment_digits_and_change_grundy_anims = []
        for digit_mobject in grundy_xor_digits:
            increment_digits_and_change_grundy_anims.append(create_scroll_digit_to_animation(digit_mobject, digit_mobject.digitValue, digit_mobject.digitValue - amount_to_subtract_from_disttracker, digitMObjectScale = digit_mobject.text_scale_factor))
            digit_mobject.digitValue = digit_mobject.digitValue - amount_to_subtract_from_disttracker

        increment_digits_and_change_grundy_anims.append(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor))
        grundy_value_mobject.digitValue = distTracker.grundyNumber()

        scene.play(FadeOutAndShift(propagate_text, UP), FadeOut(nodes_at_current_dist_rect), FadeOutAndShift(addToAllDists_text, UP), *increment_digits_and_change_grundy_anims, LaggedStart(*fadeSubtractAnims))

        # Collect
        collect_text = TexMobject(r'\textit{Collect}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        collect_text.scale(disttracker_text_scale)
        collect_text.align_on_border(RIGHT)
        collect_text.set_y(disttracker_title_display.get_y())
        scene.play(FadeInFrom(collect_text, DOWN))

        def create_brace_for_node_pair(nodeLeft, nodeRight, tex = None):
            dummyLeftMObject = Circle(radius = 0)
            dummyLeftMObject.move_to(g.mobject_for_node[nodeLeft].get_center())
            dummyLeftMObject.shift(nodeLeft.config['radius'] * UP)

            dummyRightMObject = Circle(radius = 0)
            dummyRightMObject.move_to(g.mobject_for_node[nodeRight].get_center())
            dummyRightMObject.shift(nodeRight.config['radius'] * UP)

            dummyGroup = VGroup()
            dummyGroup.add(dummyLeftMObject)
            dummyGroup.add(dummyRightMObject)

            brace = Brace(dummyGroup, color = BLACK, fill_color = BLACK, direction = UP )
            if tex:
                text = TexMobject(tex, color = BLACK, fill_opacity = 1, fill_color = BLACK)
                text.text_scale_factor = 1.5
                text.scale(text.text_scale_factor)
                text.next_to(brace, UP)
                brace.text = text

            return brace

        brace = None
        distance_from_centre_label = None
        distance_from_center = 1
        for layer in descendents_by_height:
            anims = []

            if not brace:
                brace = create_brace_for_node_pair(centre_node, layer[0], 1)
                distance_from_centre_label = brace.text
                distance_from_centre_label.digitValue = 1
                anims.append(GrowFromCenter(brace))
                anims.append(Write(distance_from_centre_label))
                scene.play(*anims)
            else:
                brace_target = create_brace_for_node_pair(centre_node, layer[0], distance_from_centre_label.digitValue)
                brace_target.text = brace.text
                anims.append(Transform(brace, brace_target))
                scene.play(Transform(brace, brace_target), create_scroll_digit_to_animation(distance_from_centre_label, distance_from_centre_label.digitValue, distance_from_centre_label.digitValue + 1, digitMObjectScale = distance_from_centre_label.text_scale_factor, x_move_amount = x_dist_between_node_centres / 2))
                distance_from_centre_label.digitValue = distance_from_centre_label.digitValue + 1 

            nodes_with_coins = []
            for node in layer:
                if 'coin_colour' in node.config:
                    nodes_with_coins.append(node)

            if not nodes_with_coins:
                cross_out = Cross(collect_text)
                scene.play(Write(cross_out), run_time = 0.3)
                scene.play(FadeOut(cross_out), run_time = 0.3)
            else:
                for node in nodes_with_coins:
                    coin_colour = node.config['coin_colour']
                    print("coin_colour:", coin_colour, " coin_mobject:", hasattr(node, 'coin_mobject'), " node: ", repr(node))
                    is_first_coin = (len(grundy_xor_elements) == 1)
                    new_objects = []
                    coin_digit_mobject = TexMobject(str(distance_from_center), colour = BLACK, fill_opacity = 1, fill_color = coin_colour)
                    coin_digit_mobject.scale(disttracker_text_scale)
                    grundy_xor_elements_targets = []
                    anims = []
                    grundy_xor_y = grundy_xor_elements[-1].get_y()

                    coin_copy = g.mobject_for_node[node].coin_mobject.copy()

                    for i in grundy_xor_elements:
                        target = i.copy()
                        grundy_xor_elements_targets.append(target)
                        anims.append(Transform(i, target))
                    if is_first_coin:
                        equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        equal_symbol.scale(disttracker_text_scale)
                        equal_symbol.equal_symbol = True
                        print("equal_symbol:", repr(equal_symbol))
                        new_objects.append(equal_symbol)
                        grundy_xor_elements_targets.insert(0, equal_symbol)
                        grundy_xor_elements_targets.insert(0, coin_digit_mobject)
                        grundy_xor_elements.insert(0, equal_symbol)
                        grundy_xor_elements.insert(0, coin_copy)
                    else:
                        equals_index = -1
                        print("enumerating grundy_xor_elements:")
                        for i,obj in enumerate(grundy_xor_elements):
                            print(" ", repr(i))
                            if hasattr(obj, 'equal_symbol'):
                                equals_index = i
                                break
                        assert(equals_index != -1)
                        xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        xor_symbol.scale(disttracker_text_scale)
                        new_objects.append(xor_symbol)
                        grundy_xor_elements_targets.insert(equals_index, coin_digit_mobject)
                        grundy_xor_elements_targets.insert(equals_index, xor_symbol)
                        grundy_xor_elements.insert(equals_index, coin_copy)
                        grundy_xor_elements.insert(equals_index, xor_symbol)


                    align_objects_sequentially(grundy_xor_elements_targets, grundy_xor_y, centre_horizontally_around_x = 0)
                    anims.append(Transform(coin_copy, coin_digit_mobject))
                    for i in new_objects:
                        anims.append(FadeIn(i))

                    insertDist_text = TexMobject(r'\textit{insertDist}(',  str(distance_from_center), ')', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                    insertDist_text[1].set_color(coin_colour)
                    insertDist_text.scale(disttracker_text_scale)
                    insertDist_text.next_to(collect_text.get_center(), 2 * DOWN)
                    insertDist_text.align_on_border(RIGHT)
                    anims.append(WiggleOutThenIn(distance_from_centre_label, scale_value = 2, run_time = 1))
                    anims.append(FadeInFrom(insertDist_text, DOWN))

                    scene.play(*anims)

                    coin_copy.text_scale_factor = disttracker_text_scale
                    coin_copy.digitValue = distance_from_center
                    grundy_xor_digits.append(coin_copy)

                    distTracker.insertDist(distance_from_center)
                    scene.play(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor),
                               FadeOutAndShift(insertDist_text, UP))
                    grundy_value_mobject.digitValue = distTracker.grundyNumber()

                    if distance_from_center == 2:
                        scene.save_thumbnail()


            distance_from_center = distance_from_center + 1

        
        g.restore_from_state(previous_graph_state)

        scene.play(g.create_animation(), FadeOutAndShift(brace, UP), FadeOutAndShift(distance_from_centre_label, UP), FadeOutAndShift(collect_text, UP))


        if not reverse_branch_order:
            branch_to_straighten_index = (branch_to_straighten_index + 1) % len(branch_roots)
            scene.play(RotateGraph(g, centre_node, PI / 2), FadeOutAndShift(branch_label, UP))
        else:
            branch_to_straighten_index = (branch_to_straighten_index + len(branch_roots) - 1) % len(branch_roots)
            scene.play(RotateGraph(g, centre_node, -PI / 2), FadeOutAndShift(branch_label, UP))

class MoveCoins2Editorial_4_collect_and_propagate_branches_naive(SSJGZScene):
    def construct(self):
        super().construct()

        do_collect_and_propagate_branches_naive(self)

class MoveCoins2Editorial_5_collect_and_propagate_branches_naive_backwards(SSJGZScene):
    def construct(self):
        super().construct()

        do_collect_and_propagate_branches_naive(self, reverse_branch_order =  True)

class MoveCoins2Editorial_6_collect_and_propagate_along_node_chain_left_to_right_partial_grid(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, dist_tracker_implementation = 'partial_grid', right_to_left = False)
        
class MoveCoins2Editorial_7_collect_and_propagate_along_node_chain_left_to_right_optimised(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, dist_tracker_implementation = 'optimised', right_to_left = False)

