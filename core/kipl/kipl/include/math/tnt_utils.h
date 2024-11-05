

#ifndef __TNT_UTILS_H
#define __TNT_UTILS_H

#include <tnt_array2d.h>
#include <jama_lu.h>
#include <sstream>
#include <string>

namespace TNT {
template<typename T>
TNT::Array2D<T> invert(const TNT::Array2D<T> &M)
{
    assert(M.dim1() == M.dim2()); // square matrices only please

    // solve for inverse with LU decomposition
    JAMA::LU<T> lu(M);

    // create identity matrix
    TNT::Array2D<T> id(M.dim1(), M.dim2(), (T)0);
    for (int i = 0; i < M.dim1(); i++) id[i][i] = 1;

        // solves A * A_inv = Identity
    return lu.solve(id);
}

template <typename T>
TNT::Array2D<T> transpose(const TNT::Array2D<T> &M)
{
    TNT::Array2D<T> tran(M.dim2(), M.dim1() );
    for(int r=0; r<M.dim1(); ++r)
        for(int c=0; c<M.dim2(); ++c)
            tran[c][r] = M[r][c];
    return tran;
}

template <typename T>
std::string serializeArray2D(const TNT::Array2D<T> &M)
{
    std::ostringstream str;
    for (int r=0; r<M.dim1(); ++r)
    {
       for (int c=0; c<M.dim2(); ++c)
       {
           str<<M[r][c]<<" ";
       }
       if (r<M.dim1()-1)
           str<<"\n";
    }

    return str.str();
}

template <typename T>
std::string serializeArray1D(const TNT::Array1D<T> &M)
{
    std::ostringstream str;
    for (int r=0; r<M.dim1(); ++r)
    {
       str<<M[r]<<" ";
    }

    return str.str();
}
}
#endif
