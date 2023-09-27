
const float PI = 3.1415926535897932384626433832795f;
const float INV_PI = 0.31830988618379067153776752674503f;
const float POSITIVE_INFINITY = 1e30f;
const float NEGATIVE_INFINITY = -1e30f;
const float EPSILON = 1e-5f;

float3 reflect(float3 v, float3 n) { return v - 2.0f * dot(v, n) * n; }
float3 make_float3(float x, float y, float z) {
  float3 f;
  f.x = x;
  f.y = y;
  f.z = z;
  return f;
}
int3 make_int3(int x, int y, int z) {
  int3 f;
  f.x = x;
  f.y = y;
  f.z = z;
  return f;
}
void swap(float *a, float *b) {
  float tmp = *a;
  *a = *b;
  *b = tmp;
}

/*
    Interval
*/
typedef struct Interval {
  float t_min, t_max;
} Interval_t;

Interval_t make_interval(float t_min, float t_max) {
  Interval_t interval;
  interval.t_min = t_min;
  interval.t_max = t_max;
  return interval;
}

bool interval_contains(const Interval_t *interval, float t) {
  return interval->t_min <= t && t <= interval->t_max;
}

void interval_expand(Interval_t *interval, float t) {
  interval->t_min = interval->t_min < t ? interval->t_min : t;
  interval->t_max = interval->t_max > t ? interval->t_max : t;
}

/*
    Ray
*/
typedef struct Ray {
  float originx, originy, originz;             // 12 bytes
  float directionx, directiony, directionz;    // 12 bytes
  float rdirectionx, rdirectiony, rdirectionz; // 12 bytes
} Ray_t;                                       // 36 bytes

Ray_t make_ray(float3 origin, float3 direction) {
  Ray_t r;
  r.originx = origin.x;
  r.originy = origin.y;
  r.originz = origin.z;
  r.directionx = direction.x;
  r.directiony = direction.y;
  r.directionz = direction.z;
  r.rdirectionx = 1.0f / direction.x;
  r.rdirectiony = 1.0f / direction.y;
  r.rdirectionz = 1.0f / direction.z;
  return r;
}

Ray_t make_ray_from_to(float3 from, float3 to) {
  return make_ray(from, normalize(to - from));
}

float3 ray_point_at(const Ray_t *r, float t) {
  return make_float3(r->originx + t * r->directionx,
                     r->originy + t * r->directiony,
                     r->originz + t * r->directionz);
}

/*
    AABB
*/
typedef struct AABB {
  float minx, miny, minz;
  float maxx, maxy, maxz;
  float extentx, extenty, extentz;
} AABB_t;

AABB_t make_aabb(float3 min, float3 max) {
  AABB_t aabb;
  aabb.minx = min.x;
  aabb.miny = min.y;
  aabb.minz = min.z;
  aabb.maxx = max.x;
  aabb.maxy = max.y;
  aabb.maxz = max.z;
  aabb.extentx = max.x - min.x;
  aabb.extenty = max.y - min.y;
  aabb.extentz = max.z - min.z;
  return aabb;
}

AABB_t make_aabb_from_points(float3 *points, int num_points) {
  AABB_t aabb;
  aabb.minx = POSITIVE_INFINITY;
  aabb.miny = POSITIVE_INFINITY;
  aabb.minz = POSITIVE_INFINITY;
  aabb.maxx = NEGATIVE_INFINITY;
  aabb.maxy = NEGATIVE_INFINITY;
  aabb.maxz = NEGATIVE_INFINITY;
  for (int i = 0; i < num_points; i++) {
    aabb.minx = fmin(aabb.minx, points[i].x);
    aabb.miny = fmin(aabb.miny, points[i].y);
    aabb.minz = fmin(aabb.minz, points[i].z);
    aabb.maxx = fmax(aabb.maxx, points[i].x);
    aabb.maxy = fmax(aabb.maxy, points[i].y);
    aabb.maxz = fmax(aabb.maxz, points[i].z);
  }
  aabb.extentx = aabb.maxx - aabb.minx;
  aabb.extenty = aabb.maxy - aabb.miny;
  aabb.extentz = aabb.maxz - aabb.minz;
  return aabb;
}

void aabb_expand(AABB_t *aabb, float3 point) {
  aabb->minx = fmin(aabb->minx, point.x);
  aabb->miny = fmin(aabb->miny, point.y);
  aabb->minz = fmin(aabb->minz, point.z);
  aabb->maxx = fmax(aabb->maxx, point.x);
  aabb->maxy = fmax(aabb->maxy, point.y);
  aabb->maxz = fmax(aabb->maxz, point.z);
  aabb->extentx = aabb->maxx - aabb->minx;
  aabb->extenty = aabb->maxy - aabb->miny;
  aabb->extentz = aabb->maxz - aabb->minz;
}

bool aabb_contains(const AABB_t *aabb, float3 point) {
  return aabb->minx <= point.x && point.x <= aabb->maxx &&
         aabb->miny <= point.y && point.y <= aabb->maxy &&
         aabb->minz <= point.z && point.z <= aabb->maxz;
}

float3 aabb_centroid(const AABB_t *aabb) {
  return make_float3((aabb->minx + aabb->maxx) * 0.5f,
                     (aabb->miny + aabb->maxy) * 0.5f,
                     (aabb->minz + aabb->maxz) * 0.5f);
}

float aabb_hit(const AABB_t *aabb, const Ray_t *ray, Interval_t interval) {
  if (ray->directionx == 0.0f && ray->directiony == 0.0f &&
      ray->directionz == 0.0f) {
    return POSITIVE_INFINITY;
  }

  float t0 = (aabb->minx - ray->originx) * ray->rdirectionx;
  float t1 = (aabb->maxx - ray->originx) * ray->rdirectionx;
  float t2 = (aabb->miny - ray->originy) * ray->rdirectiony;
  float t3 = (aabb->maxy - ray->originy) * ray->rdirectiony;
  float t4 = (aabb->minz - ray->originz) * ray->rdirectionz;
  float t5 = (aabb->maxz - ray->originz) * ray->rdirectionz;

  float t_min = fmax(fmax(fmin(t0, t1), fmin(t2, t3)), fmin(t4, t5));
  float t_max = fmin(fmin(fmax(t0, t1), fmax(t2, t3)), fmax(t4, t5));
  t_min = fmax(t_min, interval.t_min);
  t_max = fmin(t_max, interval.t_max);
  if (t_max < t_min || t_max < 0.0f) {
    return POSITIVE_INFINITY;
  }
  return t_min;
}

float aabb_surface_area(AABB_t *aabb) {
  return 2.0f * (aabb->extentx * aabb->extenty + aabb->extenty * aabb->extentz +
                 aabb->extentz * aabb->extentx);
}

/*
    Material
*/
typedef struct Material {
  float m_real_relative_permittivity_a;
  float m_real_relative_permittivity_b;
  float m_conductivity_c; // S/m
  float m_conductivity_d; // S/m
} Material_t;             // 16 bytes

/*
    IntersectRecord
*/
struct Triangle;

typedef struct IntersectRecord {
  float pointx, pointy, pointz;               // 12 bytes
  float normalx, normaly, normalz;            // 12 bytes
  struct Triangle *triangle_ptr, *dummy_ptr1; // 16 bytes
  float t;                                    // 4 bytes
  float reserved1;                            // 4 bytes
} IntersectRecord_t;                          // 44 bytes

/*
    Triangle
*/
typedef struct Triangle {
  float p1x, p1y, p1z;             // 12 bytes
  float p2x, p2y, p2z;             // 12 bytes
  float p3x, p3y, p3z;             // 12 bytes
  AABB_t aabb;                     // 36 bytes
  Material_t *mat_ptr, *dummy_ptr; // 16 bytes
} Triangle_t;                      // 88 bytes

float3 tri_get_normal(const Triangle_t *tri) {
  float3 p1 = make_float3(tri->p1x, tri->p1y, tri->p1z);
  float3 p2 = make_float3(tri->p2x, tri->p2y, tri->p2z);
  float3 p3 = make_float3(tri->p3x, tri->p3y, tri->p3z);
  float3 e1 = p2 - p1;
  float3 e2 = p3 - p1;
  return normalize(cross(e1, e2));
}

bool tri_hit(const Triangle_t *tri, const Ray_t *ray,
             const Interval_t *interval, IntersectRecord_t *record) {
  float3 p1 = make_float3(tri->p1x, tri->p1y, tri->p1z);
  float3 p2 = make_float3(tri->p2x, tri->p2y, tri->p2z);
  float3 p3 = make_float3(tri->p3x, tri->p3y, tri->p3z);
  float3 e1 = p2 - p1;
  float3 e2 = p3 - p1;

  float3 ray_dir =
      make_float3(ray->directionx, ray->directiony, ray->directionz);
  float3 ray_origin = make_float3(ray->originx, ray->originy, ray->originz);
  float3 pvec = cross(ray_dir, e2);
  float det = dot(pvec, e1);
  if (fabs(det) < EPSILON) {
    return false;
  }
  float inv_det = 1.0f / det;
  float3 tvec = ray_origin - p1;
  float u = dot(tvec, pvec) * inv_det;
  if (u < 0.0f || u > 1.0f) {
    return false;
  }

  float3 qvec = cross(tvec, e1);
  float v = dot(ray_dir, qvec) * inv_det;
  if (v < 0.0f || u + v > 1.0f) {
    return false;
  }

  float t = dot(e2, qvec) * inv_det;

  if (interval_contains(interval, t)) {
    record->t = t;
    float3 point = ray_point_at(ray, t);
    record->pointx = point.x;
    record->pointy = point.y;
    record->pointz = point.z;
    float3 normal = tri_get_normal(tri);
    record->normalx = normal.x;
    record->normaly = normal.y;
    record->normalz = normal.z;
    record->triangle_ptr = (Triangle_t *)tri;
    return true;
  }
  return false;
}

/*
    BVH
*/
typedef struct BVHNode {
  float3 aabb_min;
  uint left_first;
  float3 aabb_max;
  uint tri_count;
} BVHNode_t;

typedef struct BVHIntance {
  uint dummy1, dummy2;
  float16 transform_point;
  float16 transform_vector;
  float16 inv_transform_point;
  float16 inv_transform_vector;
  uint dummy[9];
} BVHInstance_t;

/*
    TLAS
*/
typedef struct TLASNode {
  float3 aabb_min;
  uint left_first;
  float3 aabb_max;
  uint blas_idx;
} TLASNode_t;

/*
    Helper functions
*/
bool is_scattering(const Material_t mat) { return true; }

/*
    Kernel
*/
// __kernel void coverage_trace(const int cell_size, __global float *vec) {
//   // for (int i = 0; i < cell_size; i++) {
//   int i = get_global_id(0);
//   vec[i] = cell_size;

//   float3 origin;
//   origin = make_float3(0.0f, 1.0f, 0.0f);

//   Triangle_t tri;
//   tri.p1 = make_float3(0.0f, 0.0f, 0.0f);
//   tri.p2 = make_float3(1.0f, 0.0f, 0.0f);
//   tri.p3 = make_float3(0.0f, 1.0f, 0.0f);

//   printf("vec[%d] = %f\n", i, vec[i]);
//   printf("origin = %f, %f, %f\n", origin.x, origin.y, origin.z);
//   // }
// }

__kernel void ray_trace(__global IntersectRecord_t *recs,
                        __global Triangle_t *triangles, __global Ray_t *rays,
                        const float t_min, const float t_max) {
  int i = get_global_id(0);
  Interval_t interval = make_interval(t_min, t_max);
  Triangle_t *tri = &triangles[i];
  Ray_t *ray = &rays[i];
  IntersectRecord_t *record = &recs[i];
  printf("tri->p1 = %f, %f, %f\n", tri->p1x, tri->p1y, tri->p1z);
  printf("tri->p2 = %f, %f, %f\n", tri->p2x, tri->p2y, tri->p2z);
  printf("tri->p3 = %f, %f, %f\n", tri->p3x, tri->p3y, tri->p3z);
  printf("tri->aabb.min = %f, %f, %f\n", tri->aabb.minx, tri->aabb.miny,
         tri->aabb.minz);
  printf("tri->aabb.max = %f, %f, %f\n", tri->aabb.maxx, tri->aabb.maxy,
         tri->aabb.maxz);

  printf("\n");
  printf("size of tri_t = %d\n", sizeof(Triangle_t));
  printf("size of ray_t = %d\n", sizeof(Ray_t));
  printf("size of interval_t = %d\n", sizeof(Interval_t));
  printf("size of record_t = %d\n", sizeof(IntersectRecord_t));
  printf("size of AABB_t = %d\n", sizeof(AABB_t));
  printf("size of Material_t = %d\n", sizeof(Material_t));

  printf("size of float3 = %d\n", sizeof(float3));
  printf("size of float = %d\n", sizeof(float));
  printf("size of pointer to struct Triangle_t = %d\n", sizeof(Triangle_t *));

  printf("\n");
  printf("rays[%d].origin = %f, %f, %f\n", i, ray->originx, ray->originy,
         ray->originz);
  printf("rays[%d].direction = %f, %f, %f\n", i, ray->directionx,
         ray->directiony, ray->directionz);
  printf("rays[%d].rdirection = %f, %f, %f\n", i, ray->rdirectionx,
         ray->rdirectiony, ray->rdirectionz);

  printf("interval.t_min = %f\n", interval.t_min);
  printf("interval.t_max = %f\n", interval.t_max);

  printf("record[%d].point = %f, %f, %f\n", i, record->pointx, record->pointy,
         record->pointz);
  printf("record[%d].normal = %f, %f, %f\n", i, record->normalx,
         record->normaly, record->normalz);
  printf("record[%d].triangle_ptr = %p\n", i, record->triangle_ptr);
  printf("record[%d].t = %f\n", i, record->t);
  printf("record[%d].reserved1 = %f\n", i, record->reserved1);

  tri_hit(tri, ray, &interval, record);
}

// __kernel void ray_trace(__global Triangle_t *triangles) {
//   int i = get_global_id(0);
//   Triangle_t tri = triangles[i];
//   printf("tri.p1 = %f, %f, %f\n", tri.p1.x, tri.p1.y, tri.p1.z);
//   printf("tri.p2 = %f, %f, %f\n", tri.p2.x, tri.p2.y, tri.p2.z);
//   printf("tri.p3 = %f, %f, %f\n", tri.p3.x, tri.p3.y, tri.p3.z);
// }