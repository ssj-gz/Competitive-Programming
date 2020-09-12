from manimlib.imports import *

from ssjgz_scene import *
from graph import *

class MoveTheCoinsCreatingTestEditorial_1_schematic_for_finding_all_non_descendents_up_to_height(SSJGZScene):

    def find_descendents_at_height(self, graph, root, ignore_nodes = []):
        nodes_at_height = []
        nodes_at_height.append([root])
        ignore_nodes = ignore_nodes.copy()

        self.find_descendents_at_height_aux(graph, root, 1, nodes_at_height, ignore_nodes)
        print("num layers:", len(nodes_at_height))


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
        nodes_at_height[height].append(root)
        print("Appending a node at height:", height)
        ignore_nodes.append(root)
        for child in children:
            self.find_descendents_at_height_aux(graph, child, height + 1, nodes_at_height, ignore_nodes)

    def construct(self):
        super().construct()

        RED = "#ff0000"
        BLUE = "#0000ff"
        YELLOW = "#ffff00"

        g = Graph(self, globals()["Node"], globals()["NodeMObject"])
        node_radius = 0.15
        root = g.create_node(0, 5, { 'radius' : node_radius, 'fill_color' : BLUE})

        last_node_layer = [root]
        nodes_at_height = [[root]]
        tree_height = 11
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

        for node in g.nodes:
            node.config['is_descendent'] = False

        self.play(g.create_animation())

        node_to_reparent_height = 4
        node_to_reparent = nodes_at_height[node_to_reparent_height][2]
        up_to_height = 8

        height_label = TexMobject('h', color = BLACK, fill_color = BLACK)
        height_label.next_to(g.mobject_for_node[nodes_at_height[up_to_height][-1]], RIGHT)


        self.play(create_change_node_color_anim(node_to_reparent, YELLOW))
        self.play(Write(height_label))

        descendents = self.find_descendents_at_height(g, node_to_reparent, ignore_nodes = [node_to_reparent.config['parent']])

        highlight_descendents_anims = []
        for descendents_at_height in descendents:
            print("num descendents_at_height:", len(descendents_at_height))
            highlight_descendents_anims = []
            for node in descendents_at_height:
                node.config['is_descendent'] = True
                highlight_descendents_anims.append(create_change_node_color_anim(node, YELLOW))
            self.play(*highlight_descendents_anims)

        #self.play(*highlight_descendents_anims)

        show_non_descendents_up_to_height_anims = []
        for height in range(0, up_to_height + 1):
            for node in nodes_at_height[height]:
                if not node.config['is_descendent']:
                    show_non_descendents_up_to_height_anims.append(create_change_node_color_anim(node, RED))
        
        self.play(*show_non_descendents_up_to_height_anims)

        def create_polygon_around_nodes(node_layers, dist_from_node, colour):
            num_layers = len(node_layers)
            node_layers = node_layers.copy()
            for layer in node_layers:
                layer.sort(key = lambda n: n.config['center_x'])
                print("num in layer:", len(layer))
            points = []
            for top_node in node_layers[0]:
                above_node_point = [top_node.config['center_x'], top_node.config['center_y'] + dist_from_node + node_radius, 0]
                print("above_node_point:", above_node_point)
                points.append(above_node_point)
            for layer in node_layers:
                right_node = layer[-1]
                to_right_of_node_point = [right_node.config['center_x'] + dist_from_node + node_radius, right_node.config['center_y'], 0]
                print("to_right_of_node_point:", to_right_of_node_point)
                points.append(to_right_of_node_point)
            for bottom_node in reversed(node_layers[-1]):
                below_node_point = [bottom_node.config['center_x'], bottom_node.config['center_y'] - dist_from_node - node_radius, 0]
                print("below_node_point:", below_node_point)
                points.append(below_node_point)
            for layer in reversed(node_layers):
                left_node = layer[0]
                to_left_of_node_point = [left_node.config['center_x'] - dist_from_node - node_radius, left_node.config['center_y'], 0]
                print("to_left_of_node_point:", to_left_of_node_point)
                points.append(to_left_of_node_point)

            print("points:", points)
            return Polygon(*points, fill_opacity = 0, color = colour)

        blah = create_polygon_around_nodes(descendents, 0.1, RED)
        self.play(Write(blah))








                    








