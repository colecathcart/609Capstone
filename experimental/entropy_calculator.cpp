#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char *argv[])
{
    uint64_t map[256] = {0}; // Initialize the frequency map
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

    // Open the file
    f = fopen(argv[1], "r");
    if (!f)
    {
        fprintf(stderr, "Can't open %s\n", argv[1]);
        return 1;
    }

    // Read the file and count the frequency of each byte
    while (!feof(f))
    {
        char buf[1024 * 8];
        size_t r;
        r = fread(buf, 1, sizeof(buf), f);
        if (r == 0)
            break;
        for (i = 0; i < r; i++)
        {
            size_t index = (unsigned char)buf[i];
            map[index]++;
        }
    }

    // Get the file length
    flen = ftell(f);
    if (flen == -1L) {
        perror("ftell");
        fclose(f);
        return 1;
    }
    fclose(f);

    // Calculate the entropy
    for (i = 0; i < (sizeof(map) / sizeof(map[0])); i++)
    {
        double freq;
        if (map[i] == 0)
            continue;
        freq = (double)map[i] / flen;
        info += freq * log2(freq);
    }
    info = -info;

    // Print the entropy
    printf("%f\n", info);
    return 0;
}