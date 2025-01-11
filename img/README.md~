# Mapping of Stokes parameter trajectories onto the Poincaré sphere

Read Stokes parameter triplets from an external file, of format as described in
this documentation, and generate  MetaPost  code  describing the mapping of the
read trajectory or trajectories of Stokes parameters onto the Poincaré sphere.

The  generated  MetaPost  code  can in turn be compiled into, say, DVI, EPS, PDF or SVG images to be included in TeX, LibreOffice or Word  documents.  MetaPost  compilation  is on Linux systems typically done using the mpost tool, installed together with texlive or similar standard TeX distributions.

## Examples

### `example-d` in enclosed `Makefile`

This example illustrates a set of closed trajectories, each being constructed
out of concatenated pieces so as to create a plurality of arrow heads in each
cycle. This example also illustrates how additional indicatory arrows may be
included in the map by using the `--arrow` option repeatedly.
```
example-d:
	make example-d-data
	make example-d-solid

example-d-data:
	make poincare
	echo 1|$(AWK) 'BEGIN {jj=5; kk=90; pi=3.1415926535; ta=0.0; tb=1.0;}{\
	for (j=1;j<jj;j++) {\
	   phia=(j/jj)*pi;\
	   printf("p b urgt \"$$\(\\vartheta=%1.1f\)$$\"\n",phia);\
	   for (k=0;k<=kk;k++) {\
	      t=ta+(k/kk)*(tb-ta);\
	      phi=t*2*pi+pi/2; theta=(j/jj)*pi+(pi/17)*sin(phia)*cos(5*phi);\
	      x=cos(phi)*sin(theta); y=sin(phi)*sin(theta); z=-cos(theta);\
	      printf("%-6.4f %-6.4f %-6.4f\n",x,y,z);\
	   }\
	   printf("q\n");\
	}\
	}END{}' > example-d.dat

example-d-solid:
	@./poincare --verbose --normalize --bezier \
		--inputfile example-d.dat  --outputfile example-d.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" rt \
		--rotatephi 15.0  --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4 \
		--arrowheadangle 30.0 --draw_paths_as_arrows \
		--arrow 1.0 0.7 -0.58 1.0 0.7 0.15 0 0.9 \
		--arrow 0.28 0.9 0.52 0.3 0.9 1.5 0 0.9
	$(METAPOST) example-d.mp
	$(TEX) -jobname=example-d '\input epsf\nopagenumbers\centerline{\epsfxsize=155mm\epsfbox{example-d.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-d -o example-d.eps
	@$(PS2PDF) example-d.eps example-d.pdf
	@$(PDFCROP) example-d.pdf example-d-crop.pdf
	@$(PDF2SVG) example-d-crop.pdf example-d.svg
```
![example-d](img/example-d.svg)

## Compiling the source
```
CC     = gcc
CCOPTS = -O2 -Wall -pedantic -ansi
LNOPTS = -lm

all:
        make poincare

        poincare: ./poincare.o
                $(CC) $(CCOPTS) -o ./poincare ./poincare.o $(LNOPTS)

        poincare.o: ./poincare.c
                $(CC) $(CCOPTS) -c ./poincare.c             
```

## Copyright
Copyright (C) 1997-2025, Fredrik Jonsson, under GPLv3. See enclosed LICENSE.

## Location of master source code
The source and documentation can be found at https://github.com/hp35/poincare
