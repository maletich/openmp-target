#define N 5000
#include "timer.h"

/*
  Multiplies two matrices of dimension n x n and passes back resulting matrix.
 */
template<typename T>                                     
void gemm(int n, T alpha, const T* __restrict__ A, const T* __restrict__ B, T* __restrict__ result)
{
#pragma omp target teams distribute collapse(2) map(to:A[:n*n], B[:n*n]) map(from:result[:n*n])  
  for (int row = 0; row < n; row++)                                              
    for (int col = 0; col < n; col++)
      {
	const T* __restrict__ A_row = A + row * n;
	T sum(0);
	const T* __restrict__ B_col = B + col;
#pragma omp parallel for reduction(+:sum)
	for(int i = 0; i < n; i++)
	  {
	    sum += A_row[i] * B_col[i * n];
	  }
	const int index = (row * n) + col;
	result[index] = sum * alpha;
      } 
}

/*
  Prints 1 dimensional matrix of dimension n x n.
 */
template<typename T>
void printMatrix(int n, T* __restrict__ A)
{
  for(int i = 0; i < n * n; i++)
    std::cout << A[i];
}

/*
  Creates 1 dimensional matrix of size n and fills with T(1).
 */
template<class T>
T* allocate(size_t n)
{                                                                                                     
  T* ptr = new T[n];                                                                     
  std::fill_n(ptr, n, T(1));                                            
#pragma omp target enter data map(to : ptr[:n])
  return ptr;                                                                                        
}

/*
  Frees up space from 1 dimensional matrix.
 */
template<class T>
void deallocate(T* ptr, size_t n)
{
#pragma omp target exit data map(delete : ptr[:n])
  delete[] ptr;
}

void test(int n)
{
  std::cout << "Testing variable matrix fill.\n";
  int dim = n;
  auto* A = allocate<float>(dim * dim);
  auto* B = allocate<float>(dim * dim);
  auto* C = allocate<float>(dim * dim);

  for(int i = 0; i < dim * dim; i++)
    {
      A[i] = i + i;
      B[i] = i + i;
    }

  Timer local("GEMM");
  gemm(dim, 1.0f, A, B, C);

  
  deallocate(A, dim * dim);
  deallocate(B, dim * dim);
  deallocate(C, dim * dim);
  
}

void testtbt()
{
std::cout << "Testing 3x3 matrix multiplication.\n";
  int dim = 3;
  auto* C = allocate<float>(dim * dim);
  auto* D = allocate<float>(dim * dim);
  auto* R = allocate<float>(dim * dim);
  std::cout << "Result calculated by hand: 010202010\n";
  for(int i = 0; i < dim * dim; i++)
    {
      if( i % 2 == 0)
        {
          C[i] = 0;
          D[i] = 1;
        } else
        {
          C[i] = 1;
          D[i] = 0;
        }
    }

  gemm(dim, 1.0f, C, D, R);

  std::cout << "Matrix C: "; printMatrix(dim, C); std::cout << "\n";
  std::cout << "Matrix D: "; printMatrix(dim, D); std::cout << "\n";
  std::cout << "Matrix R: "; printMatrix(dim, R); std::cout << "\n";

  deallocate(C, dim * dim);
  deallocate(D, dim * dim);
  deallocate(R, dim * dim);
}

int main()
{
  auto* A    = allocate<float>(N * N);                                                                        
  auto* B    = allocate<float>(N * N);                                                  
  auto* result = allocate<float>(N * N);

  // Debugging
  //std::cout << "Matrix A: "; printMatrix(N, A); std::cout << "\n";
  //std::cout << "Matrix B: "; printMatrix(N, B); std::cout << "\n";

  /*
  Timer local("GEMM");
  gemm(N, 1.0f, A, B, result);
  */

  test(N);

  // Debugging
  //std::cout << "Matrix Result: "; printMatrix(N, result); std::cout << "\n";
   
  deallocate(A, N * N);
  deallocate(B, N * N);
  deallocate(result, N * N);

}