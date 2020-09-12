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
        GREEN = "#00ff00"
        YELLOW = "#ffff00"
        GREY = "#999999"
        MAGENTA = "#aa00aa"

        A_COLOUR = BLUE
        B_COLOUR = ORANGE
        AD_COLOUR = "#009900"
        DH_COLOUR = BLACK
        D_COLOUR = YELLOW

        g = Graph(self, globals()["Node"], globals()["NodeMObject"])
        node_radius = 0.15
        root = g.create_node(0, 5, { 'radius' : node_radius, 'fill_color' : GREY})

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
                    new_child = g.create_node(0, 0, {'radius' : node_radius, 'fill_color' : GREY})
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

        create_dividing_lines_anims = []
        for height in range(0, tree_height - 1):
            line_y = nodes_at_height[height][0].config['center_y'] + node_radius + gap_between_parent_and_child / 2
            line = Line([-self.camera.get_frame_width(), line_y, 0],
                        [+self.camera.get_frame_width(), line_y, 0], color = '#DDDDDD')
            self.bring_to_back(line)

            create_dividing_lines_anims.append(Write(line))
                        

        self.play(g.create_animation(), *create_dividing_lines_anims)

        node_to_reparent_height = 4
        node_to_reparent = nodes_at_height[node_to_reparent_height][2]
        up_to_height = 8

        height_label = TexMobject('h', color = BLACK, fill_color = BLACK)
        height_label.next_to(g.mobject_for_node[nodes_at_height[up_to_height][-1]], RIGHT)


        node_to_reparent_mobject = g.mobject_for_node[node_to_reparent]
        self.bring_to_front(node_to_reparent_mobject)
        node_to_reparent_label = TexMobject(r'\textit{nodeToReparent}', color = BLACK, fill_color = BLACK)
        node_to_reparent_label.next_to(node_to_reparent_mobject, LEFT)

        self.play(Write(node_to_reparent_label), WiggleOutThenIn(node_to_reparent_mobject, scale_value = 2))
        self.play(Write(Cross(node_to_reparent_mobject)))
        self.play(Write(height_label), FadeOut(node_to_reparent_label))
        self.play(WiggleOutThenIn(height_label, scale_value = 2))

        descendents = self.find_descendents_at_height(g, node_to_reparent, ignore_nodes = [node_to_reparent.config['parent']])

        highlight_descendents_anims = []
        for descendents_at_height in descendents:
            for node in descendents_at_height:
                node.config['is_descendent'] = True
                highlight_descendents_anims.append(create_change_node_color_anim(node, YELLOW))

        d_label = TexMobject(r'D', color = BLACK, fill_color = D_COLOUR)
        d_label.next_to(node_to_reparent_mobject, DOWN)

        self.play(*highlight_descendents_anims, Write(d_label))

        show_non_descendents_up_to_height_anims = []
        for height in range(0, up_to_height + 1):
            for node in nodes_at_height[height]:
                if not node.config['is_descendent']:
                    show_non_descendents_up_to_height_anims.append(create_change_node_color_anim(node, RED))
        
        want_all_the_text = TexMobject(r'\text{Want all the }', color = BLACK, fill_color = BLACK)
        want_all_the_text.align_on_border(LEFT)
        want_all_the_text.shift(3 * DOWN)
        red_text = TexMobject(r'\text{red }', color = BLACK, fill_color = RED)
        red_text.next_to(want_all_the_text)
        nodes_text = TexMobject(r'\text{nodes}.', color = BLACK, fill_color = BLACK)
        nodes_text.next_to(red_text)

        self.play(*show_non_descendents_up_to_height_anims, Write(want_all_the_text), Write(red_text), Write(nodes_text))

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
            return Polygon(*points, fill_opacity = 0, color = colour, stroke_width = 5)

        def create_rectangle_around_nodes(node_layers, dist_from_node, colour):
            rectangle_top = node_layers[0][0].config['center_y'] + node_radius + dist_from_node
            rectangle_bottom = node_layers[-1][0].config['center_y'] - node_radius - dist_from_node

            rectangle_left = 1000
            rectangle_right = -1000
            for layer in node_layers:
                rectangle_left = min(rectangle_left, layer[0].config['center_x'] - node_radius - dist_from_node)
                rectangle_right = max(rectangle_right, layer[-1].config['center_x'] + node_radius + dist_from_node)

            rectangle = Rectangle(width = rectangle_right - rectangle_left, height = rectangle_top - rectangle_bottom, color = colour, stroke_width = 5)
            # Shift so that top-left is at origin.
            rectangle.shift(rectangle.get_width() / 2 * RIGHT, rectangle.get_height() / 2 * DOWN)
            # Shift to correct position.
            rectangle.shift(rectangle_left * RIGHT + rectangle_top * UP)
            return rectangle



        self.wait(2)

        ARect = create_rectangle_around_nodes(nodes_at_height[0:up_to_height+1], 0.15, A_COLOUR)
        ALabel = TexMobject('A', color = BLACK, fill_color = A_COLOUR)
        ALabel.next_to(ARect, LEFT)


        red_text_2 = TexMobject(r'\text{Red }', color = BLACK, fill_color = RED)
        red_text_2.next_to(want_all_the_text, DOWN)
        red_text_2.align_on_border(LEFT)
        nodes_equal_text = TexMobject(r'\text{nodes}=', color = BLACK, fill_color = BLACK)
        a_text = TexMobject(r'A', color = BLACK, fill_color = A_COLOUR)
        minus_text = TexMobject(r'-', color = BLACK, fill_color = BLACK)
        ad_text = TexMobject(r'\textit{AD}', color = BLACK, fill_color = AD_COLOUR)

        new_text_objects = [red_text_2, nodes_equal_text, a_text, minus_text, ad_text]
        for i in range(1, len(new_text_objects)):
            new_text_objects[i].next_to(new_text_objects[i - 1], RIGHT)
            previous_mobject = new_text_objects[i]

        AD = create_polygon_around_nodes(descendents[0:up_to_height - node_to_reparent_height + 2], 0.1, AD_COLOUR)
        ADLabel = TexMobject('AD', color = BLACK, fill_color = AD_COLOUR)
        ADLabel.next_to(AD, LEFT)

        self.play(Write(ARect), Write(ALabel), Write(AD), Write(ADLabel), *map(Write, new_text_objects))

        self.wait(2)

        ad_text_2 = TexMobject(r'\textit{AD}', color = BLACK, fill_color = AD_COLOUR)
        ad_text_2.next_to(red_text_2, DOWN)
        ad_text_2.align_on_border(LEFT)
        equal_text = TexMobject(r'=', color = BLACK, fill_color = BLACK)
        d_text = TexMobject(r'\textit{D}', color = BLACK, fill_color = D_COLOUR)
        minus_text_2 = TexMobject(r'-', color = BLACK, fill_color = BLACK)
        dh_text = TexMobject(r'\textit{DH}', color = BLACK, fill_color = DH_COLOUR)

        new_text_objects = [ad_text_2, equal_text, d_text, minus_text_2, dh_text]
        for i in range(1, len(new_text_objects)):
            new_text_objects[i].next_to(new_text_objects[i - 1], RIGHT)
            previous_mobject = new_text_objects[i]

        DH = create_polygon_around_nodes(descendents[len(descendents) - node_to_reparent_height + 2:len(descendents)], 0.1, DH_COLOUR)
        DHLabel = TexMobject('DH', color = BLACK, fill_color = DH_COLOUR)
        DHLabel.next_to(DH, DOWN)
        self.play(Write(DH), Write(DHLabel), *map(Write, new_text_objects))

        self.wait(2)

        dh_text_2 = TexMobject(r'\textit{DH}', color = BLACK, fill_color = DH_COLOUR)
        dh_text_2.next_to(ad_text_2, DOWN)
        dh_text_2.align_on_border(LEFT)
        equal_text_2 = TexMobject(r'=', color = BLACK, fill_color = BLACK)
        dots_text = TexMobject(r'... ', color = BLACK, fill_color = BLACK)
        descendents_of_text = TexMobject(r'\textit{ proper}\text{ descendents of }', color = BLACK, fill_color = BLACK)

        last_descendent_row_mobjects = []
        for node in descendents[up_to_height - node_to_reparent_height + 1]:
            last_descendent_row_mobjects.append(g.mobject_for_node[node])
            self.bring_to_front(g.mobject_for_node[node])

        last_descendent_row_anims = []
        for node_mobject in last_descendent_row_mobjects:
            last_descendent_row_anims.append(WiggleOutThenIn(node_mobject, scale_value = 2))

        new_text_objects = [dh_text_2, equal_text_2, descendents_of_text, dots_text]
        for i in range(1, len(new_text_objects)):
            new_text_objects[i].next_to(new_text_objects[i - 1], RIGHT)
            previous_mobject = new_text_objects[i]

        self.play(*map(Write, new_text_objects))
        self.play(*last_descendent_row_anims, WiggleOutThenIn(dots_text, scale_value = 1.5))

        last_descendent_row_anims = []
        for node_mobject in last_descendent_row_mobjects:
            node_copy = Circle(radius = node_radius, color = BLACK, fill_color = MAGENTA, fill_opacity = 1) 
            node_copy.move_to(node_mobject.get_center())
            last_descendent_row_anims.append(Transform(node_mobject, node_copy))

        dots_target = Circle(radius = node_radius, color = BLACK, fill_color = MAGENTA, fill_opacity = 1) 
        dots_target.move_to(dots_text.get_center())
        dots_target.scale(descendents_of_text.get_height() / dots_target.get_height())

        self.play(Transform(dots_text, dots_target), *last_descendent_row_anims)
        self.save_thumbnail()

        self.wait(2)
