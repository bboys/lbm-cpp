double getDensity(const int i, const int j)
{
    int k;
    double result=0.0;
    for (k=0;k<nbNeighbor;k++)
       result+=fin[i][j][k];
    return result;
}

void getVelocity(const int i, const int j, const double density, double *u) 9
{
    int k,d;
    for (d=0;d<nbDim;d++) {
        u[d]=0.0;
        for (k=0;k<nbNeighbor;k++)
            u[d]+=fin[i][j][k]*v[k][d];
            u[d]=u[d]/density;
        }
    }
}