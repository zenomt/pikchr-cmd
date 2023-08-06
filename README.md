Pikchr Command-Line Driver
==========================
[This preprocessor tool][repo] reads text from the standard input, [replacing][Unix]
suitably-delimited [Pikchr][] diagrams (a [PIC][]-like diagram language) with
inline Scalable Vector Graphics (SVG), and writes to the standard output.
It’s intended for use with your favorite Markdown processor, but can be used
standalone or with other markup languages too.

This tool uses the [C reference implementation of Pikchr by Richard Hipp][DRH]
which is included here for convenience (as permitted by its generous license).

<div style="max-width:755px"><svg xmlns='http://www.w3.org/2000/svg' viewBox="0 0 755.453 217.44">
<polygon points="164.16,37.44 152.64,41.76 152.64,33.12" style="fill:rgb(0,0,0)"/>
<path d="M2.16,37.44L158.4,37.44"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="83.16" y="25.74" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Markdown+Pikchr</text>
<text x="83.16" y="49.14" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Source</text>
<path d="M171.66,72.72L279.463,72.72A7.5 7.5 0 0 0 286.963 65.22L286.963,9.66A7.5 7.5 0 0 0 279.463 2.16L171.66,2.16A7.5 7.5 0 0 0 164.16 9.66L164.16,65.22A7.5 7.5 0 0 0 171.66 72.72Z"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="225.562" y="17.28" text-anchor="middle" font-family="monospace" fill="rgb(0,0,0)" dominant-baseline="central">pikchr</text>
<text x="225.562" y="37.44" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Preprocessor</text>
<text x="225.562" y="57.6" text-anchor="middle" font-family="monospace" fill="rgb(0,0,0)" dominant-baseline="central">(main.c)</text>
<polygon points="448.963,37.44 437.443,41.76 437.443,33.12" style="fill:rgb(0,0,0)"/>
<path d="M286.963,37.44L443.203,37.44"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="367.963" y="25.74" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Markdown+SVG</text>
<text x="367.963" y="49.14" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Intermediate</text>
<path d="M456.463,62.64L579.473,62.64A7.5 7.5 0 0 0 586.973 55.14L586.973,19.74A7.5 7.5 0 0 0 579.473 12.24L456.463,12.24A7.5 7.5 0 0 0 448.963 19.74L448.963,55.14A7.5 7.5 0 0 0 456.463 62.64Z"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="517.968" y="27.36" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Any Markdown</text>
<text x="517.968" y="47.52" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Formatter</text>
<polygon points="748.973,37.44 737.453,41.76 737.453,33.12" style="fill:rgb(0,0,0)"/>
<path d="M586.973,37.44L743.213,37.44"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="667.973" y="25.74" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">HTML+SVG</text>
<text x="667.973" y="49.14" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Output</text>
<polygon points="225.562,72.72 229.882,84.24 221.242,84.24" style="fill:rgb(0,0,0)"/>
<polygon points="225.562,144.72 221.242,133.2 229.882,133.2" style="fill:rgb(0,0,0)"/>
<path d="M225.562,78.48L225.562,138.96"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="233.562" y="108.72" text-anchor="start" fill="rgb(0,0,0)" dominant-baseline="central">C API</text>
<path d="M180.07,215.28L271.054,215.28A7.5 7.5 0 0 0 278.554 207.78L278.554,152.22A7.5 7.5 0 0 0 271.054 144.72L180.07,144.72A7.5 7.5 0 0 0 172.57 152.22L172.57,207.78A7.5 7.5 0 0 0 180.07 215.28Z"  style="fill:none;stroke-width:2.16;stroke:rgb(0,0,0);" />
<text x="225.562" y="159.84" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Pikchr</text>
<text x="225.562" y="180" text-anchor="middle" fill="rgb(0,0,0)" dominant-baseline="central">Formatter</text>
<text x="225.562" y="200.16" text-anchor="middle" font-family="monospace" fill="rgb(0,0,0)" dominant-baseline="central">(pikchr.c)</text>
</svg>
</div>


    ``` pikchr
    arrow right 225% "Markdown+Pikchr" "Source"
    Tool: box rad 5px "pikchr" mono "Preprocessor" "(main.c)" mono fit
    arrow same "Markdown+SVG" "Intermediate"
    box same "Any Markdown" "Formatter" fit
    arrow same "HTML+SVG" "Output"
    arrow <-> down from Tool.s; "C API" ljust at 4pt right of last arrow
    box with .n at last arrow.s same "Pikchr" "Formatter" "(pikchr.c)" mono fit
    ```

GitHub doesn’t allow inline SVGs, so here’s the above diagram as an external
image reference:

<img width="755px" src="usage.svg"/>

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

and ends with the next line that is any of the following strings exactly (or
the end of the file):

* <code>```</code>
* `~~~`
* `.PE`

Anything not between a pair of start and end delimiters is copied to the
standard output unchanged.

Note that generic Markdown [fenced code blocks][fenced] can begin with three
_or more_ `~` (`TILDE`) or <code>&#96;</code> (`GRAVE ACCENT` backtick)
characters indented by up to three spaces, and continue to a matching fence.
For simplicity, this tool is less flexible when looking for Pikchr diagrams,
requiring exactly three unindented tildes or backticks (or the traditional
[PIC][] delimiters)

Building
--------

    $ git clone https://github.com/zenomt/pikchr-cmd
    $ cd pikchr-cmd
    $ make all
    $ ./pikchr -h


  [repo]: https://github.com/zenomt/pikchr-cmd
  [DRH]: https://github.com/drhsqlite/pikchr
  [Pikchr]: https://pikchr.org/
  [PIC]: https://en.wikipedia.org/wiki/PIC_(markup_language)
  [fenced]: https://spec.commonmark.org/0.30/#fenced-code-blocks
  [Unix]: https://en.wikipedia.org/wiki/Unix_philosophy
