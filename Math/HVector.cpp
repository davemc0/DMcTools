//////////////////////////////////////////////////////////////////////
// HVector.cpp - Math for long vectors.
//
// Copyright David K. McAllister, Mar. 1998.

#include <Math/HVector.h>

using namespace std;

string HVector::print() const
{
	ASSERT_R(data != NULL);
	
	string s;
	char ii[20];
	
	for(int i=0; i<r; i++)
	{
		s += gcvt(data[i], 15, ii);
		s += " ";
	}
	
	return s;
}

ostream& operator<<(ostream& os, const HVector& h)
{
	os << h.print();
	return os;
}

ostream& operator<<(ostream& os, const Matrix& m)
{
	os << m.print();
	return os;
}

HVector & HVector::rand()
{
	for(int i=0; i<r; i++)
		data[i] = DRand();
	
	return *this;
}

HVector & HVector::nrand(const double sigma)
{
	for(int i=0; i<r; i++)
		data[i] = NRand(sigma);
	
	return *this;
}

// Takes an array of HVectors.
HVector Mean(const HVector *VA, const int count)
{
	ASSERT_R(count > 0);
	
	int dim = VA[0].size();
	
	HVector Mu(dim);
	Mu.zero();
	
	for(int i=0; i<count; i++)
	{
		ASSERT_D(VA[i].size() == dim);
		
		Mu += VA[i];
	}
	
	Mu /= count;
	
	return Mu;
}

Matrix OuterProd(const HVector &A, const HVector &B)
{
	ASSERT_R(A.data != NULL);
	ASSERT_R(B.data != NULL);
	ASSERT_R(A.r == B.r);
	
	Matrix C(A.r, A.r);
	
	for(int i=0; i<A.r; i++)
		for(int j=0; j<A.r; j++)
		{
			C(i, j) = A.data[i] * B.data[j];
		}
		
		return C;
}

// Compute the covariance matrix of this array of vectors.
Matrix Covariance(const HVector *VA, const int count)
{
	ASSERT_R(count > 0);
	
	int dim = VA[0].size();
	Matrix Cov(dim, dim);
	Cov.zero();
	
	HVector Mu = Mean(VA, count);
	HVector *VL = new HVector[count];
	ASSERT_RM(VL, "memory alloc failed");
	
	int i;
	for(i=0; i<count; i++)
	{
		ASSERT_D(VA[i].size() == dim);
		VL[i] = VA[i] - Mu;
	}

	for(i=0; i<dim; i++)
	{
		for(int j=0; j<dim; j++)
		{
			double t = 0;
			for(int k=0; k<count; k++)
				t += VL[k][i] * VL[k][j];
			
			Cov(i, j) += t;
		}
	}
	
	Cov /= double(count);
	
	delete [] VL;
	
	return Cov;
}
