import pygame as pg

pg.init()

kletkas_states = {'nullState': '', 'krestikState': 'X', 'nolikState': 'O'}

class Game:
    def __init__(self, screen):
        self.worldOffset = 100
        self.font = pg.font.SysFont(None, 80)
        self.screen = screen
        self.kletkas = []
        self.game_over = False
        self.winner = None
        self.last_ckick_state = kletkas_states["nolikState"]

        self.countOfLines = 0
        self.n = 0

    def add_pole(self, countOfLines=3, n=3):
        self.countOfLines = countOfLines
        self.n = n

        self.kletkas = []

        self.worldOffset = 300 // countOfLines
        self.font = pg.font.SysFont(None, 300 // countOfLines)
        for y in range(countOfLines):
            for x in range(countOfLines):
                pos = (x * self.worldOffset, y * self.worldOffset)
                rect = pg.Rect(pos, (self.worldOffset, self.worldOffset))
                self.kletkas.append([rect, kletkas_states['nullState']])

    def draw_pole(self):
        for rect, state in self.kletkas:
            pg.draw.rect(self.screen, (0, 0, 0), rect, 2)
            if state:
                text = self.font.render(str(state), True, (0, 0, 0))
                text_rect = text.get_rect(center=rect.center)
                self.screen.blit(text, text_rect)

    def check_events(self):
        for event in pg.event.get():
            if event.type == pg.QUIT:
                return False
            if event.type == pg.MOUSEBUTTONDOWN:
                if self.game_over:
                    if hasattr(self, 'restart_rect') and self.restart_rect.collidepoint(event.pos):
                        self.reset_game()
                        self.game_over = False
                        self.winner = None
                    return True
                for kletka in self.kletkas:
                    if kletka[0].collidepoint(event.pos) and kletka[1] == kletkas_states['nullState']:
                        if self.last_ckick_state == kletkas_states["nolikState"]:
                            self.last_ckick_state = kletkas_states["krestikState"]
                            kletka[1] = kletkas_states['krestikState']
                        else:
                            self.last_ckick_state = kletkas_states["nolikState"]
                            kletka[1] = kletkas_states['nolikState']
                        self.winner = self.check_win()
                        if self.winner:
                            self.game_over = True
        return True

    def get_win_coords(self, size, win_len):
        win_coords = []

        for r in range(size):
            for c in range(size - win_len + 1):
                start = r * size + c
                win_coords.append(tuple(range(start, start + win_len)))

        for r in range(size - win_len + 1):
            for c in range(size):
                start = r * size + c
                win_coords.append(tuple(range(start, start + win_len * size, size)))

        for r in range(size - win_len + 1):
            for c in range(size - win_len + 1):
                start = r * size + c
                win_coords.append(tuple(range(start, start + win_len * (size + 1), size + 1)))

        for r in range(size - win_len + 1):
            for c in range(win_len - 1, size):
                start = r * size + c
                win_coords.append(tuple(range(start, start + win_len * (size - 1), size - 1)))
        return win_coords

    def check_win(self):
        states = [k[1] for k in self.kletkas]
        win_coords = self.get_win_coords(self.countOfLines, self.n)

        for r in win_coords:
            line_values = [states[i] for i in r]
            if line_values[0] != kletkas_states['nullState'] and all(x == line_values[0] for x in line_values):
                return line_values[0]

        if kletkas_states['nullState'] not in states:
            return 'Ничья'
        return None

    def show_end_screen(self, winner):
        overlay = pg.Surface((300, 300))
        overlay.set_alpha(180)
        overlay.fill((255, 255, 255))
        self.screen.blit(overlay, (0, 0))
        win_text = "Draw!" if winner == 'Ничья' else f"{winner} Wins!"
        text_surf = self.font.render(win_text, True, (0, 0, 0))
        text_rect = text_surf.get_rect(center=(150, 100))
        self.screen.blit(text_surf, text_rect)
        self.restart_rect = pg.Rect(75, 180, 150, 50)
        pg.draw.rect(self.screen, (100, 200, 100), self.restart_rect)
        btn_text = pg.font.SysFont(None, 40).render("Restart", True, (255, 255, 255))
        btn_rect = btn_text.get_rect(center=self.restart_rect.center)
        self.screen.blit(btn_text, btn_rect)

    def reset_game(self):
        self.add_pole(self.countOfLines)
        self.last_ckick_state = kletkas_states["nolikState"]

background_colour = (234, 212, 252)
screen = pg.display.set_mode((300, 300))
game = Game(screen)
game.add_pole(3, 3)

running = True
while running:
    running = game.check_events()

    screen.fill(background_colour)
    game.draw_pole()
    if game.game_over:
        game.show_end_screen(game.winner)
    pg.display.flip()

pg.quit()
