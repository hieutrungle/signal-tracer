/*
Written by Hieu Le

Part of this is taken from the following source:
https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
Written by Matthew Fisher

A standard 3D plane (space plane.)  Essentially just the surface defined by a*x + b*y + c*z + d = 0
See Plane.h for a description of these functions.
*/

#include "constant.hpp"
#include "hittable.hpp"
#include "glm/glm.hpp"
#include <math.h>

namespace SignalTracer {
    class Plane : public Hittable {
    public:
        Plane() = default;
        Plane(const Plane& P);
        Plane(float a, float b, float c, float d);
        Plane(const glm::vec3& normal, float d);
        Plane(const glm::vec3& point, const glm::vec3& normal);
        Plane(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3);

        virtual glm::vec3 get_normal() const { return m_normal; }
        virtual float get_d() const { return m_d; }

        bool is_hit(const Ray& ray, const Interval& interval, IntersectRecord& record) const override;
        static bool is_hit_pplane(const Plane& plane1, const Plane& plane2, IntersectRecord& record);
        AABB bounding_box() const override { return AABB{}; };
        std::ostream& print(std::ostream& out) const override;

        float calc_distance(const glm::vec3& point) const;
        float calc_signed_distance(const glm::vec3& point) const;
        float calc_unsigned_distance(const glm::vec3& point) const;

        glm::vec3 find_closest_point(const glm::vec3& point) const;

        //dot product of a plane and a 4D vector
        static float dot(const Plane& plane, const glm::vec4& vec4);
        //dot product of a plane and a 3D coordinate
        static float dot(const Plane& plane, const glm::vec3& vec3);
        //dot product of a plane and a 3D normal
        static float dot_normal(const Plane& plane, const glm::vec3& normal);

        // bool fit_to_points(const std::vector<glm::vec3>& points, float& residual_error);
        // bool fit_to_points(const std::vector<glm::vec4>& Points, glm::vec3& basis1, glm::vec3& basis2, float& normal_eigenvalue, float& residual_error);

    private:
        glm::vec3 m_normal{};
        float m_d{ 0.0f };
    };

    //     /*
    //     The remainder of the code in this file implements the function Plane::FitToPoints.
    //     Part of this is taken from:
    //     http://www.google.com/codesearch?hl=en&q=+best+fit+plane+show:HZpynAfUBdA:AwtEUprnjf8:-KEKP5FuK5I&sa=N&cd=1&ct=rc&cs_p=ftp://figment.csee.usf.edu/pub/segmentation-comparison/USF-segger-code.tar.Z&cs_f=calcplane.c#first
    //     */

    //     /*    This routine finds the scatter matrix of a number of points equal
    //     **    to "Points_Total".  The x,y and z coordinates of the points are
    //     **    stored in the "X_Coord", "Y_Coord" and "Z_Coord" arrays.  "centroid"
    //     **    is a 3-element array containing the center of gravity of the set
    //     **    of points.  The scatter matrix will be returned in the 3x3 array
    //     **    called "scatter_mat".  The xyz placement in the Scatter Matrix
    //     **    will be returned by the 3 element array "order", where the index
    //     **    of order indicates the column (and row) in "scatter_mat", and
    //     **    a value of 0 means x, 1 means y, and 2 means z.
    //     */

    //     void Find_ScatterMatrix(
    //         const std::vector<glm::vec4>& points,
    //         const glm::vec3& centroid,
    //         float scatter_mat[3][3],
    //         int order[3]
    //     ) {
    //         int    i, tmp_i;
    //         float    tmp_d;

    //         /*    To compute the correct scatter matrix, the centroid must be
    //         **    subtracted from all points.  If the plane is to be forced to pass
    //         **    through the origin (0,0,0), then the centroid was earlier set
    //         **    equal to (0,0,0).  This, of course, is NOT the true centroid of
    //         **    the set of points!  Since the matrix is symmetrical about its
    //         **    diagonal, one-third of it is redundant and is simply found at
    //         **    the end.
    //         */
    //         for (i = 0; i < 3; i++) {
    //             scatter_mat[i][0] = scatter_mat[i][1] = scatter_mat[i][2] = 0;
    //         }

    //         for (i = 0; i < static_cast<int>(points.size()); i++) {
    //             const glm::vec4& point = points[i];
    //             glm::vec3 d = glm::vec3(point.x, point.y, point.z) - centroid;
    //             float weight = point.w;
    //             scatter_mat[0][0] += d.x * d.x * weight;
    //             scatter_mat[0][1] += d.x * d.y * weight;
    //             scatter_mat[0][2] += d.x * d.z * weight;
    //             scatter_mat[1][1] += d.y * d.y * weight;
    //             scatter_mat[1][2] += d.y * d.z * weight;
    //             scatter_mat[2][2] += d.z * d.z * weight;
    //         }
    //         scatter_mat[1][0] = scatter_mat[0][1];
    //         scatter_mat[2][0] = scatter_mat[0][2];
    //         scatter_mat[2][1] = scatter_mat[1][2];

    //         /*    Now, perform a sort of "Matrix-sort", whereby all the larger elements
    //         **    in the matrix are relocated towards the lower-right portion of the
    //         **    matrix.  This is done as a requisite of the tred2 and tqli algorithms,
    //         **    for which the scatter matrix is being computed as an input.
    //         **    "order" is a 3 element array that will keep track of the xyz order
    //         **    in the scatter_mat.
    //         */
    //         order[0] = 0;        /* Beginning order is x-y-z, as found above */
    //         order[1] = 1;
    //         order[2] = 2;
    //         if (scatter_mat[0][0] > scatter_mat[1][1]) {
    //             tmp_d = scatter_mat[0][0];
    //             scatter_mat[0][0] = scatter_mat[1][1];
    //             scatter_mat[1][1] = tmp_d;
    //             tmp_d = scatter_mat[0][2];
    //             scatter_mat[0][2] = scatter_mat[2][0] = scatter_mat[1][2];
    //             scatter_mat[1][2] = scatter_mat[2][1] = tmp_d;
    //             tmp_i = order[0];
    //             order[0] = order[1];
    //             order[1] = tmp_i;
    //         }
    //         if (scatter_mat[1][1] > scatter_mat[2][2]) {
    //             tmp_d = scatter_mat[1][1];
    //             scatter_mat[1][1] = scatter_mat[2][2];
    //             scatter_mat[2][2] = tmp_d;
    //             tmp_d = scatter_mat[0][1];
    //             scatter_mat[0][1] = scatter_mat[1][0] = scatter_mat[0][2];
    //             scatter_mat[0][2] = scatter_mat[2][0] = tmp_d;
    //             tmp_i = order[1];
    //             order[1] = order[2];
    //             order[2] = tmp_i;
    //         }
    //         if (scatter_mat[0][0] > scatter_mat[1][1]) {
    //             tmp_d = scatter_mat[0][0];
    //             scatter_mat[0][0] = scatter_mat[1][1];
    //             scatter_mat[1][1] = tmp_d;
    //             tmp_d = scatter_mat[0][2];
    //             scatter_mat[0][2] = scatter_mat[2][0] = scatter_mat[1][2];
    //             scatter_mat[1][2] = scatter_mat[2][1] = tmp_d;
    //             tmp_i = order[0];
    //             order[0] = order[1];
    //             order[1] = tmp_i;
    //         }
    //     }



    //     /*
    //     **    This code is taken from ``Numerical Recipes in C'', 2nd
    //     **    and 3rd editions, by Press, Teukolsky, Vetterling and
    //     **    Flannery, Cambridge University Press, 1992, 1994.
    //     **
    //     */

    //     /*
    //     **    tred2 Householder reduction of a float, symmetric matrix a[1..n][1..n].
    //     **    On output, a is replaced by the orthogonal matrix q effecting the
    //     **    transformation. d[1..n] returns the diagonal elements of the
    //     **    tridiagonal matrix, and e[1..n] the off-diagonal elements, with
    //     **    e[1]=0.
    //     **
    //     **    For my problem, I only need to handle a 3x3 symmetric matrix,
    //     **    so it can be simplified.
    //     **    Therefore n=3.
    //     **
    //     **    Attention: in the book, the index for array starts from 1,
    //     **    but in C, index should start from zero. so I need to modify it.
    //     **    I think it is very simple to modify, just substract 1 from all the
    //     **    index.
    //     */

    // #define    SIGN(a,b)    ((b)<0? -fabs(a):fabs(a))

    //     void tred2(float a[3][3], float d[3], float e[3]) {
    //         int        l, k, i, j;
    //         float    scale, hh, h, g, f;

    //         for (i = 3;i >= 2;i--) {
    //             l = i - 1;
    //             h = scale = 0.0;
    //             if (l > 1) {
    //                 for (k = 1;k <= l;k++)
    //                     scale += fabs(a[i - 1][k - 1]);
    //                 if (scale == 0.0)        /* skip transformation */
    //                     e[i - 1] = a[i - 1][l - 1];
    //                 else {
    //                     for (k = 1;k <= l;k++) {
    //                         a[i - 1][k - 1] /= scale;    /* use scaled a's for transformation. */
    //                         h += a[i - 1][k - 1] * a[i - 1][k - 1];    /* form sigma in h. */
    //                     }
    //                     f = a[i - 1][l - 1];
    //                     g = f > 0 ? -sqrt(h) : sqrt(h);
    //                     e[i - 1] = scale * g;
    //                     h -= f * g;    /* now h is equation (11.2.4) */
    //                     a[i - 1][l - 1] = f - g;    /* store u in the ith row of a. */
    //                     f = 0.0;
    //                     for (j = 1;j <= l;j++) {
    //                         a[j - 1][i - 1] = a[i - 1][j - 1] / h; /* store u/H in ith column of a. */
    //                         g = 0.0;    /* form an element of A.u in g */
    //                         for (k = 1;k <= j;k++)
    //                             g += a[j - 1][k - 1] * a[i - 1][k - 1];
    //                         for (k = j + 1;k <= l;k++)
    //                             g += a[k - 1][j - 1] * a[i - 1][k - 1];
    //                         e[j - 1] = g / h; /* form element of p in temorarliy unused element of e. */
    //                         f += e[j - 1] * a[i - 1][j - 1];
    //                     }
    //                     hh = f / (h + h);    /* form K, equation (11.2.11) */
    //                     for (j = 1;j <= l;j++) /* form q and store in e overwriting p. */
    //                     {
    //                         f = a[i - 1][j - 1]; /* Note that e[l]=e[i-1] survives */
    //                         e[j - 1] = g = e[j - 1] - hh * f;
    //                         for (k = 1;k <= j;k++) /* reduce a, equation (11.2.13) */
    //                             a[j - 1][k - 1] -= (f * e[k - 1] + g * a[i - 1][k - 1]);
    //                     }
    //                 }
    //             }
    //             else
    //                 e[i - 1] = a[i - 1][l - 1];
    //             d[i - 1] = h;
    //         }


    //         /*
    //         **    For computing eigenvector.
    //         */
    //         d[0] = 0.0;
    //         e[0] = 0.0;

    //         for (i = 1;i <= 3;i++)/* begin accumualting of transfomation matrices */
    //         {
    //             l = i - 1;
    //             if (d[i - 1]) /* this block skipped when i=1 */
    //             {
    //                 for (j = 1;j <= l;j++) {
    //                     g = 0.0;
    //                     for (k = 1;k <= l;k++) /* use u and u/H stored in a to form P.Q */
    //                         g += a[i - 1][k - 1] * a[k - 1][j - 1];
    //                     for (k = 1;k <= l;k++)
    //                         a[k - 1][j - 1] -= g * a[k - 1][i - 1];
    //                 }
    //             }
    //             d[i - 1] = a[i - 1][i - 1];
    //             a[i - 1][i - 1] = 1.0; /* reset row and column of a to identity matrix for next iteration */
    //             for (j = 1;j <= l;j++)
    //                 a[j - 1][i - 1] = a[i - 1][j - 1] = 0.0;
    //         }
    //     }



    //     /*
    //     **    QL algo with implicit shift, to determine the eigenvalues and
    //     **    eigenvectors of a float,symmetric  tridiagonal matrix, or of a float,
    //     **    symmetric matrix previously reduced by algo tred2.
    //     **    On input , d[1..n] contains the diagonal elements of the tridiagonal
    //     **    matrix. On output, it returns the eigenvalues. The vector e[1..n]
    //     **    inputs the subdiagonal elements of the tridiagonal matrix, with e[1]
    //     **    arbitrary. On output e is destroyed. If the eigenvectors of a
    //     **    tridiagonal matrix are desired, the matrix z[1..n][1..n] is input
    //     **    as the identity matrix. If the eigenvectors of a matrix that has
    //     **    been reduced by tred2 are required, then z is input as the matrix
    //     **    output by tred2. In either case, the kth column of z returns the
    //     **    normalized eigenvector corresponding to d[k].
    //     **
    //     */
    //     void tqli(float d[3], float e[3], float z[3][3]) {
    //         int        m, l, iter, i, k;
    //         float    s, r, p, g, f, dd, c, b;

    //         for (i = 2;i <= 3;i++)
    //             e[i - 2] = e[i - 1];    /* convenient to renumber the elements of e */
    //         e[2] = 0.0;
    //         for (l = 1;l <= 3;l++) {
    //             iter = 0;
    //             do {
    //                 for (m = l;m <= 2;m++) {
    //                     /*
    //                     **    Look for a single small subdiagonal element
    //                     **    to split the matrix.
    //                     */
    //                     dd = fabs(d[m - 1]) + fabs(d[m]);
    //                     if (fabs(e[m - 1]) + dd == dd)
    //                         break;
    //                 }
    //                 if (m != l) {
    //                     if (iter++ == 30) {
    //                         printf("\nToo many iterations in TQLI");
    //                     }
    //                     g = (d[l] - d[l - 1]) / (2.0f * e[l - 1]); /* form shift */
    //                     r = sqrt((g * g) + 1.0f);
    //                     g = d[m - 1] - d[l - 1] + e[l - 1] / (g + SIGN(r, g)); /* this is dm-ks */
    //                     s = c = 1.0;
    //                     p = 0.0;
    //                     for (i = m - 1;i >= l;i--) {
    //                         /*
    //                         **    A plane rotation as in the original
    //                         **    QL, followed by Givens rotations to
    //                         **    restore tridiagonal form.
    //                         */
    //                         f = s * e[i - 1];
    //                         b = c * e[i - 1];
    //                         if (fabs(f) >= fabs(g)) {
    //                             c = g / f;
    //                             r = sqrt((c * c) + 1.0f);
    //                             e[i] = f * r;
    //                             c *= (s = 1.0f / r);
    //                         }
    //                         else {
    //                             s = f / g;
    //                             r = sqrt((s * s) + 1.0f);
    //                             e[i] = g * r;
    //                             s *= (c = 1.0f / r);
    //                         }
    //                         g = d[i] - p;
    //                         r = (d[i - 1] - g) * s + 2.0f * c * b;
    //                         p = s * r;
    //                         d[i] = g + p;
    //                         g = c * r - b;
    //                         for (k = 1;k <= 3;k++) {
    //                             /*
    //                             **    Form eigenvectors
    //                             */
    //                             f = z[k - 1][i];
    //                             z[k - 1][i] = s * z[k - 1][i - 1] + c * f;
    //                             z[k - 1][i - 1] = c * z[k - 1][i - 1] - s * f;
    //                         }
    //                     }
    //                     d[l - 1] = d[l - 1] - p;
    //                     e[l - 1] = g;
    //                     e[m - 1] = 0.0f;
    //                 }
    //             } while (m != l);
    //         };
    //     }

    //     bool Plane::fit_to_points(const std::vector<glm::vec3>& points, float& residual_error) {
    //         glm::vec3 basis1, basis2;
    //         std::vector<glm::vec4> weighted_points(points.size());
    //         for (std::size_t i = 0; i < points.size(); i++) {
    //             weighted_points[i] = glm::vec4(points[i], 1.0f);
    //         }
    //         float normal_eigenvalue;
    //         return fit_to_points(weighted_points, basis1, basis2, normal_eigenvalue, residual_error);
    //     }

    //     bool Plane::fit_to_points(const std::vector<glm::vec4>& points, glm::vec3& basis1, glm::vec3& basis2, float& normal_eigenvalue, float& residual_error) {
    //         glm::vec3 centroid, normal;

    //         float scatter_mat[3][3];
    //         int  order[3];
    //         float DiagonalMatrix[3];
    //         float OffDiagonalMatrix[3];

    //         // Find centroid
    //         centroid = glm::vec3{ 0.0f };
    //         float TotalWeight = 0.0f;
    //         for (std::size_t i = 0; i < points.size(); i++) {
    //             TotalWeight += points[i].w;
    //             centroid += glm::vec3(points[i].x, points[i].y, points[i].z) * points[i].w;
    //         }
    //         centroid /= TotalWeight;

    //         // Compute scatter matrix
    //         Find_ScatterMatrix(points, centroid, scatter_mat, order);

    //         tred2(scatter_mat, DiagonalMatrix, OffDiagonalMatrix);
    //         tqli(DiagonalMatrix, OffDiagonalMatrix, scatter_mat);

    //         /*
    //         **    Find the smallest eigenvalue first.
    //         */
    //         float Min = DiagonalMatrix[0];
    //         float Max = DiagonalMatrix[0];
    //         uint MinIndex = 0;
    //         uint MiddleIndex = 0;
    //         uint MaxIndex = 0;
    //         for (uint i = 1; i < 3; i++) {
    //             if (DiagonalMatrix[i] < Min) {
    //                 Min = DiagonalMatrix[i];
    //                 MinIndex = i;
    //             }
    //             if (DiagonalMatrix[i] > Max) {
    //                 Max = DiagonalMatrix[i];
    //                 MaxIndex = i;
    //             }
    //         }
    //         for (uint i = 0; i < 3; i++) {
    //             if (MinIndex != i && MaxIndex != i) {
    //                 MiddleIndex = i;
    //             }
    //         }
    //         /*
    //         **    The normal of the plane is the smallest eigenvector.
    //         */
    //         for (uint i = 0; i < 3; i++) {
    //             normal[order[i]] = scatter_mat[i][MinIndex];
    //             basis1[order[i]] = scatter_mat[i][MiddleIndex];
    //             basis2[order[i]] = scatter_mat[i][MaxIndex];
    //         }
    //         normal_eigenvalue = std::fabs(DiagonalMatrix[MinIndex]);
    //         basis1.SetLength(DiagonalMatrix[MiddleIndex]);
    //         basis2.SetLength(DiagonalMatrix[MaxIndex]);

    //         if (!basis1.Valid() || !basis2.Valid() || !normal.Valid()) {
    //             auto a = glm::vec3{ 1.0f, 0.0f, 0.0f };
    //             *this = Plane{ centroid, glm::vec3{1.0f, 0.0f, 0.0f} };
    //             basis1 = glm::vec3{ 0.0f, 1.0f, 0.0f };
    //             basis2 = glm::vec3{ 0.0f, 0.0f, 1.0f };
    //         }
    //         else {
    //             *this = Plane{ centroid, normal };
    //             // *this = ConstructFromPointNormal(centroid, normal);
    //         }

    //         residual_error = 0.0f;
    //         for (std::size_t i = 0; i < points.size(); i++) {
    //             residual_error += calc_unsigned_distance(glm::vec3(points[i].x, points[i].y, points[i].z));
    //         }
    //         residual_error /= static_cast<float>(points.size());

    //         return true;
    //     }
}