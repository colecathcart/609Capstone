#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char *argv[])
{
    uint64_t map[256] = {0}; // Frequency map
    size_t i;
    FILE *f;
    long int flen;
    double info = 0.0;

    // Check if the file name is provided
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    printf("%s\n", argv[1]);

    // Open the file in binary mode
    f = fopen(argv[1], "rb");
    if (!f)
    {
        fprintf(stderr, "Can't open %s\n", argv[1]);
        return 1;
    }

    // Get the file length
    fseek(f, 0, SEEK_END);
    flen = ftell(f);
    if (flen == -1L)
    {
        perror("ftell");
        fclose(f);
        return 1;
    }
    rewind(f); // Reset file pointer to the beginning

    // Read the file and count byte frequencies
    while (!feof(f))
    {
        unsigned char buf[1024 * 8];
        size_t r = fread(buf, 1, sizeof(buf), f);
        if (r == 0)
            break;
        for (i = 0; i < r; i++)
        {
            map[buf[i]]++;
        }
    }
    fclose(f);

    // Calculate the entropy
    for (i = 0; i < 256; i++)
    {
        if (map[i] == 0)
            continue;
        double freq = (double)map[i] / flen;
        info += freq * log2(freq);
    }
    info = -info;

    // Print the entropy
    printf("Entropy: %f bits per byte\n", info);
    return 0;
}
