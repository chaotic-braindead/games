import pygame
import math

pygame.font.init()
W, H = 1000, 900
window = pygame.display.set_mode((W, H))
pygame.display.set_caption("Angle")

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

    def draw(self, win):

        pygame.draw.circle(win, self.color, (self.x, self.y), self.r)


class Line:
    def __init__(self, x, y, color, end1, end2):
        self.x = x
        self.y = y
        self.color = color
        self.end1 = end1
        self.end2 = end2

    def draw(self, win):
        pygame.draw.line(win, self.color, (self.x, self.y), (self.end1, self.end2))


ball = Ball(W // 2, H // 2, 20, COLORS["WHITE"])
clock = pygame.time.Clock()
anglex = 0.0
angley = 0.0


def redraw():
    window.fill(COLORS["BLACK"])
    ball.draw(window)

    pygame.draw.line(window, COLORS["YELLOW"], (ball.x, ball.y), (0, 0))
    pygame.draw.line(window, COLORS["GREEN"], (0, 0), (0, dy))
    pygame.draw.line(window, COLORS["RED"], (ball.x, ball.y), (0, dy))
    # circular motion
    # pygame.draw.circle(
    #     window,
    #     COLORS["GREEN"],
    #     (75 * math.cos(anglex) + 500, 75 * math.sin(angley) + 450),
    #     50,
    # )

    # line1.draw(window)
    # line2.draw(window)
    # line3.draw(window)

    pygame.display.update()


distance = 0
run = True
speed = 5

if __name__ == "__main__":
    while run:
        # line movement
        rad = math.atan2(ball.y - 0, ball.x - 0)
        dist = math.hypot(ball.y - 0, ball.x - 0)
        dy = dist * math.sin(rad)
        dx = math.hypot(dy, dist)
        deg = math.degrees(rad)
        arc_length = (deg / 360) * 2 * 3.1416 * ball.r
        anglex += 0.1
        angley += 0.1

        # ball movement
        mpos = pygame.mouse.get_pos()
        mx, my = mpos
        ball_rad = math.atan2(my - ball.y, mx - ball.x)
        distance = math.hypot(my - ball.y, mx - ball.x)
        velx = math.cos(ball_rad) * distance * 0.05
        vely = math.sin(ball_rad) * distance * 0.05

        if distance:
            ball.x += velx
            ball.y += vely
            dist -= 1

            velx *= 0.98
            vely *= 0.98

        ball.x += math.cos(anglex) * 15
        ball.y += math.sin(angley) * 15

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
        redraw()
        clock.tick(60)
