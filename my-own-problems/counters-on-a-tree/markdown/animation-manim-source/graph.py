from manimlib.imports import *

class Node:
    def __init__(self, node_id, config):
        self.node_id = node_id
        self.config = config

class Edge:
    def __init__(self, edge_id, start_node, end_node, config):
        self.edge_id = edge_id
        self.start_node = start_node
        self.end_node = end_node
        self.config = config

    def get_start_and_end_point(self, alpha):
        sx = self.start_node.config['center_x']
        sy = self.start_node.config['center_y']
        ex = self.end_node.config['center_x']
        ey = self.end_node.config['center_y']
        print("get_start_and_end_point: sx: ", sx, " sy: ", sy, " ex:", ex, " ey: ", ey)

        length = math.sqrt((ex - sx) * (ex - sx) + (ey - sy) * (ey - sy))

        if length == 0:
            return [[sx, sy], [ex, ey]]

        start_node_radius = self.start_node.config['radius']
        end_node_radius = self.end_node.config['radius']

        t = start_node_radius / length

        line_start_x = sx + t * (ex - sx)
        line_start_y = sy + t * (ey - sy)

        t = (length - end_node_radius) / length

        line_end_x = sx + t * (ex - sx)
        line_end_y = sy + t * (ey - sy)

        return [[line_start_x, line_start_y], [line_end_x, line_end_y]]

class GraphNodeMoveAnim(Animation):
    def __init__(self, graph, scene):
        self.nodes_move_items = []
        self.node_objects_to_move = []
        self.graph = graph
        self.scene = scene
        dummyMObject = Circle()
        dummyMObject.set_opacity(0)

        self.alpha = 0
        blah = None
        dummy_animations = []
        for node in graph.nodes:
            if node in graph.mobject_for_node:
                node_mobject = graph.mobject_for_node[node]
                print("node pos:", node.config['center_x'], node.config['center_y'], " node_mobject pos:", node_mobject.config['center_x'], node_mobject.config['center_y'])
                if node.config['center_x'] != node_mobject.config['center_x'] or node.config['center_y'] != node_mobject.config['center_y'] or node.config['radius'] != node_mobject.config['radius']:
                       print("Appending: node with value: ", node.config['value'])
                       self.nodes_move_items.append([node, node_mobject, [node.config['center_x'], node.config['center_y'], node.config['radius']], [node_mobject.config['center_x'], node_mobject.config['center_y'], node_mobject.config['radius']]])
                       self.node_objects_to_move.append(node_mobject)

        Animation.__init__(self, dummyMObject)
        print("# node_objects_to_move", len(self.node_objects_to_move))

    def interpolate_mobject(self, alpha):
        print ("interpolate_mobject alpha:", alpha, " # to move: ", len(self.nodes_move_items))
        for node_move_item in self.nodes_move_items:
            node_to_move = node_move_item[0]
            node_mobject_to_move = node_move_item[1]
            start_x = node_move_item[2][0]
            start_y = node_move_item[2][1]
            start_radius = node_move_item[2][2]
            end_x = node_move_item[3][0]
            end_y = node_move_item[3][1]
            end_radius = node_move_item[3][2]

            new_x = start_x + (1 - alpha) * (end_x - start_x)
            new_y = start_y + (1 - alpha) * (end_y - start_y)
            new_radius = start_radius + (1 - alpha) * (end_radius - start_radius)

            node_to_move.config['center_x'] = new_x
            node_to_move.config['center_y'] = new_y

            node_mobject_to_move.scale(new_radius / node_mobject_to_move.config['radius'])

            node_to_move.config['radius'] = new_radius

            print("newx/y for node:", node_to_move.config['value'], new_x, new_y, " alpha:", alpha, " self: ", repr(self))
            node_mobject_to_move.move_to([new_x, new_y, 0])
            node_mobject_to_move.config['center_x'] = new_x
            node_mobject_to_move.config['center_y'] = new_y
            node_mobject_to_move.config['radius'] = new_radius
            print("Moved mobject ", repr(node_mobject_to_move), " for node:", node_to_move.config['value'], " to ", node_mobject_to_move.get_center())

            for edge in self.graph.edges:
                if edge.start_node == node_to_move or edge.end_node == node_to_move:
                    start_and_end_point = edge.get_start_and_end_point(alpha)
                    edge_object = self.graph.mobject_for_edge[edge]
                    edge_object.set_start_and_end_attrs([start_and_end_point[0][0], start_and_end_point[0][1], 0], [start_and_end_point[1][0], start_and_end_point[1][1], 0])
                    edge_object.generate_points()

class NodeMObject(VMobject):
    def __init__(self, node_id, config):
        VMobject.__init__(self)
        self.node_id = node_id
        self.config = config

        circle_radius = config['radius']
        circle = Circle(radius=circle_radius,color=BLACK,fill_opacity=1, fill_color=WHITE)
        self.add(circle)
        if 'value' in config:
            self.value = config['value']
            value_mobject = TexMobject(str(self.value))
            value_mobject.set_color(BLACK)
            self.value_mobject = value_mobject
            self.add(value_mobject)

    def copy(self):
        return self.deepcopy()

class Graph:
    def __init__(self, scene, nodeClass, nodeMObjectClass):
        self.scene = scene
        self.next_node_id = 0
        self.next_edge_id = 0
        self.nodes = []
        self.edges = []
        self.mobject_for_node = {}
        self.mobject_for_edge = {}
        self.nodeMObjectClass = nodeMObjectClass
        self.nodeClass = nodeClass

    def create_node(self, center_x, center_y, config):
        config['center_x'] = center_x
        config['center_y'] = center_y
        newNode = self.nodeClass(node_id = self.next_node_id, config = config)
        self.nodes.append(newNode)
        self.next_node_id = self.next_node_id + 1
        return newNode

    def create_mobject_for_node(self, node):
        assert(node not in self.mobject_for_node)
        newNodeMObject = self.nodeMObjectClass(node_id = node.node_id, config = node.config.copy())
        self.mobject_for_node[node] = newNodeMObject
        return newNodeMObject


    def create_edge(self, start_node, end_node, config):
        newEdge = Edge(self.next_edge_id, start_node, end_node, config)
        self.edges.append(newEdge)
        self.next_edge_id = self.next_edge_id + 1
        return newEdge

    def get_edge(self, node1, node2):
        edge_to_remove = None
        for edge in self.edges:
            if (edge.start_node == node1 and edge.end_node == node2) or (edge.start_node == node2 and edge.end_node == node1):
                return edge

    def layout_tree(self, rootNode, gap_between_parent_and_child = 0.5, min_gap_between_siblings = 0.2, ignore_node_list = []):
        x_offset_from_parent_for_node = {}
        ignore_node_list_copy = ignore_node_list.copy()
        self.get_subtree_width(rootNode, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list_copy, x_offset_from_parent_for_node)

        ignore_node_list_copy = ignore_node_list.copy()

        self.set_node_positions(rootNode, None, x_offset_from_parent_for_node, gap_between_parent_and_child, ignore_node_list_copy)

    def layout_bst(self, rootNode, gap_between_parent_and_child = 0.5, min_gap_between_siblings = 0.2):
        x_offset_from_parent_for_node = {}
        ignore_node_list_copy = []
        self.get_bst_subtree_width(rootNode, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list_copy, x_offset_from_parent_for_node)

        ignore_node_list_copy = []

        self.set_node_positions(rootNode, None, x_offset_from_parent_for_node, gap_between_parent_and_child, ignore_node_list_copy)

    def get_bst_subtree_width(self, rootNode, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list, x_offset_from_parent_for_node):
        leftChild = rootNode.leftChild
        rightChild = rootNode.rightChild

        assert(rootNode.config['radius'])
        extentToLeft = rootNode.config['radius']
        extentToRight = rootNode.config['radius']

        if not leftChild and not rightChild:
            return [extentToLeft, extentToRight]

        ignore_node_list.append(rootNode)

        leftChildExtents = None
        if leftChild:
            leftChildExtents = self.get_bst_subtree_width(leftChild, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list, x_offset_from_parent_for_node)

        rightChildExtents = None
        if rightChild:
            rightChildExtents = self.get_bst_subtree_width(rightChild, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list, x_offset_from_parent_for_node)

        # Place them such that the right edge of the left subtree is vertically aligned with rootNode's center,
        # and the left edge of the right subtree is vertically aligned with rootNode's center.
        if leftChild:
            x_offset_from_parent_for_node[leftChild] = -(leftChildExtents[1] + min_gap_between_siblings / 2)
            extentToLeft = max(extentToLeft, leftChildExtents[0] + leftChildExtents[1] + min_gap_between_siblings / 2)
        if rightChild:
            x_offset_from_parent_for_node[rightChild] = +rightChildExtents[0] + min_gap_between_siblings / 2
            extentToRight = max(extentToRight, rightChildExtents[0] + rightChildExtents[1] + min_gap_between_siblings / 2)

        return [extentToLeft, extentToRight]

    def get_subtree_width(self, rootNode, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list, x_offset_from_parent_for_node):
        children = []
        for edge in self.edges:
            neighbour = None
            if rootNode == edge.start_node:
                neighbour = edge.end_node
            elif rootNode == edge.end_node:
                neighbour = edge.start_node
            if neighbour and neighbour not in ignore_node_list:
                children.append(neighbour)

        ignore_node_list.append(rootNode)

        child_subtree_widths = []
        for child in children:
            child_subtree_widths.append(self.get_subtree_width(child, gap_between_parent_and_child, min_gap_between_siblings, ignore_node_list, x_offset_from_parent_for_node))

        print("sum(child_subtree_widths):", sum(child_subtree_widths), " min_gap_between_siblings:", min_gap_between_siblings)
        total_children_width = 0
        if children:
            total_children_width = sum(child_subtree_widths) + min_gap_between_siblings * (len(children) - 1)
        print("total_children_width:", total_children_width)

        x_offset_from_parent = -total_children_width / 2
        for i,child in enumerate(children):
            print("child i", i, " child_subtree_widths[i]:", child_subtree_widths[i])
            x_offset_from_parent_for_node[child] = x_offset_from_parent + child_subtree_widths[i] / 2
            x_offset_from_parent = x_offset_from_parent + child_subtree_widths[i] + min_gap_between_siblings

        assert(rootNode.config['radius'])
        subtree_width = max(rootNode.config['radius'] * 2, total_children_width)

        return subtree_width

    def set_node_positions(self, rootNode, parentNode, x_offset_from_parent_for_node, gap_between_parent_and_child, ignore_node_list):
        print("set_node_positions: rootNode id:", rootNode.node_id)
        children = []
        for edge in self.edges:
            neighbour = None
            if rootNode == edge.start_node:
                neighbour = edge.end_node
            elif rootNode == edge.end_node:
                neighbour = edge.start_node
            if neighbour and neighbour not in ignore_node_list:
                children.append(neighbour)

        if parentNode:
            print("node id", rootNode.node_id, " center_x/y was ", rootNode.config['center_x'], rootNode.config['center_y'])
            print("parent center_x/y was ", parentNode.config['center_x'], parentNode.config['center_y'], " gap_between_parent_and_child:" , gap_between_parent_and_child)
            rootNode.config['center_x'] = parentNode.config['center_x'] + x_offset_from_parent_for_node[rootNode]
            rootNode.config['center_y'] = parentNode.config['center_y'] - (gap_between_parent_and_child + rootNode.config['radius'] + parentNode.config['radius'])
            print("set node id", rootNode.node_id, " center_x/y to: ", rootNode.config['center_x'], "|" , rootNode.config['center_y'])
            print("node id", rootNode.node_id, " x_offset_from_parent_for_node: ", x_offset_from_parent_for_node[rootNode])
            print("gap_between_parent_and_child:" , gap_between_parent_and_child)

        ignore_node_list.append(rootNode)
        for child in children:
            self.set_node_positions(child, rootNode, x_offset_from_parent_for_node, gap_between_parent_and_child, ignore_node_list)

    def find_descendents_at_height(self, root, ignore_node_list = []):
        nodes_at_height = []
        ignore_node_list_copy = ignore_node_list.copy()
        ignore_node_list.append(root)

        self.find_descendents_at_height_aux(root, 0, nodes_at_height, ignore_node_list_copy)
        print("num layers:", len(nodes_at_height))


        return nodes_at_height

    def find_descendents_at_height_aux(self, root, height, nodes_at_height, ignore_node_list = []):
        children = []
        for edge in self.edges:
            neighbour = None
            if root == edge.start_node:
                neighbour = edge.end_node
            elif root == edge.end_node:
                neighbour = edge.start_node
            if neighbour and neighbour not in ignore_node_list:
                children.append(neighbour)

        if len(nodes_at_height) == height:
            nodes_at_height.append([])
        nodes_at_height[height].append(root)
        print("Appending a node at height:", height)
        ignore_node_list.append(root)
        for child in children:
            self.find_descendents_at_height_aux(child, height + 1, nodes_at_height, ignore_node_list)


    def create_animation(self, run_time = 1):
        animations = []
        mobjects_to_move = []
        for node in self.nodes:
            if node not in self.mobject_for_node:
                newNodeMObject = self.create_mobject_for_node(node)
                print("Created newNodeMObject:", repr(newNodeMObject))
                newNodeMObject.shift([node.config['center_x'], node.config['center_y'], 0])
                self.scene.add(newNodeMObject)
                animations.append(GrowFromCenter(newNodeMObject, run_time = run_time))
            mobjects_to_move.append(self.mobject_for_node[node])

        for edge in self.edges:
            if edge not in self.mobject_for_edge:
                line_start_and_end = edge.get_start_and_end_point(1)
                newEdgeMObject = Line([line_start_and_end[0][0], line_start_and_end[0][1], 0],
                                      [line_start_and_end[1][0], line_start_and_end[1][1], 0])
                edge.get_start_and_end_point(alpha = 1)
                if 'colour' in edge.config:
                    newEdgeMObject.set_color(edge.config.colour)
                else:
                    newEdgeMObject.set_color(BLACK)
                self.mobject_for_edge[edge] = newEdgeMObject
                self.scene.add(newEdgeMObject)
                self.scene.bring_to_back(newEdgeMObject)
                animations.append(GrowFromCenter(newEdgeMObject, run_time = run_time))
            mobjects_to_move.append(self.mobject_for_edge[edge])

        animations.append(GraphNodeMoveAnim(self, self.scene))

        # Create a "dummy" Animation for all graph node and edges mobjects - without these,
        # the scene won't render the changes to the nodes and edges as we make them.
        for mobject in mobjects_to_move:
            animations.append(Animation(mobject, run_time = run_time))

        return AnimationGroup(*animations)
