CC = g++
OPT = 0
GTEST_DIR=../googletest/googletest
ifeq ($(CC), clang++)
	STDLIB = --stdlib=libc++
	STDLINK = -L$(CLANG)/lib -Wl,-rpath,$(CLANG)/lib -lc++abi
	WARNINGS = -Weverything -Wno-c++98-c++11-c++14-compat -Wno-c++98-compat \
				-Wno-exit-time-destructors -Wno-global-constructors -Wno-c++98-compat-pedantic \
				-Wno-padded -Wno-undefined-func-template -Wno-missing-variable-declarations \
				-Wno-missing-prototypes -Wno-weak-vtables
else
	STDLIB =
	STDLINK =
	WARNINGS = -Wstrict-null-sentinel -Wchkp -Wsuggest-override -Wlogical-op \
				-Wall -Wextra -Wpedantic -Wshadow -Wformat \
				-Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wdouble-promotion \
				-Wimplicit-fallthrough -Wuninitialized
endif
CFLAGS = -g --std=c++17 -isystem $(GTEST_DIR)/include -pthread -Werror $(WARNINGS) $(STDLIB) -O$(OPT)
OBJDIR = ./build
BINDIR = ./bin
OBJS = $(OBJDIR)/Strategies.o $(OBJDIR)/BlackJack.o

all: debug gtest test

#This second expansion directive is necessary in order to get the change of    \
    variant
#to apply in the expansion of OBJS
.SECONDEXPANSION:
release: OPT = 3
release: $$(OBJS)
	$(CC) $(CFLAGS) main.cc $(OBJS) -o $(BINDIR)/blackjack $(STDLINK)

debug: $$(OBJS)
	$(CC) $(CFLAGS) main.cc $(OBJS) -o $(BINDIR)/blackjack $(STDLINK)

test: $$(OBJS)
	$(CC) $(CFLAGS) test_runner.cc $(BINDIR)/libgtest.a $(OBJS) -o $(BINDIR)/test_runner $(STDLINK)

gtest:
	$(CC) -g -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc -o $(OBJDIR)/gtest-all.o
	ar -rv $(BINDIR)/libgtest.a $(OBJDIR)/gtest-all.o

#There is probably a way to do this in a patterned based way, but oh well
#also have to make the associated bin directories...
MAKE_DIR=@mkdir -p $(@D)
MAKE_OBJ=$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/%.o : %.cc %.h
	$(MAKE_DIR)
	$(MAKE_OBJ)
clean:
	rm $(OBJDIR)/*.o $(BINDIR)/* || true
