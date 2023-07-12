maze_viewer: maze_viewer.c
	gcc -o2 -std=c17 maze_viewer.c -o maze_viewer $(shell pkg-config allegro-5 allegro_primitives-5 allegro_font-5 --libs --cflags)

cellular_automata_maze: cellular_automata_maze.c
	gcc -o2 -std=c17 cellular_automata_maze.c -o cellular_automata_maze

bsp_maze: bsp_maze.c
	gcc -o2 -std=c17 bsp_maze.c -o bsp_maze

.PHONY: clean
clean:
	-rm cellular_automata_maze maze_viewer bsp_maze 2> /dev/null

.PHONY: ca_diff_ssize_demo
ca_diff_ssize_demo: maze_viewer cellular_automata_maze
	./cellular_automata_maze -r 40x40 -l 0.395 -n 10 | ./maze_viewer -r 640x480 -d -t 0.02

.PHONY: ca_midsize_demo
ca_midsize_demo: maze_viewer cellular_automata_maze
	./cellular_automata_maze -r 100x100 -l 0.415 -n 10 | ./maze_viewer -r 640x480

.PHONY: ca_hres_demo
ca_hres_demo: maze_viewer cellular_automata_maze
	./cellular_automata_maze -r 800x640 -l 0.415 -n 25 | ./maze_viewer -r 640x480 -t 0.1

PHONY: bsp_demo
bsp_demo: maze_viewer bsp_maze
	./bsp_maze | ./maze_viewer -r 800x640 -t 0.1 -d