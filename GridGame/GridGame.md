# Brainstorming

* Show 3-state visibility (unseen, last seen, visible)
  * Notes on grid-based visibility
    * https://www.albertford.com/shadowcasting/
    * http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html
* Movement - Grid vs Free vs Link to the Past?
* Hard separation between game state and render
* kbd / kbd+mouse / controller / glass

# Data Structures
* Landscape
* Actor
How much overlap is there between the two?
*Stop overthinking and get something running*

# Data Formats
* json, csv, raw?

Object flags

- [ ] Blocks Movement
- [ ] Moveable



| Object   | Character | Flags           |
| -------- | :-------- | --------------- |
| Empty    | .         | none            |
| Player    | @    | Moveable    |
| Rock     | *         | Moveable        |
| Stick    | /         | Moveable        |
| Grass    | +         | none            |
| Mountain | ^         | Blocks Movement |

```Object Layer
.//.........
.......*.*..
.........**.
............
...../.../..
../.........
```

```Landscape Layer
++++++++++++
++++++++++++
++^^^+++++++
++++^^^+++++
++++++++++++
++++++++++++
```
