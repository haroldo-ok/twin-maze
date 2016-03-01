SHELL=C:/Windows/System32/cmd.exe
CC	= sdcc -mz80

BINS	= maze.sms


all:	$(BINS)

gfx.c: gfx/font.fnt
	folder2c gfx gfx

maze.rel: gfx.c

%.rel:	%.c
	$(CC) -c --std-sdcc99 $< -o $@

%.sms:	%.ihx
	ihx2sms $< $@

maze.ihx:	maze.rel SMSlib/SMSlib.rel PSGlib/PSGlib.rel gfx.rel
	$(CC) --data-loc 0xC000 $^

clean:
	rm -f *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.asm *.sms gfx.*
