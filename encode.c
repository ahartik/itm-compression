#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

typedef struct
{
    uint32_t lower;
    uint32_t upper;
    int underflow;

    uint8_t* data;
    int reserved;
    int di;
    int offset;
}aencoder;

void aen_write_bit(aencoder* t,int bit)
{
    if (bit)
        t->data[t->di] |= (1<<t->offset);
    t->offset++;
    if(t->offset == 8)
    {
        t->offset = 0;
        t->di++;
        if (t->di >= t->reserved)
        {
            t->data = realloc(t->data,t->reserved*2+1);
            t->reserved = t->reserved*2+1;
            memset(t->data+t->di,0,t->reserved-t->di);
        }
    }
}

#define MSBMATCH(a,b) (((a)>>31) == ((b)>>31))
#define IS_UNDERFLOW(l,u) (((l>>30)==1) && ((u>>30)==2))

void aen_init(aencoder* enc);
void aen_read_bits(aencoder* enc);
void aen_encode_range(aencoder* enc, uint32_t lower, uint32_t upper,uint32_t total);

void aen_init(aencoder* enc)
{
    enc->lower = 0;
    enc->upper = ~0;
    enc->underflow = 0;

    enc->offset = 0;
    enc->di = 0;
    enc->reserved = 1<<16;
    enc->data = malloc(enc->reserved);
    memset(enc->data, 0, enc->reserved);
}

void aen_encode_range(aencoder* enc,uint32_t l,uint32_t u,uint32_t total)
{
    uint64_t range = (uint64_t)(enc->upper) - enc->lower+1;
    uint64_t scaled_low = (l * range) / total;
    uint64_t scaled_up = (u * range) / total;
    enc->upper = enc->lower + scaled_up - 1;
    enc->lower = enc->lower + scaled_low;

    while (MSBMATCH(enc->lower,enc->upper))
    {
        int msb = enc->lower>>31;
        aen_write_bit(enc, msb);
        while(enc->underflow){
            aen_write_bit(enc, !msb);
            enc->underflow--;
        }
        enc->lower <<= 1;
        enc->upper <<= 1;
        enc->upper |= 1;
    }

    while (IS_UNDERFLOW(enc->lower,enc->upper))
    {
        enc->underflow++;
        enc->lower <<= 1;
        enc->upper <<= 1;
        enc->upper |= 1;
        enc->lower &= ~(1<<31);
        enc->upper |=  (1<<31);
    }
}

void aen_finish(aencoder* enc)
{
    int msb = (enc->lower&(1<<30)) != 0;
    aen_write_bit(enc, msb);
    enc->underflow++;
    while(enc->underflow){
        aen_write_bit(enc, !msb);
        enc->underflow--;
    }
    enc->di++;
}

#include"model.c"

void ex1_encode() {
    aencoder enc;
    aen_init(&enc);
    FILE* side = fopen("ex1_side.dat","r");
    FILE* class = fopen("ex1_class.dat","r");
    while(!feof(class))
    {
        int data[4];
        int x0;
        fscanf(class,"%i",&x0);
        fscanf(side,"%i %i %i %i",data,data+1,data+2,data+3);
        int r=0;
        uint32_t prob;
        for(int i=0; i<4; ++i) {
            r += data[i]<<i;
        }
        prob = cprobs[r];
        //printf("%f\n", (double)prob/TOTALPROB);
        if (x0)
            aen_encode_range(&enc, prob, TOTALPROB, TOTALPROB);
        else
            aen_encode_range(&enc, 0, prob, TOTALPROB);
    }
    aen_finish(&enc);
    FILE* fout = fopen("ex1_data.bin", "w");
    fwrite(enc.data, 1, enc.di, fout);
    printf("wrote %u bytes\n", enc.di);
    fclose(fout);
}
#include "model2.c"
#define MAXR 200000
int mat[MAXR][4];
void ex2_encode() {
    FILE* f = fopen("ex2.csv","r");
	assert(f);
	int i=0;
	while(!feof(f)) {
		int a,b,c,d;
		fscanf(f, "%d,%d,%d,%d\n", &a,&b,&c,&d);
		if (feof(f)) break;
		mat[i][0] = a;
		mat[i][1] = b;
		mat[i][2] = c;
		mat[i][3] = d;
		++i;
	}
	int total=i;
//	printf("read %d\n", total);

	int tout=0;
	for(int a=0; a<4; ++a) {
		aencoder enc;
		aen_init(&enc);
		double prev = mat[a][0];
		double var = 0.001;
		for(i=1; i<total; ++i) {
			uint32_t v = mat[a][i];
			uint32_t low = sym2prob(v, prev, var);
			uint32_t hi = sym2prob(v+1, prev, var);
//			printf("range %u %u\n", low, hi);
			aen_encode_range(&enc, low, hi, TOTALPROB);
			double dx = v-prev;
			var = (5*var + dx*dx)/6;
			prev = v;
		}
		aen_finish(&enc);
		char fname[32];
		sprintf(fname, "ex2_%d.bin", a);
		FILE* fout = fopen(fname, "w");
		fwrite(enc.data, 1, enc.di, fout);
		printf("wrote %u bytes\n", enc.di);
		fclose(fout);
		tout += enc.di;
	}
	printf("total output size: %d\n", tout);
}

int main()
{
	ex1_encode();
	ex2_encode();
}

