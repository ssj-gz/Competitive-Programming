from manimlib.imports import *

from ssjgz_scene import *
from graph import *

class MoveTheCoinsCreatingTestEditorial_1_schematic_for_finding_all_non_descendents_up_to_height(SSJGZScene):

    def construct(self):
        super().construct()

        BLUE = "#0000ff"

        g = Graph(self, globals()["Node"], globals()["NodeMObject"])
        node_radius = 0.1
        root = g.create_node(0, 5, { 'radius' : node_radius, 'fill_color' : BLUE})

        last_node_layer = [root]
        nodes_at_height = [[root]]
        tree_height = 10
        for height in range(1, tree_height):
            next_node_layer = []

            for node in last_node_layer:
                if height == 1:
                    num_children = 2
                else:
                    rand = random.randint(0, 10)
                    if rand > 9:
                        num_children = 3
                    elif rand > 6:
                        num_children = 2
                    else:
                        num_children = 1

                for i in range(0, num_children):
                    new_child = g.create_node(0, 0, {'radius' : node_radius, 'fill_color' : BLUE})
                    g.create_edge(node, new_child, {})
                    next_node_layer.append(new_child)

            print("height:", height, " # nodes in layer:", len(next_node_layer))
            nodes_at_height.append(next_node_layer)
            last_node_layer = next_node_layer

        gap_between_parent_and_child = 0.3
        min_gap_between_siblings = 0.1
        blah = False
        if blah:
            last_layer_width = (len(nodes_at_height[tree_height - 1]) - 1) * min_gap_between_siblings + len(nodes_at_height[tree_height - 1]) * 2 * node_radius
            
            for height in range(1, tree_height):
                num_in_layer = len(nodes_at_height[height])
                for node in nodes_at_height[height]:
                    node.config['center_y'] = nodes_at_height[height - 1][0].config['center_y'] - 2 * node_radius - gap_between_parent_and_child

                width_of_layer = (height / tree_height) * last_layer_width
                gap_between_nodes = (width_of_layer - num_in_layer * 2 * node_radius) / (num_in_layer - 1)
                node_x = -width_of_layer / 2

                for node in nodes_at_height[height]:
                    node.config['center_x'] = node_x
                    node_x = node_x + 2 * node_radius + gap_between_nodes
        else:
            g.layout_tree(root, gap_between_parent_and_child, min_gap_between_siblings)


        self.play(g.create_animation())

                    








