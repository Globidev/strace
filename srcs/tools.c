int null_byes_in_word(long word)
{
    static long low_magic = 0x0101010101010101L;
    static long high_magic = 0x8080808080808080L;

    return (((word - low_magic) & ~word & high_magic) != 0);
}
