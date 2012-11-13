#define EX3_VARS 9
#define EX3_SIZE 58000
#define EX3_CLASS 7
static double kernel(int* a, int* b) {
    double r=1;
    for(int i=0; i<=EX3_VARS; ++i) {
        double x = a[i], y = b[i];
        r *= 1+x*y;
    }
    return r;
}
typedef struct SVM {
    int n;
    double ws[EX3_SIZE];
    int idx[EX3_SIZE];
} SVM;
int ex3_side[EX3_SIZE][EX3_VARS+1];
SVM svms[EX3_CLASS];

static double ppredict(int c, int ix) {
    SVM* svm = &svms[c];
    int* x = ex3_side[ix];
    int n = svm->n;
    double r=0;
    for(int i=0; i<n; ++i) {
        double w = svm->ws[i];
        int j = svm->idx[i];
        r += w * kernel(ex3_side[j], x);
    }
    return r;
}
static void pupdate(int c, int ix, int y) {
    SVM* svm = &svms[c];
    int n = svm->n++;
    svm->ws[n] = y;
    svm->idx[n] = ix;
}
