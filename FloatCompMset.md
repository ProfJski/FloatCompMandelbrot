# The Problem in More Detail
This document adds detail to the cursory overview provided in the [readme](https://github.com/ProfJski/FloatCompMandelbrot#readme).

### Preliminary Definitions
(For the sake of clarity, at the expense of stating the obvious.)

The **Mandelbrot Set** is defined as the set of points on the complex plane for which the iterated function F(Z) = Z^2+C remains bounded when iterated. Z is initially equal to zero and C is the point being tested.  If the magnitude of Z is greater than 2, then further iterations will produce unboundedly larger values of Z, so a magnitude check of whether |Z|>2 at every iteration is the standard test to evaluate whether a point is outside the Mandelbrot Set.  Often the points of the Mandelbrot Set are colored black.[^1]  As such, the *Mandelbrot Set* (the black part) is only part of what I call the **Mandelbrot Set Image**, which also includes a color-coded valuation of all points that are unbounded, colored by how many iterations of F(Z) it took a point to escape the boundary check of |F(z)|>2.  While unbounded points are not members of the Mandelbrot Set, they are still interesting.

[^1]: People have developed coloring algorithms for points in the Mandelbrot Set, but we do not employ them here.

The **trajectory** of a point in the Mandelbrot Set Image is the sequence of complex values obtained by iterating Z=Z^2+C.  This sequence may be plotted on the complex plane.  By definition, the trajectory of any point outside the Mandelbrot set will extend beyond a circle of radius two centered on the origin of the complex plane, while for points in the Mandelbrot set, the trajectory remains bounded within this circle.  The trajectory of a point in the set may include a repeating series of values, sometimes called an *orbit*.  

I call **The Mandelbrot Set Algorithm** the computer program whereby one produces a Mandelbrot Set Image.  Several practical factors enter into the pure mathematics of the Mandelbrot set as soon as we turn to its computational implementation, including the way complex numbers are represented and calculated.  Another consideration is how long to iterate.  In computation, it may take many iterations (thousands, even millions) for the trajectory of a point outside the Mandelbrot Set to exceed the boundary check.  Points *in* the Mandelbrot Set **never** will, by definition.  Customarily *for the sake of the algorithm terminating,* if a point's trajectory has not escaped the boundary by a certain specified maximum number of iterations, it is "presumed" to be part of the Mandelbrot Set and colored black.[^2]  Of course, further iterations could prove otherwise, and near the edge of the Mandelbrot set, this is often the case.  A trajectory which would escape the |Z|>2 boundary only after 100,000 iterations of slowly growing larger in its magnitude will be presumed part of the set and the point colored back if an algorithm's max iteration limit is 50K.  A third consideration is what section of the complex plane to plot.  Because of the fractal nature of the Mandelbrot Set, arbitrarily small sections of the complex plane surrounding its boundary can be calculated to produce images of great complexity.  "How deep to zoom" is thus another consideration which bears on how long the algorithm iterates and the precision of the computer in dealing with arbitrarily small quantities.

[^2]: There are other ways of terminating the Mandelbrot Set algorithm besides specifying a maximum iteration, such as checking for a periodic orbit, but we do not employ period checking here, for reasons discussed below.

We define the **parameters** of the Mandelbrot Set Algorithm to be the choices of what points to evaluate (what region of the complex plane, at what level of magnification), how to color them by value (the palette), and the max iterations before presuming a point is in the Mandelbrot Set.  By contrast, we define the **floating point implementation** to refer to how non-integer complex numbers are handled in software and hardware, aside from the aforementioned parameters, because we seek to isolate the effect that decisions about floating point implementation have on the resulting Mandelbrot Set Image.

With these definitions, we may state our goal more exactly.

### Definition of Our Investigation
This program seeks to investigate how different floating point implementations may lead to different Mandelbrot Set Images for the same parameters.  How does floating point implementation affect both the shape of the Mandelbrot Set (i.e., the black points) *and* the shape of the colored points outside the set (where there is also considerable visual complexity), when we calculate the same points in the same region of the complex plane, with the same boundary check, with the same palette, and the same maximum iterations, but using different floating point implementations?

I have yet to see a detailed treatment of this question from the perspective of [numerical analysis](https://en.wikipedia.org/wiki/Numerical_analysis), but several parts of the Mandelbrot Set algorithm are identified in numerical analysis literature as potentially problematic.  I am not a professional mathematician or numerical analyst.  So therefore I must take an empirical approach to the problem to see how extensive an effect floating point implementations may have on Mandelbrot Set Images.  The program presented here is my humble tool for this investigation.  It can benefit from the wisdom of more learned programmers.

If you know of numerical analysis literature which discusses the computation of Mandelbrot Set Images, please let me know!  It surprises me that information is not readily available online, given the fame of the Mandelbrot set, the ubiquity of programs calculating it, and the fact that from its first image, it has been generated on a wide variety of computers.

### Select Online Bibliography
[David Goldberg](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#10061), *What Every Computer Scientist Should Know About Floating-Point Arithmetic*.
  
[L. H. de Figueiredo, D. Nehab, J. Stolfi, and J. B. Oliveira](https://mathr.co.uk/blog/2018-08-20_trustworthy_fractals.html) *Images of the Julia Set That You Can Trust* [Print format](http://webdoc.sub.gwdg.de/ebook/serien/e/IMPA_A/721.pdf)

[Weather Vane on StackExchange](https://math.stackexchange.com/questions/2731582/are-the-intriguing-and-lovely-mandelbrot-set-hoops-and-curls-the-result-of-float) *Are the intriguing and lovely Mandelbrot Set hoops and curls the result of floating point computation inaccuracy?*

[William Kahan's Homepage](https://people.eecs.berkeley.edu/~wkahan/) A world authority on the challenges of floating-point math.  An architect of the IEEE 754 standard.

[The Floating Point Guide](https://floating-point-gui.de/formats/fp/)

[Wikipedia s.v. "Floating Point Arithmetic"](https://en.wikipedia.org/wiki/Floating-point_arithmetic)

[Robert Sedgewick and Kevin Wayne](https://introcs.cs.princeton.edu/java/91float/) 9.1 Floating Point: Textbook excerpt with good discussion of many problems: roundoff error accumulation, catastrophic cancellation, and well-conditioned vs. ill-conditioned problems, all of which apply to Mandelbrot Set algorithms.

[Jeff Arnold, CERN](https://indico.cern.ch/event/626147/attachments/1456066/2247140/FloatingPoint.Handout.pdf), "An Introduction to Floating-Point Arithmetic and Computation" (CERN openlab, 9 May 2017)


## The Inaccuracies of Mandelbrot Representation

### Floating point cannot represent all rational values accurately

### Floating point equality tests are inaccurate
Periodicity checks depend on some arbitary epsilon range to decide whether one value of F(z) is the same as a previous one.  Different bits of precision may affect the result.  Moreover, only exact equality means that F(z) will cycle forever within a fixed boundary.  But suppose instead an orbit which increased in magnitude, ultimately becoming unbounded and |F(Z)|>2, but at an incredibly slow rate, such that millions of iterations were necessary to establish that fact.  Such a point might erroneously be reckoned in the Mandelbrot Set because a periodicity check with too large a floating point epsilon would mistake a very slowly growing orbit for cyclical repetition of the same values.

### Error compounds in repeated floating point operations
Give example from Goldstein; another from Stack Exchange

### Different floating point implementations handle rounding and guard digits differently

### Pixels to Points
The Mandelbrot Set is a set of points on the complex plane.  Points do not have length or width.  The algorithm displays pixels, which are small rectangular areas, not points.  A one-to-one relationship between pixels and points should be established.  Super-sampling is an arbitrary solution to the fact that rectangular pixel spans a limitless number of points on the complex plane.


