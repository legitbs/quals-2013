void RecvString(int, char *, int);
void SendString(int, unsigned char *, char *);

// keep them from sending stupid big files
#define MAX_WAV_SIZE 1000000

#define SAMPLE_RATE 44100
// 44100/147 = 300 (so 300 characters per second)
//#define SAMPLES_PER_CHARACTER 140
//#define SAMPLES_PER_SILENCE 7
#define SAMPLES_PER_CHARACTER 2105
#define SAMPLES_PER_SILENCE 100
#define PREAMBLE_SIZE 8

// wav header structure
struct wav_header {
        unsigned int ChunkID;
        unsigned int ChunkSize;
        unsigned int Format;
        unsigned int Subchunk1ID;
        unsigned int Subchunk1Size;
        unsigned short AudioFormat;
        unsigned short NumChannels;
        unsigned int SampleRate;
        unsigned int ByteRate;
        unsigned short BlockAlign;
        unsigned short BitsPerSample;
        unsigned int Subchunk2ID;
        unsigned int Subchunk2Size;
};
