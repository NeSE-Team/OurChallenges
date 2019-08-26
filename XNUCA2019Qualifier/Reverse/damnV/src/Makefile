CFLAGS = -Wall -Wextra -O2

ifdef KVM_DEBUG
CFLAGS += -g -DKVM_DEBUG
endif

damnV: hypervisor/hypervisor.o hypervisor/fragment.o guest/payload.o
	$(CC) $^ -o $@

guest/payload.o:  guest/guest.img.o
	$(LD) -T guest/payload.ld $^ -o $@

guest/entry.o: guest/entry.s
	nasm -f elf32 -o $@ $^

guest/guest.o: guest/guest.c 
	$(CC) $(CFLAGS) -m32 -ffreestanding -fno-pic -c -o $@ $^

guest/guest.img: guest/guest.o guest/entry.o
	$(LD) -T guest/guest.ld -m elf_i386 $^ -o $@


guest/guest.img.o: guest/guest.img
	$(LD) -b binary -r $^ -o $@


.PHONY: debug
debug:
	$(MAKE) KVM_DEBUG=1

.PHONY: data
data:
	$(MAKE) -C gendata

.PHONY: clean
clean:
	$(RM) guest/*.o guest/*.bin guest/*.img guest/*.img.o hypervisor/*.o damnV