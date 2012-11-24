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
        fscanf(class,"%i\n",&x0);
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
int lmat[MAXR][4];
void ex2_encode() {
    FILE* f = fopen("four-stocks.csv","r");
    assert(f);
    int i=0;
    while(!feof(f)) {
        for(int j=0; j<4; ++j) {
            int low,hi;
            fscanf(f, "%d.%d", &hi,&low);
            if (feof(f)) goto doneread;
            fgetc(f);
//            mat[i][j] = low+100*hi;
            mat[i][j] = hi;
            lmat[i][j] = low;
        }
        ++i;
    }
doneread:;
    int total=i;
//  printf("read %d\n", total);
#if 0
    for (uint32_t c = 0; c < MAX_VALUE; c++)
    {
        uint32_t p = sym2prob(c,5225,100);
        uint32_t p2 = sym2prob(c+1,5225,100);
        assert(prob2sym(p,5225,100)==c);
        assert(prob2sym(p2-1,5225,100)==c);
    }
#endif

    aencoder enc;
    aen_init(&enc);
    double prev = mat[0][0];
    ld var = 1;
    const int learn = LEARN_RATE;
    for(int a=0; a<4; ++a) {
        for(i=a==0; i<total; ++i) {
            uint32_t v = mat[i][a];
            uint32_t low = sym2prob(v, prev, var);
            uint32_t hi = sym2prob(v+1, prev, var);
//            printf("range %f %f : %f\n", (double)low/TOTALPROB, (double)hi/TOTALPROB, (hi-low)/(double)TOTALPROB);
            aen_encode_range(&enc, low, hi, TOTALPROB);
            double dx = v-prev;
            prev = v;
            if (fabs(dx)>9) continue;
            var = (learn*var + dx*dx)/(learn+1);
        }
    }
    printf("integer part size: %u\n", enc.di);
    uint32_t counts[100];
    for(int i=0; i<100; ++i) counts[i] = 1;
    const int C = LOW_LEARN_RATE;
    uint32_t csum = 100;
        for(int i=0; i<total; ++i) {
    for(int a=0; a<4; ++a) {
            int v = lmat[i][a];
            uint32_t low=0;
            for(int j=0; j<v; ++j) low += counts[j];
            uint32_t hi = low + counts[v];
//            printf("low %d %d %d\n", low, hi, csum);
            aen_encode_range(&enc, low, hi, csum);

            counts[v] += C;
            csum += C;
            const int K = LOW_UNLEARN;
            if (i>=K) {
                int old = lmat[i-K][a];
                counts[old] -= C;
                csum -= C;
            }
        }
    }
    aen_finish(&enc);
    FILE* fout = fopen("ex2_data.bin", "w");
    fwrite(enc.data, 1, enc.di, fout);
    printf("wrote %u bytes\n", enc.di);
    fclose(fout);
}

//#include "model3.c"
int ex3_side[58101][10];
typedef struct DTree_rec
{
    int var;
    int split;
    struct DTree_rec* less;
    struct DTree_rec* more;
    int leaf_class;
} DTree;



#define SWAP(a,b) do{\
        typeof(a) _t_ = a;\
        a = b;\
        b = _t_;\
    }while(0)



int ex3_class[58101];
int ex3_sort[58101];

double ex3_entropy(int begin,int end)
{
    int count[10] = {};
    for(int i=begin; i != end; ++i)
    {
        int pi = ex3_sort[i];
        count[ex3_class[pi]]++;
    }
    int total = end-begin;
    double h = 0;
    for(int i=1;i<=9;i++){
        if (count[i] != 0)
            h+=count[i] * -log2((double)count[i]/ total);
    }
    return total*h;
}

int ex3_partition(int begin,int end,int pivot,int a)
{
    int pp = begin;

    for(int i=begin;i < end; ++i)
    {
        int ir = ex3_sort[i];
        if (ex3_side[ir][a] < ex3_side[pivot][a])
        {
            SWAP(ex3_sort[pp],ex3_sort[ir]);
            pp++;
        }
    }
    //SWAP(ex3_sort[pp],ex3_sort[end-1]);
    return pp;
}


int ex3_cmp_v = 1;
int ex3_cmp(const int* ap,const int* bp)
{
    int a = *ap;
    int b = *bp;
    int da = ex3_side[a][ex3_cmp_v];
    int db = ex3_side[b][ex3_cmp_v];
    if(da == db) return 0;
    if(da < db) return -1;
    return 1;
}


DTree* ex3_create_tree(int begin, int end)
{
    int best_v=0;
    int best_split=0;
    double min_entropy = 1e40;
    DTree* ret = malloc(sizeof(DTree));

    int count[10] = {};
    for(int i=begin; i != end; ++i)
    {
        int pi = ex3_sort[i];
        count[ex3_class[pi]]++;
    }

    ret->leaf_class = 1;
    for(int i=1;i<=7;i++)
    {
        if(count[i] > count[ret->leaf_class])
            ret->leaf_class = i;
    }
    int same = 1;
    for(int i=begin;i<end;i++)
    {
        int ri = ex3_sort[i];
        int cl = ex3_class[ri];
        if(cl!=ret->leaf_class)
        {
            same = 0;
            break;
        }
    }
    if (same){
        return ret;
    }

    double mye = ex3_entropy(begin,end);
    for(int v=0;v<9;v++)
    {
        //printf("v = %i\n",v);
        ex3_cmp_v = v;

        qsort(ex3_sort+begin,end-begin,sizeof(int),ex3_cmp);

        for(int i=begin;i<end;i++)
        {
            int ri = ex3_sort[i];
            if(i!=begin)
            {
                int rl = ex3_sort[i-1];
                if(ex3_side[ri][v]==ex3_side[rl][v])
                    continue;
            }
            double e = ex3_entropy(begin,i)+ex3_entropy(i,end);
            if (e < min_entropy )
            {
                min_entropy = e;
                //printf("entropy %i %f\n",i, min_entropy);
                best_v = v;
                best_split = ex3_side[ri][v];
            }
        }
    }


#if 0
    if(mye - min_entropy < EX3_CUTOFF){
        return ret;
    }
#endif
    ret->leaf_class = 0;

    //printf("%f %f\n",min_entropy, mye);
    assert(min_entropy < mye);
    ret->var = best_v;

    ex3_cmp_v = best_v;
    qsort(ex3_sort+begin,end-begin,sizeof(int),ex3_cmp);
    ret->split = best_split;

    int pp=begin;
    for(;pp<end;pp++)
        if(ex3_side[ex3_sort[pp]][best_v] >= ret->split)
            break;


    //printf("split %i-%i at %i done: x[%i] < %f\n", begin,end,pp, best_v,ret->split);

    ret->less = ex3_create_tree(begin, pp);
    ret->more = ex3_create_tree(pp, end);
    return ret;
}

void ex3_print_tree(DTree* t,FILE* out)
{
    if (t->leaf_class)
    {
        fprintf(out,"%i",t->leaf_class);
    } else
    {
        fprintf(out,"(x[%i] < %i ? ",t->var, t->split);
        ex3_print_tree(t->less,out);
        fprintf(out,":");
        ex3_print_tree(t->more,out);
        fprintf(out,")");
    }
}

int dotnode = 0;
int ex3_tree_dot(DTree* t,FILE* out){
    int r = dotnode++;
    if(t->leaf_class)
    {
        fprintf(out, "n%i [label = \"%i\"] ;\n",r,t->leaf_class);
    }else
    {
        int l = ex3_tree_dot(t->less,out);
        int m = ex3_tree_dot(t->more,out);
        fprintf(out, "n%i [label = \"x%i < %i\"] ;\n",r,t->var,t->split);
        fprintf(out, "n%i -> n%i [label = \"<\"] ;\n",r,l);
        fprintf(out, "n%i -> n%i [label = \"\u2265\"] ;\n",r,m);
    }
    return r;
}
void ex3_print_tree_dot(DTree* t){
    FILE* out = fopen("ex3_tree.dot","w");
    fprintf(out, "digraph dtree{\n");
    ex3_tree_dot(t, out);
    fprintf(out, "}\n");
    fclose(out);
}

void ex3_print_tree_code(DTree* t){
    FILE* out = fopen("model3_tree.h","w");
    fprintf(out,
    "#pragma once\n"
    "int ex3_class(int* x) {\n"
    "   return ");
    ex3_print_tree(t, out);
    fprintf(out,";\n}\n");
    fclose(out);
}
short ex3_array[256][4];
int ex3_makearray(DTree* t, int n) {
//    printf("makearray %d %d\n", n, c);
    ex3_array[n][2] = t->var;
    ex3_array[n][3] = t->split;

    int c=n+1;
    if (t->less->leaf_class) {
        ex3_array[n][0] = -t->less->leaf_class;
    } else {
        ex3_array[n][0] = c;
        c = ex3_makearray(t->less, c);
    }
    if (t->more->leaf_class) {
        ex3_array[n][1] = -t->more->leaf_class;
    } else {
        ex3_array[n][1] = c;
        c = ex3_makearray(t->more, c);
    }
    return c;
}
void ex3_print_array(DTree* t) {
    int n = ex3_makearray(t, 0);
    FILE* out = fopen("model3_tree2.h", "w");
    fprintf(out, "#pragma once\nEx3Node ex3_tree[%d] = { ", n);
    for(int i=0; i<n; ++i) {
        fprintf(out, "{{%d,%d}, %d, %d}, ", ex3_array[i][0], ex3_array[i][1], ex3_array[i][2], ex3_array[i][3]);
    }
    fputs("};\n", out);
    fclose(out);
}

int ex3_tree_classify(DTree* t,int* x)
{
    if(t->leaf_class)return t->leaf_class;
    if(x[t->var] < t->split)
        return ex3_tree_classify(t->less, x);
    return ex3_tree_classify(t->more, x);
}

void ex3_encode() {
    FILE* side = fopen("shuttle.side","r");
    FILE* class = fopen("shuttle.class","r");
    int row=0;
    while(!feof(class)) {
        fscanf(class, "%d\n", &ex3_class[row]);
        for(int i=0; i<9; ++i) {
            fscanf(side, "%d", &ex3_side[row][i]);
        }
        ex3_side[row][9] = 1;
        ex3_sort[row] = row;
        ++row;
    }

    //for(int i=0;i<row;i++)
        //printf("%i ",ex3_class[i]);
    const int SAMPLED = row;
    DTree* tree = ex3_create_tree(0,SAMPLED);
    ex3_print_tree_code(tree);
    ex3_print_tree_dot(tree);

    ex3_print_array(tree);

    double correct = 0;
    for(int i=0;i<row;i++)
    {
        int c = ex3_tree_classify(tree, ex3_side[i]);
        if(c == ex3_class[i])
            correct++;
        else{
#if 0
            for(int j=0;j<9;j++)
                printf("%f ",ex3_side[i][j]);
            printf("-> %i (%i)\n",ex3_class[i],c);
#endif
        }
    }
#if 0
    printf("\n");
    printf("classify rate %f\n",correct/row);
    printf("\n");
#endif
}
#include "model4.c"
int ex4_mat[512][512];
void ex4_encode() {
    aencoder enc;
    aen_init(&enc);
    FILE* in = fopen("kiel.arr", "r");
    for(int i=0; i<512; ++i) for(int j=0; j<512; ++j) fscanf(in, "%d",&ex4_mat[i][j]);

    for(int i=0; i<16; ++i) for(int j=0; j<16; ++j) {
        int x = ex4_mat[i][j] - EX4_S_LOW;
        assert(x>=0);
        assert(x<EX4_S_HIGH-EX4_S_LOW);
        aen_encode_range(&enc, x, x+1, EX4_S_HIGH-EX4_S_LOW);
    }
    for(int i=0, block=0; i<5; ++i) {
        int s = 16<<i;
        for(int j=1; j<4; ++j, ++block) {
//            double alpha = alphas[block], beta = betas[block];
            double alpha=0,beta=0;
            int sy = s*(j&1), sx = s*(j>>1);
            for(int y=0; y<s; ++y) for(int x=0; x<s; ++x) {
                int v = ex4_mat[sy+y][sx+x];
                uint32_t low = ex4_sym2prob(v, alpha, beta);
                uint32_t hi = ex4_sym2prob(v+1, alpha, beta);
//                printf("range %d: %f %f\n", v, (double)low/TOTALPROB, (double)hi/TOTALPROB);
                assert(low<hi);
                assert(hi<TOTALPROB);
                aen_encode_range(&enc, low, hi, TOTALPROB);
            }
            printf("part %d %d done\n", i, j);
        }
    }

    aen_finish(&enc);
    FILE* fout = fopen("ex4_data.bin", "w");
    fwrite(enc.data, 1, enc.di, fout);
    printf("wrote %u bytes\n", enc.di);
    fclose(fout);
}

int main()
{
    ex1_encode();
    ex2_encode();
    ex3_encode();
    ex4_encode();
}
