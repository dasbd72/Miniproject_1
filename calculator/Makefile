CC			:= gcc
EXE			:= calculator
H_FILES		:= lex.h parser.h codeGen.h utilities.h
C_FILES		:= lex.c parser.c codeGen.c utilities.c main.c
O_FILES		:= lex.o parser.o codeGen.o utilities.o main.o

.PHONY: merge

all: $(O_FILES)
	@$(CC) -o $(EXE) $(O_FILES)

%.o: %.c
	@$(CC) -c $^ -o $@

merge:
	@cat $(H_FILES) $(C_FILES) | sed "s/#include \".*\"//g" > merged/main.bundle_v1-0.c

clean:
	@rm -rf $(O_FILES) $(EXE) $(EXE).exe

winclean:
	@del *.o calculator calculator.exe