
CC = /home/yxs/risc-v/host-tools/gcc/riscv64-linux-musl-x86_64/bin/riscv64-unknown-linux-musl-gcc
OPT_LEVEL=-Os -march=rv64imafdcvxthead -mcmodel=medany -mabi=lp64d -static


source  = spacc_test.c 
object  = $(source:%.cc=%.o)

all: $(object)
	@$(CC) $(object)  -o spacc_test $(OPT_LEVEL)

%.o: %.c
	@$(CC) $(CCCFLAGS) -c $<

clean:
	@$(RM) *.o spacc_test
