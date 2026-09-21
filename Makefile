all: build/kernel.bin

build/boot.o: kernel/boot.asm
	nasm -f elf32 kernel/boot.asm -o build/boot.o

build/kernel.o: kernel/kernel.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/kernel.c -o build/kernel.o

build/keyboard.o: kernel/keyboard.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/keyboard.c -o build/keyboard.o

build/memory.o: kernel/memory.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/memory.c -o build/memory.o

build/task.o: kernel/task.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/task.c -o build/task.o

build/paging.o: kernel/paging.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/paging.c -o build/paging.o

build/interrupts.o: kernel/interrupts.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/interrupts.c -o build/interrupts.o	

build/pagefault.o: kernel/pagefault.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/pagefault.c -o build/pagefault.o

build/pagefault_asm.o: kernel/pagefault.asm
	nasm -f elf32 kernel/pagefault.asm -o build/pagefault_asm.o

build/timer.o: kernel/timer.asm
	nasm -f elf32 kernel/timer.asm -o build/timer.o

build/tss.o: kernel/tss.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/tss.c -o build/tss.o

build/tss_asm.o: kernel/tss.asm
	nasm -f elf32 kernel/tss.asm -o build/tss_asm.o

build/tss.o: kernel/tss.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/tss.c -o build/tss.o

build/tss_asm.o: kernel/tss.asm
	nasm -f elf32 kernel/tss.asm -o build/tss_asm.o

build/user_mode.o: kernel/user_mode.asm
	nasm -f elf32 kernel/user_mode.asm -o build/user_mode.o

build/syscall.o: kernel/syscall.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel/syscall.c -o build/syscall.o

build/syscall_asm.o: kernel/syscall.asm
	nasm -f elf32 kernel/syscall.asm -o build/syscall_asm.o	

build/kernel.bin: build/boot.o build/kernel.o build/keyboard.o build/memory.o build/task.o build/paging.o build/interrupts.o build/pagefault.o build/pagefault_asm.o build/timer.o build/tss.o build/tss_asm.o build/user_mode.o build/syscall.o build/syscall_asm.o linker.ld
		ld -m elf_i386 -T linker.ld -o build/kernel.bin build/boot.o build/kernel.o build/keyboard.o build/memory.o build/task.o build/paging.o build/interrupts.o build/pagefault.o build/pagefault_asm.o build/timer.o build/tss.o build/tss_asm.o build/user_mode.o build/syscall.o build/syscall_asm.o
iso: build/kernel.bin
	mkdir -p build/isodir/boot/grub
	cp build/kernel.bin build/isodir/boot/kernel.bin
	cp boot/grub/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/ValaOS.iso build/isodir

run: iso
	qemu-system-i386 -cdrom build/ValaOS.iso -boot d

clean:
	rm -rf build/*