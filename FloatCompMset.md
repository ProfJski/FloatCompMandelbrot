# The Problem in More Detail
This document adds detail to the cursory overview provided in the [readme](https://github.com/ProfJski/FloatCompMandelbrot#readme).

### Preliminary Definitions
For the sake of clarity, at the expense of stating the obvious to Mandelbrot set veterans...

The **Mandelbrot Set** is defined as the set of points on the complex plane for which the iterated function F(Z) = Z^2+C remains bounded when iterated with Z initially equal to zero and C as the point being tested.  If the magnitude of the function is greater than 2, i.e., |F(z)|>2, then further iterations will produce unboundededly larger values of Z, so a magnitude check of whether |Z|>2 at every iteration is the standard test to evaluate whether a point is outside the Mandelbrot Set.  Customarily, the bounded points, which are part of the Mandelbrot Set, are colored black.[^1]  As such, the *Mandelbrot Set* (the black part) is only part of what I am calling the **Mandelbrot Set Image**, which also includes a color-coded valuation of all the evaluated points that escaped the boundary check, colored by how many iterations of F(Z) it took a point to escape.  These points are not members of the Mandelbrot Set, but they are still interesting.

[^1]: People have developed coloring algorithms for points in the Mandelbrot Set which reflect some aspect of their trajectory, but we do not employ them here.

It may take many iterations, however, for the orbit of a point to exceed the boundary check, and points *in* the Mandelbrot Set **never** will (by definition), *so for the sake of the algorithm terminating,* if a point has not escaped the boundary by a certain specified maximum number of iterations, it is customarily "presumed" to be part of the Mandelbrot set and colored black as well.[^2]  Of course, further iterations may reveal otherwise, and near the edge of the Mandelbrot set, this is often the case.  An example would be a point which escapes the boundary check only after 100,000 iterations of slowly growing larger.  If our max iterations is 50K, the point will not produce a Z-value such that |Z|>2 during its first 50K iterations, and so the point will end up being "presumed" part of the Mandelbrot Set for max iterations < 100K.  Yet if we iterate longer, it will become apparent that this point's orbit is indeed unbounded and it is not part of the set because after 100K iterations, the value of Z quickly grows to infinity.

[^2]: There are other ways of terminating the Mandelbrot Set algorithm besides specifying a maximum iteration.  We will discuss periodicity checking below.

This program seeks to investigate the effect of floating point implementations on *both* the shape of the Mandelbrot Set itself (i.e., the black points) *and* the shape of the iteration bands of colored points around it, where there is also considerable visual complexity.  In other words, we are interested in the whole Mandelbrot Set Image, as defined above.

I call **The Mandelbrot Set Algorithm** the computational implementation whereby one produces the Mandelbrot Set Image.  One important feature of this algorithm is the way complex numbers are handled, including their floating-point representation.  There are several other important choices to make in designing the algorithm, especially those which decrease computation time, such as the techniques of periodicity checking or the perturbation algorithm.  *Because these time-saving features introduce additional layers of potential inaccuracy, I have intentionally chosen not to employ them here.*  The Mandelbrot Set Algorithm employed here is the simplest and therefore it most directly manifests the way that floating point implementation affects the visual result.  Our Mandelbrot Set Algorithm simply iterates F(z), performs a boundary check, and if the boundary is exceeded, it returns the iteration number, and if not, it continues until max iterations have been performed.  If the boundary has not been exceeded by max iterations, then a value of zero is returned.

I call the **parameters** of the Mandelbrot Set Algorithm the choices of what points to evaluate (some rectangular region of the complex plane, at some level of magnification), how to color them, and the max iterations before presuming a point is in the Mandelbrot Set.

The point is: The Mandelbrot Set is one thing, the Mandelbrot Set Image is another (involving several decisions about its realization as well as its parameters), and the Mandelbrot Set Algorithm is another still (involving further choices of implementation).

We call **"chaotic"** those regions of the complex plane wherein a small variation in C leads to a large variation in result.  "Small" here is relative to the size of the region of the plane under consideration.  

## The Inaccuracies of Mandelbrot Representation

### Floating point cannot represent all rational values accurately

### Floating point equality tests are inaccurate
Periodicity checks depend on some arbitary epsilon range to decide whether one value of F(z) is the same as a previous one.  Different bits of precision may affect the result.  Moreover, only exact equality means that F(z) will cycle forever within a fixed boundary.  But suppose instead an orbit which increased in magnitude, ultimately becoming unbounded and |F(Z)|>2, but at an incredibly slow rate, such that millions of iterations were necessary to establish that fact.  Such a point might erroneously be reckoned in the Mandelbrot Set because a periodicity check with too large a floating point epsilon would mistake a very slowly growing orbit for cyclical repetition of the same values.

### Error compounds in repeated floating point operations
Give example from Goldstein; another from Stack Exchange

### Pixels to Points
The Mandelbrot Set is a set of points on the complex plane.  Points do not have length or width.  The algorithm displays pixels, which are small rectangular areas, not points.  A one-to-one relationship between pixels and points should be established.  Super-sampling is an arbitrary solution to the fact that rectangular pixel spans a limitless number of points on the complex plane.


