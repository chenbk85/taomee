icc  -Wall -g -c client.c -O3 -parallel -axSSE4.2 -ivdep -ansi-alias -restrict -fargument-alias -funroll-all-loops -opt-prefetch
icc -O3 -parallel -axSSE4.2 -ivdep -ansi-alias -restrict -fargument-alias -funroll-all-loops -opt-prefetch client.o -o client -lgd -ljpeg -lz -lm
