import pygame
from queue import PriorityQueue
import math

W, H = 1000, 1000
HW, HH = W // 2, H // 2
window = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()
FPS = 60

RED = (255, 0, 0)
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 100, 255)
PURPLE = (255, 0, 255)
GREEN = (0, 255, 0)
BLUEGREEN = (0, 255, 255)
YELLOW = (255, 255, 0)


class Spot:
    def __init__(self, row, col, width, total_rows):
        self.row = row
        self.col = col
        self.x = row * width
        self.y = col * width
        self.color = WHITE
        self.width = width
        self.total_rows = total_rows

    def addEdges(self, grid):
        self.edges = []
        if (
            self.row < self.total_rows - 1
            and grid[self.row + 1][self.col].color != BLACK
        ):  # down
            self.edges.append(grid[self.row + 1][self.col])
        if self.row > 0 and grid[self.row - 1][self.col].color != BLACK:
            self.edges.append(grid[self.row - 1][self.col])
        if (
            self.col < self.total_rows - 1
            and grid[self.row][self.col + 1].color != BLACK
        ):
            self.edges.append(grid[self.row][self.col + 1])
        if self.col > 0 and grid[self.row][self.col - 1].color != BLACK:
            self.edges.append(grid[self.row][self.col - 1])

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.width))


def get_clicked_pos(pos, rows, width):
    gap = width // rows
    y, x = pos

    row = y // gap
    col = x // gap

    return row, col


def make_grid(rows, width):
    grid = []
    gap = width // rows
    for i in range(rows):
        grid.append([])
        for j in range(rows):
            spot = Spot(i, j, gap, rows)
            grid[i].append(spot)
    return grid


def draw_grid(rows, width):
    gap = width // rows
    for i in range(rows):
        pygame.draw.line(window, (0, 0, 0), (0, i * gap), (width, i * gap))
        for j in range(rows):
            pygame.draw.line(window, (0, 0, 0), (j * gap, 0), (j * gap, width))


def redraw(grid, rows, width):
    window.fill(BLACK)
    for row in grid:
        for spot in row:
            spot.draw(window)

    draw_grid(rows, width)
    pygame.display.update()


def create_path(draw, path, current):
    while current in path:
        if (current.row, current.col) != start:
            current.color = YELLOW
        current = path[current]
        draw()


def get_score(start, end):
    x1, y1 = start
    x2, y2 = end
    return abs(x1 - x2) + abs(y1 - y2)


def a_star(draw):
    row, col = start
    count = 0
    path = {}
    queue = PriorityQueue()
    queue.put((0, count, grid[row][col]))
    g_score = {spot: float("inf") for row in grid for spot in row}
    g_score[grid[row][col]] = 0
    f_score = {spot: float("inf") for row in grid for spot in row}
    f_score[grid[row][col]] = get_score(start, end)

    visited = {grid[row][col]}

    while not queue.empty():

        current = queue.get()[2]
        visited.remove(current)

        for edge in current.edges:
            if edge.color == RED:
                create_path(draw, path, current)
                return True

            temp_g = g_score[current] + 1
            if temp_g < g_score[edge]:
                path[edge] = current
                g_score[edge] = temp_g
                f_score[edge] = temp_g + get_score((edge.row, edge.col), end)
                if edge not in visited and edge.color != RED:
                    count += 1
                    queue.put((f_score[edge], count, edge))
                    visited.add(edge)
                    edge.color = PURPLE
        draw()

        if (current.row, current.col) != start:
            current.color = BLUEGREEN
    return False


def depth(draw):
    path = {}
    row, col = start
    stack = [grid[row][col]]
    visited = {grid[row][col]}
    while len(stack) > 0:
        current = stack.pop()
        for i in current.edges:
            if i.color == RED:
                current.color = BLUEGREEN
                create_path(draw, path, current)
                return True
            if i not in visited:
                path[i] = current
                stack.append(i)
                visited.add(i)
                i.color = PURPLE
        draw()
        if (current.row, current.col) != start:
            current.color = BLUEGREEN


def breadth(draw):
    row, col = start
    queue = [grid[row][col]]
    path = {}
    visited = {grid[row][col]}
    while queue:
        current = queue.pop(0)
        for i in current.edges:
            if i.color == RED:
                create_path(draw, path, current)
                return True
            if i not in visited:
                path[i] = current
                queue.append(i)
                visited.add(i)
                i.color = PURPLE

        draw()
        if (current.row, current.col) != start:
            current.color = BLUEGREEN
    return False


run = True
rows = 30
grid = make_grid(rows, W)
s = grid[rows // 2 - 1][rows // 2]
e = grid[rows // 2][rows // 2]
start = (s.row, s.col)
end = (e.row, e.col)
pressed = False
s.color = BLUE
e.color = RED

while run:
    redraw(grid, rows, W)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        m = pygame.mouse.get_pressed()
        m_pos = pygame.mouse.get_pos()

        if m[0]:
            row, col = get_clicked_pos(m_pos, rows, W)
            spot = grid[row][col]
            if spot.color == BLUE:
                spot.color = WHITE
                start = None
            elif not start and spot.color != RED:
                spot.color = BLUE
                start = (spot.row, spot.col)
            elif spot.color == RED:
                spot.color = WHITE
                end = None
            elif not end and spot.color != BLUE:
                spot.color = RED
                end = (spot.row, spot.col)
            elif spot.color == WHITE:
                spot.color = BLACK

        if m[2]:
            row, col = get_clicked_pos(m_pos, rows, W)
            spot = grid[row][col]
            if spot.color == BLACK:
                spot.color = WHITE
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE and start and end and not pressed:
                pressed = True
                for row in grid:
                    for spot in row:
                        spot.addEdges(grid)
                breadth(lambda: redraw(grid, rows, W))

            if event.key == pygame.K_RETURN:
                pressed = False
                grid = make_grid(rows, W)
                s = grid[rows // 2 - 1][rows // 2]
                e = grid[rows // 2][rows // 2]
                start = (s.row, s.col)
                end = (e.row, e.col)
                s.color = BLUE
                e.color = RED

    clock.tick(FPS)
