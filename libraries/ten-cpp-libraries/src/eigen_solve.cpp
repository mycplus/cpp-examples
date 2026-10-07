// eigen_solve.cpp - linear algebra with Eigen: solve Ax = b, then fit a
// straight line to noisy points by least squares. Header-only.
#include <Eigen/Dense>
#include <iomanip>
#include <iostream>

int main()
{
    Eigen::Matrix3d A;
    A << 2, 1, -1,
        -3, -1, 2,
        -2, 1, 2;
    Eigen::Vector3d b(8, -11, -3);
    Eigen::Vector3d x = A.partialPivLu().solve(b);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "x = " << x.transpose() << '\n';
    std::cout << "residual |Ax - b| = " << (A * x - b).norm() << '\n';

    // Least squares: y = m t + c through five points.
    Eigen::VectorXd t(5), y(5);
    t << 0, 1, 2, 3, 4;
    y << 1.1, 2.9, 5.2, 7.1, 8.8;
    Eigen::MatrixXd M(5, 2);
    M.col(0) = t;
    M.col(1).setOnes();
    Eigen::Vector2d mc = M.colPivHouseholderQr().solve(y);
    std::cout << std::setprecision(3)
              << "best fit: y = " << mc(0) << " t + " << mc(1) << '\n';
}
