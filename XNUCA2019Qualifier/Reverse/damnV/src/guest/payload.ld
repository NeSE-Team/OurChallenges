SECTIONS
{
        .payload 0 : {
                guest = .;
                guest/guest.img.o
                guest_end = .;
        }
}
