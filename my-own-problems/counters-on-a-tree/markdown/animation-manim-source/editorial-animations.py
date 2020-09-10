from manimlib.imports import *

from ssjgz_scene import *
from downarrow import *
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

class MoveCoins2Editorial_1_collect_and_propagate_along_node_chain_left_to_right_naive(SSJGZScene):
    def construct(self):
        super().construct()

        # Graph, coins, and the Grundy numbers beneath the coins.
        num_nodes = 8
        frame_width = self.camera.get_frame_width()
        frame_height = self.camera.get_frame_height()

        node_diameter_to_gap_ratio = 1.3
        proportion_of_frame_width_to_use = 0.9
        proportion_of_frame_height_to_use = 0.9
        node_diameter = (frame_width * proportion_of_frame_width_to_use) / (num_nodes + (num_nodes - 1) / node_diameter_to_gap_ratio)
        gap_between_nodes = node_diameter / node_diameter_to_gap_ratio
        node_radius = node_diameter / 2

        arrow_total_length = node_diameter
        arrow_dist_above_node = node_diameter / 5

        node_left = -frame_width * proportion_of_frame_width_to_use  / 2
        g = Graph(self, globals()["Node"], globals()["NodeMObject"])
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

        self.play(g.create_animation(run_time = 2))

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
        self.play(GrowFromCenter(arrow))

        coin_radius = node_radius / 2

        def create_coin_for_node(node, coin_colour):
            coin_mobject = Circle(color=BLACK, fill_opacity = 1, fill_color = coin_colour, radius = coin_radius)
            node.config['coin_mobject'] = coin_mobject
            coin_mobject.move_to([node.config['center_x'], node.config['center_y'], 0])
            return coin_mobject


        coin_mobjects = []
        coin_mobjects.append(create_coin_for_node(nodes[0], RED))
        coin_mobjects.append(create_coin_for_node(nodes[1], GREEN))
        coin_mobjects.append(create_coin_for_node(nodes[4], BLUE))
        self.play(GrowFromCenter(coin_mobjects[0]),
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

        self.play(*grundy_tex_mobjects_appear_anims)

        grundy_tex_mobjects_change_to_0_anims = []
        for grundy_tex in grundy_tex_mobjects:
            target_0_grundy_tex = TexMobject('0', color = grundy_node_tex_colour)
            target_0_grundy_tex.move_to(grundy_tex)
            grundy_tex_mobjects_change_to_0_anims.append(Transform(grundy_tex, target_0_grundy_tex))

        self.play(*grundy_tex_mobjects_change_to_0_anims)

        # DistTracker display.
        disttracker_text_scale = 1.5
        disttracker_top_y = 0
        disttracker_title_display = TexMobject(r'DistTracker^\text{TM}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        disttracker_title_display.scale(disttracker_text_scale)
        disttracker_title_display.align_on_border(LEFT)
        disttracker_title_display.set_y(disttracker_top_y + disttracker_title_display.get_height() / 2)

        # Grundy number display.
        grundy_number_label = TexMobject(r'grundy number =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        grundy_number_label.scale(disttracker_text_scale)
        grundy_number_label.next_to(disttracker_title_display, 2 * DOWN)
        grundy_number_label.set_x(0)

        grundy_number_second_equals = None

        disttracker_grundy_color = BLUE
        grundy_value_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = BLUE)
        grundy_value_mobject.scale(disttracker_text_scale)
        grundy_value_mobject.next_to(grundy_number_label, RIGHT)

        self.play(AnimationGroup(Write(disttracker_title_display), Write(grundy_number_label), Write(grundy_value_mobject)))

        # Ok - move through the node chain!
        distTracker = DistTracker()
        tracked_distance_mobjects = []
        for node in nodes:
            # Propagate.
            propagate_text = TexMobject('propagate', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            propagate_text.scale(disttracker_text_scale)
            propagate_text.align_on_border(RIGHT)
            propagate_text.set_y(disttracker_title_display.get_y())
            self.play(FadeInFrom(propagate_text, DOWN))

            grundy_value_to_move = grundy_value_mobject.copy()
            node_grundy = node.config['grundy_text']
            node_grundy_target = node_grundy.copy()
            node_grundy_target.shift(DOWN)
            xor_symbol = TexMobject(r'\oplus', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            xor_symbol.next_to(node_grundy_target, RIGHT)
            grundy_value_destination = TexMobject(str(distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = disttracker_grundy_color)
            grundy_value_destination.next_to(xor_symbol, RIGHT)
            equal_symbol = TexMobject(r'=', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            equal_symbol.next_to(grundy_value_destination)

            self.play(
                      Transform(node_grundy, node_grundy_target),
                      Transform(grundy_value_to_move, grundy_value_destination),
                      FadeIn(xor_symbol),
                      FadeIn(equal_symbol))

            new_grundy_tex = TexMobject(str(node.config['grundy_number'] ^ distTracker.grundyNumber()), colour = BLACK, fill_opacity = 1, fill_color = grundy_node_tex_colour)
            new_grundy_tex.next_to(equal_symbol, RIGHT)
            self.play(FadeIn(new_grundy_tex))

            node_grundy_to_fade_out = node_grundy.copy()
            node_grundy.become(new_grundy_tex)
            self.remove(new_grundy_tex)
            node_grundy_target = node_grundy.copy()
            node_grundy_target.next_to(g.mobject_for_node[node], DOWN)

            self.play(FadeOutAndShift(propagate_text, UP),
                      FadeOut(node_grundy_to_fade_out),
                      FadeOut(grundy_value_to_move),
                      FadeOut(xor_symbol),
                      FadeOut(equal_symbol),
                      Transform(node_grundy, node_grundy_target))

            # Collect.
            collect_text = TexMobject('collect', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
            collect_text.scale(disttracker_text_scale)
            collect_text.align_on_border(RIGHT)
            collect_text.set_y(disttracker_title_display.get_y())
            self.play(FadeInFrom(collect_text, DOWN))

            coin_mobject_for_node = node.config['coin_mobject']
            if not coin_mobject_for_node:
                cross_out = Cross(collect_text)
                self.play(Write(cross_out))
                self.play(FadeOutAndShift(collect_text, UP),
                          FadeOutAndShift(cross_out, UP))
            else:
                shift_elements_to_right_by = 0
                new_tracked_distance_mobject = TexMobject(r'0', colour = BLACK, fill_opacity = 1, fill_color = coin_mobject_for_node.get_fill_color())
                new_tracked_distance_mobject.scale(disttracker_text_scale)
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

                self.play(*animations)

                tracked_distance_mobjects.append(new_tracked_distance_mobject)


                    

                






