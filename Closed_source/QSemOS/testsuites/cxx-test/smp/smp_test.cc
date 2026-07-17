#include <iostream>
#include <stdexcept>
#include <string>
#include <Eigen/Dense>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include <time.h>

class Shape {
public:
    virtual double calculateArea() const = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {
        if(r <= 0) throw std::invalid_argument("Radius must be positive");
    }
    double calculateArea() const override {
        return 3.14159 * radius * radius;
    }
};

template<typename T>
class Container {
    T* elements;
    size_t capacity;
public:
    Container(size_t size) : capacity(size), elements(new T[size]) {}
    ~Container() { delete[] elements; }
    
    T& operator[](size_t index) {
        if(index >= capacity) throw std::out_of_range("Index out of bounds");
        return elements[index];
    }
};

void cxx_test_func()
{
    Shape *shapes[] = {new Circle(2.5), new Circle(1.0)};
    for(auto shape : shapes) {
        try {
            std::cout << "Area: " << shape->calculateArea() << std::endl;
            delete shape;
        } catch(const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            delete shape;
        }
    }

    Container<double> container(3);
    container[0] = 3.14;
    container[1] = 2.71;
    std::cout << "Element 0: " << container[0] << std::endl;

    try {
        Shape *shape = new Circle(-1.0);
    } catch(const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    try {
        container[5] = 1.618;
    } catch(const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    std::cout << "cxx func test success" << std::endl;
}

void eigen_test_func()
{
    std::cout << "=== Matrix Operations ===" << std::endl;
    Eigen::Matrix3d A;
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9;
    Eigen::Matrix3d B = Eigen::Matrix3d::Random();
    
    std::cout << "Matrix A:\n" << A << std::endl;
    std::cout << "Matrix B (random):\n" << B << std::endl;
    std::cout << "A + B:\n" << A + B << std::endl;
    std::cout << "A * B:\n" << A * B << std::endl;
    std::cout << "A.transpose():\n" << A.transpose() << std::endl;

    std::cout << "\n=== Vector Operations ===" << std::endl;
    Eigen::Vector3d v(1, 2, 3);
    Eigen::Vector3d w(4, 5, 6);
    
    std::cout << "Vector v:\n" << v << std::endl;
    std::cout << "Vector w:\n" << w << std::endl;
    std::cout << "Dot product: " << v.dot(w) << std::endl;
    std::cout << "Cross product:\n" << v.cross(w) << std::endl;

    std::cout << "\n=== Linear Equation Solving ===" << std::endl;
    Eigen::Vector3d b(1, 1, 1);
    Eigen::Vector3d x = A.colPivHouseholderQr().solve(b);
    std::cout << "Solution x to Ax = b:\n" << x << std::endl;
    std::cout << "Residual norm: " << (A * x - b).norm() << std::endl;

    std::cout << "\n=== Eigenvalue Computation ===" << std::endl;
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver(A);
    if (eigensolver.info() != Eigen::Success) {
        std::cout << "Eigenvalue computation failed!" << std::endl;
    } else {
        std::cout << "Eigenvalues:\n" << eigensolver.eigenvalues() << std::endl;
        std::cout << "Eigenvectors:\n" << eigensolver.eigenvectors() << std::endl;
    }

    std::cout << "eigen func test success" << std::endl;
}

extern "C" {
#include <prt_task.h>
#include <prt_sys.h>
#include <prt_sem.h>
#include <prt_hwi.h>
#include <rpmsg_backend.h>

#define STR_SIZE 1024 * 1024

SemHandle smp_msg_sem;
bool smp_data_finish = false;
char *rcv_buffer = NULL , *result_buffer = NULL;
int data_len = 0;
int cxx_test_result = 0;

void create_random_char(char *buffer)
{
    for (int i = 0; i < STR_SIZE; i++) {
        buffer[i] = 'B' + rand() % 26;
    }
}

// 通信接口测试用例
void test_smp_rcv_data()
{
    printf("test_smp_rcv_data: start\n");
    char msg[] = "rtos rcv ok";
    int ret = 0, i = 0;

    srand(time(NULL));
    rcv_buffer = (char *)malloc(STR_SIZE + 1);
    if (rcv_buffer == NULL) {
        printf("test_smp_rcv_data: rcv_buffer malloc failed\n");
    }

    result_buffer = (char *)malloc(STR_SIZE + 1);
    if (result_buffer == NULL) {
        printf("test_smp_rcv_data: result_buffer malloc failed\n");
    }

    rcv_buffer[STR_SIZE] = 0;
    result_buffer[STR_SIZE] = 0;
    create_random_char(result_buffer);

    while(1) {
        memset(rcv_buffer, 0, STR_SIZE);
        ret = rcv_data_from_nrtos(rcv_buffer, &data_len);
        if (ret != 0)
            break;
        printf("test_smp_rcv_data: rcv buffer %s data_len %d \n", rcv_buffer + STR_SIZE - 10, data_len);
        PRT_SemPost(smp_msg_sem);
        PRT_TaskDelay(100);
        result_buffer[STR_SIZE - 1] = '0' + cxx_test_result;
        printf("test_smp_rcv_data: result buffer %s data_len %d \n", result_buffer + STR_SIZE - 10, STR_SIZE);
        send_data_to_nrtos(result_buffer, STR_SIZE);
    }
}

// C++测试用例，包含C++基础功能与Eigen功能测试
void test_smp_cxx()
{
    printf("test_smp_cxx: start\n");
    while(1) {
        PRT_SemPend(smp_msg_sem, OS_WAIT_FOREVER);
        printf("test_smp_cxx: rcv data len from nrtos %d\n", data_len);
        cxx_test_func();
        eigen_test_func();
        cxx_test_result++;
        printf("test_smp_cxx: cxx test result %d\n", cxx_test_result);
    }
}

// libxml2测试用例
#if defined(OS_SUPPORT_LIBXML2)
int uniproton_testapi(int argc, char **argv);
int uniproton_testchar(void);
int uniproton_testdict(void);
int uniproton_testrecurse(int argc, char **argv);

static int uniproton_testchar_arg(int argc, char **argv) {
    return uniproton_testchar();
}

static int uniproton_testdict_arg(int argc, char **argv) {
    return uniproton_testdict();
}

int uniproton_runtest(int argc, char **argv);

typedef int (*test_fn)(int argc, char **argv);

typedef struct test_case {
    char *name;
    test_fn fn;
    int skip;
} test_case_t;

#define TEST_CASE(func, s) {         \
    .name = #func,                   \
    .fn = func,                      \
    .skip = s                        \
}

#define TEST_CASE_Y(func) TEST_CASE(func, 0)
#define TEST_CASE_N(func) TEST_CASE(func, 1)

static test_case_t g_cases[] = {
    TEST_CASE_Y(uniproton_runtest),
    TEST_CASE_Y(uniproton_testrecurse),
    TEST_CASE_Y(uniproton_testchar_arg),
    TEST_CASE_Y(uniproton_testdict_arg),
    TEST_CASE_Y(uniproton_testapi),
};

void test_smp_xml2()
{
    int i = 0;
    int cnt = 0;
    int fails = 0;
    int len = sizeof(g_cases) / sizeof(test_case_t);
    printf("\n===test start=== \n");
    for (i = 0, cnt = 0; i < len; i++) {
        test_case_t *tc = &g_cases[i];
        printf("\n===%s start === \n", tc->name);
        if (tc->skip) {
            continue;
        }
        cnt++;
        int ret = tc->fn(0, NULL);
        if (ret) {
            fails++;
            printf("\n===%s failed ===\n", tc->name);
        } else {
            printf("\n===%s success ===\n", tc->name);
        }
    }
    printf("\n===test end, total: %d, fails:%d ===\n", cnt, fails);
}
#endif
}