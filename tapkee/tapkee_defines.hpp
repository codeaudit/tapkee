/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Copyright (c) 2012, Sergey Lisitsyn, Fernando J. Iglesias García
 *
 * This code uses Any type developed by C. Diggins under Boost license, version 1.0.
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef TAPKEE_DEFINES_H_
#define TAPKEE_DEFINES_H_

#include "utils/any.hpp"
#include "utils/time.hpp"
#include "utils/logging.hpp"
#include <map>
#include <vector>
#include <utility>

#ifndef TAPKEE_DEBUG
	#define EIGEN_NO_DEBUG
#endif
#define EIGEN_RUNTIME_NO_MALLOC
#define EIGEN_DONT_PARALLELIZE
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/SparseCholesky>
//#include <eigen3/Eigen/SuperLUSupport>

#ifdef EIGEN_RUNTIME_NO_MALLOC
	#define RESTRICT_ALLOC Eigen::internal::set_is_malloc_allowed(false);
	#define UNRESTRICT_ALLOC Eigen::internal::set_is_malloc_allowed(true);
#else
	#define RESTRICT_ALLOC
	#define UNRESTRICT_ALLOC
#endif

// Customizable types
#ifdef TAPKEE_CUSTOM_TYPES
	#include TAPKEE_CUSTOM_TYPES
#else
	//! default scalar value (currently only double is supported and tested, float is unstable)
	typedef double DefaultScalarType;
	//! dense vector type 
	typedef Eigen::Matrix<DefaultScalarType,Eigen::Dynamic,1> DenseVector;
	//! dense matrix type
	typedef Eigen::Matrix<DefaultScalarType,Eigen::Dynamic,Eigen::Dynamic> DenseMatrix;
	//! dense symmetric matrix (currently just dense matrix, can be improved later)
	typedef DenseMatrix DenseSymmetricMatrix;
	//! sparse weight matrix type
	typedef Eigen::SparseMatrix<DefaultScalarType> SparseWeightMatrix;
	//! default selfadjoint solver
	typedef Eigen::SelfAdjointEigenSolver<DenseMatrix> DefaultDenseSelfAdjointEigenSolver;
	//! default sparse solver
	typedef Eigen::SimplicialLDLT<SparseWeightMatrix> DefaultSparseSolver;
#endif

#ifdef TAPKEE_CUSTOM_PROPERTIES
	#include TAPKEE_CUSTOM_PROPERTIES
#else
	#define COVERTREE_BASE 1.3
#endif

//! Parameters that are used by the library
enum TAPKEE_PARAMETERS
{
	/* TAPKEE_METHOD */	REDUCTION_METHOD,
	/* unsigned int */ NUMBER_OF_NEIGHBORS,
	/* unsigned int */ TARGET_DIMENSION,
	/* unsigned int */ CURRENT_DIMENSION,
	/* TAPKEE_EIGEN_EMBEDDING_METHOD */ EIGEN_EMBEDDING_METHOD,
	/* TAPKEE_NEIGHBORS_METHOD */ NEIGHBORS_METHOD,
	/* unsigned int */ DIFFUSION_MAP_TIMESTEPS,
	/* DefaultScalarType */ GAUSSIAN_KERNEL_WIDTH,
	/* unsigned int */ MAX_ITERATION,
	/* bool */ SPE_GLOBAL_STRATEGY,
	/* DefaultScalarType */ SPE_TOLERANCE,
	/* unsigned int */ SPE_NUM_UPDATES,
	/* DefaultScalarType */ LANDMARK_RATIO,
	/* DefaultScalarType */ EIGENSHIFT
};
//! Parameters map type
typedef std::map<TAPKEE_PARAMETERS, any> ParametersMap;

//! Dimension reduction method
//! All methods require 
enum TAPKEE_METHOD
{
	//! Locally Linear Embedding / Kernel Locally Linear Embedding.
	//! Requires Mercer kernel callback (linear kernel makes the algorithm a 
	//! standard Locally Linear Embedding) and number of neighbors 
	//! ::NUMBER_OF_NEIGHBORS. 
	//!
	//! References:
	//!
	//! @cite Saul, L. K., Ave, P., Park, F., & Roweis, S. T. (2001).
	//!      An Introduction to Locally Linear Embedding. 290(5500), 2323-2326.
	//!
	//! @cite Decoste, D. (2001). Visualizing Mercer Kernel Feature Spaces 
	//!      Via Kernelized Locally-Linear Embeddings.
	//!      The 8th International Conference on Neural Information Processing ICONIP2001
	//!
	//! @cite Zhao, D. (2006). Formulating LLE using alignment technique.
	//!      Pattern Recognition, 39(11), 2233-2235.
	KERNEL_LOCALLY_LINEAR_EMBEDDING,
	//! Neighborhood preserving embedding.
	//! Requires two callbacks: linear kernel callback and feature vector access callback.
	//! As any other local method requires ::NUMBER_OF_NEIGHBORS parameter to be set.
	//!
	//! References:
	//!
	//! @cite He, X., Cai, D., Yan, S., & Zhang, H.-J. (2005).
	//!       Neighborhood preserving embedding.
	//!       Tenth IEEE International Conference on Computer Vision ICCV05 Volume 1, 2, 1208-1213. Ieee.
	NEIGHBORHOOD_PRESERVING_EMBEDDING,
	//! Local Tangent Space Alignment / Kernel Local Tangent Space Alignment.
	//! Requires Mercer kernel callback (linear kernel makes the algorithm a 
	//! standard Local Tangent Space Alignment) and number of neighbors 
	//! ::NUMBER_OF_NEIGHBORS. 
	//!
	//! References:
	//!
	//! @cite Zhang, Z., & Zha, H. (2002). Principal Manifolds
	//!       and Nonlinear Dimension Reduction via Local Tangent Space Alignment.
	//!       Journal of Shanghai University English Edition, 8(4), 406-424. SIAM.
	KERNEL_LOCAL_TANGENT_SPACE_ALIGNMENT,
	//! Linear Local Tangent Space Alignment. 
	//! Requires two callbacks: linear kernel callback and feature vector access callback.
	//! As any other local method requires ::NUMBER_OF_NEIGHBORS parameter to be set.
	//!
	//! References:
	//!
	//! @cite Zhang, T., Yang, J., Zhao, D., & Ge, X. (2007).
	//!       Linear local tangent space alignment and application to face recognition.
	//!       Neurocomputing, 70(7-9), 1547-1553.
	LINEAR_LOCAL_TANGENT_SPACE_ALIGNMENT,
	HESSIAN_LOCALLY_LINEAR_EMBEDDING,
	LAPLACIAN_EIGENMAPS,
	LOCALITY_PRESERVING_PROJECTIONS,
	DIFFUSION_MAP,
	ISOMAP,
	LANDMARK_ISOMAP,
	MULTIDIMENSIONAL_SCALING,
	LANDMARK_MULTIDIMENSIONAL_SCALING,
	STOCHASTIC_PROXIMITY_EMBEDDING,
	KERNEL_PCA,
	PCA,
	UNKNOWN_METHOD
};

//! Neighbors computation method
enum TAPKEE_NEIGHBORS_METHOD
{
	//! Brute force method with not least than 
	//! \f$ O(N N \log k) \f$ time complexity.
	//! Recommended to be used only in debug purposes.
	BRUTE_FORCE,
	//! Covertree-based method with \f$ O(\log N) \f$ time complexity.
	//! Recommended to be used as a default method.
	COVER_TREE,
	UNKNOWN_NEIGHBORS_METHOD
};

//! Eigendecomposition-based embedding methods enumeration
enum TAPKEE_EIGEN_EMBEDDING_METHOD
{
	//! ARPACK-based method (requires the ARPACK library
	//! binaries to be available around). Recommended to be used as a 
	//! default method. Supports both generalized and standard eigenproblems.
	ARPACK,
	//! Randomized method (implementation taken from the redsvd lib). 
	//! Supports only standard but not generalized eigenproblems.
	RANDOMIZED,
	//! Eigen library dense method (useful for debugging). Computes
	//! all eigenvectors thus can be very slow doing large-scale.
	EIGEN_DENSE_SELFADJOINT_SOLVER,
	UNKNOWN_EIGEN_METHOD
};

// Internal types
#define INTERNAL_VECTOR std::vector
#define INTERNAL_PAIR std::pair

typedef Eigen::Triplet<DefaultScalarType> SparseTriplet;
typedef INTERNAL_VECTOR<SparseTriplet> SparseTriplets;
typedef INTERNAL_VECTOR<unsigned int> LocalNeighbors;
typedef INTERNAL_VECTOR<LocalNeighbors> Neighbors;
typedef INTERNAL_PAIR<DenseMatrix,DenseVector> EmbeddingResult;
typedef INTERNAL_PAIR<DenseMatrix,DenseVector> ProjectionResult;
typedef Eigen::DiagonalMatrix<DefaultScalarType,Eigen::Dynamic> DenseDiagonalMatrix;
typedef INTERNAL_VECTOR<unsigned int> Landmarks;
typedef INTERNAL_PAIR<SparseWeightMatrix,DenseDiagonalMatrix> Laplacian;
typedef INTERNAL_PAIR<DenseSymmetricMatrix,DenseSymmetricMatrix> DenseSymmetricMatrixPair;

#undef INTERNAL_VECTOR
#undef INTERNAL_PAIR

#include "callbacks/traits.hpp"

struct ProjectingImplementation
{
	virtual ~ProjectingImplementation();
	virtual DenseVector project(const DenseVector& vec) = 0;
};

struct ProjectingFunction
{
	ProjectingFunction(ProjectingImplementation* impl) : implementation(impl) {};
	inline DenseVector operator()(const DenseVector& vec)
	{
		return implementation->project(vec);
	}
	ProjectingImplementation* implementation;
};

#endif
