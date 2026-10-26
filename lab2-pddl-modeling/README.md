# Lab 2: Modeling with PDDL

[`domain.pddl`](domain.pddl) completes a partial PDDL model of the *vampire* domain, an exercise
from the Planning and Optimization course at the University of Basel. The task was to write the
effects of the two actions without adding actions or predicates.

## The domain

Rooms form a circular corridor, so each room has a clockwise and an anti-clockwise neighbor. A
vampire and a vampire slayer are somewhere in the rooms. The planner can only toggle lights and
watch fights. The rules are:

- **Vampire.** The vampire avoids light. When its room is lit, it moves to a dark neighboring
  room, preferring the anti-clockwise one. If both neighbors are lit, it moves clockwise.
- **Slayer.** The slayer stays in the light. When her room goes dark, she moves clockwise if that
  room is lit, and anti-clockwise otherwise.
- **Fight.** When both are in the same room, they fight.
  - The vampire wins in a dark room without garlic.
  - The slayer wins in a lit room or a room with garlic.

The goal is a plan of light switches that lets the slayer win.

## How the model works

Both actions rely heavily on **conditional effects** (`when`):

- **`toggle-light`** takes a room and its two neighbors. It flips the room's light. It then moves
  each character according to the lights in the neighboring rooms, and sets `fighting` when the
  two end up in the same room.
- **`watch-fight`** ends the fight. Depending on the room's light and garlic, either the vampire
  or the slayer dies.

The problem instances were provided by the course and are not included here.

## Part 2: the International Planning Competition

The second exercise surveyed the top planners of the 2018 International Planning Competition
(classical tracks).

| Rank | Optimal track | Approach |
|---|---|---|
| 1 | Delfi | Portfolio that picks a planner configuration from an image of the task, using a convolutional neural network; components use A\* with LM-cut, canonical and zero-one PDBs, merge-and-shrink and blind search |
| 2 | Complementary2 | A\* with pattern databases |
| 3 | Planning-PDBs | A\* with pattern databases |
| 4 | Symbolic bidirectional search (baseline) | Symbolic bidirectional blind search |
| 5 | Scorpion | A\* with Cartesian abstractions and pattern databases (hill climbing and systematic patterns) |

| Rank | Satisficing track | Approach |
|---|---|---|
| 1 | Fast Downward Stone Soup 2018 | Portfolio of many search configurations and heuristics |
| 2 | Fast Downward Remix | Portfolio of many search configurations and heuristics |
| 3 | LAPKT-DUAL-BFWS | Best-first width search with the landmark heuristic, helpful actions and h<sup>FF</sup> |
| 4 | Saarplan | Greedy best-first search with h<sup>FF</sup> |
| 5 | DecStar | Star-topology decoupled search with GBFS, PDBs, LM-cut, merge-and-shrink, landmark count and h<sup>FF</sup> |
