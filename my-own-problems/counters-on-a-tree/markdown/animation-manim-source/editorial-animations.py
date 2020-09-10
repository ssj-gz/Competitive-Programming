from manimlib.imports import *

from ssjgz_scene import *
from downarrow import *
from graph import *

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
            if previous_node:
                g.create_edge(previous_node, newNode, {})
            node_left = node_left + node_diameter + gap_between_nodes
            previous_node = newNode
            nodes.append(newNode)

        self.play(g.create_animation(run_time = 2))

        arrow = create_down_arrow(
                arrow_tip = [nodes[0].config['center_x'], nodes[0].config['center_y'], 0] + (node_diameter * 2 / 3) * UP,
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
        disttracker_top_y = 0
        disttracker_title_display = TexMobject(r'DistTracker^\text{TM}', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        disttracker_title_display.align_on_border(LEFT)
        disttracker_title_display.set_y(disttracker_top_y + disttracker_title_display.get_height() / 2)

        # Grundy number display.
        grundy_number_label = TexMobject(r'grundy number =', colour = BLACK, fill_opacity = 1, fill_color = BLACK)
        grundy_number_label.next_to(disttracker_title_display, DOWN)
        grundy_number_label.set_x(0)

        self.play(AnimationGroup(Write(disttracker_title_display), Write(grundy_number_label)))



