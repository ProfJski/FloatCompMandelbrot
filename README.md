# FloatCompMandelbrot
What impact does floating point precision have on Mandelbrot set calculations?

## The Problem In A Nutshell
How much is a Mandelbrot set image affected by the floating point precision of the code used to compute it?

1. The points in a region of a Mandelbrot set image have *sensitive dependence upon initial conditions.*  (This property is the "chaos" in "chaos theory.")  A small deviation in the value of C in Z=Z^2+C can lead to a very different outcome for one point versus another point that is only a miniscule distance from it.  This fact remains true at all scales of magnification, i.e., "miniscule" can be arbitrarily small. 
2. Floating point math cannot perfectly represent many rational numbers.  Moreover, floating point math can generate inaccuracies that can snowball with repeated calculations, including repeated operations of multiplication and addition.  Repeated multiplication and addition are the core of the Mandelbrot set algorithm.
3. Since the Mandelbrot set is always generated on a computer, typically using floating point math of some precision or other, we ask: How much of a Mandelbrot set image's visual complexity is due to the mathematical properties of the algorithm, and how much is due to floating point inaccuracies in its computer calculation?  Could some of the familiar visual properties of the Mandelbrot set simply be "noise" introduced by floating point calculations that have iterated thousands or even millions of times?

### The Problem at Large
This question has been posed [at least once on Stack Exchange](https://math.stackexchange.com/questions/2731582/are-the-intriguing-and-lovely-mandelbrot-set-hoops-and-curls-the-result-of-float) but the original post was not understood, so no satisfactory answer resulted.

A 2013 academic paper, ["Images of Julia Sets That You Can Trust" ](https://mathr.co.uk/blog/2018-08-20_trustworthy_fractals.html), by L. H. de Figueiredo, D. Nehab, J. Stolfi, and J. B. Oliveira, raises similar questions for Julia sets, which are mathematical relatives of the Mandelbrot set.

### Background
If you are unfamiliar with the limitations of floating point arithmetic, read the classic essay ["What Every Computer Scientist Should Know About Floating-Point Arithmetic"](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html) by David Goldberg for a good overview.  Basic introductions to the math involved in generating the Mandelbrot set are ubiquitous if you are unfamiliar with the algorithm.

## The Solution in a Nutshell
I wrote easy-to-modify c++ code that simply uses `typedef` to allow the user to explore the Mandelbrot set and to compare how the Mandelbrot set image looks when generated using the same parameters on the same points, but using three different floating-point implementations to calculate the result.  In addition to standard `float` and `double` types, the `Boost::Multiprecision` [cpp_bin_float](https://www.boost.org/doc/libs/1_79_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/cpp_bin_float.html) class was used to implement extended-precision floating-point types of the user's choice.

The program is multi-threaded so it generates the desired Mandelbrot set image in all three float types simultaneously.  Keys `<1>`, `<2>`, and `<3>` display the image as generated by the three different types of floating point precision that the user has selected.  Keys `<4>`, `<5>`, and `<6>` _subtract pairs of images_ to highlight any pixels that are different:

- `Key <4>` -- Subtracts image 1 from 2
- `Key <5>` -- Subtracts image 1 from 3
- `Key <6>` -- Subtracts image 2 from 3

Green pixels indicate a positive value following the subtraction, red pixels a negative value.
Basic statistics of the comparison are given in console output, such as the total number of pixels that are different, the percentage of the total image that is different, and the greatest difference between the values of the pixels (which tends to be close to the max iterations that one has selected for the Mandelbrot algorithm).  The console output also prints basic information about the three types one has chosen at start-up, such as digits of precision.

### An Example
Here are some sample images from a zoom in the neighborhood of C = -0.104943 + 0.927572i. Magnification is approximately 33E+6. The distance between adjacent pixels is 1/(2^33).  Max iterations is 30K.  Here's the image generated with standard `double`:


![Standard double](images/Set3-CppDouble.png)

Here is the same image generated with cpp_bin_float_quad (113 bits of precision).  Superficially it looks largely the same:


![Quad float](images/Set3-CppQuad.png)

But upon subtraction, we find that 8.77% ofthe pixels are different!  92,007 pixels out of 1,048,576 pixels total (or 59.5%) have different values.  These differences include points estimated to be in the Mandelbrot Set in one image and the other, as well as points which exceeded the orbit boundary check (i.e., the magnitude of Z > 2) at different iterations.  Here is the difference between the two images:

![Difference](images/Set3-QuadMinusDouble.png)


## Lines to Customize in the Code
The three floating point types are `typedef`ed as `first_float`, `second_float` and `third_float` throughout the code.  Variable names also have these names as suffixes.  Yes, templated code would be more elegant than coding everything in triplicate.  I began with templated code, but finding the proper arguments to pass to `std::asynch` for templated overloaded functions proved too hard.

### Set your types
Alter these three lines in the code to choose your own floating point implementations and _voila!_ You are all set to go.
```
typedef double first_float;
typedef cpp_bin_float_quad second_float;
typedef cpp_bin_float_oct third_float;
```

### Tweak your multi-threading preferences (optional)
Depending on how many cores you have you may wish to change `unsigned int numThreads;` This setting creates `numThreads` _for each of the three floating point types_.  So `unsigned int numThreads=4;` will start 12 threads initially.  Built-in types (like `float`, `double,` `long double`) go very quickly on most platforms because of intrinsic processor support.  So threads dedicated to those types will finish far more quickly than those using  `Boost::multiprecision`'s `cpp_bin_float` types.  Software-emulated floating-point support is always slower.  That's why they invented FPUs.

Each thread calculates a batch of pixels before rejoining the main thread.  You can tweak the amount per batch here:
```
unsigned int batchSize_first_float=10240;
unsigned int batchSize_second_float=2048;
unsigned int batchSize_third_float=512;
```
This can be useful, for example, for giving you a built-in type image quickly on screen 1 (batching 10K pixels at a time), while allowing different workloads for a 64-bit precision `cpp_bin_float` vs. 500-bit precision `cpp_bin_float` which will take much longer because of all the additional digits to multiply.

## Basic program controls
The mouse left-click is used to recenter the image; keys are used for everything else.  The GUI interface is functional but not as pretty or convenient as professional programs.  This tool is made for research.  Get Ultrafractal or Kalles Fractaler to make beautiful images. ;-)

- **Period** - Zooms in by a factor of two
- **Comma** - Zooms out by a factor of two.  These are the only two zoom controls.
- **M, K, O** - Increase max iterations by 1, 10 or 100, respectively.
- **N, J, I** - Decrease max iterations by 1, 10 or 100, respectively.
- **P** - Pause calculation toggle.  This waits for existing threads to complete and then waits for unpause to spawn new ones, so its effect is not instant if you are working on some computationally intensive threads which must finish first.  Keep your batch sizes small for responsiveness.
- **U** - Unpause.  Doesn't toggle.
- **R** - Revert to prior coordinates for image center.  Works once.
- **C** - Toggle a small semi-transparent indicator of the center of the screen.  Useful for zooming.
- **/** (Forward slash) with mouse over a pixel - Gives coordinate info for that pixel in console


# The Problem and Solution in More Detail: 

## The Inaccuracies of Mandelbrot Representation

### Definitions
The **Mandelbrot Set** is defined as the set of points on the complex plane for which the iterative function F(Z) = Z^2+C remains bounded when iterated with Z initially equal to zero and C as the point being tested.  If the magnitude of the function is greater than 2, i.e., |F(z)|>2, then further iterations will produce unboundededly larger values of Z, so a magnitude check of whether |Z|>2 at every iteration is the standard test to evaluate whether a point is outside the Mandelbrot Set.  Customarily, the bounded points, which are part of the Mandelbrot Set, are colored black.  As such, the *Mandelbrot Set* (the black part) is only part of what I am calling the **Mandelbrot Set Image**, which also includes a color-coded valuation of all the points that escaped the boundary check, colored by how many iterations of F(Z) it took a point to do so.  These points are not members of the Mandelbrot Set, but they are still interesting.

It may take many iterations, however, for the orbit of a point to exceed the boundary check, and points in the Mandelbrot Set never will (by definition), *so for the sake of the algorithm terminating,* if a point has not escaped the boundary by a certain specified maximum number of iterations, it is customarily "presumed" to be part of the Mandelbrot set and colored black as well.  Of course, further iterations may reveal otherwise, and near the edge of the Mandelbrot set, this is often the case.  An example would be a point which escapes the boundary check only after 100,000 iterations of slowly growing larger.  If our max iterations is 50K, the point will not produce a Z-value such that |Z|>2 during its first 50K iterations, and so the point will end up being "presumed" part of the Mandelbrot Set for max iterations < 100K.  Yet if we iterate longer, it will become apparent that this point's orbit is indeed unbounded and it is not part of the set because after 100K iterations, the value of Z quickly grows to infinity.

This program seeks to investigate the effect of floating point implementations on _both_ the shape of the Mandelbrot set (i.e., the black points) and the shape of the iteration bands of colored points around it, where there is also considerable visual complexity.

I call **The Mandelbrot Set Algorithm** the computational implementation whereby one attempts to ascertain whether a certain point on the complex point is in the set.  One important feature of this algorithm is the way complex numbers are handled, including their floating-point representation.  There are several other important choices to make as well, especially those which decrease computation time, such as the techniques of periodicity checking or the perturbation algorithm.  *Because these time-saving features introduce additional layers of potential inaccuracy, I have intentionally chosen not to employ them here.*  The Mandelbrot Set Algorithm employed here is the simplest and therefore it most directly manifests the way that floating point implementation affects the visual result.  Our Mandelbrot Set Algorithm simply iterates F(z), performs a boundary check, and if the boundary is exceeded, it returns the iteration number, and if not, it continues until max iterations have been performed.  If the boundary has not been exceeded by max iterations, then a value of zero is returned.

### Pixels to Points
The Mandelbrot algorithm 








