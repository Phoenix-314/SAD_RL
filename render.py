import pygame as pg
from pygame.locals import QUIT, KEYDOWN, K_BACKSPACE, K_KP_ENTER, K_RETURN, K_TAB
from copy import deepcopy
from pathlib import Path
from typing import List, Tuple

from SADenv.states import *
from SADenv.ents import *
from SADenv.reversedIDs import *

# The renderer was mostly written by copilot since it does not necessarily need to be accurate
class Render:
    def __init__(self, size=(1920, 1080)):
        pg.init()
        self.size = size
        self.screen = pg.display.set_mode(size)
        self.screen.fill((255, 220, 220))
        pg.display.set_caption("Ent Renderer")
        pg.display.flip()
        self.font_cache = {}
        self.image_cache = {}
        self.image_dir = Path(__file__).resolve().parent / "imgs"
        self.card_size = (220, 120)
        self.side_square_size = 110
        self.console_height = 72
        self.console_prompt = ">>> "
        self.console_buffer = ""
        self.console_history: List[str] = []
        self.console_lines: List[str] = []
        self.EventInputFinished = pg.event.Event(pg.USEREVENT, attr1='EventInputFinished')

    def _side_type_to_image_name(self, side_type: int) -> str:
        mapping = {
            SideType.DAMAGE: "DAMAGE.png",
            SideType.SHIELD: "SHIELD.png",
            SideType.HEAL: "HEAL.png",
            SideType.HEALSHIELD: "HEALSHIELD.png",
            SideType.MANA: "MANA.png",
            SideType.BLANK: "BLANK.png",
            SideType.REROLL: "REROLL.png",
            SideType.REDIRECTINCOMING: "REDIRECTINCOMING.png",
            SideType.SUMMON: "SUMMON.png",
            SideType.RECHARGE: "RECHARGE.png",
            SideType.RESURRECT: "RESURRECT.png",
            SideType.ENCHANT: "ENCHANT.png",
            SideType.KILL: "KILL.png",
            SideType.UNDYING: "UNDYING.png",
            SideType.DODGE: "DODGE.png",
            SideType.SELF_DAMAGE: "SELF_DAMAGE.png",
            SideType.DAMAGE_ALL: "DAMAGE_ALL.png",
            SideType.DAMAGE_EVERYONE: "DAMAGE_EVERYONE.png",
            SideType.DAMAGE_FLANKING: "DAMAGE_FLANKING.png",
            SideType.SHIELD_ALL: "SHIELD_ALL.png",
            SideType.HEAL_ALL: "HEAL_ALL.png",
            SideType.STUN: "STUN.png",
        }
        return mapping.get(side_type, "BLANK.png")
    
    def _get_position(self, index: int, entities: List[Ent], is_enemy: bool = False, ent: Ent | None = None) -> Tuple[int, int]:
        width, height = self.screen.get_size()
        card_w, base_card_h = self.card_size
        margin_x = 40
        top_margin = 40
        bottom_margin = 180

        card_h = self._get_card_height(ent) if ent is not None else base_card_h
        card_h = max(card_h, 1)

        if not entities or len(entities) <= 1:
            y = (height - card_h) // 2
        else:
            total_size = sum(max(1, int(getattr(e, "size", 30) or 30)) for e in entities)
            if total_size <= 0:
                total_size = max(1, len(entities) * 30)

            prior_size = sum(max(1, int(getattr(e, "size", 30) or 30)) for e in entities[:index])
            available_height = max(0, height - top_margin - bottom_margin - card_h)
            y = top_margin + int(round((prior_size / total_size) * available_height))
            y = min(max(y, top_margin), max(top_margin, height - bottom_margin - card_h))

        x = margin_x if not is_enemy else width - margin_x - card_w
        if is_enemy and ent is not None:
            if not getattr(ent, "backrow", False):
                x = width - margin_x - card_w
            else:
                x = width - margin_x - card_w + 40
        return x, y

    def _resolve_reverse_id_name(self, value: int, mapping: dict, label: str) -> str:
        if value not in mapping:
            raise KeyError(f"Missing {label} mapping for value {value}")
        return mapping[value]

    def _get_render_scale(self, ent: Ent) -> float:
        size = getattr(ent, "size", 30) or 30
        return size / 30.0

    def _get_card_height(self, ent: Ent) -> int:
        return max(1, int(round(self.card_size[1] * self._get_render_scale(ent))))

    def _get_font(self, key: str, base_size: int):
        font = self.font_cache.get(key)
        if font is None:
            font = pg.font.SysFont(None, base_size)
            self.font_cache[key] = font
        return font

    def _draw_console_overlay(self):
        width, height = self.screen.get_size()
        width -= 400 # For state overlay, rhs
        panel_top = height - self.console_height
        panel_rect = (0, panel_top, width, self.console_height)

        overlay = pg.Surface((width, self.console_height), pg.SRCALPHA)
        overlay.fill((20, 20, 24, 230))
        self.screen.blit(overlay, (0, panel_top))
        pg.draw.rect(self.screen, (220, 220, 220), panel_rect, width=2)

        font = self._get_font("console", 22)
        history_font = self._get_font("console_history", 18)
        history_start_y = panel_top + 6
        history_lines = self.console_history[-2:]
        for index, line in enumerate(history_lines):
            text = history_font.render(line, True, (220, 220, 220))
            self.screen.blit(text, (12, history_start_y + index * 18))

        prompt_y = panel_top + self.console_height - 28
        prompt_text = font.render(f"{self.console_prompt}{self.console_buffer}", True, (255, 255, 255))
        self.screen.blit(prompt_text, (12, prompt_y))

    def _submit_console_line(self, line: str):
        self.console_lines.append(line)
        self.console_history.append(f"{self.console_prompt}{line}")
        if len(self.console_history) > 8:
            self.console_history = self.console_history[-8:]

    def addActionToConsoleHistory(self, act: int):
        self.console_history.append(f"Action: {act}")
        if len(self.console_history) > 8:
            self.console_history = self.console_history[-8:]

    def handle_console_event(self, event) -> str | None:
        if event.type != KEYDOWN:
            return None

        if event.key in (K_RETURN, K_KP_ENTER):
            line = self.console_buffer
            self.console_buffer = ""
            if line.strip() != "":
                self._submit_console_line(line)
            return line

        if event.key == K_BACKSPACE:
            self.console_buffer = self.console_buffer[:-1]
            return None

        if event.key == K_TAB:
            self.console_buffer += "    "
            return None

        text = getattr(event, "unicode", "")
        if text and text.isprintable():
            self.console_buffer += text

        return None

    def get_console_line(self) -> str | None:
        if self.console_lines:
            return self.console_lines.pop(0)
        return None

    def readConsoleLine(self, prompt: str = ">>> ", state: State = None):
        self.console_prompt = prompt
        self.console_buffer = ""

        clock = pg.time.Clock()
        self.render(state)
        while True:
            for event in pg.event.get():
                if event.type == QUIT:
                    pg.quit()
                    return None
                if event.type == KEYDOWN:
                    result = self.handle_console_event(event)
                    if result is not None:
                        return result

           
            self._draw_console_overlay()
            pg.display.flip()
            clock.tick(60)

    def _draw_shield_indicator(self, ent: Ent, x: int, y: int, card_w: int, card_h: int, isEnemy: bool, square_size: int):
        if ent.shield == 0:
            return

        triangle_w = 36
        triangle_h = 30
        fill_color = (120, 120, 120)
        pad = 8

        if isEnemy:
            tip_x = x + pad
            base_x = x + pad + triangle_w
            points = [(tip_x, y + card_h // 2), (base_x, y + pad), (base_x, y + card_h - pad)]
        else:
            tip_x = x + card_w - pad
            base_x = x + card_w - pad - triangle_w
            points = [(tip_x, y + card_h // 2), (base_x, y + pad), (base_x, y + card_h - pad)]

        if not isEnemy:
            points = [(p[0] + square_size + 64, p[1]) for p in points]
        else:
            points = [(p[0] - square_size - 64, p[1]) for p in points]

        pg.draw.polygon(self.screen, fill_color, points)

        font = self._get_font("shield", 24)
        text = font.render(str(ent.shield), True, (255, 255, 255))
        text_rect = text.get_rect(center=(sum(p[0] for p in points) // 3, sum(p[1] for p in points) // 3))
        self.screen.blit(text, text_rect)

    def _get_condition_icon_positions(self, x: int, y: int, card_w: int, card_h: int, count: int) -> List[Tuple[int, int]]:
        if count <= 0:
            return []

        icon_size = 16
        spacing = 4
        max_per_row = 6
        row_height = icon_size + spacing
        rows = (count + max_per_row - 1) // max_per_row

        positions = []
        start_x = x + 8
        start_y = y + card_h - 8 - (rows * row_height) + spacing

        for i in range(count):
            row = i // max_per_row
            col = i % max_per_row
            pos_x = start_x + col * (icon_size + spacing)
            pos_y = start_y + row * row_height
            positions.append((pos_x, pos_y))

        return positions

    def _draw_hp_bar(self, ent: Ent, x: int, y: int, card_w: int, card_h: int):
        max_hp = max(1, ent.maxHP)
        current_hp = max(0, ent.hp)
        if max_hp <= 15:
            cells_per_row = 5
            cell_size = 12
            spacing = 3
        else:
            cells_per_row = 10
            cell_size = 8
            spacing = 2

        rows = (max_hp + cells_per_row - 1) // cells_per_row
        bar_width = cells_per_row * cell_size + (cells_per_row - 1) * spacing
        start_x = x + (card_w - bar_width) // 2
        start_y = y + 8

        for i in range(max_hp):
            row = i // cells_per_row
            col = i % cells_per_row
            cell_x = start_x + col * (cell_size + spacing)
            cell_y = start_y + row * (cell_size + spacing)
            is_filled = i < current_hp
            if is_filled:
                pg.draw.rect(self.screen, (220, 40, 40), (cell_x, cell_y, cell_size, cell_size))
                if ent.armor != 0:
                    pg.draw.rect(self.screen, (180, 180, 180), (cell_x, cell_y, cell_size, cell_size), width=1)
                if ent.specialHP is not None and len(ent.specialHP) > i + 1 and ent.specialHP[i + 1]:
                    center_x = cell_x + cell_size // 2
                    center_y = cell_y + cell_size // 2
                    pg.draw.line(self.screen, (255, 255, 255), (center_x - 2, center_y), (center_x + 2, center_y), 2)
                    pg.draw.line(self.screen, (255, 255, 255), (center_x, center_y - 2), (center_x, center_y + 2), 2)
            else:
                pg.draw.rect(self.screen, (120, 80, 140), (cell_x, cell_y, cell_size, cell_size), width=1)

        if ent.hp < 0:
            font = self._get_font("negative_hp", 40)
            text = font.render(str(ent.hp), True, (220, 40, 40))
            text_rect = text.get_rect(center=(x + card_w // 2, start_y + 14))
            self.screen.blit(text, text_rect)

    def _draw_dice_layout(self, ent: Ent, x: int, y: int, card_w: int, card_h: int, isEnemy: bool, square_size: int):
        if not getattr(ent, "sides", None):
            return

        if ent.shield != 0:
            offset_x = square_size + 110
        else:
            offset_x = square_size + 80

        if isEnemy:
            center_x = x - offset_x - 80
        else:
            center_x = x + card_w + offset_x

        layout_y = y + (card_h - 90) // 2
        tile_size = min(card_h // 3.5, 40)
        spacing = 10

        positions = [
            (0, 0),
            (-tile_size - spacing, tile_size + spacing),
            (0, tile_size + spacing),
            (tile_size + spacing, tile_size + spacing),
            (2 * (tile_size + spacing), tile_size + spacing),
            (0, 2 * (tile_size + spacing)),
        ]

        for idx, pos in enumerate(positions):
            if idx == 0:
                side = ent.sides[0]
            elif idx <= 4:
                side = ent.sides[idx]
            else:
                side = ent.sides[5]

            if side is None:
                continue

            image_name = self._side_type_to_image_name(side.type)
            image_path = self.image_dir / image_name
            if not image_path.exists():
                continue

            image_key = (image_name, tile_size)
            image = self.image_cache.get(image_key)
            if image is None:
                image = pg.image.load(image_path)
                image = pg.transform.scale(image, (tile_size, tile_size))
                self.image_cache[image_key] = image

            draw_x = center_x + pos[0]
            draw_y = layout_y + pos[1]
            self.screen.blit(image, (draw_x, draw_y))

            if ent.petrified is not None and len(ent.petrified) > idx and ent.petrified[idx]:
                pg.draw.line(self.screen, (120, 120, 120), (draw_x + 4, draw_y + 4), (draw_x + tile_size - 4, draw_y + tile_size - 4), 4)
                pg.draw.line(self.screen, (120, 120, 120), (draw_x + tile_size - 4, draw_y + 4), (draw_x + 4, draw_y + tile_size - 4), 4)

            highlight = idx == ent.currentSideNum
            if highlight:
                pg.draw.rect(self.screen, (255, 255, 255), (draw_x - 2, draw_y - 2, tile_size + 4, tile_size + 4), width=3)

            font = self._get_font("side_value", 20)
            value_text = font.render(str(side.value), True, (255, 255, 255))
            value_rect = value_text.get_rect(bottomright=(draw_x + tile_size, draw_y + tile_size))
            self.screen.blit(value_text, value_rect)

    def _draw_side_preview(self, side: Side | None, x: int, y: int, size: int = 24):
        if side is None:
            return

        image_name = self._side_type_to_image_name(side.type)
        image_path = self.image_dir / image_name
        if image_path.exists():
            image = self.image_cache.get((image_name, size))
            if image is None:
                image = pg.image.load(image_path)
                image = pg.transform.scale(image, (size, size))
                self.image_cache[(image_name, size)] = image
            self.screen.blit(image, (x, y))

        font = self._get_font("state_side_value", 16)
        value_text = font.render(str(side.value), True, (255, 255, 255))
        value_rect = value_text.get_rect(bottomright=(x + size, y + size))
        self.screen.blit(value_text, value_rect)

    def _render_state_overlay(self, state: State):
        width, height = self.screen.get_size()
        font = self._get_font("state_text", 18)
        small_font = self._get_font("state_text_small", 16)
        line_height = 22
        left_x = 20
        left_y = height - self.console_height - 150

        spell_data_text = f"SpellData={state.spellData}"
        self.screen.blit(font.render(spell_data_text, True, (0, 0, 0)), (left_x, left_y))
        self.screen.blit(font.render(f"rerolls: {state.rerolls}", True, (0, 0, 0)), (left_x, left_y + line_height))

        right_x = left_x + 320
        self.screen.blit(font.render(f"turn: {state.turn}", True, (0, 0, 0)), (right_x, left_y))
        if state.lastTarget is not None:
            self.screen.blit(font.render(f"lastTarget: {state.lastTarget.sourceID}-{state.lastTarget.positionID}", True, (0, 0, 0)), (right_x, left_y + line_height))
        else:
            self.screen.blit(font.render(f"lastTarget: None", True, (0, 0, 0)), (right_x, left_y + line_height))
        self.screen.blit(font.render(f"prevLastStrikeVal: {state.prevLastStrikeVal}", True, (0, 0, 0)), (right_x, left_y + 2 * line_height))
        self.screen.blit(font.render(f"lastStrikeVal: {state.lastStrikeVal}", True, (0, 0, 0)), (right_x, left_y + 3 * line_height))
        self.screen.blit(font.render("lastDie:", True, (0, 0, 0)), (right_x, left_y + 4 * line_height))
        self._draw_side_preview(state.lastDie, right_x + 70, left_y + 4 * line_height - 2)

        mid_x = width // 2 - 220
        mid_y = height - 150
        self.screen.blit(font.render(f"mana: {state.mana}", True, (0, 0, 0)), (mid_x, mid_y))
        self.screen.blit(font.render(f"fortoldMana: {state.fortoldMana}", True, (0, 0, 0)), (mid_x, mid_y + line_height))

        mid_right_x = mid_x + 240
        self.screen.blit(font.render(f"level: {state.level}", True, (0, 0, 0)), (mid_right_x, mid_y))
        self.screen.blit(font.render(f"playerLevels: {state.playerLevels}", True, (0, 0, 0)), (mid_right_x, mid_y + line_height))

        top_x = width // 2 - 220
        top_y = 12
        imminent_names = []
        if state.imminentSummons:
            for ent in state.imminentSummons:
                if ent is None:
                    continue
                try:
                    imminent_names.append(self._resolve_reverse_id_name(ent.sourceID, entsReversedIDs, "sourceID"))
                except Exception:
                    imminent_names.append(str(ent.sourceID))
        
        self.screen.blit(font.render("imminentSummons:", True, (0, 0, 0)), (top_x, top_y))
        self.screen.blit(small_font.render(", ".join(imminent_names) if imminent_names else "[]", True, (0, 0, 0)), (top_x, top_y + line_height))

        reinf_x = top_x + 260
        reinf_names = []
        if state.reinforcements:
            for ent in state.reinforcements:
                if ent is None:
                    continue
                try:
                    reinf_names.append(self._resolve_reverse_id_name(ent.sourceID, entsReversedIDs, "sourceID"))
                except Exception:
                    reinf_names.append(str(ent.sourceID))
        self.screen.blit(font.render("reinforcements:", True, (0, 0, 0)), (reinf_x, top_y))
        self.screen.blit(small_font.render(", ".join(reinf_names) if reinf_names else "[]", True, (0, 0, 0)), (reinf_x, top_y + line_height))

        right_x = width - 360
        right_y = height - 150
        self.screen.blit(font.render(f"enemyDamageD: {state.enemyDamageD}", True, (0, 0, 0)), (right_x, right_y))
        self.screen.blit(font.render(f"hexiaD: {state.hexiaD}", True, (0, 0, 0)), (right_x, right_y + line_height))
        self.screen.blit(font.render(f"bansheeD: {state.bansheeD}", True, (0, 0, 0)), (right_x, right_y + 2 * line_height))
        self.screen.blit(font.render(f"brambleD: {state.brambleD}", True, (0, 0, 0)), (right_x, right_y + 3 * line_height))
        self.screen.blit(font.render(f"madnessD: {state.madnessD}", True, (0, 0, 0)), (right_x, right_y + 4 * line_height))
        self.screen.blit(font.render(f"handD: {state.handD}", True, (0, 0, 0)), (right_x, right_y + 5 * line_height))
        self.screen.blit(font.render(f"warchiefD: {state.warchiefD}", True, (0, 0, 0)), (right_x, right_y + 6 * line_height))
    
    def renderEnt(self, ent: Ent, position: Tuple[int, int], isEnemy: bool = False):
        x, y = position
        card_w, _ = self.card_size
        card_h = self._get_card_height(ent)
        square_size = int(self.side_square_size * 0.8* self._get_render_scale(ent))

        border_color = (80, 80, 80)
        fill_color = (0, 0, 0)
        square_x = x + card_w if not isEnemy else x - square_size
        square_y = y

        if ent.dodged or ent.intangibled:
            fill_color = (40, 40, 45)
        else:
            fill_color = (0, 0, 0)

        pg.draw.rect(self.screen, border_color, (x, y, card_w, card_h), width=3)
        pg.draw.rect(self.screen, fill_color, (x + 6, y + 6, card_w - 12, card_h - 12))

        if ent.dead:
            pg.draw.line(self.screen, (220, 40, 40), (x + 10, y + 10), (x + card_w - 10, y + card_h - 10), 10)
            pg.draw.line(self.screen, (220, 40, 40), (x + card_w - 10, y + 10), (x + 10, y + card_h - 10), 10)

        font = self._get_font("meta_text", 18)
        source_name = self._resolve_reverse_id_name(ent.sourceID, entsReversedIDs, "sourceID")
        source_text = font.render(f"sourceID: {ent.sourceID}, [{source_name}]", True, (0, 0, 0))
        self.screen.blit(source_text, (x + 6, max(4, y - 18)))

        if ent.spellID != -1:
            spell_name = self._resolve_reverse_id_name(ent.spellID, spellsReversedIDs, "spellID")
            spell_text = font.render(f"spellID: {ent.spellID}, [{spell_name}]", True, (0, 0, 0))
            self.screen.blit(spell_text, (x + 6, y + card_h + 4))

        square_border_color = (255, 255, 255)
        if ent.usedDie:
            square_border_color = (140, 0, 0)

        pg.draw.rect(self.screen, square_border_color, (square_x, square_y, square_size, square_size), width=3)
        pg.draw.rect(self.screen, fill_color, (square_x + 6, square_y + 6, square_size - 12, square_size - 12))

        
        stat_items = []
        if ent.weaken != 0:
            stat_items.append(("weaken", ent.weaken, (180, 80, 220), f"-{ent.weaken}"))
        if ent.smith != 0:
            stat_items.append(("smith", ent.smith, (180, 180, 180), f"+{ent.smith}"))
        if ent.boost != 0:
            stat_items.append(("boost", ent.boost, (220, 40, 40), f"+{ent.boost}"))
        if ent.permaboost != 0:
            stat_items.append(("permaboost", ent.permaboost, (255, 255, 255), f"+{ent.permaboost}"))

        if stat_items:
            font = self._get_font("square_stat", 18)
            icon_w = 18
            gap = 6
            total_w = len(stat_items) * icon_w + (len(stat_items) - 1) * gap
            start_x = square_x + max(4, (square_size - total_w) // 2)
            start_y = max(4, square_y - 20)
            for i, (_, _, color, label) in enumerate(stat_items):
                text = font.render(label, True, color)
                self.screen.blit(text, (start_x + i * (icon_w + gap), start_y))

        self._draw_hp_bar(ent, x, y, card_w, card_h)

        effect_positions = []
        effects = []
        if ent.poison != 0:
            effects.append(("poison", ent.poison, (0, 180, 0)))
        if ent.regen != 0:
            effects.append(("regen", ent.regen, (220, 40, 40)))
        if ent.vulnerable != 0:
            effects.append(("vulnerable", ent.vulnerable, (220, 140, 0)))
        if ent.incomingPoison != 0:
            effects.append(("incomingPoison", ent.incomingPoison, (120, 220, 0)))
        if ent.incomingDamage != 0:
            effects.append(("incomingDamage", ent.incomingDamage, (220, 220, 0)))

        if effects:
            icon_size = 16
            spacing = 4
            max_per_row = 3
            row_height = icon_size + spacing
            total_width = max_per_row * (icon_size + spacing) - spacing
            if isEnemy:
                start_x = x + card_w - 6 - total_width
            else:
                start_x = x + 10
            start_y = y + 14
            for i, effect in enumerate(effects):
                row = i // max_per_row
                col = i % max_per_row
                effect_positions.append((start_x + col * (icon_size + spacing), start_y + row * row_height))

        for (name, value, color), (icon_x, icon_y) in zip(effects, effect_positions):
            font = self._get_font(f"effect_{name}", 18)
            sign = "+" if value > 0 else ""
            if name == "poison":
                sign = "+" if value > 0 else "-"
            elif name in {"regen", "vulnerable", "incomingPoison", "incomingDamage"}:
                sign = "+" if value > 0 else ""
            label = f"{sign}{abs(value)}"
            text = font.render(label, True, color)
            self.screen.blit(text, (icon_x, icon_y))

        if ent.dodged:
            checker_size = 6
            for yy in range(0, 24, checker_size * 2):
                for xx in range(0, 24, checker_size * 2):
                    pg.draw.rect(self.screen, (140, 140, 140), (x + card_w - 34 + xx, y + 32 + yy, checker_size, checker_size))
                    pg.draw.rect(self.screen, (90, 90, 90), (x + card_w - 34 + xx + checker_size, y + 32 + yy + checker_size, checker_size, checker_size))

        if ent.intangibled:
            note_x = x + card_w - 18
            note_y = y + 32
            pg.draw.line(self.screen, (255, 255, 255), (note_x, note_y + 10), (note_x, note_y + 2), 2)
            pg.draw.line(self.screen, (255, 255, 255), (note_x + 2, note_y + 8), (note_x + 2, note_y), 2)
            pg.draw.circle(self.screen, (255, 255, 255), (note_x + 2, note_y + 12), 3, width=2)
            pg.draw.circle(self.screen, (255, 255, 255), (note_x + 2, note_y + 2), 3, width=2)

        current_side = ent.currentSide
        if current_side is not None:
            image_name = self._side_type_to_image_name(current_side.type)
            image_path = self.image_dir / image_name
            if image_path.exists():
                image = self.image_cache.get(image_name)
                if image is None:
                    image = pg.image.load(image_path)
                    image = pg.transform.scale(image, (square_size - 20, square_size - 20))
                    self.image_cache[image_name] = image

                image_rect = image.get_rect(center=(square_x + square_size // 2, square_y + square_size // 2))
                self.screen.blit(image, image_rect)

                font = self._get_font("current_side_value", 18)
                value_text = font.render(str(current_side.value), True, (255, 255, 255))
                value_rect = value_text.get_rect(bottomright=(square_x + square_size - 6, square_y + square_size - 6))
                self.screen.blit(value_text, value_rect)
        
        if ent.exerted:
            pg.draw.line(self.screen, (180, 80, 220), (square_x + 8, square_y + 8), (square_x + square_size - 8, square_y + square_size - 8), 20)
            pg.draw.line(self.screen, (180, 80, 220), (square_x + square_size - 8, square_y + 8), (square_x + 8, square_y + square_size - 8), 20)
            pg.draw.line(self.screen, (180, 80, 220), (square_x + 8, square_y + square_size - 8), (square_x + square_size - 8, square_y + square_size - 8), 20)
            pg.draw.line(self.screen, (180, 80, 220), (square_x + 8, square_y + 8), (square_x + square_size - 8, square_y + 8), 20)

        condition_positions = self._get_condition_icon_positions(x, y, card_w, card_h, sum(
            1 for value in [
                ent.undead,
                ent.petrifyAmount,
                ent.toexert,
                ent.cleansed,
                ent.inflictedPained,
                ent.inflictedExerted,
                ent.inflictedDeathed,
                ent.inflictedSingleUse,
            ] if value not in (False, 0)
        ))

        pos_index = 0
        for value in [
            ("undead", ent.undead),
            ("petrify", ent.petrifyAmount),
            ("toexert", ent.toexert),
            ("cleansed", ent.cleansed),
            ("pain", ent.inflictedPained),
            ("exerted", ent.inflictedExerted),
            ("deathed", ent.inflictedDeathed),
            ("singleuse", ent.inflictedSingleUse),
        ]:
            if value[1] in (False, 0):
                continue
            pos = condition_positions[pos_index]
            icon_x, icon_y = pos
            if value[0] == "undead":
                pg.draw.circle(self.screen, (255, 255, 255), (icon_x + 8, icon_y + 8), 8, width=2)
            elif value[0] == "petrify":
                pg.draw.rect(self.screen, (180, 180, 180), (icon_x + 1, icon_y + 1, 14, 14), width=2)
                font = self._get_font("petrify", 18)
                text = font.render(str(value[1]), True, (180, 180, 180))
                self.screen.blit(text, (icon_x + 2, icon_y + 2))
            elif value[0] == "toexert":
                pg.draw.line(self.screen, (220, 220, 220), (icon_x + 4, icon_y + 2), (icon_x + 4, icon_y + 14), 2)
                pg.draw.line(self.screen, (220, 220, 220), (icon_x + 2, icon_y + 4), (icon_x + 6, icon_y + 10), 2)
            elif value[0] == "cleansed":
                points = [(icon_x + 2, icon_y + 14), (icon_x + 14, icon_y + 8), (icon_x + 2, icon_y + 2)]
                pg.draw.polygon(self.screen, (255, 255, 255), points)
            elif value[0] in {"pain", "exerted", "deathed", "singleuse"}:
                letter = "S" if value[0] == "singleuse" else value[0][0].upper()
                font = self._get_font("condition_letter", 18)
                text = font.render(letter, True, (180, 80, 220))
                self.screen.blit(text, (icon_x + 3, icon_y + 2))
            pos_index += 1
        assert pos_index == len(condition_positions), "Mismatch in condition icon positions"

        self._draw_shield_indicator(ent, x, y, card_w, card_h, isEnemy, square_size)
        self._draw_dice_layout(ent, x, y, card_w, card_h, isEnemy, square_size)

        value = ent.timesUsed if not isEnemy else ent.timesDamaged
        if value != 0:
            font = self._get_font("usage_value", 22)
            text = font.render(str(value), True, (220, 40, 40))
            text_rect = text.get_rect(center=(square_x + square_size // 2, square_y + square_size + 16))
            self.screen.blit(text, text_rect)

        pg.display.flip()

    def _render_attack_lines(self, state: State):
        playerPositions = [self._get_position(i, state.players, is_enemy=False, ent=ent) for i, ent in enumerate(state.players)]
        enemyPositions = [self._get_position(i, state.enemies, is_enemy=True, ent=ent) for i, ent in enumerate(state.enemies)]

        for i, attackData in enumerate(state.enemyTargets):
            monPos = enemyPositions[i]
            for target in attackData:
                if target.sourceID < 0:
                    continue
                j = state.players.index(target)
                heroPos = playerPositions[j]
                
                start_x = monPos[0] + self.card_size[0] // 2
                start_y = monPos[1] + self._get_card_height(state.enemies[i]) // 2
                end_x = heroPos[0] + self.card_size[0] // 2 + 400
                end_y = heroPos[1] + self._get_card_height(state.players[j]) // 2
                pg.draw.line(self.screen, (220, 40, 40), (start_x, start_y), (end_x, end_y), width=4)

    def render(self, state: State):
        self.screen.fill((255, 220, 220))

        for i, ent in enumerate(state.players):
            pos = self._get_position(i, state.players, is_enemy=False, ent=ent)
            self.renderEnt(ent, pos, isEnemy=False)

        for i, ent in enumerate(state.enemies):
            pos = self._get_position(i, state.enemies, is_enemy=True, ent=ent)
            self.renderEnt(ent, pos, isEnemy=True)
        
        self._render_attack_lines(state)

        self._render_state_overlay(state)
        self._draw_console_overlay()
        pg.display.flip()

    def waitForInput(self):
        pg.event.clear()
        while True:
            event = pg.event.wait()
            
            if event.type == QUIT:
                pg.quit()
                return
            elif event.type == KEYDOWN:
                if event.key in (K_RETURN, K_KP_ENTER):
                    return