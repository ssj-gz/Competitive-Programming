from manimlib.imports import *

from ssjgz_scene import *
from downarrow import *
from digit_scroll_animation import *
from graph import *

class DistTracker():
    def __init__(self):
        self.distances = []

    def insertDist(self, newDist):
        self.distances.append(newDist)

    def adjustAllDistances(self, toAdd):
        for i,dist in enumerate(self.distances):
            self.distances[i] = self.distances[i] + toAdd

    def grundyNumber(self):
        result = 0
        for dist in self.distances:
            result = result ^ dist
        return result

    def clear(self):
        self.distances = []

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
            newNode = g.create_node(node_left, frame_height * proportion_of_frame_height_to_use / 2 - (arrow_total_length + arrow_dist_above_node), { 'radius' : node_radius})
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
        coin_mobjects.append(create_coin_for_node(nodes[0], RED))
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

        elif dist_tracker_implementation == 'partial_grid':
            grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE) # TODO
            CELL_HEIGHT = 0.75
            CELL_WIDTH = 0.75
            partial_grid = VGroup()
            num_in_row = 2
            partial_grid.item_at = []
            red_one_zone_for_row = []
            partial_grid.addition_coins_for_row = []

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
                red_one_zone = Rectangle(color="#ff0000",fill_opacity=1, fill_color="#ff0000", width = CELL_WIDTH * red_one_zone_num_cells, height = CELL_HEIGHT)
                red_one_zone_width = CELL_WIDTH * red_one_zone_num_cells
                print("num_in_row:", num_in_row)
                # Reposition so that top-left is at origin.
                red_one_zone.shift([+red_one_zone_width / 2, +CELL_HEIGHT / 2, 0])
                # Place in the correct place.
                red_one_zone.shift([grid_topleft_x + red_one_zone_width, grid_topleft_y - row * CELL_HEIGHT, 0])
                print("red_one_zone: get_center():", red_one_zone.get_center(), " CELL_WIDTH:", CELL_WIDTH)
                red_one_zone_for_row.append(red_one_zone)

                num_in_row = num_in_row * 2

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
                plus_object.align_on_border(RIGHT)
                plus_object.set_y(partial_grid.item_at[bitNum][0].get_y())
                to_add_mobject.next_to(plus_object, LEFT)
                if is_last:
                    plus_object.set_opacity(0)

                to_add_mobjects.append(to_add_mobject)
                plus_sign_mobjects.append(plus_object)

            addition_line_y = to_add_mobject[-1].get_y() - to_add_mobject[-1].get_height() / 2 - MED_SMALL_BUFF
            addition_line_left = to_add_mobject[0].get_x() - to_add_mobject[0].get_width()
            addition_line_right = to_add_mobject[0].get_x() + to_add_mobject[0].get_width()
            addition_line_mobject = Line([addition_line_left, addition_line_y, 0], [addition_line_right, addition_line_y, 0], color = BLACK)

            grundy_value_mobject.scale(text_scale_factor)
            grundy_value_mobject.next_to(addition_line_mobject, DOWN)

            partial_grid.to_add_mobjects = to_add_mobjects
            partial_grid.powers_of_two_mobjects = powers_of_two_mobjects

            partial_grid.coin_mobjects_for_row = []
            for bitNum in range(0, NUM_BITS):
                partial_grid.coin_mobjects_for_row.append([])

            scene.play(*intro_anims, Write(partial_grid), *map(Write, powers_of_two_mobjects), *map(Write, red_one_zone_for_row), *map(Write, to_add_mobjects), *map(Write, plus_sign_mobjects), Write(addition_line_mobject), Write(grundy_value_mobject))
            #self.play(ApplyMethod(thing.shift, LEFT * CELL_WIDTH),
            #          Transform(grid, grid.copy()))
            #self.play(ApplyMethod(thing.shift, RIGHT * CELL_WIDTH),
            #          Transform(grid, grid.copy()))
            ##self.play(WiggleOutThenIn(grid))



        # Ok - move through the node chain!
        distTracker = DistTracker()
        tracked_distance_mobjects = []
        for node_index,node in enumerate(nodes):
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

                elif dist_tracker_implementation == 'partial_grid':
                    coin_mobjects_to_transform = []

                    for bitNum in range(0, NUM_BITS):
                        coin_copy = coin_mobject_for_node.copy()
                        coin_target_mobject = coin_mobject_for_node.copy()
                        coin_target_mobject.move_to(partial_grid.item_at[bitNum][0])
                        coin_copy.target = coin_target_mobject

                        coin_copy.addition_representative = coin_copy.copy()
                        coin_copy.addition_representative.set_opacity(0)
                        coin_copy.addition_representative.cross = Cross(coin_copy.addition_representative)

                    
                        coin_mobjects_to_transform.append(coin_copy)
                        # TODO - handle the case where there are more than one coins
                        # at the left of the row.
                        for existing_coin in partial_grid.coin_mobjects_for_row[bitNum]:
                            if existing_coin.pos_in_row == 0:
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
                        coin_copy.pos_in_row = 0

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

                        addPlusOneAnims.append(GrowFromCenter(plusOne))

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
                elif dist_tracker_implementation == 'partial_grid':
                    scene.play(*intro_animations)

                    coin_advance_anims = []
                    coin_addition_representative_transforms = []
                    addition_representatives_to_hide = []
                    addition_representatives_to_show = []
                    for bitNum in range(0, NUM_BITS):
                        print("row:", bitNum, " num coins in row:", len(partial_grid.coin_mobjects_for_row[bitNum]))
                        num_in_row = len(partial_grid.item_at[bitNum])

                        for coin in partial_grid.coin_mobjects_for_row[bitNum]:
                            was_in_red = coin.pos_in_row >= num_in_row / 2

                            x_offset_to_new_pos = 0

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
                            in_red = coin.pos_in_row >= num_in_row / 2

                            
                            if not was_in_red and in_red:
                                coin.addition_representative.become(coin.copy())
                                coin.addition_representative.set_opacity(0)
                                coin.addition_representative.cross.set_opacity(0)
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
                                target_cross.set_opacity(0)
                                target_cross.move_to(new.get_center())
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
                            target_cross.set_opacity(0)
                            target_cross.move_to(new.get_center())
                            coin_addition_representative_transforms.append(Transform(coin_addition_representative, new))
                            coin_addition_representative_transforms.append(Transform(coin_addition_representative.cross, target_cross))
                            x = x + coin_radius * 2 + SMALL_BUFF
                            print(" new at:", new.get_x())

                    # Advance the coins.
                    scene.play(*coin_advance_anims)

                    # Update the addition representative coins.
                    for m in addition_representatives_to_show:
                        m.set_opacity(1)
                    scene.play(*coin_addition_representative_transforms)
                    for m in addition_representatives_to_hide:
                        m.set_opacity(0)

                    # Add crosses and update all numbers.

                    number_update_anims = []
                    for bitNum in range(0, NUM_BITS):
                        addition_coins_for_row = partial_grid.addition_coins_for_row[bitNum].copy()
                        while len(addition_coins_for_row) >= 2:
                            addition_coins_for_row[0].cross.set_opacity(1)
                            addition_coins_for_row[0].cross.move_to(addition_coins_for_row[0].get_center())
                            number_update_anims.append(Write(addition_coins_for_row[0].cross))
                            addition_coins_for_row.pop(0)

                            addition_coins_for_row[0].cross.set_opacity(1)
                            addition_coins_for_row[0].cross.move_to(addition_coins_for_row[0].get_center())
                            number_update_anims.append(Write(addition_coins_for_row[0].cross))
                            addition_coins_for_row.pop(0)
                        if addition_coins_for_row:
                            target_cross = addition_coins_for_row[0].cross
                            number_update_anims.append(Transform(addition_coins_for_row[0].cross,target_cross))

                    scene.play(*number_update_anims)


                    scene.play(*outtro_animations)


class MoveCoins2Editorial_1_collect_and_propagate_along_node_chain_left_to_right_naive(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, right_to_left = False)
        
class MoveCoins2Editorial_2_collect_and_propagate_along_node_chain_right_to_left_naive(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, right_to_left = True)

class MoveCoins2Editorial_3_collect_and_propagate_along_node_chain_left_to_right_partial_grid(SSJGZScene):

    def construct(self):
        super().construct()
        do_collect_and_propagate_along_node_chain_naive(self, dist_tracker_implementation = 'partial_grid', right_to_left = False)
        

