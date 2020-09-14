from manimlib.imports import *

from ssjgz_scene import *
from downarrow import *
from digit_scroll_animation import *
from graph import *

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

        scene.play(g.create_animation(run_time = 2))

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
        scene.play(GrowFromCenter(arrow))

        scene.play(GrowFromCenter(coin_mobjects[0]),
                  GrowFromCenter(coin_mobjects[1]),
                  GrowFromCenter(coin_mobjects[2]))

        grundy_node_tex_colour = "#2600ff"
        
        grundy_tex_mobjects = []
        for node in nodes:
            node_mobject = g.mobject_for_node[node]
            node_grundy_tex = TexMobject('grundy', color = grundy_node_tex_colour, fill_opacity = 1, fill_color = grundy_node_tex_colour)
            node_grundy_tex.next_to(node_mobject, DOWN)
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
            grundy_number_label = TexMobject(r'\textit{grundy number} =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            grundy_number_label.scale(disttracker_text_scale)
            grundy_number_label.next_to(disttracker_title_display, 2 * DOWN)
            grundy_number_label.align_on_border(LEFT)

            grundy_number_second_equals = None

            grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE)
            grundy_value_mobject.scale(disttracker_text_scale)
            grundy_value_mobject.next_to(grundy_number_label, RIGHT)

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

        # Save a thumbnail now that the initial UI has been set up. 
        # We might overwrite it later, though!
        scene.save_thumbnail()

        # Ok - move through the node chain!
        distTracker = DistTracker()
        tracked_distance_mobjects = []
        for node_index,node in enumerate(nodes):

            if dist_tracker_implementation == 'optimised' and node_index == 7:
                scene.save_thumbnail() # This state makes for a nice thumbnail :)

            # Propagate.
            propagate_text = TexMobject(r'\textit{Propagate}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            propagate_text.scale(disttracker_text_scale)
            propagate_text.align_on_border(RIGHT)
            propagate_text.set_y(disttracker_title_display.get_y())
            scene.play(FadeInFrom(propagate_text, DOWN))

            grundy_value_to_move = grundy_value_mobject.copy()
            node_grundy = node.config['grundy_text']
            node_grundy_target = node_grundy.copy()
            node_grundy_target.shift([-node_grundy_target.get_width() * 2, -1, 0])
            xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            xor_symbol.next_to(node_grundy_target, RIGHT, buff = SMALL_BUFF)
            grundy_value_destination = TexMobject(str(distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = disttracker_grundy_color)
            grundy_value_destination.next_to(xor_symbol, RIGHT, buff = SMALL_BUFF)
            equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            equal_symbol.next_to(grundy_value_destination, buff = SMALL_BUFF)

            scene.play(
                      Transform(node_grundy, node_grundy_target),
                      Transform(grundy_value_to_move, grundy_value_destination),
                      FadeIn(xor_symbol),
                      FadeIn(equal_symbol))

            new_grundy_tex = TexMobject(str(node.config['grundy_number'] ^ distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = grundy_node_tex_colour)
            new_grundy_tex.next_to(equal_symbol, RIGHT, buff = SMALL_BUFF)
            scene.play(FadeIn(new_grundy_tex))

            node_grundy_to_fade_out = node_grundy.copy()
            node_grundy.become(new_grundy_tex)
            scene.remove(new_grundy_tex)
            node_grundy_target = node_grundy.copy()
            node_grundy_target.next_to(g.mobject_for_node[node], DOWN)

            scene.play(FadeOutAndShift(propagate_text, UP),
                      FadeOut(node_grundy_to_fade_out),
                      FadeOut(grundy_value_to_move),
                      FadeOut(xor_symbol),
                      FadeOut(equal_symbol),
                      Transform(node_grundy, node_grundy_target))

            # Collect.
            collect_text = TexMobject(r'\textit{Collect}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            collect_text.scale(disttracker_text_scale)
            collect_text.align_on_border(RIGHT)
            collect_text.set_y(disttracker_title_display.get_y())

            coin_mobject_for_node = node.config['coin_mobject']

            outtro_anims = [FadeOutAndShift(collect_text, UP)]

            if coin_mobject_for_node:
                scene.play(FadeInFrom(collect_text, DOWN),
                           WiggleOutThenIn(coin_mobject_for_node))
            else:
                scene.play(FadeInFrom(collect_text, DOWN))

            if not coin_mobject_for_node:
                cross_out = Cross(collect_text)
                scene.play(Write(cross_out))
                scene.play(FadeOutAndShift(collect_text, UP),
                          FadeOutAndShift(cross_out, UP))
            else:
                if dist_tracker_implementation == 'naive':
                    shift_elements_to_right_by = 0
                    new_tracked_distance_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = coin_mobject_for_node.get_fill_color())
                    new_tracked_distance_mobject.scale(disttracker_text_scale)
                    new_tracked_distance_mobject.digit = 0
                    shift_pairs = []
                    new_element_to_add = None
                    if not tracked_distance_mobjects:
                        new_tracked_distance_mobject.next_to(grundy_number_label, RIGHT)

                        assert(not grundy_number_second_equals)
                        grundy_number_second_equals = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        grundy_number_second_equals.scale(disttracker_text_scale)
                        grundy_number_second_equals.next_to(new_tracked_distance_mobject, RIGHT)

                        grundy_value_mobject_target = grundy_value_mobject.copy()
                        grundy_value_mobject_target.next_to(grundy_number_second_equals, RIGHT)
                        shift_pairs.append([grundy_value_mobject, grundy_value_mobject_target])

                        new_element_to_add = grundy_number_second_equals
                    else:
                        xor_text = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                        xor_text.scale(disttracker_text_scale)
                        xor_text.next_to(tracked_distance_mobjects[-1], RIGHT)
                        new_tracked_distance_mobject.next_to(xor_text, RIGHT)

                        grundy_number_second_equals_target = grundy_number_second_equals.copy()
                        grundy_number_second_equals_target.next_to(new_tracked_distance_mobject)

                        grundy_value_mobject_target = grundy_value_mobject.copy()
                        grundy_value_mobject_target.next_to(grundy_number_second_equals_target, RIGHT)

                        shift_pairs.append([grundy_value_mobject, grundy_value_mobject_target])
                        shift_pairs.append([grundy_number_second_equals, grundy_number_second_equals_target])

                        new_element_to_add = xor_text


                    coin_copy = coin_mobject_for_node.copy()

                    animations = [Transform(coin_copy, new_tracked_distance_mobject),
                                  Transform(grundy_value_mobject, grundy_value_mobject_target),
                                  FadeIn(new_element_to_add),
                                  FadeOutAndShift(collect_text, UP)]
                    for shift_pair in shift_pairs:
                        animations.append(Transform(shift_pair[0], shift_pair[1]))

                    scene.play(*animations, *outtro_anims)

                    scene.remove(coin_copy)
                    scene.add(new_tracked_distance_mobject)
                    tracked_distance_mobjects.append(new_tracked_distance_mobject)

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
                adjust_dists_text = TexMobject(r'\textit{adjustAllDistances}(1)', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                adjust_dists_text.scale(disttracker_text_scale)
                adjust_dists_text.align_on_border(RIGHT)
                adjust_dists_text.set_y(disttracker_title_display.get_y())

                intro_animations = [FadeInFrom(adjust_dists_text, DOWN)]
                outtro_animations = [FadeOutAndShift(adjust_dists_text, UP), ApplyMethod(arrow.shift, [nodes[node_index + 1].config['center_x'] - node.config['center_x'], 0, 0])]

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
                    for digit_mobject in tracked_distance_mobjects:
                        plusOne = add_plus_one(digit_mobject)

                        fadedMovedPlusOne = plusOne.copy()
                        fadedMovedPlusOne.set_opacity(0)
                        fadedMovedPlusOne.shift((0, 3 * plusOne.get_height(), 0))

                        addPlusOneAnims.append(FadeInFrom(plusOne, DOWN))

                        fadePlusOneAnims.append(Transform(plusOne, fadedMovedPlusOne))

                    scene.play(*intro_animations,
                              LaggedStart(*addPlusOneAnims))

                    animations = []
                    for digit_mobject in tracked_distance_mobjects:
                        new_digit_value = digit_mobject.digit + 1
                        animations.append(create_scroll_digit_to_animation(digit_mobject, digit_mobject.digit, new_digit_value, digitMObjectScale = disttracker_text_scale))

                    print("current_grundy_value:", current_grundy_value, " new_grundy_value:", new_grundy_value)

                    animations.append(create_scroll_digit_to_animation(grundy_value_mobject, current_grundy_value, new_grundy_value, digitMObjectScale = disttracker_text_scale))
                    animations.extend(outtro_animations)

                    for digit_mobject in tracked_distance_mobjects:
                        digit_mobject.digit = digit_mobject.digit + 1
                    
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
            self.value_mobject = value_mobject
            self.add(value_mobject)

    def copy(self):
        return self.deepcopy()

class MoveCoins2Editorial_3_show_branches(SSJGZScene):
    def construct(self):
        super().construct()

class MoveCoins2Editorial_4_collect_and_propagate_branches_naive(SSJGZScene):
    def construct(self):
        super().construct()

        g = Graph(self, globals()["Node"], globals()["NodeCoinMObject"])
        node_radius = 0.3

        MAGENTA = "#ff00ff"
        RED = "#ff0000"
        BROWN = "#8B4513"
        PURPLE = "#800080"
        ORANGE = "#FF8C00"

        distTracker = DistTracker()

        grundy_node_tex_colour = "#2600ff"

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
            if parent:
                g.create_edge(new_node, parent, {})

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


        b1 = create_node(centre_node, 0.2, -0.8, random.randint(0, 7), None)
        b1_nodeA = create_node(b1, 0.9, -0.9, random.randint(0, 7), ORANGE)
        b1_nodeB = create_node(b1, 0.1, -1.1, random.randint(0, 7), BROWN)
        b1_nodeC = create_node(b1, -0.6, -0.8, random.randint(0, 7), None)
        b1_nodeD = create_node(b1_nodeA, -0.2, -0.7, random.randint(0, 7), None)
        branch_roots.append(b1)

        rotate_tree_90_degrees_counter_clockwise(g)

        b2 = create_node(centre_node, -0.2, -0.7, random.randint(0, 7), None)
        b2_nodeA = create_node(b2, 0.9, -0.9, random.randint(0, 7), YELLOW)
        b2_nodeB = create_node(b2, 0.1, -1.1, random.randint(0, 7), None)
        b2_nodeC = create_node(b2, -0.6, -0.8, random.randint(0, 7), None)
        b2_nodeD = create_node(b2_nodeB, -0.2, -0.7, random.randint(0, 7), GREEN)
        b2_nodeE = create_node(b2_nodeA, -0.2, -0.7, random.randint(0, 7), None)
        branch_roots.append(b2)

        rotate_tree_90_degrees_counter_clockwise(g)

        b3 = create_node(centre_node, -0.2, -0.7, random.randint(0, 7), MAGENTA)
        b3_nodeA = create_node(b3, 0.6, -0.9, random.randint(0, 7), None)
        b3_nodeB = create_node(b3, -0.1, -1.1, random.randint(0, 7), None)
        b3_nodeC = create_node(b3_nodeA, 0.1, -0.9, random.randint(0, 7), None)
        b4_nodeD = create_node(b3_nodeC, -0.9, 0.0, random.randint(0, 7), RED)
        branch_roots.append(b3)

        rotate_tree_90_degrees_counter_clockwise(g)

        b4 = create_node(centre_node, 0.3, -0.8, random.randint(0, 7), PURPLE)
        b4_nodeA = create_node(b4, 0.6, -0.8, random.randint(0, 7), None)
        b4_nodeB = create_node(b4_nodeA, 0.1, -0.7, random.randint(0, 7), GREY)
        branch_roots.append(b4)

        furthest_point_from_centre = 0.0
        for node in g.nodes:
            dx = node.config['center_x'] - centre_node.config['center_x']
            dy = node.config['center_y'] - centre_node.config['center_y']
            furthest_point_from_centre = max(furthest_point_from_centre, math.sqrt(dx * dx + dy * dy))

        print("furthest_point_from_centre:", furthest_point_from_centre)

        required_center_x = -self.camera.get_frame_width() / 2 +  DEFAULT_MOBJECT_TO_EDGE_BUFFER + furthest_point_from_centre
        required_center_y = self.camera.get_frame_height() / 2 -  DEFAULT_MOBJECT_TO_EDGE_BUFFER - furthest_point_from_centre
        dx = required_center_x - centre_node.config['center_x']
        dy = required_center_y - centre_node.config['center_y']
        print("dx: ", dx, " dy: ", dy)
        for node in g.nodes:
            node.config['center_x'] = node.config['center_x'] + dx
            node.config['center_y'] = node.config['center_y'] + dy

        self.play(g.create_animation())

        # Graph is displayed; now the DistTracker.
        graph_bottom = centre_node.config['center_y'] - furthest_point_from_centre

        disttracker_top_y = graph_bottom
        disttracker_text_scale = 1.5
        disttracker_title_display = TexMobject(r'\textit{DistTracker}^\text{TM}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        disttracker_title_display.scale(disttracker_text_scale)
        disttracker_title_display.align_on_border(LEFT)
        disttracker_title_display.set_y(disttracker_top_y - disttracker_title_display.get_height() / 2 - MED_LARGE_BUFF)

        # Grundy number display.
        grundy_number_second_equals = None

        grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE)
        grundy_value_mobject.digitValue = 0
        grundy_value_mobject.scale(disttracker_text_scale)
        grundy_value_mobject.text_scale_factor = disttracker_text_scale

        grundy_value_mobject.set_y(grundy_value_mobject.get_height() / 2 - self.camera.get_frame_height() / 2 + DEFAULT_MOBJECT_TO_EDGE_BUFFER)

        grundy_number_label = TexMobject(r'\textit{grundy number} =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        grundy_number_label.scale(disttracker_text_scale)
        grundy_number_label.set_x(0)
        grundy_number_label.next_to(grundy_value_mobject, UP)


        grundy_xor_elements = [grundy_value_mobject]

        self.play(Write(disttracker_title_display), Write(grundy_number_label), Write(grundy_value_mobject))

        enlarged_node_radius = node_radius * 2
        horizontal_gap_between_nodes = enlarged_node_radius
        vertical_gap_between_nodes = enlarged_node_radius
        branch_to_straighten_index = 2
        # Iterate through the branches.
        for i in range(0, len(branch_roots)):
            print("branch_to_straighten_index:", branch_to_straighten_index)

            previous_graph_state = g.get_restorable_state()
            # Move the center node out of the way of the nodes from the other branches
            centre_node.config['center_x'] = centre_node.config['center_x'] + 2 * enlarged_node_radius
            centre_node.config['radius'] = enlarged_node_radius

            branch_to_straighten = branch_roots[branch_to_straighten_index]
            rightmost_branch = branch_to_straighten
            other_branches = branch_roots.copy()
            other_branches.remove(branch_to_straighten)

            descendents_by_height = g.find_descendents_at_height(centre_node, ignore_node_list = other_branches)
            descendents_by_height.pop(0) # Ditch the root
            for layer in descendents_by_height:
                layer.sort(key = lambda m: -m.config['center_y'])

            x = centre_node.config['center_x'] + node_radius + horizontal_gap_between_nodes + enlarged_node_radius
            for layer in descendents_by_height:
                y = centre_node.config['center_y']
                for node in layer:
                    node.config['center_x'] = x
                    node.config['center_y'] = y
                    node.config['radius'] = enlarged_node_radius
                    y = y - 2 * enlarged_node_radius - vertical_gap_between_nodes
                x = x + 2 * enlarged_node_radius + horizontal_gap_between_nodes

            def align_objects_sequentially(mobjects, y, centre_horizontally = False):
                for i,mobject in enumerate(mobjects):
                    if i >= 1:
                        mobjects[i].next_to(mobjects[i - 1], RIGHT)
                    mobjects[i].set_y(y)

                if centre_horizontally:
                    left_edge = mobjects[0].get_x() - mobjects[0].get_width() / 2
                    total_width = (mobjects[-1].get_x() + mobjects[-1].get_width() / 2) - left_edge
                    desired_left_edge = -total_width / 2
                    for mobject in mobjects:
                        mobject.set_x(mobject.get_x() + (desired_left_edge - left_edge))



            # Propagate.
            propagate_text = TexMobject(r'\textit{Propagate}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            propagate_text.scale(disttracker_text_scale)
            propagate_text.align_on_border(RIGHT)
            propagate_text.set_y(disttracker_title_display.get_y())

            self.play(g.create_animation(), FadeInFrom(propagate_text, DOWN))

            def create_rectangle_around_nodes(nodes, gap_size):
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

                rectangle = Rectangle(color=RED, width = rect_right - rect_left, height = rect_top - rect_bottom)
                # Shift so that top-left is at origin.
                rectangle.shift(rectangle.get_width() / 2 * RIGHT, rectangle.get_height() / 2 * DOWN)
                # Shift to correct position.
                rectangle.shift(rect_left * RIGHT + rect_top * UP)

                return rectangle

            nodes_at_current_dist_rect = create_rectangle_around_nodes([centre_node], gap_size = SMALL_BUFF)
            self.play(Write(nodes_at_current_dist_rect))
            for layer in descendents_by_height:
                nodes_at_current_dist_rect_target = create_rectangle_around_nodes(layer, gap_size = SMALL_BUFF)
                self.play(Transform(nodes_at_current_dist_rect, nodes_at_current_dist_rect_target))

            self.play(FadeOutAndShift(propagate_text, UP), FadeOut(nodes_at_current_dist_rect))

            # Collect
            collect_text = TexMobject(r'\textit{Collect}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            collect_text.scale(disttracker_text_scale)
            collect_text.align_on_border(RIGHT)
            collect_text.set_y(disttracker_title_display.get_y())
            self.play(FadeInFrom(collect_text, DOWN))

            def create_brace_for_node_pair(nodeLeft, nodeRight, tex):
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
                text = TexMobject(tex, color = BLACK, fill_opacity = 1, fill_color = BLACK)
                text.text_scale_factor = 1.5
                text.scale(text.text_scale_factor)
                text.next_to(brace, UP)
                brace.text = text

                return brace

            brace = None
            digit = None
            distance_from_center = 1
            for layer in descendents_by_height:
                anims = []

                if not brace:
                    brace = create_brace_for_node_pair(centre_node, layer[0], 1)
                    digit = brace.text
                    digit.digitValue = 1
                    anims.append(GrowFromCenter(brace))
                    anims.append(Write(digit))
                    self.play(*anims)
                else:
                    brace_target = create_brace_for_node_pair(centre_node, layer[0], digit.digitValue)
                    digit_target = digit.copy()
                    digit_target.next_to(brace_target, UP)
                    anims.append(Transform(brace, brace_target))
                    anims.append(Transform(digit, digit_target))
                    self.play(*anims, run_time = 0.5)
                    self.play(create_scroll_digit_to_animation(digit, digit.digitValue, digit.digitValue + 1, digitMObjectScale = digit.text_scale_factor), run_time = 0.5)
                    digit.digitValue = digit.digitValue + 1 

                nodes_with_coins = []
                for node in layer:
                    if 'coin_colour' in node.config:
                        nodes_with_coins.append(node)

                if not nodes_with_coins:
                    cross_out = Cross(collect_text)
                    self.play(Write(cross_out), run_time = 0.3)
                    self.play(FadeOut(cross_out), run_time = 0.3)
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
                            self.save_thumbnail() # TODO - remove
                            assert(equals_index != -1)
                            xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
                            xor_symbol.scale(disttracker_text_scale)
                            new_objects.append(xor_symbol)
                            grundy_xor_elements_targets.insert(equals_index, coin_digit_mobject)
                            grundy_xor_elements_targets.insert(equals_index, xor_symbol)
                            grundy_xor_elements.insert(equals_index, coin_copy)
                            grundy_xor_elements.insert(equals_index, xor_symbol)


                        align_objects_sequentially(grundy_xor_elements_targets, grundy_xor_y, centre_horizontally = True)
                        anims.append(Transform(coin_copy, coin_digit_mobject))
                        for i in new_objects:
                            anims.append(FadeIn(i))
                        self.play(*anims)

                        distTracker.insertDist(distance_from_center)
                        self.play(create_scroll_digit_to_animation(grundy_value_mobject, grundy_value_mobject.digitValue, distTracker.grundyNumber(), digitMObjectScale = grundy_value_mobject.text_scale_factor), run_time = 0.5)
                        grundy_value_mobject.digitValue = distTracker.grundyNumber()


                distance_from_center = distance_from_center + 1


            g.restore_from_state(previous_graph_state)

            self.play(g.create_animation(), FadeOutAndShift(brace, UP), FadeOutAndShift(digit, UP), FadeOutAndShift(collect_text, UP))

            self.play(RotateGraph(g, centre_node, PI / 2))

            branch_to_straighten_index = (branch_to_straighten_index + 1) % len(branch_roots)


        #b4_nodeB.config['radius'] = 1
        #b4_nodeB.config['center_y'] = b4_nodeB.config['center_y'] - 2
        #b4_nodeB.config['center_x'] = b4_nodeB.config['center_x'] - 4.5
        #blah_mobject = g.mobject_for_node[b4_nodeB]
        #print("Before radius change:", blah_mobject.get_width())

        #self.play(g.create_animation())

        #print("After radius change:", blah_mobject.get_width())
        #b4_nodeB.config['radius'] = node_radius
        #self.play(g.create_animation())



class MoveCoins2Editorial_6_collect_and_propagate_along_node_chain_left_to_right_partial_grid(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, dist_tracker_implementation = 'partial_grid', right_to_left = False)
        
class MoveCoins2Editorial_7_collect_and_propagate_along_node_chain_left_to_right_optimised(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, dist_tracker_implementation = 'optimised', right_to_left = False)

