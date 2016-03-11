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

maze.ihx:	SMSlib/crt0_sms.rel maze.rel SMSlib/SMSlib.lib gfx.rel
	$(CC) --no-std-crt0 --data-loc 0xC000 -o maze.ihx $^

clean:
	rm -f *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.asm *.sms gfx.*
