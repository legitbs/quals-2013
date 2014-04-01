// The following line must be defined before including math.h to correctly define M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "common.h"

#define PI	M_PI	/* pi to machine precision, defined in math.h */
#define TWOPI	(2.0*PI)

short f[9] = {
        15000,
        15250,
        15500,
        15750,
        16000,
        16250,
        16500,
        16750,
        17000
};

// buffer pointer to the received wav file
#ifdef VULN
char wav[MAX_WAV_SIZE];
#else
char wav[MAX_WAV_SIZE*2];
#endif

// buffer to hold the key wav 
char key[MAX_WAV_SIZE];

void SendString(int, unsigned char *, char *);

double GetFrequencyIntensity(double re, double im) {
	return sqrt((re*re)+(im*im));
}

void four1(double data[], int nn, int isign)
{
#error Implement your own FFT stuff.  We were only licensed to distribute in binary form.
  return;
}

unsigned char ReadChar(int start, int end) {
	int i,j;
	int Nx;
	int NFFT;
	double *x;
	double *X;
	struct wav_header *w = (struct wav_header *)wav;
	int num_samples;
	short *sample;
	double intensity;
	double max_intensity = -9999.0;
	int max_intensity_bin;
	double bin_size;
	int freq_bins[9];
	char c;
	int wav_data_start;

	// number of samples over which to calc the FFT
	Nx = end-start;

	// calculate the number of FFT bins as the next higher power of 2 >= Nx
	NFFT = (int)pow(2.0, ceil(log((double)Nx)/log(2.0)));

	// figure out the FFT bin size in Hz
	bin_size = (double)SAMPLE_RATE/NFFT;
//	printf("bin_size: %lf\n", bin_size);

	// figure out which bins hold the frequencies we want
	for (i = 0; i < 9; i++) {
		freq_bins[i] = (int)(f[i]/bin_size);
	}

	// allocate memory for NFFT complex numbers (note the +1)
	X = (double *) malloc((2*NFFT+1) * sizeof(double));

	if (X == NULL) {
		exit(-1);
	}

	// Storing x(n) in a complex array to make it work with four1. 
	// This is needed even though x(n) is purely real in this case.

	// here's the vulnerability.  We don't sanity check the Subchunk2Size
	// or even use that value anywhere else.  So, the teams can 
	// send a value of their choosing.  The key is stored in a global
	// buffer right before the wav global.  So, if they send a 
	// very large Subchunk2Size, they can force this function to 
	// decode the key variable rather than the wav variable
	// and have the program send them back they key.
	wav_data_start = w->ChunkSize+8-w->Subchunk2Size;
	for(i=0; i<Nx; i++) {
		//sample = wav+44+((i+start)*2);
		sample = (short *)(wav+wav_data_start+((i+start)*2));
		//X[2*i+1] = x[i];
		X[2*i+1] = (double)*sample;
		X[2*i+2] = 0.0;
	}

	// pad the remainder of the array with zeros (0 + 0 j)
	for(i=Nx; i<NFFT; i++) {
		X[2*i+1] = 0.0;
		X[2*i+2] = 0.0;
	}

	// calculate FFT
	four1(X, NFFT, 1);

	// build the character encoded in the audio stream
	c = 0;
	for (i = 1; i < 9; i++) {
		intensity = GetFrequencyIntensity(X[2*(freq_bins[i])+1], X[2*(freq_bins[i])+2]);
		if (intensity > 1000000) {
			c = c | (1<<(i-1));
		}
	}

	free(X);

	return(c);
}

//
// read in the wav file
//
void RecvString(int sockfd, char *buf, int max_size) {
	char local[8];
	struct wav_header *w;
	int curr_bytes;
	int total_bytes;
	int i;
	int start_sample;
	int end_sample;

	// zero out our receive buffer
	bzero(buf, max_size);

	// read in the first 8 bytes of the wav header
	if (read(sockfd, local, 8) != 8) {
		perror("first read failed");
		close(sockfd);
		exit(-1);
	}

	// overlay the wav_header structure
	w = (struct wav_header *)local;

	// malloc a buffer to hold the entire wav file
	if (w->ChunkSize+8 > MAX_WAV_SIZE) {
		// finish reading it in anyway (but don't store it)
		total_bytes = read(sockfd, wav, MAX_WAV_SIZE);
		while (total_bytes < w->ChunkSize) {
			if ((curr_bytes = read(sockfd, wav, MAX_WAV_SIZE)) == -1) {
				perror("second read failed");
				exit(-1);
			}
			total_bytes += curr_bytes;
		}
		SendString(sockfd, "wav too big\n", NULL);
//		printf("wav too big: %d\n", w->ChunkSize);
//		close(sockfd);
//		exit(-1);
		return;
	}
	//wav = malloc(w->ChunkSize + 8);
	//printf("ChunkSize: %d\n", w->ChunkSize);

	// make sure malloc didn't fail
	//if (wav == NULL) {
	//	perror("malloc failed\n");
	//	close(sockfd);
	//	exit(-1);
	//}

	bzero(wav, MAX_WAV_SIZE);

	// copy in the first 8 bytes we read earlier
	memcpy(wav, local, 8);

	// overlay the wav_header structure
	w = (struct wav_header *)wav;

	// and then read the rest of the wav file
	total_bytes = read(sockfd, wav+8, w->ChunkSize);
	while (total_bytes < w->ChunkSize) {
		if ((curr_bytes = read(sockfd, wav+8+total_bytes, w->ChunkSize-total_bytes)) == -1) {
			perror("second read failed");
			exit(-1);
		}
		total_bytes += curr_bytes;
		//printf("total_bytes: %d (%d)\n", total_bytes, w->ChunkSize);
	}


	// make sure the wav header contains what we expect
	if (	w->ChunkID	 	!= 0x46464952 ||
		w->Format	 	!= 0x45564157 ||
		w->Subchunk1ID	 	!= 0x20746d66 ||
		w->Subchunk2ID	 	!= 0x61746164 ||
		w->Subchunk1Size 	!= 16 ||
		w->AudioFormat 		!= 1 || 
		w->NumChannels 		!= 1 || 
		w->BitsPerSample 	!= 16 || 
		w->SampleRate 		!= 44100) {

		//free(wav);
		SendString(sockfd, "Only know how to process 16-bit mono pcm sampled at 44100Hz\n", NULL);
//		printf("Only know how to process 16-bit mono pcm sampled at 44100Hz\n");
//		printf("w->ChunkID: %08x\n", w->ChunkID);
//		printf("w->Format: %08x\n", w->Format);
//		printf("w->Subchunk1ID: %08x\n", w->Subchunk1ID);
//		printf("w->Subchunk2ID: %08x\n", w->Subchunk2ID);
//		printf("w->Subchunk1Size: %d\n", w->Subchunk1Size);
//		printf("w->AudioFormat: %d\n", w->AudioFormat);
//		printf("w->NumChannels: %d\n", w->NumChannels);
//		printf("w->BitsPerSample: %d\n", w->BitsPerSample);
//		printf("w->SampleRate: %d\n", w->SampleRate);
		return;
	}
	//printf("w->ChunkSize: %d\n", w->ChunkSize);
	//printf("w->Subchunk2Size: %d\n", w->Subchunk2Size);

	// walk through the chars contained in the wav
	// start after the preamble
	start_sample = 8 * (SAMPLES_PER_CHARACTER + SAMPLES_PER_SILENCE);
	end_sample = start_sample + (SAMPLES_PER_CHARACTER + SAMPLES_PER_SILENCE);
	i = 0;
	while (end_sample*2 < w->ChunkSize+8-(8*(SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE)*2)) {
		buf[i++] = ReadChar(start_sample, end_sample);
		start_sample = end_sample+1;
		end_sample = start_sample + (SAMPLES_PER_CHARACTER + SAMPLES_PER_SILENCE);
		if (i > max_size-1) {
			break;
		}
	}

	//free(wav);
}

void GenerateSilence(char *b) {
	int i;
	short v = 0;

	// play silence for the specified number of samples
	for (i = 0; i < SAMPLES_PER_SILENCE; i++) {
		memcpy(b+i*2, &v, 2);
	}
}

void GenerateWave(char *b, unsigned char byte) {
	int i,j;
	double v;
	short o;

	// play the necessary frequencies for # samples
	for (i = 0; i < SAMPLES_PER_CHARACTER; i++) {

		// start with the carrier frequency f[0] which is constant
		v = sin(2*M_PI*f[0]*i/SAMPLE_RATE);

		// then add each frequency if the corresponding bit is a '1'
		for (j = 1; j < 9; j++) {
			if (byte & (1<<(j-1))) {
				v += sin(2*M_PI*f[j]*i/SAMPLE_RATE);
			}
		}

		// set the wave's amplitude
		v*=3000;

		// wav files store samples as shorts
		o = (short)v;
		memcpy(b+i*2, &o, 2);
	}

	// tack on the silence samples
	GenerateSilence(b+(SAMPLES_PER_CHARACTER*2));
}

#ifdef VULN
void SendString(int sockfd, unsigned char *str, char *dstbuf) {
	int len;
	int num_samples;
	struct wav_header *w;
	int retval;
	int i;
	int wav_size;
	char *wav_pos;

	// figure out how many samples we'll need to encode this string
	num_samples = PREAMBLE_SIZE*(SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE);
	len = strlen(str);
	num_samples += (SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE)*len;
	num_samples += PREAMBLE_SIZE*(SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE);

	// malloc some space for the wav file
	wav_size = sizeof(struct wav_header) + (num_samples*2);
	if (wav_size > MAX_WAV_SIZE) {
		SendString(sockfd, "String too large", NULL);
		return;
	}
		
	//wav = malloc(wav_size);
	//if (wav == NULL) {
	//	exit(-1);
	//}
	bzero(wav, wav_size);
	w = (struct wav_header *)wav;

	// form up the wav header
	w->ChunkID = 0x46464952;	// "RIFF"
	w->ChunkSize = wav_size - 8;
	w->Format = 0x45564157;		// "WAVE"
	w->Subchunk1ID = 0x20746d66;	// "fmt "
	w->Subchunk1Size = 16;
	w->AudioFormat = 1;		// pcm
	w->NumChannels = 1;		// mono
	w->SampleRate = SAMPLE_RATE;	// 44100
	w->BitsPerSample = 16;
	w->ByteRate = w->SampleRate * w->NumChannels * (w->BitsPerSample/8);
	w->BlockAlign = w->NumChannels * (w->BitsPerSample/8);
	w->Subchunk2ID = 0x61746164;	// "data"
	w->Subchunk2Size = num_samples * w->NumChannels * (w->BitsPerSample/8);

	// generate the preamble
	wav_pos = wav+44;
	for (i = 0; i < PREAMBLE_SIZE; i++) {
		GenerateWave(wav_pos, 0xFF);
		wav_pos += (SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE)*2;
	}

	// generate the wav data
	for (i = 0; i < len; i++) {
		GenerateWave(wav_pos, str[i]);
		wav_pos += (SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE)*2;
	}

	// generate the postamble
	for (i = 0; i < PREAMBLE_SIZE; i++) {
		GenerateWave(wav_pos, 0xFF);
		wav_pos += (SAMPLES_PER_CHARACTER+SAMPLES_PER_SILENCE)*2;
	}

	// if we've been given a dstbuf pointer, we should write this wav to that
	// instead of the socket
	if (dstbuf) {
		//printf("w->ChunkSize+8: %d\n", w->ChunkSize+8);
		memcpy(dstbuf, wav, w->ChunkSize+8);
	} else {
		// transmit the wav
		if (send(sockfd, wav, w->ChunkSize+8, 0) != w->ChunkSize+8) {
			//free(wav);
			exit(-1);
		}
	}

	//free(wav);

}
#endif
