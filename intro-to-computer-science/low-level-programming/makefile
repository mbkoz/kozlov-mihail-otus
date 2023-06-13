# build project for 1st exercise
longest_seq: longest_seq.asm
	nasm -f elf -g -F dwarf longest_seq.asm && ld -m elf_i386 -o longest_seq longest_seq.o
	nm -f sysv longest_seq.o longest_seq > symt

# build project for 2nd exercise
up_down: up_down.asm
	nasm -f elf -g -F dwarf up_down.asm && ld -m elf_i386 -o up_down up_down.o
	nm -f sysv up_down.o up_down > symt

# build projector 3rd exercise
wday_search: wday_search.asm
	nasm -f elf -g -F dwarf wday_search.asm && ld -m elf_i386 -o wday_search wday_search.o
	nm -f sysv wday_search.o wday_search > symt

# aux cmd
clean_up:
	rm ./*.o symt

