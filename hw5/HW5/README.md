# Programming Assignment V: CUDA Programming

**Parallel Programming by Prof. Yi-Ping You**

### Due date: **23:59, Dec 16, Thursday, 2021**

The purpose of this assignment is to familiarize yourself with CUDA programming.

---

**Table of Contents**
- [Programming Assignment V: CUDA Programming](#programming-assignment-v-cuda-programming)
  - [Due Date](#due-date-2359-dec-16-thursday-2021)
  - [Problem Statement: Paralleling Fractal Generation with CUDA](#1-problem-statement-paralleling-fractal-generation-with-cuda)
  - [Requirements](#2-requirements) 
  - [Grading Policy](#3-grading-policy)
  - [Evaluation Platform](#4-evaluation-platform)
  - [Submission](#5-submission)
  - [References](#6-references)

---

Get the source code:

```shell
$ wget https://nycu-sslab.github.io/PP-f21/HW5/HW5.zip
$ unzip HW5.zip -d HW5
$ cd HW5
```

## 1. Problem Statement: Paralleling Fractal Generation with CUDA

Following [part 2 of HW2](https://nycu-sslab.github.io/PP-f21/HW2/#2-part-2-parallel-fractal-generation-using-stdthread), we are going to parallelize fractal generation by using CUDA.

Build and run the code in the `HW5` directory of the code base. (Type `make` to build, and `./mandelbrot` to run it. `./mandelbrot --help` displays the usage information.)

The following paragraphs are quoted from [part 2 of HW2](https://nycu-sslab.github.io/PP-f21/HW2/#2-part-2-parallel-fractal-generation-using-stdthread).

> This program produces the image file `mandelbrot-test.ppm`, which is a visualization of a famous set of complex numbers called the Mandelbrot set. [Most platforms have a `.ppm` viewer. For example, to view the resulting images, use [tiv](https://github.com/stefanhaustein/TerminalImageViewer.git) command (already installed) to display them on the terminal.]
>
> As you can see in the images below, the result is a familiar and beautiful fractal. Each pixel in the image corresponds to a value in the complex plane, and the brightness of each pixel is proportional to the computational cost of determining whether the value is contained in the Mandelbrot set. To get image 2, use the command option `--view 2`. You can learn more about the definition of the [Mandelbrot set](http://en.wikipedia.org/wiki/Mandelbrot_set).
>
![Mandelbrot Set](https://camo.githubusercontent.com/80f2e33b4e20f3f86809c6203402dc6807b389bc/687474703a2f2f67726170686963732e7374616e666f72642e6564752f636f75727365732f6373333438762d31382d77696e7465722f617373745f696d616765732f61737374312f6d616e64656c62726f745f76697a2e6a7067)

Your job is to parallelize the computation of the images using CUDA. A starter code that spawns CUDA threads is provided in function `hostFE()`, which is located in `kernel.cu`. This function is the host front-end function that allocates the memory and launches a GPU kernel.

Currently `hostFE()` does not do any computation and returns immediately. You should add code to `hostFE()` function and finish `mandelKernel()` to accomplish this task.

The kernel will be implemented, of course, based on `mandel()` in `mandelbrotSerial.cpp`, which is shown below. You may want to customized it for your kernel implementation.

```c
int mandel(float c_re, float c_im, int maxIteration)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < maxIteration; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}
```

## 2. Requirements

1. You will modify only `kernel.cu`, and use it as the template.
2. You need to implement three approaches to solve the questions:
	1. Method 1: Each CUDA thread processes one pixel. Use `malloc` to allocate the host memory, and use `cudaMalloc` to allocate GPU memory. Name the file `kernel1.cu`. (_Note that you are not allowed to use the image input as the host memory directly_)
	2. Method 2: Each CUDA thread processes one pixel. Use `cudaHostAlloc` to allocate the host memory, and use `cudaMallocPitch` to allocate GPU memory. Name the file `kernel2.cu`.
	3. Method 3: Each CUDA thread processes a group of pixels. Use `cudaHostAlloc` to allocate the host memory, and use `cudaMallocPitch` to allocate GPU memory. You can try different size of the group. Name the file `kernel3.cu`.
3. **Q1** What are the pros and cons of the three methods? Give an assumption about their performances. 
4. **Q2** How are the performances of the three methods?
  Plot a chart to show the differences among the three methods
    - for VIEW 1 and VIEW 2, and
    - for different `maxIteration` (1000, 10000, and 100000).

  You may want to measure the running time via the `nvprof` command to get a comprehensive view of performance.
5. **Q3** Explain the performance differences thoroughly based on your experimental results. Does the results match your assumption? Why or why not. 
6. **Q4** Can we do even better? Think a better approach and explain it. Implement your method in `kernel4.cu`.

Answer the questions (marked with "**Q1-Q4**") in a **REPORT** using [HackMD](https://hackmd.io/).
Notice that in this assignment a higher standard will be applied when grading the quality of your report.

## 3. Grading Policy

**NO CHEATING!!** You will receive no credit if you are found cheating.

Total of 100%:
- Implementation correctness: 54%
	- `kernel1.cu`: 18%
	- `kernel2.cu`: 18%
	- `kernel3.cu`: 18%
	- For each kernel implementation,
		- (8%) the output (in terms of both views) should be correct for any `maxIteration` between 256 and 100000, and 
		- (10%) the speedup over the reference implementation should always be greater than 0.6x (kernel1 & kernel2) / 0.3x (kernel3) for `maxIteration` between 256 and 100000 regarding VIEW 1. 
- Competition: 20%
	- Use `kernel4.cu` to compete with your classmates. The competition is judged by the running times of your program for generating both views for `maxIteration` of 100000 with the metric below.
		- VIEW 1: 10%
		- VIEW 2: 10%
- Questions: 26%
	- Q1: 5%
	- Q2: 5%
	- Q3: 10%
	- Q4: 6%

Metric for each view:

$$
\frac{T-Y}{T-F} \times 60\%, \text{if} \; Y < T +
\begin{cases}
  20\%, \text{if} \; Y < F \times 2 \\\\
  40\%, \text{if} \; Y < F \times 1.5
\end{cases}
$$

where $Y$ and $F$ indicate the execution time of your program and the fastest program, respectively, and $T = F \times 1.5$.

## 4. Evaluation Platform

Your program should be able to run on UNIX-like OS platforms. We will evaluate your programs on the workstations dedicated for this course. You can access these workstations by `ssh` with the following information.

The workstations are based on Ubuntu 20.04 with Intel(R) Core(TM) i5-7500 CPU @ 3.40GHz processors and GTX 1060 6GB. `g++-10`, `clang++-11`, and `cuda10.1` have been installed.

|IP|Port|User Name | Password|
|---|---|---|---|
| 140.113.215.195 | 37072 ~ 37080 | {student_id} | {your_passwd} |

Login example:

```shell
$ ssh <student_id>@140.113.215.195 -p <port>
```

You can use the testing script `test_hw5` to check your answer _for reference only_. Run `test_hw5` in a directory that contains your `HW5_XXXXXXX.zip` file on the workstation. 


## 5. Submission

All your files should be organized in the following hierarchy and zipped into a `.zip` file, named `HW5_xxxxxxx.zip`, where `xxxxxxx` is your student ID.

Directory structure inside the zipped file:
- `HW5_xxxxxxx.zip` (root)
	- `kernel1.cu`
	- `kernel2.cu`
	- `kernel3.cu`
	- `kernel4.cu`
	- `url.txt`

Notice that you just need to provide the URL of your HackMD report in `url.txt`, and enable the write permission for someone who knows the URL so that TAs can give you feedback directly in your report.

Zip the file:

```shell
$ zip HW5_xxxxxxx.zip kernel1.cu kernel2.cu kernel3.cu kernel4.cu url.txt
```

Be sure to upload your zipped file to new E3 e-Campus system by the due date.

**You will get _NO POINT_ if your ZIP's name is wrong or the ZIP hierarchy is incorrect.**

## 6. References

1. [CUDA C++ Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html)
2. [cudaMallocPitch API Document](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__MEMORY.html#group__CUDART__MEMORY_1g32bd7a39135594788a542ae72217775c)
3. [CUDA 開發環境設定與簡易程式範例](https://tigercosmos.xyz/post/2020/12/system/cuda-basic/)
4. [CPU 與 GPU 計算浮點數的差異](https://tigercosmos.xyz/post/2020/12/system/floating-number-cpu-gpu/)
