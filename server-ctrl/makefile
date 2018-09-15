SRCDIR = .
OBJDIR = obj
BINDIR = bin

SRC_C = $(wildcard $(SRCDIR)/*.c)
OBJ_C = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(notdir $(SRC_C))))

ctrl : $(OBJ_C)
	gcc -o $(BINDIR)/ctrl $^ -lpthread

$(OBJDIR)/%.o : %.c
	gcc -c %< -o $@

clean :
	rm $(BINDIR)/*
	rm $(OBJDIR)/*