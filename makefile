files = corth.c main.c
input_file = std.corth
link = -lm

out: $(files)
	gcc $(files) -o corth $(link)

debug: $(files)
	gcc -g $(files) $(link)

clean:
	del *.exe

run: debug out $(input_file)
	./corth $(input_file)