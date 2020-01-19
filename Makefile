nao:
	gcc -o nao temp.c `pkg-config fuse --cflags --libs`

clean:
	rm nao
