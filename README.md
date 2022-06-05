# FloatCompMandelbrot
What impact does floating point precision have on Mandelbrot set calculations?

## The Problem In A Nutshell
How much is a Mandelbrot set image affected by the floating point precision of the code used to compute it?

1. The points in a region of a Mandelbrot set image have *sensitive dependence upon initial conditions.*  (This property is the "chaos" in "chaos theory.")  A small deviation in the value of C in Z=Z^2+C can lead to a very different outcome for one point versus another point that is only a miniscule distance from it.  This fact remains true at all scales of magnification, i.e., "miniscule" can be arbitrarily small. 
2. Floating point math cannot perfectly represent many rational numbers.  Moreover, floating point math can generate inaccuracies that can snowball with repeated calculations, including repeated operations of multiplication and addition.  Repeated multiplication and addition are the core of the Mandelbrot set algorithm.
3. Since the Mandelbrot set is always generated on a computer, typically using floating point math of some precision or other, how much of a Mandelbrot set image's visual complexity is due to the mathematical properties of the algorithm, and how much is due to floating point inaccuracies in its computer calculation?  Could some of the familiar visual properties of the Mandelbrot set simply be due to the "noise" introduced by floating point calculations that have iterated thousands or even millions of times?

### The Problem at Large
This question has been posed [at least once on Stack Exchange](https://math.stackexchange.com/questions/2731582/are-the-intriguing-and-lovely-mandelbrot-set-hoops-and-curls-the-result-of-float) but the original post was not understood, so no satisfactory answer resulted.

A 2013 academic paper, ["Images of Julia Sets That You Can Trust" ](https://mathr.co.uk/blog/2018-08-20_trustworthy_fractals.html), by L. H. de Figueiredo, D. Nehab, J. Stolfi, and J. B. Oliveira, raises similar questions for Julia sets, which are mathematical relatives of the Mandelbrot set.

### Background
If you are unfamiliar with the limitations of floating point arithmetic, read the classic essay ["What Every Computer Scientist Should Know About Floating-Point Arithmetic"](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html) by David Goldberg for a good overview.

