#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <cuda_runtime.h>
//#include <cuda_gl_interop.h>
//#include <cudpp.h> //Cuda Parallel Primitives

//static CUDPPHandle* theCudpp;

extern "C" {
	#include "util.h"
}

union BoidPack {
	char buffer[sizeof(Boid)];
	Boid m_Boid;
};

#define check_cuda_error() {								\
	if (cudaError_t e = cudaGetLastError()) {				\
		fprintf(stderr, "%s:%i: %s\n", __FILE__, __LINE__,	\
				cudaGetErrorString(e));						\
		exit(-1);											\
	}														\
}

__global__ static void neighbourhood(int *neighbours, unsigned int *flags,
		const float *distances, const int n, const int EPSILON) {
	int ix = blockIdx.x * blockDim.x + threadIdx.x,
		iy = blockIdx.y * blockDim.y + threadIdx.y,
		offset = ix + iy * n;
	if (ix < n && iy < n && distances[offset] <= EPSILON) {
		neighbours[offset] = ix;
		flags[offset] = 1;
	}
}
/*
__global__ static void compact(int *neighbours,
		const unsigned int *scanned_flags, const int n) {
	const int delim = INT_MAX;
	int ix = blockIdx.x * blockDim.x + threadIdx.x,
		iy = blockIdx.y * blockDim.y + threadIdx.y,
		offset = iy * n;
//#define scanned_flags(x) tex1Dfetch(flags_texture, (x))
	if (iy < n) {
		if (0 == ix) {
			neighbours[offset + scanned_flags(offset + n - 1)] = delim;
		} else if (ix < n && scanned_flags(offset + ix) >
				scanned_flags(offset + ix - 1)) {
			neighbours[offset + scanned_flags(offset + ix) - 1] =
				neighbours[offset + ix];
		}
	}
#undef scanned_flags
}

static CUDPPHandle prepare_scan_plan(int n, size_t pitch) {
	CUDPPResult r = cudppCreate(theCudpp);
		if (CUDPP_SUCCESS != r) {
		printf("Error creating CUDPPHandle\n");
		exit(-1);
	}
	CUDPPConfiguration config;
	config.datatype = CUDPP_UINT;
	config.algorithm = CUDPP_SCAN;
	config.options = CUDPP_OPTION_FORWARD;
	config.op = CUDPP_ADD;
	CUDPPHandle planhandle = 0;
	CUDPPResult result = cudppPlan(*theCudpp, &planhandle, config, n, n, pitch);
	if (CUDPP_SUCCESS != result) {
		printf("Error creating CUDPPPlan\n");
		exit(-1);
	}
	// Should be cleaned up using `result = cudppDestroyPlan(planhandle);` 
	return planhandle;
}
*/
static void find_neighbours(int *d_neighbours, int n, float *d_distances,
		int eps) {
	static unsigned int *d_flags = NULL;
	const unsigned int blocksize = 16;
	unsigned int flags_bytes = n * n * sizeof(*d_flags);
	//static CUDPPHandle planhandle = 0;
	const dim3 threads(blocksize, blocksize);
	dim3 blocks(n / blocksize, n / blocksize);
	if (!d_flags) {
		size_t pitch;
		cudaMallocPitch((void**) &d_flags, &pitch, n * sizeof(*d_flags), n);
		//planhandle = prepare_scan_plan(n, pitch / sizeof(*d_flags));
		//cudaBindTexture(0, flags_texture, d_flags, n * n * sizeof(*d_flags));
		check_cuda_error();
	}
	cudaMemset(d_flags, 0, flags_bytes);
	neighbourhood<<<blocks, threads>>>(d_neighbours, d_flags, d_distances, n,
			eps * eps);
	//compact<<<blocks, threads>>>(d_neighbours, d_flags, n);
}

__global__ static void count_distance(float *distance, const int n) {
   int ix = blockIdx.x * blockDim.x + threadIdx.x,
	   iy = blockIdx.y * blockDim.y + threadIdx.y,
	   iz = blockIdx.z * blockDim.z + threadIdx.z,
	   off_a = ix * sizeof(Boid), off_b = iy * sizeof(Boid);
   Boid b;//boid_from_texture(off_b);
   Boid a; //boid_from_texture(off_a);
   if (ix < n && iy < n)
	   distance[ix + n * iy] = Square(b.y - a.y) + Square(b.x - a.x);
}

static void reload_distance_cache(float *d_cache, int n) {
	const int blocksize = 16;
	dim3 threads(blocksize, blocksize);
	dim3 blocks(n / blocksize, n / blocksize);
	count_distance<<<blocks, threads>>>(d_cache, n);
}

__device__ static void separation(Boid *boids, int self, const int *neighbours,
		const int n, const float *distance_cache) {
	float x = 0, y = 0, z = 0;
	int count = 0, divisor, i;
	const int weight = 50;
	for (i = 0; neighbours[i] != INT_MAX; ++i) {
		int index = neighbours[i];
		float distance = sqrtf(distance_cache[self + n * index]) + 0.01f;
		x += (boids[self].x - boids[index].x) / distance;
		y += (boids[self].y - boids[index].y) / distance;
		z += (boids[self].z - boids[index].z) / distance;
		++count;
	}
	divisor = count * weight;
	boids[self].fx = x / divisor;
	boids[self].fy = y / divisor;
	boids[self].fz = z / divisor;
}

__device__ static void alignment(Boid *boids, Boid *self, const int *neighbours) {
	float vx = 0, vy = 0, vz = 0;
	int count = 0, i;
	const int weight = 10;
	for (i = 0; neighbours[i] != INT_MAX; ++i) {
		int index = neighbours[i];
		vx += boids[index].vX;
		vy += boids[index].vY;
		vz += boids[index].vZ;
		++count;
	}
	self->fx += vx / count / weight;
	self->fy += vy / count / weight;
	self->fz += vz / count / weight;
}

__device__ static void cohesion(const Boid *boids, Boid *self,
		const int *neighbours) {
	const int weight = 1000;
	float x = 0, y = 0;
	int i;
	for (i = 0; neighbours[i] != INT_MAX; ++i) {
		x += boids[neighbours[i]].x;
		y += boids[neighbours[i]].y;
	}
	x = x / i - self->x;
	y = y / i - self->y;
	self->fx += x / weight;
	self->fy += y / weight;
}

__global__ static void calculate_forces(Boid *boids,
		const float *distance_cache, const int n, const int *neighbours) {
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	if (ix < n && *(neighbours + n * ix) != INT_MAX) {
		separation(boids, ix, neighbours + n * ix, n, distance_cache);
		alignment(boids, boids + ix, neighbours + n * ix);
		cohesion(boids, boids + ix, neighbours + n * ix);
	}
}

static void calculate_all_forces(Boid* d_boids, int n, int eps,
		float *d_distance_cache) {
	const int blocksize = 64;
	int neighbours_bytes = n * n * sizeof(int);
	dim3 threads(blocksize), blocks(n / blocksize);
	static int *d_neighbours = NULL;
	if (!d_neighbours) {
		cudaMalloc((void**) &d_neighbours, neighbours_bytes);
		assert(d_neighbours);
	}
	check_cuda_error();
	find_neighbours(d_neighbours, n, d_distance_cache, eps);
	check_cuda_error();
	calculate_forces<<<blocks, threads>>>(d_boids, d_distance_cache, n,
			d_neighbours);
	check_cuda_error();
}

__global__ static void attraction(Boid* boids, int n, float x, float y, float z) {
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	if (ix < n) {
		const float sin = sinf(TURNING_SPEED), cos = cosf(TURNING_SPEED), nSin = sinf(-TURNING_SPEED), nCos = cosf(-TURNING_SPEED);
		float next_x = boids[ix].x + boids[ix].vX, next_y = boids[ix].y + boids[ix].vY, next_z = boids[ix].z + boids[ix].vZ;
		float det = boids[ix].x * y * z + next_x * boids[ix].y *  + x * next_y
			- x * boids[ix].y - boids[ix].x * next_y - next_x * y;
		if (det > 0) {
			boids[ix].fx += boids[ix].vX - (boids[ix].vX * cos - boids[ix].vY * sin);
			boids[ix].fy += boids[ix].vY - (boids[ix].vX * sin + boids[ix].vY * cos);
			boids[ix].fz += boids[ix].vZ;
		} else if (det < 0) {
			boids[ix].fx += boids[ix].vX - (boids[ix].vX * nCos - boids[ix].vY * nSin);
			boids[ix].fy += boids[ix].vY - (boids[ix].vX * nSin + boids[ix].vY * nCos);
			boids[ix].fz += boids[ix].vZ;
		}
	}
}

static void calculate_attraction(Boid* d_boids, int n, int x, int y) {
	const int blocksize = 64;
	const int z = 0;
	dim3 threads(blocksize), blocks(n / blocksize);
	attraction<<<blocks, threads>>>(d_boids, n, x, y, z);
	check_cuda_error();
}

__device__ static void normalize_speed(Boid* boids) {
	const float SQUARE_LIMIT = Square(MAX_SPEED);
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	float speedSquared = Square(boids[ix].vX) + Square(boids[ix].vY) + Square(boids[ix].vZ);
	if (speedSquared > SQUARE_LIMIT) {
		float coeff = MAX_SPEED / sqrtf(speedSquared);
		boids[ix].vY *= coeff;
		boids[ix].vX *= coeff;
		boids[ix].vZ *= coeff;
	}
}

__global__ static void apply_forces(Boid* boids, float dt, int width, int height, int depth) {
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	boids[ix].vX += boids[ix].fx * dt;
	boids[ix].vY += boids[ix].fy * dt;
	boids[ix].vZ += boids[ix].fz * dt;
	boids[ix].fx = boids[ix].fy = 0;
	normalize_speed(boids);
	boids[ix].x += boids[ix].vX * dt;
	if (boids[ix].x >= width)
		boids[ix].x -= width;
	else if (boids[ix].x < 0)
		boids[ix].x += width;
	boids[ix].y += boids[ix].vY * dt;
	if (boids[ix].y >= height)
		boids[ix].y -= height;
	else if (boids[ix].y < 0)
		boids[ix].y += height;
	boids[ix].z += boids[ix].vZ * dt;
	if (boids[ix].z >= depth)
		boids[ix].z -= depth;
	else if (boids[ix].z < 0)
		boids[ix].z += depth;
}

static void apply_all_forces(Boid* pBoids, int n, float dt, int width, int height, int depth) {
	const int SIZE = 64;
	dim3 threads(SIZE), blocks(n / SIZE);
	apply_forces<<<blocks, threads>>>(pBoids, dt, width, height, depth);
	check_cuda_error();
}

static float* prepare_distance_cache(int n) {
	float *d_distance_cache = NULL;
	assert(n > 0);
	cudaMalloc((void**) &d_distance_cache, n * n * sizeof(float));
	assert(d_distance_cache);
	return d_distance_cache;
}

static Boid * prepare_device_boids(int n) {
	int boidsBytes = n * sizeof(Boid);
	Boid *pBoids = NULL;
	assert(n > 0);
	cudaMalloc((void**) &pBoids, boidsBytes);
	assert(pBoids);
	//cudaBindTexture(0, boids_texture, pBoids, n * sizeof(Boid));
	check_cuda_error();
	return pBoids;
}

__global__ static void init_kernel(Boid *boids, int n) {
	int ix = blockIdx.x * blockDim.x + threadIdx.x;
	if (ix < n)
		boids[ix].x = boids[ix].y + (ix % 3);
}

static void CountIntensity(Sim *sp) {
	int i, j, max = 3;
	float coeff = 0xff;
	memset(sp->intensity, 0, sizeof(char) * sp->width * sp->height);
	for (i = 0; i < sp->n; ++i) {
		Boid b = sp->boids[i];
		int value = ++sp->intensity[((int) b.y) * sp->width + ((int) b.x)];
		if (value > max)
			max = value;
	}
	coeff /= max;
	#pragma omp parallel for private(j)
	for (i = 0; i < sp->height; ++i) {
		for (j = 0; j < sp->width; ++j)
			sp->intensity[i * sp->width + j] *= coeff;
	}
}

void simulate(Sim *sp) {
	static float *d_distance_cache = NULL;
	static Boid *pBoids = NULL;
	int boids_bytes = sp->n * sizeof(Boid);
	if (!d_distance_cache) {
		d_distance_cache = prepare_distance_cache(sp->n);
		pBoids = prepare_device_boids(sp->n);
		init_kernel<<<64, 64>>>(pBoids, sp->n);
		cudaMemcpy(pBoids, sp->boids, boids_bytes, cudaMemcpyHostToDevice);
		check_cuda_error();
	}
	reload_distance_cache(d_distance_cache, sp->n);
	calculate_all_forces(pBoids, sp->n, sp->eps, d_distance_cache);
	if (sp->attractors)
		calculate_attraction(pBoids, sp->n, sp->attractors->x,
				sp->attractors->y);
	apply_all_forces(pBoids, sp->n, sp->dt, sp->width, sp->height, sp->depth);
	cudaThreadSynchronize();
	check_cuda_error();
	cudaMemcpy(sp->boids, pBoids, boids_bytes, cudaMemcpyDeviceToHost);
	check_cuda_error();
	CountIntensity(sp);
}
