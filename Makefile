#
# Makefile designed for use with gcc
#
# Copyright (C) 1996-2025 under GPLv3, Fredrik Jonsson
#
PROJECT = poincare

#
# C-specific declarations:
#
CC     = gcc
CCOPTS = -O2 -Wall -pedantic -ansi -std=iso9899:1990
LNOPTS = -lm
AWK    = awk

#
# TeX-specific / document preparing declarations:
#
TEX       = tex
DVIPS     = dvips
DVIPSOPTS = -D1200 -E
METAPOST  = mpost
PS2PDF    = ps2pdf
PDFCROP   = pdfcrop
PDF2SVG   = pdf2svg

all: $(PROJECT) # example example-c

$(PROJECT): $(PROJECT).o
	$(CC) $(CCOPTS) -o $(PROJECT) $(PROJECT).o $(LNOPTS)

$(PROJECT).o: $(PROJECT).c
	$(CC) $(CCOPTS) -c $(PROJECT).c

example:
	make poincare
	@echo 1|$(AWK) 'BEGIN {n=440; pi=3.1415926535;}{\
	printf("p b urgt \"LCP\"\n");\
	for (k=0;k<=n;k++) {\
		t=k/n; phi=t*16*pi; theta=t*pi;\
		x=cos(phi)*sin(theta); y=sin(phi)*sin(theta); z=cos(theta);\
		printf("%-6.4f %-6.4f %-6.4f\n",x,y,z);\
	}\
	printf("q e lrgt \"RCP\"\n");\
	}END{}' > example.dat
	@./poincare --verbose --normalize --draw_hidden_dashed \
		--inputfile example.dat  --outputfile example.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" rt \
		--rotatephi 15.0 --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4
	@$(METAPOST) example.mp
	@$(TEX) -jobname=example "\input macros/epsf.tex\nopagenumbers\
		\centerline{\epsfxsize=155mm\epsfbox{example.1}}\bye"
	@$(DVIPS) $(DVIPSOPTS) example -o example.eps
	@$(PS2PDF) example.eps example.pdf
	@$(PDFCROP) example.pdf example-crop.pdf
	@$(PDF2SVG) example-crop.pdf example.svg

#
# example-c:
#
#    In this example figure, several trajectories are mapped onto the same
#    sphere. This is something useful for illustrating, for example, wave
#    propagation in isotropic media, in which case the form of the Stokes
#    trajectory is invariant under rotation around the s_3-axis of the
#    coordinate system (s_1,s_2,s_3).
#
example-c:
	make example-c-data-solid
	make example-c-data-chopped
	make example-c-solid
	make example-c-chopped
	make example-c-chopped-reversed
	make example-c-chopped-dash

example-c-data-solid:
	make poincare
	echo 1|$(AWK) 'BEGIN {jj=10; kk=90; pi=3.1415926535; ta=0.1; tb=0.9;}{\
	for (j=1;j<=jj;j++) {\
	   phia=(j/jj)*2*pi;\
	   printf("p\n");\
	   for (k=1;k<=kk;k++) {\
	      t=ta+(k/kk)*(tb-ta); phi=phia+t*1.2*pi; theta=t*pi;\
	      x=cos(phi)*sin(theta); y=sin(phi)*sin(theta); z=-cos(theta);\
	      printf("%-6.4f %-6.4f %-6.4f\n",x,y,z);\
	   }\
	   printf("q\n");\
	}\
	}END{}' > example-cs.dat

example-c-data-chopped:
	make poincare
	@echo 1|$(AWK) 'BEGIN {jj=10; kk=90; mm=4; pi=3.1415926535; \
		ta=0.1; tb=0.9;}{\
	for (j=1;j<=jj;j++) {\
	   phia=(j/jj)*2*pi;\
	   dkk=kk/mm;\
	   for (m=1;m<=mm;m++) {\
	   printf("p\n");\
	   for (k=1+(m-1)*dkk;k<=1+m*dkk;k++) {\
	      t=ta+(k/kk)*(tb-ta); phi=phia+t*1.2*pi; theta=t*pi;\
	      x=cos(phi)*sin(theta); y=sin(phi)*sin(theta); z=-cos(theta);\
	      printf("%-6.4f %-6.4f %-6.4f\n",x,y,z);\
	   }\
	   printf("q\n");\
	   }\
	}\
	}END{}' > example-cc.dat

example-c-solid:
	@./poincare --verbose --normalize --bezier \
		--inputfile example-cs.dat  --outputfile example-cs.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" urt \
		--rotatephi 15.0  --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4 \
		--arrowheadangle 20.0 --draw_paths_as_arrows
	$(METAPOST) example-cs.mp
	$(TEX) -jobname=example-cs '\input epsf\nopagenumbers\centerline{\epsfxsize=155mm\epsfbox{example-cs.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-cs -o example-cs.eps

example-c-chopped:
	@./poincare --verbose --normalize --bezier \
		--inputfile example-cc.dat  --outputfile example-cc.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" rt \
		--rotatephi 15.0  --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4 \
		--arrowheadangle 20.0 --draw_paths_as_arrows
	$(METAPOST) example-cc.mp
	$(TEX) -jobname=example-cc '\input epsf\nopagenumbers\centerline{\epsfxsize=155mm\epsfbox{example-cc.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-cc -o example-cc.eps

example-c-chopped-reversed:
	@./poincare --verbose --normalize --bezier \
		--inputfile example-cc.dat  --outputfile example-ccr.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" rt \
		--rotatephi 15.0  --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4 \
		--arrowheadangle 20.0 --draw_paths_as_arrows \
		--reverse_arrow_paths
	$(METAPOST) example-ccr.mp
	$(TEX) -jobname=example-ccr '\input epsf\nopagenumbers\centerline{\epsfxsize=155mm\epsfbox{example-ccr.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-ccr -o example-ccr.eps

example-c-chopped-dash:
	@./poincare --verbose --normalize --bezier --draw_hidden_dashed \
		--inputfile example-cc.dat  --outputfile example-cd.mp \
		--axislengths 0.3 1.7 0.3 2.4 0.3 1.5 \
		--axislabels  "s_1(t)" bot "s_2(t)" bot "s_3(t)" rt \
		--rotatephi 15.0  --rotatepsi -60.0 --shading 0.75 0.99 \
		--rhodivisor 50  --phidivisor 80  --scalefactor 20.0 \
		--paththickness 0.8 --arrowthickness 0.4 \
		--arrowheadangle 20.0 --draw_paths_as_arrows
	$(METAPOST) example-cd.mp
	$(TEX) -jobname=example-cd '\input epsf\nopagenumbers\centerline{\epsfxsize=155mm\epsfbox{example-cd.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-cd -o example-cd.eps

#
# example-d:
#
#    This example illustrates a set of closed trajectories, each
#    being constructed out of concatenated pieces so as to create
#    a plurality of arrow heads in each cycle. This example also
#    illustrates how additional indicatory arrows may be included
#    in the map by using the '--arrow' option repeatedly.
#
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
	$(TEX) -jobname=example-d '\input epsf\nopagenumbers		\centerline{\epsfxsize=155mm\epsfbox{example-d.1}}\bye'
	$(DVIPS) $(DVIPSOPTS) example-d -o example-d.eps
	@$(PS2PDF) example-d.eps example-d.pdf
	@$(PDFCROP) example-d.pdf example-d-crop.pdf
	@$(PDF2SVG) example-d-crop.pdf example-d.svg

clean:
	-rm -f *~ *.aux *.bbl *.dvi *.log *.blg *.toc *.lof *.plt *.1 *.mpx \
		*.o poincare *.eps stoke.mp example-*.* copagraph*

archive:
	make -ik clean
	tar --gzip --directory=../ -cf ../$(PROJECT).tar.gz $(PROJECT)
