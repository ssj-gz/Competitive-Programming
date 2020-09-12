from manimlib.imports import *

from ssjgz_scene import *
from graph import *

class MoveTheCoinsCreatingTestEditorial_1_schematic_for_finding_all_non_descendents_up_to_height(SSJGZScene):

    def find_descendents_at_height(self, graph, root, ignore_nodes = []):
        nodes_at_height = []
        nodes_at_height.append([root])
        ignore_nodes = ignore_nodes.copy()

        self.find_descendents_at_height_aux(graph, root, 1, nodes_at_height, ignore_nodes)

        return nodes_at_height
        

    def find_descendents_at_height_aux(self, graph, root, height, nodes_at_height, ignore_nodes = []):
        children = []
        for edge in graph.edges:
            neighbour = None
            if root == edge.start_node:
                neighbour = edge.end_node
            elif root == edge.end_node:
                neighbour = edge.start_node
            if neighbour and neighbour not in ignore_nodes:
                children.append(neighbour)

        if len(nodes_at_height) == height:
            nodes_at_height.append([])
        nodes_at_height[height].extend(children)
        for child in children:
            ignore_nodes.append(child)
            self.find_descendents_at_height_aux(graph, child, height + 1, nodes_at_height, ignore_nodes)

    def construct(self):
        super().construct()

        BLUE = "#0000ff"
        YELLOW = "#ffff00"

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
                    new_child.config['parent'] = node
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

        def create_change_node_color_anim(node, new_color):
            node_mobject = g.mobject_for_node[node]
            config = node.config
            config['fill_color'] = new_color

            new_node_mobject = NodeMObject(0, config)
            new_node_mobject.move_to(node_mobject.get_center())
            return Transform(node_mobject, new_node_mobject)


        self.play(g.create_animation())
        node_to_reparent_height = 4
        node_to_reparent = nodes_at_height[node_to_reparent_height][2]
        self.play(create_change_node_color_anim(node_to_reparent, YELLOW))

        descendents = self.find_descendents_at_height(g, node_to_reparent, ignore_nodes = [node_to_reparent.config['parent']])

        blah = []
        print("descendents heights:", len(descendents))
        for descendents_at_height in descendents:
            print("blee:", str(descendents_at_height))
            for node in descendents_at_height:
                blah.append(create_change_node_color_anim(node, YELLOW))

        print("Blah:", repr(blah))
        self.play(*blah)



                    








