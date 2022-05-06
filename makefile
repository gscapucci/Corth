main = std
files = corth.c main.c
input_file = $(main).corth
link = -lm

out: $(files)
	gcc $(files) -o corth $(link)

debug: $(files)
	gcc -g $(files) $(link)

gcccom: out
	./corth -com $(input_file)
	./fasm $(main).asm

clean:
	del *.exe
	del a.out
	del corth
	del micro

sim: debug out $(input_file)
	./corth $(input_file)

run: gcccom
	./$(main)