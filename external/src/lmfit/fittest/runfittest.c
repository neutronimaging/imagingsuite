#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h> // for getopt(3), compare https://bugzilla.kernel.org/show_bug.cgi?id=197375

#include "fittest.h"
#include "lmmin.h"

lm_control_struct control;

typedef struct {
    ffunc_type f;
} eval_data_struct;

void evaluate(
    const double* x, int m, const void* data, double* fvec, int* outcome)
{
    const eval_data_struct* edata = (const eval_data_struct*) data;
    ffunc_type f = edata->f;
    (*f)(x, m, fvec);
}

void minimizer(int n, double* x, int m, double* y, ffunc_type f, int* outcome, int* nfev)
{
    lm_status_struct status;
    eval_data_struct edata;
    edata.f = f;
    lmmin( n, x, m, y, (void*) &edata, evaluate, &control, &status );
    *outcome = status.outcome;
    *nfev = status.nfev;
}

int main(int argc, char **argv)
{
    control = lm_control_double;
    control.patience = 1000;
    // Parse minimizer-specific options.
    int c;
    while ((c = getopt (argc, argv, "he:t:v:")) != -1) {
        switch (c) {
        case 'e':
            control.epsilon = atof(optarg);
            break;
        case 't': {
            double tol = atof(optarg);
            control.ftol = tol;
            control.gtol = tol;
            control.xtol = tol;
            break;
        }
            /*
        case 'p':
            control.patience = atoi(optarg);
            break;
            */
        case 'v':
            control.verbosity = atoi(optarg);
            break;
        case 'h':
            print_help();
        }
    }
    // Run.
    run_tests(argc, argv, minimizer);
}
