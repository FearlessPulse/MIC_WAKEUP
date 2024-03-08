#common makefile header

DIR_INC = ../../include
DIR_BIN = ../../bin
DIR_LIB = ../../libs

TARGET	= ivw_record_sample
BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE = 
CFLAGS = -g -Wall -I$(DIR_INC)
LDFLAGS := -L$(DIR_LIB)


LDFLAGS += -laikit -lrt -ldl -lpthread -lasound -lstdc++

COBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
CPPOBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

$(BIN_TARGET) : $(COBJECTS) $(CPPOBJECTS)
	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@

%.o : %.cpp
	$(CROSS_COMPILE)g++ -c $(CFLAGS) $< -o $@

clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot
