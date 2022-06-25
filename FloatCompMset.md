# The Problem in More Detail
This document adds detail to the cursory overview provided in the [readme](https://github.com/ProfJski/FloatCompMandelbrot#readme).

## Preliminary Definitions
(For the sake of clarity, at the expense of stating the obvious.)

The **Mandelbrot Set** is defined as the set of points on the complex plane for which the iterated function F(Z) = Z^2+C remains bounded when iterated. Z is initially equal to zero and C is the point being tested.  If the magnitude of Z is greater than 2, then further iterations will produce unboundedly larger values of Z, so a magnitude check of whether |Z|>2 at every iteration is the standard test to evaluate whether a point is outside the Mandelbrot Set.  Often the points of the Mandelbrot Set are colored black.[^1]  As such, the *Mandelbrot Set* (the black part) is only part of what I call the **Mandelbrot Set Image**, which also includes a color-coded valuation of all points that are unbounded, colored by how many iterations of F(Z) it took a point to escape the boundary check of |F(z)|>2.  While unbounded points are not members of the Mandelbrot Set, they are still interesting.

[^1]: People have developed coloring algorithms for points in the Mandelbrot Set, but we do not employ them here.

The **trajectory** of a point in the Mandelbrot Set Image is the sequence of complex values obtained by iterating Z=Z^2+C.  This sequence may be plotted on the complex plane.  By definition, the trajectory of any point outside the Mandelbrot set will extend beyond a circle of radius two centered on the origin of the complex plane, while for points in the Mandelbrot set, the trajectory remains bounded within this circle.  The trajectory of a point in the set may include a repeating series of values, sometimes called a *periodic orbit*.  

I call **The Mandelbrot Set Algorithm** the computer program whereby one produces a Mandelbrot Set Image.  Several practical factors enter into the pure mathematics of the Mandelbrot set as soon as we turn to its computational implementation, including the way complex numbers are represented and calculated.  Another consideration is how long to iterate.  In computation, it may take many iterations (thousands, even millions) for the trajectory of a point outside the Mandelbrot Set to exceed the boundary check.  Points *in* the Mandelbrot Set **never** will, by definition.  Customarily *for the sake of the algorithm terminating,* if a point's trajectory has not escaped the boundary by a certain specified maximum number of iterations, it is "presumed" to be part of the Mandelbrot Set and colored black.[^2]  Of course, further iterations could prove otherwise, and near the edge of the Mandelbrot set, this is often the case.  A trajectory which would escape the |Z|>2 boundary only after 100,000 iterations of slowly growing larger in its magnitude will be presumed part of the set and the point colored back if an algorithm's max iteration limit is 50K.  A third consideration is what section of the complex plane to plot.  Because of the fractal nature of the Mandelbrot Set, arbitrarily small sections of the complex plane surrounding its boundary can be calculated to produce images of great complexity.  "How deep to zoom" is thus another consideration which bears on how long the algorithm iterates and the precision of the computer in dealing with arbitrarily small quantities.

[^2]: There are other ways of terminating the Mandelbrot Set algorithm besides specifying a maximum iteration, such as checking for a periodic orbit, but we do not employ period checking here, for reasons discussed below.

We define the **parameters** of the Mandelbrot Set Algorithm to be the choices of what points to evaluate (what region of the complex plane, at what level of magnification), how to color them by value (the palette), and the max iterations before presuming a point is in the Mandelbrot Set.  By contrast, we define the **floating point implementation** to refer to how non-integer complex numbers are handled in software and hardware, aside from the aforementioned parameters, because we seek to isolate the effect that decisions about floating point implementation have on the resulting Mandelbrot Set Image.

With these definitions, we may state our goal more exactly.

## Definition of Our Investigation
This program seeks to investigate how different floating point implementations may lead to different Mandelbrot Set Images for the same parameters.  How does floating point implementation affect both the shape of the Mandelbrot Set (i.e., the black points) *and* the shape of the colored points outside the set (where there is also considerable visual complexity), when we calculate the same points in the same region of the complex plane, with the same boundary check, with the same palette, and the same maximum iterations, but using different floating point implementations?

I have yet to see a detailed treatment of this question from the perspective of [numerical analysis](https://en.wikipedia.org/wiki/Numerical_analysis), but several parts of the Mandelbrot Set algorithm are identified in numerical analysis literature as potentially problematic.  I am not a professional mathematician or numerical analyst.  So therefore I must take an empirical approach to the problem to see how extensive an effect floating point implementations may have on Mandelbrot Set Images.  The program presented here is my humble tool for this investigation.  It can benefit from the wisdom of more learned programmers.

If you know of numerical analysis literature which discusses the computation of Mandelbrot Set Images, please let me know!  It surprises me that information is not readily available online, given the fame of the Mandelbrot set, the ubiquity of programs calculating it, and the fact that from its first image, it has been generated on a wide variety of computers.

# Sources of Floating Point Variance in the Mandelbrot Algorithm

## Initial Round-Off Error: Assignment of C
Floating point implementations can only represent exactly those rational numbers with a denominator that is a power of two (dyadic rationals) and these dyadics within certain limits determined by the size of the numerator and denominator.  Non-terminating floating point representations of C introduce round-off error from the very first iteration of F(Z).

## Accumulated Round-Off Error
Even with initial values of C that can be exactly represented in floating point form, the problem of round-off error can only be postponed so long for most values of C.  Consider, by analogy, the operation of repeatedly squaring a decimal number like 0.5.  The trajectory of such an operation doubles the number of digits to the right of the decimal point each time: 0.5, 0.25, 0.0625, 0.00390625, etc.  Similarly for the multiply-then-add operation of Z=Z^2+C.  Eventually the number of binary digits which a floating point type can retain is exceeded and round-off error accumulates with every subsequent iteration.  Higher precision (more bits) only postpones this eventuality.  One could attempt to avoid the problem by using arbitrary-precision floating point representation, or rational representation (using two integers, for numerator and denominator, i.e., keep everything as fractions), but that quickly leads to so many significant digits to multiply in every iteration that calculations become enormously slow.  For many points in a Mandelbrot Set Image, the number of significant digits in a trajectory tends to double with each iteration, leading to O(n^2) time complexity scaling with n=number of iterations. Iterations must then be kept very low, which means that the true value of higher-iteration points cannot be discerned.  So accumulated round-off error is hard to avoid.

Volker Schatz [^11] makes the following observation which directly applies to Mandelbrot set orbits: "Careful with iterations.  In many if not most cases numerical computations involve iteration rather than just the evaluation of a single formula. This can cause errors to accumulate, as the chance of an intermediate result not being exactly representable rises. Even if the target solution is an attractive fixed point of your iteration formula, numerical errors may catapult you out of its domain of attraction."

## "Catastrophic Cancellation"
Subtracting two values that are close to each other leads to a large loss in bits of precision called "catastrophic cancellation" in numerical analysis.  See [^3] and [^9] for discussion.

A subtraction is always involved in every iteration of F(Z), since the real component of Z^2 = (a^2-b^2) if we represent Z as (a+bi).  Moreover, depending on the signs of Z's components and C's components, another subtraction may be involved in both the real and imaginary parts of every iteration when C is added to Z^2, since the addition of numbers with opposite signs has the same potentially catastrophic effect as a subtraction.  For a point that iterates thousands of times, how likely is a catastrophic cancellation to happen during the whole trajectory?  (Using the grid overlay on display mode 8 may give some visual indication, since it will happen when the trajectory's points are very close to the diagonal lines of the grid overlay that appears when `Key <G>` is pressed.)

## Floating-point math is not always associative
Depending on the magnitude of the quantities involved, `(a+b)+c` may not equal `a+(b+c)` or `a*(b+c)` may not equal `(a*b)+(a*c)`.  Moreover, the order of operations can affect the loss of precision.  

David Goldberg [^3] explicitly notes that the expression `(x^2 - y^2)` typically results in more loss of precision than if it were calculated as `(x+y)*(x-y)` (except when x>>y or y<<x, see his footnote #7).  As noted above, `x^2-y^2` is calcualted in every iteration as the real component of Z^2.  Arnold [^10] notes that some optimizing compilers will execute `x^2-y^2` as `(x+y)*(x-y)` but Goldberg's analysis indicates that this may not always be an optimal substitution, and only a disassembly of the code will tell what is really happening in processor instructions.

## "Ill-conditioned" vs. "well-conditioned" equations
Both Arnold [^10] and Sedgewick & Wayne [^9] discuss "ill-conditioned" vs. "well-conditioned" equations.  As Arnold defines it: "Well conditioned: small delta-x produces small delta-y.  Ill-conditioned: small delta-x produces large delta-y."  He then defines a way to quantify how ill-conditioned an equation is by its "condition number" which equals abs(delta-y/y)/abs(delta-x/x).  The more chaotic areas of a Mandelbrot Set Image correspond to a high "condition number," which means they are ill-conditioned, i.e., likely to lose precision rapidly.

Sedgewick & Wayne give the Verhulst equation (logistic map) as an example of a system that is ill-conditioned because it displays chaotic behavior for certain values of R.  "The system itself is ill-conditioned, so there is no way to restructure the computation to iterate the system using floating point arithmetic. Although we cannot expect our floating point algorithms to correctly handle ill-conditioned problem, we can ask that they report back an error rangle associated with the solution so that at least we are alerted to potential problems. For example, when solving linear systems of equations (see section 9.5), we can compute something called a condition number: this quantity can be used to bound the error of the resulting solution."

The Mandelbrot Set displays [similar chaotic behavior](https://en.wikipedia.org/wiki/Mandelbrot_set#/media/File:Verhulst-Mandelbrot-Bifurcation.jpg) to the logistic map.  

## Different Rounding Modes
Different computer languages and compilers may handle rounding modes differently.  "Round to nearest even" is a common standard today, but not universally guaranteed.  "Round to nearest" is what it sounds like: the familiar operation of rounding to the nearest number within your limit of precision.  E.g. within 3 decimal digits 0.1282 rounds to 0.128 but 0.3789 rounds to 0.379.  But what happens when a 5 is the terminal digit?  Does 0.0625 round up to 0.063 or down to 0.062, since both are equidistant from 0.0625?  In such cases, "Round to nearest even" rounds to a final digit which is even (here 0.062), which is a simply way of rounding down "about half the time" and rounding up "about half the time," versus always rounding up in such cases, which might lead, over many repeated operations, to a progressive accumulation of error in one direction.  Similarly with always rounding down or truncating.  The latter approach apparently once led to the substantial devaluation of the [Vancouver Stock Market](https://web.ma.utexas.edu/users/arbogast/misc/disasters.html).

If you care to muck around with compiler directives and `#PRAGMA` statements, you can change the rounding mode of the program to see its effect, but "round to nearest even" is the best bet to minimize (but not eliminate) accumulated rounding mode error.

## Floating-point equality comparisons are not precise
Because of floating point inaccuracy, direct tests of equality may fail. See [^7], [Comparison](https://floating-point-gui.de/errors/comparison/).

[David Monniaux](https://arxiv.org/pdf/cs/0701192.pdf), "The Pitfalls of Verifying Floating-Point Computations" discusses vexing and counter-intuitive variations of floating-point computation among IEEE-754-compliant implementations.  In particular, Monniaux wishs to dispel four myths, including "Arithmetic operations are deterministic; that is, if I do `z=x+y` in two places in the same program and my program never touches `x` and `y` in the meantime, then the results should be the same. A variant: “If `x < 1` tests true at one point, then `x < 1` stays true later if I never modify `x`.”

While this issue does not affect our core algorithm, it is a reason why period checking (a/k/a cycle detection) is not employed in our program.

# Program Design
In light of the foregoing, here are some design choices I made in my code, and some thoughts about the "Trustworthiness" [^2] of Mandelbrot Set Images.

## Initial Values: C is dyadic
To avoid introducing round-off error from the very start, pixels correspond to dyadic rational values.  Pixels are plotted on a texture of 1024 x 1024 pixels.  At lower zoom levels, this eliminates all *initial* round-off error. At higher zoom levels, however, the problem resurfaces when the distance between two points is smaller than can be accurately represented by the precision of the floating point type.

Pixel locations are integer values, but how many programs convert them into floating-point complex number components by either scaling them by decimal factors (which may be non-terminating in binary) or dividing them by a screen resolution that is not a power of two?  Doing so will immediately introduce round-off error in the value of C, which is added in every iteration of F(Z).  

## x^2-y^2
As noted above by Goldberg, there is no one-size-fits-all best way to evaluate `x^2-y^2`.  Since our code is not templated, it would be easy to modify one of the msetPoint functions to use `(x^2-y^2)` and another to use `(x+y)*(x-y)`, set them to the same floating-point type and see what difference this programming decision alone makes in the result.

## No period checking
A common speed optimization in Mandelbrot Set calculation involves checking the trajectory to see if prior points in the trajectory have been revisited, a/k/a "cycle detection."  If so, the trajectory forms a loop, which means it will always be bounded because it will cycle through the same set of points forever and therefore the point in question belongs to the Mandelbrot Set.  Cycle detection can often be a much faster way to determine that a point is in the Mandelbrot set than computing the maximum number of interations and presuming it is.  Moreover, it's a certain proof that the point belongs to the set, rather than the presumption that it does because max iterations has been reached.  

Cycle detection would work if floating point quantities could be compared exactly, but they cannot.  It can become difficult to specify a sufficiently small error margin for equality comparisons that distinguish between a truly cyclic trajectory despite floating point inaccuracy versus a trajectory that spirals out very slowly (say, in 100K iterations) and thus has points which are different from each other by only a miniscule amount in every cycle.

For this reason, my program doesn't use period checking / cycle detection, because this floating point inaccuracy would obscure the inaccuracies due to accumulated round-off error, which is the primary effect that we wish to investigate.  An unhappy consequence of this choice is that for points in the Mandelbrot Set, the maximum iterations are always computed, which can be very slow for software-based extended precision types like Boost::Multiprecision.

## (Perhaps obvious but also) no supersampling
The Mandelbrot Set Image represents a set of points on the complex plane by pixels.  Points do not have length or width.  Pixels do; they are small rectangles.  Super-sampling is an arbitary solution to the fact that the rectangular area of a pixel spans an infinite number of points on the complex plane if pixels are understood to represent *areas* rather than *points*.  Often super-sampling is employed simply to provide a smoother image in fast-varying ("chaotic") areas where "noise" in the image is not desired as a matter of asthetic preference. Super-sampling averages the values of many points within the area of the pixel (typically 4, sometimes 9).  We do not employ super-sampling because it obscures the data we are trying to discern: the effect of floating point implementation on the calculation of a single trajectory -- not an averaged value of multiple trajectories.  Instead, we put pixels in a one-to-one relation to the points they represent.  You can think of this as mapping some imaginary point on the pixel, such as its center or corner, onto the complex plane, and making the whole pixel represent that value.


### Select Online Bibliography
[^3]:  [David Goldberg](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#10061), *What Every Computer Scientist Should Know About Floating-Point Arithmetic*.
  
[^4]:  [L. H. de Figueiredo, D. Nehab, J. Stolfi, and J. B. Oliveira](https://mathr.co.uk/blog/2018-08-20_trustworthy_fractals.html) *Images of the Julia Set That You Can Trust* [Print format](http://webdoc.sub.gwdg.de/ebook/serien/e/IMPA_A/721.pdf)

[^5]:  [Weather Vane on StackExchange](https://math.stackexchange.com/questions/2731582/are-the-intriguing-and-lovely-mandelbrot-set-hoops-and-curls-the-result-of-float) *Are the intriguing and lovely Mandelbrot Set hoops and curls the result of floating point computation inaccuracy?*

[^6]:  [William Kahan's Homepage](https://people.eecs.berkeley.edu/~wkahan/) A world authority on the challenges of floating-point math.  An architect of the IEEE 754 standard.

[^7]:  [The Floating Point Guide](https://floating-point-gui.de/formats/fp/)

[^8]:  [Wikipedia s.v. "Floating Point Arithmetic"](https://en.wikipedia.org/wiki/Floating-point_arithmetic)

[^9]:  [Robert Sedgewick and Kevin Wayne](https://introcs.cs.princeton.edu/java/91float/) 9.1 Floating Point: Textbook excerpt with good discussion of many problems: roundoff error accumulation, catastrophic cancellation, and well-conditioned vs. ill-conditioned problems, all of which apply to Mandelbrot Set algorithms.

[^10]:  [Jeff Arnold, CERN](https://indico.cern.ch/event/626147/attachments/1456066/2247140/FloatingPoint.Handout.pdf), "An Introduction to Floating-Point Arithmetic and Computation" (CERN openlab, 9 May 2017)

[^11]:  [Volker Schatz](https://www.volkerschatz.com/science/float.html), "What you never wanted to know about floating point but will be forced to find out"

