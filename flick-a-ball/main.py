import pygame
import math

pygame.font.init()
pygame.mixer.init()
W, H = 1000, 900
WIN = pygame.display.set_mode((W, H))
COLORS = {
    "BLACK": (0, 0, 0),
    "WHITE": (255, 255, 255),
    "GREEN": (0, 255, 0),
    "RED": (255, 0, 0),
    "YELLOW": (255, 255, 0),
}
pygame.display.set_caption("Math test")
ping_pong = pygame.mixer.Sound("ping.wav")
bounce = pygame.mixer.Sound("bounce.wav")
HW, HH = W // 2, H // 2


class Ball:
    def __init__(self, x, y, r, col):
        self.x = x
        self.y = y
        self.r = r
        self.col = col

    def draw(self, window):
        pygame.draw.circle(window, self.col, (self.x, self.y), self.r)


clock = pygame.time.Clock()
run = True
ball = Ball(HW, HH, 15, COLORS["WHITE"])

dist = 0
dx, dy = 0, 0
velocity = 0
friction = 0.98


def redrawWin():
    WIN.fill(COLORS["BLACK"])
    ball.draw(WIN)
    if clicked:
        coordx = abs(ball.x - mx)
        coordy = abs(ball.y - my)
        line_col = COLORS["GREEN"]
        if coordx > 200 or coordy > 200:
            line_col = COLORS["YELLOW"]
        if coordx > 350 or coordy > 350:
            line_col = COLORS["RED"]
        pygame.draw.line(WIN, line_col, (ball.x, ball.y), mpos, 3)
    pygame.display.update()


clicked = False

while run:
    m = pygame.mouse.get_pressed()
    mpos = pygame.mouse.get_pos()
    mx, my = mpos
    ball_clicked = False

    if m[0] and not dist:
        if mx in range(round(ball.x) - ball.r, round(ball.x) + ball.r) and my in range(
            round(ball.y) - ball.r, round(ball.y) + ball.r
        ):
            ball_clicked = True
        if ball_clicked:
            clicked = True

    if not m[0] and clicked:
        clicked = False
        pygame.mixer.Sound.play(ping_pong)
        rad = math.atan2(my - ball.y, mx - ball.x)
        dist = math.hypot(my - ball.y, mx - ball.x)
        dx = math.cos(rad) * dist * 0.25
        dy = math.sin(rad) * dist * 0.25

    if not m[0] and dist:
        dist -= 1
        if dx > 0:
            if ball.x - dx <= ball.r:
                dx *= -1
                pygame.mixer.Sound.play(bounce)
        else:
            if ball.x - dx >= W - ball.r:
                dx *= -1
                pygame.mixer.Sound.play(bounce)

        if dy > 0:
            if ball.y - dy <= ball.r:
                dy *= -1
                pygame.mixer.Sound.play(bounce)

        else:
            if ball.y - dy >= H - ball.r:
                dy *= -1
                pygame.mixer.Sound.play(bounce)

        ball.x -= dx
        ball.y -= dy

        dx *= friction
        dy *= friction
        if not round(dx) and not round(dy):
            dist = 0

    redrawWin()
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False

    clock.tick(60)
