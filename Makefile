TARGET=rgb_to_ycocg
VTARGET=sim_ycocg
CFLAGS=-Wall -Wextra -std=c99
MAKEFLAGS+=--no-print-directory

.SILENT:
.PHONY: all test clean

all: $(TARGET) sim_ycocg

$(TARGET):

$(VTARGET):
	iverilog -o $(VTARGET) *.v

test:
	@./test.sh

clean:
	rm -f $(TARGET)
	rm -f $(VTARGET)
