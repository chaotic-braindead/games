import pygame
import math

W, H = 1000, 900
HW, HH = W // 2, H // 2

window = pygame.display.set_mode((W, H))
pygame.display.set_caption("Grab-a-ball")

COLORS = {
    "BLACK": (0, 0, 0),
    "WHITE": (255, 255, 255),
    "GREEN": (0, 255, 0),
    "RED": (255, 0, 0),
    "YELLOW": (255, 255, 0),
}


class Ball:
    def __init__(self, x, y, r, color):
        self.x = x
        self.y = y
        self.r = r
        self.color = color

    def draw(self, window):
        pygame.draw.circle(window, self.color, (self.x, self.y), self.r)


ball = Ball(HW, HH, 20, COLORS["WHITE"])
clock = pygame.time.Clock()
run = True
friction = 0.80
gravity = 1
isXmoving = False
moveX, moveY = 0, 0
vel = 2
prev = 0


def redraw():
    window.fill(COLORS["BLACK"])
    ball.draw(window)
    pygame.display.update()


while run:
    m = pygame.mouse.get_pressed()
    mpos = pygame.mouse.get_pos()
    mx, my = mpos
    ballX = round(ball.x)
    ballY = round(ball.y)
    bounce = vel < 0
    if not m[0]:
        if ball.y + vel + moveY > H - ball.r:
            vel *= -1 * friction
            bounce = True
            moveY *= -1
        else:
            vel += gravity

        ball.y += vel
        ball.x += moveX * 0.5
        ball.y += moveY * 0.5
        moveX *= 0.99
        moveY *= 0.99

        if ball.x + moveX < ball.r or ball.x + ball.r + moveX > W:
            moveX *= -1

        if ball.y <= ball.r:
            ball.y *= -1
            vel *= -1
            moveY *= -1
    else:
        moveX, moveY = 0, 0
        vel = 2
        rad = math.atan2(my - ball.y, mx - ball.x)
        distance = math.hypot(mx - ball.x, my - ball.y)
        dx = math.cos(rad) * distance
        dy = math.sin(rad) * distance
        x = ball.x
        y = ball.y
        prev = y
        coord = (x + dx, y + dy)
        if coord != (x, y):
            isXmoving = True
            moveX = dx
            moveY = dy
        else:
            isXmoving = False

        ball.x += dx
        ball.y += dy

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
    redraw()
    clock.tick(60)
