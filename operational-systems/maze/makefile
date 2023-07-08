build/maze_viewer: maze_viewer.c
	mkdir -p build
	gcc -o2 -std=c17 maze_viewer.c -o build/maze_viewer $(shell pkg-config allegro-5 allegro_primitives-5 allegro_font-5 --libs --cflags)

build/cellular_automata_maze: cellular_automata_maze.c
	mkdir -p build
	gcc -o2 -std=c17 cellular_automata_maze.c -o build/cellular_automata_maze

.PHONY: clean
clean:
	rm -rf ./build

.PHONY: ca_diff_ssize_demo
ca_diff_ssize_demo: build/maze_viewer build/cellular_automata_maze
	build/cellular_automata_maze -r 40x40 -l 0.395 -n 10 | build/maze_viewer -r 640x480 -d -t 0.02

.PHONY: ca_midsize_demo
ca_midsize_demo: build/maze_viewer build/cellular_automata_maze
	build/cellular_automata_maze -r 100x100 -l 0.415 -n 10 | build/maze_viewer -r 640x480

.PHONY: ca_hres_demo
ca_hres_demo: build/maze_viewer build/cellular_automata_maze
	build/cellular_automata_maze -r 800x640 -l 0.415 -n 25 | build/maze_viewer -r 800x640 -t 0.1
