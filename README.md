Pikchr Command-Line Driver
==========================
This program is a [Unix Way™][Unix] pipeline-friendly command-line driver for
the [C reference implementation][DRH] of [Pikchr][], a [PIC][]-like diagram
language. It reads text from the standard input, replacing suitably-delimited
Pikchr diagrams with inline Scalable Vector Graphics (SVG), and writes to the
standard output. It’s intended for use with your favorite Markdown processor,
but can be used standalone or with other markup languages too.

<div style="max-width:700px"><svg xmlns='http://www.w3.org/2000/svg' viewBox="0 0 700.848 54.72">
<polygon points="164.16,27.36 152.64,31.68 152.64,23.04" style="fill:rgb(0,0,0)"/>
<path d="M2.16,27.36L158.4,27.36"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="83.16" y="15.66" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Markdown+Pikchr</text>
<text x="83.16" y="39.06" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Source</text>
<path d="M171.66,52.56L224.858,52.56A7.5 7.5 0 0 0 232.358 45.06L232.358,9.66A7.5 7.5 0 0 0 224.858 2.16L171.66,2.16A7.5 7.5 0 0 0 164.16 9.66L164.16,45.06A7.5 7.5 0 0 0 171.66 52.56Z"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="198.259" y="17.28" text-anchor="middle" font-family="monospace" fill="rgb(0,0,0)" dominant-baseline="central">pikchr</text>
<text x="198.259" y="37.44" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Filter</text>
<polygon points="394.358,27.36 382.838,31.68 382.838,23.04" style="fill:rgb(0,0,0)"/>
<path d="M232.358,27.36L388.598,27.36"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="313.358" y="15.66" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Markdown+SVG</text>
<text x="313.358" y="39.06" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Intermediate</text>
<path d="M401.858,52.56L524.868,52.56A7.5 7.5 0 0 0 532.368 45.06L532.368,9.66A7.5 7.5 0 0 0 524.868 2.16L401.858,2.16A7.5 7.5 0 0 0 394.358 9.66L394.358,45.06A7.5 7.5 0 0 0 401.858 52.56Z"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="463.363" y="17.28" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Any Markdown</text>
<text x="463.363" y="37.44" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Formatter</text>
<polygon points="694.368,27.36 682.848,31.68 682.848,23.04" style="fill:rgb(0,0,0)"/>
<path d="M532.368,27.36L688.608,27.36"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="613.368" y="15.66" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">HTML+SVG</text>
<text x="613.368" y="39.06" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Output</text>
</svg>
</div>


    ``` pikchr
    arrow right 225% "Markdown+Pikchr" "Source"
    box rad 5px "pikchr" mono "Filter" fit
    arrow same "Markdown+SVG" "Intermediate"
    box same "Markdown" "Formatter" fit
    arrow same "HTML+SVG" "Output"
    ```

GitHub doesn’t allow inline SVGs, so here’s the above diagram as an external
image reference:

<img width="700px" src="usage.svg"/>

Example: Pipeline

    $ pikchr < file.md | Markdown.pl > file.html

Example: Intermediate file

    $ pikchr < README.md.in > README.md
    $ Markdown.pl < README.md > README.html

Example: Compile the first diagram in `README.md.in` to a separate SVG file
called `usage.svg`

    $ pikchr -qb -n 1 < README.md.in > usage.svg

Delimiters
----------
A Pikchr diagram starts with a line beginning with any of the following strings:

* <code>``` pikchr</code>
* `~~~ pikchr`
* `.PS`

and ends with the next line that is any of the following strings exactly:

* <code>```</code>
* `~~~`
* `.PE`

Anything not between a pair of start and end delimiters is copied to the
standard output unchanged.

Note that generic Markdown [fenced code blocks][fenced] can begin with three
_or more_ `~` (`TILDE`) or <code>&#96;</code> (`GRAVE ACCENT` backtick)
characters indented by up to three spaces, and continue to a matching fence.
For simplicity, this tool is more restrictive when recognizing Pikchr diagrams,
requiring exactly three unindented tildes or backticks.

Why C++?
--------
Even though this driver program is trivial, C++ was chosen because the C++11
standard library includes Regular Expressions (for recognizing the start and
end delimiters), and `std::string` is super convenient for accumulating
arbitrary-length C strings in memory to pass to the `pikchr()` C function.

Building
--------
The C reference implementation of Pikchr is linked as a Git submodule, so be
sure to load submodules when cloning this repository. Note the reference
implementation repository includes a ready-to-use C source file; the
submodule doesn’t require its own build step.

    $ git clone --recurse-submodules https://github.com/zenomt/pikchr-cmd
    $ cd pikchr-cmd
    $ make
    $ ./pikchr -h


  [DRH]: https://github.com/drhsqlite/pikchr
  [Pikchr]: https://pikchr.org/
  [PIC]: https://en.wikipedia.org/wiki/PIC_(markup_language)
  [fenced]: https://spec.commonmark.org/0.30/#fenced-code-blocks
  [Unix]: https://en.wikipedia.org/wiki/Unix_philosophy
