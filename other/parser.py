# Parse the deck file and convert it to a JSON file

import random
import json
import os

from math import ceil, floor
from textwrap import wrap

os.chdir(os.path.dirname(os.path.abspath(__file__)))

with open("../mazzo.txt", "r") as f:
    deck = f.read().splitlines()

type_of_cards = ["ALL", "STUDENTE", "MATRICOLA", "STUDENTE_SEMPLICE", "LAUREANDO", "BONUS", "MALUS", "MAGIA", "ISTANTANEA"]
type_of_actions = ["GIOCA", "SCARTA", "ELIMINA", "RUBA", "PESCA", "PRENDI", "BLOCCA", "SCAMBIA", "MOSTRA", "IMPEDIRE", "INGEGNERE"]
type_of_players = ["IO", "TU", "VOI", "TUTTI"]
type_of_when_to_play = ["SUBITO", "INIZIO", "FINE", "MAI", "SEMPRE"]

cards = []
idx_line = 0

while idx_line < len(deck):
    card = {}

    card["quantity"] = int(deck[idx_line])
    card["name"] = deck[idx_line + 1]
    card["description"] = deck[idx_line + 2]
    card["type"] = f"[{int(deck[idx_line + 3])}] {type_of_cards[int(deck[idx_line + 3])]}"
    card["number_of_effects"] = int(deck[idx_line + 4])

    card["effects"] = []
    for i in range(card["number_of_effects"]):
        effect_values = list(map(int, deck[idx_line + 5 + i].split()))

        effect = {}
        effect["action"] = f"[{effect_values[0]}] {type_of_actions[effect_values[0]]}"
        effect["player_target"] = f"[{effect_values[1]}] {type_of_players[effect_values[1]]}"
        effect["card_target"] = f"[{effect_values[2]}] {type_of_cards[effect_values[2]]}"

        card["effects"] += [effect]

    card["when_to_play"] = f"[{int(deck[idx_line + 5 + card['number_of_effects']])}] {type_of_when_to_play[int(deck[idx_line + 5 + card['number_of_effects']])]}"
    card["optional"] = bool(int(deck[idx_line + 6 + card["number_of_effects"]]))
    cards += [card]

    idx_line += 7 + card["number_of_effects"]

with open("mazzo.json", "w") as f:
    json.dump(cards, f, indent=4)

# =================================================================== #

# Analysis of the deck

max_name = max(cards, key=lambda x: len(x["name"]))
print(f"Max name: \"{max_name['name']}\" ({len(max_name['name'])} characters)")

min_name = min(cards, key=lambda x: len(x["name"]))
print(f"Min name: \"{min_name['name']}\" ({len(min_name['name'])} characters)")

max_description = max(cards, key=lambda x: len(x["description"]))
print(f"Max description: \"{max_description['description']}\" ({len(max_description['description'])} characters)")

min_description = min(cards, key=lambda x: len(x["description"]))
print(f"Min description: \"{min_description['description']}\" ({len(min_description['description'])} characters)")

max_effects = max(cards, key=lambda x: x["number_of_effects"])
print(f"Max effects: {max_effects['number_of_effects']}\n")

# =================================================================== #

# Print the deck

shuffled_cards = random.sample(cards, len(cards))

cards_per_row = 8
groups_of_cards = [shuffled_cards[i:i + cards_per_row] for i in range(0, len(shuffled_cards), cards_per_row)]

card_size = 32
border_up = "┌" + "─" * (card_size-2) + "┐"
border_down = "└" + "─" * (card_size-2) + "┘"
start_line = "│ "
end_line = " │"
line_separator = start_line + (" " * (card_size - len(start_line) - len(end_line))) + end_line

for idx, group_cards in enumerate(groups_of_cards):
    max_desc = max([len(wrap(c["description"], width=card_size - len(start_line) - len(end_line))) for c in group_cards])
    max_row = 7 + max_desc
    buffer = [""] * max_row

    for idj, card in enumerate(group_cards):
        card_idx = (idx*cards_per_row)+idj+1

        buffer[0] += border_up

        card_name = card["name"]
        space_available = card_size - len(start_line) - len(end_line) - len(card_name)
        parsed_name = start_line + (" " * floor(space_available/2)) + card_name + (" " * ceil(space_available/2)) + end_line
        buffer[1] += parsed_name

        card_type = "[" + card["type"].split()[1].replace("_", " ") + "]"
        space_available = card_size - len(start_line) - len(end_line) - len(card_type)
        parsed_type = start_line + (" " * floor(space_available/2)) + card_type + (" " * ceil(space_available/2)) + end_line
        buffer[2] += parsed_type

        buffer[3] += line_separator

        card_description = card["description"]
        parsed_description = wrap(card_description, width=card_size - len(start_line) - len(end_line))
        for idk, line in enumerate(parsed_description):
            parsed_line = start_line + line.ljust(card_size - len(start_line) - len(end_line)) + end_line
            buffer[4+idk] += parsed_line

        for adj in range(1 + max_desc - len(parsed_description)):
            buffer[4+len(parsed_description)+adj] += line_separator

        card_index = f"{card_idx}/{len(cards)}"
        space_available = card_size - len(start_line) - len(end_line) - len(card_index)
        parsed_index = start_line + (" " * floor(space_available/2)) + card_index + (" " * ceil(space_available/2)) + end_line
        buffer[5+max_desc] += parsed_index

        buffer[6+max_desc] += border_down

    for line in buffer:
        print(line)

    buffer.clear()
