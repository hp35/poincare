/*-----------------------------------------------------------------------------
| File: poincare.c [ANSI-C conforming source code]                            |
| Created:       November 17, 1997, Fredrik Jonsson <fj@optics.kth.se>        |
| Last modified: January 10, 2025, Fredrik Jonsson <http://jonsson.eu>        |
| Description:                                                                |
|       This program creates maps of Stokes parameters, visualized as         |
|       trajectories on the Poincare sphere. The program generates MetaPost   |
|       source code as output, to be compiled into PostScript or Encapsulated |
|       PostScript, using John Hobby's MetaPost compiler, or to be used with  |
|       anything that understands MetaPost source code. The Stokes parameter  |
|       trajectories are fed to the program in form of regular CSV-formatted  |
|       triplets of coordinates (S1,S2,S3), with one tripplet for each row of |
|       the input file(s).                                                    |
|                                                                             |
| Copyright (C) 1997-2025, Fredrik Jonsson, under Gnu General Public License  |
| (GPL) v3. See the enclosed LICENSE for details.                             |
|                                                                             |
|       This program is free software: you can redistribute it and/or modif   |
|       it under the terms of the GNU General Public License as published by  |
|       the Free Software Foundation, either version 3 of the License, or     |
|       (at your option) any later version.                                   |
|                                                                             |
|       This program is distributed in the hope that it will be useful,       |
|       but WITHOUT ANY WARRANTY; without even the implied warranty of        |
|       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
|       GNU General Public License for more details.                          |
|                                                                             |
|       You should have received a copy of the GNU General Public License     |
|       along with this program. If not, see <https://www.gnu.org/licenses/>. |
|                                                                             |
| Compile with:                                                               |
|                                                                             |
|        gcc -O2 -g -Wall -pedantic -ansi ./poincare.c -o ./poincare -lm      |
|                                                                             |
| or use the enclosed Makefile, designed for use with GCC:                    |
|      ----------------- CUT HERE ------------------                          |
|      CC     = gcc                                                           |
|      CCOPTS = -O2 -Wall -pedantic -ansi                                     |
|      LNOPTS = -lm                                                           |
|                                                                             |
|      all:                                                                   |
|              make poincare                                                  |
|                                                                             |
|      poincare: ./poincare.o                                                 |
|              $(CC) $(CCOPTS) -o ./poincare ./poincare.o $(LNOPTS)           |
|                                                                             |
|      poincare.o: ./poincare.c                                               |
|              $(CC) $(CCOPTS) -c ./poincare.c                                |
|      ----------------- CUT HERE ------------------                          |
|                                                                             |
|                              Code by:  Fredrik Jonsson <fj@optics.kth.se>   |
|                                        Department of Physics II (Optics)    |
|                                        The Royal Institute of Technology    |
|                                        S-100 44  Stockholm,  Sweden         |
|                                                                             |
| Revision history:                                                           |
|                                                                             |
|  971117:  After a busy weekend I have now finished a program 'poincare.c'   |
|  [v.1.0]  that generates the MetaPost-code for visualizing the output       |
|           Stokes-parameters -- from traject.c -- as trajectory maps         |
|           onto a Phong shaded Poincare sphere.                              |
|                                                                             |
|  971216:  Visiting Ecole Polytechnique for a week.  Added the '-n' option   |
|  [v.1.1]  to 'poincare', which makes it capable of plotting normalized      |
|           Stokes-parameters (S1/S0,S2/S0,S3/S0) without preparing initial   |
|           data.  Also added the feature that 'poincare' writes comments     |
|           into the resulting output MetaPost source file, making it a       |
|           bit easier for the masochist who enjoy reading MetaPost source    |
|           [but -- of course -- Real Men read PostScript directly instead].  |
|                                                                             |
|  980216:  Implemented support for drawing arrows on the poincare sphere     |
|  [v.1.2]  directly by specifications in terms of command-line options.      |
|           In this first version of the arrow drawing command, only          |
|           solid-line- and dashed-line-arrows are supported, though with     |
|           arbitrary shade of gray from white to black.                      |
|                                                                             |
|  980608:  Changed the Stokes-parameter-path-drawing parts, so that hidden   |
|  [v.1.3]  parts of the paths are properly taken into account, and drawn     |
|           with dashed lines (instead of the regular in-front solid lines.   |
|           I also changed the format specifications of the input files, so   |
|           that an arbitrary number of separated paths may be present.       |
|                                                                             |
|  980610:  Added the '--bezier' option, to give the possibility of changing  |
|  [v.1.4]  between regular piecewise straight-line type paths and Bezier     |
|           interpolated paths with continuous first-derivatives.             |
|           Added the '--paththickness' option.                               |
|                                                                             |
|           Added the '--draw_hidden_dashed' option, which gives the user     |
|           a choice between drawing hidden parts of the paths (specified     |
|           with the '--input' option) with dashed or solid lines (of diffe-  |
|           rent shade than the visible, 'in-front' parts.  This required     |
|           some restructuring of the program, since I up until now have      |
|           used only black, dashed lines for hidden parts of paths, in which |
|           case the order in which the paths are written doesn't matter.     |
|           Now the path-drawing statements instead are called upon twice,    |
|           the first time for drawing hidden parts, and the second time for  |
|           drawing visible, 'in-front' parts.  This ensures that no hidden   |
|           part will be written over a visible part, giving a proper         |
|           appearance of the mapped trajectory of the specified path.        |
|                                                                             |
|           Also added the '--axislength' option which gives the possibility  |
|           tuning the axis lengths, depending on the viewpoint for the       |
|           Poincare sphere.                                                  |
|                                                                             |
|  981113:  (Ecole Polytechnique) Added the possibility to visualize an       |
|  [v.1.5]  additional coordinate system, rotated to the original one.        |
|           Useful for visualizing solutions that are best expressed in       |
|           a rotated system, but without loosing the reference to the        |
|           original one.  This feature is accessed via the somewhat          |
|           messy '--xtracoordsys' option, and its relatives                  |
|           '--xtra_axislabels', '--xtra_axislengths' et. al.                 |
|                                                                             |
|           Also added the '--axislabels' option, enabling the user to        |
|           specify his/her own axis labels instead of the default 'S_1',     |
|           'S_2' and 'S_3'.                                                  |
|                                                                             |
|  000419:  (Stockholm, busy writing my PhD thesis) Added the '--auxsource'   |
|  [v.1.6]  option, causing an auxiliary source file to be included at the    |
|           end of the generated MetaPost source. Useful for including        |
|           additional comments, labels etc.                                  |
|                                                                             |
|  030717:  (Stockholm) When compiling the source code of the poincare the    |
|  [v.1.7]  program, the GCC compiler that I now use complains about that     |
|           the string lengths of my arguments to fprintf() are greater than  |
|           the length of 509 characters that ISO C89 compilers are required  |
|           to support. Well, it is no secret that I consider the source to   |
|           be quite messy (since it produces quite a lot of text output, and |
|           in addition produces commented and readable MetaPost source).     |
|               In order to fix this potential future source of trouble, I    |
|           have chopped up all charcter strings greater than 500 characters  |
|           into smaller pieces. This, on the other hand, has made the code   |
|           look even worse; this is something that I plan to take care of    |
|           sometime in the future, whenever I have the time for it.          |
|                                                                             |
|  030721:  (Stockholm) Corrected a bug in the block that append an external  |
|  [v.1.8]  MetaPost file to the generated one (via the --auxsource option).  |
|           (The bug caused strange characters to be included into the gene-  |
|           rated MetaPost even when the --auxsource option was entirely      |
|           omitted at the command line.) Also changed the --scalefactor      |
|           argument to yield the radius of the Poincare sphere in milli-     |
|           metres, instead of centimetres as previously used.                |
|                                                                             |
|  030722:  (Stockholm) Wrote a program 'stokeproc' for preprocessing of      |
|  [v.1.9]  files containing Stokes parameters into the format as accepted    |
|           by the Poincare program. The 'stokproc' program essentially       |
|           three ASCII files, containing lists of the S1, S2, and S3 Stokes  |
|           parameters, respectively, and generates an output file (out of    |
|           these parameters), that is valid as direct input to the poincare  |
|           program.                                                          |
|                                                                             |
|  030724:  (Stockholm) Finished with the blocks for parsing the input file   |
| [v.1.10]  for possible labels at the end points of a trajectory on the      |
|           Poincare sphere. The syntax of the input file is now:             |
|               p [b <pos> <TeX labelstring at begin point of trajectory>]    |
|               <S1[1]>  <S2[1]>  <S3[1]>   (1st Stokes param. triplet)       |
|               <S1[2]>  <S2[2]>  <S3[2]>   (2nd Stokes param. triplet)       |
|                 .        .        .                                         |
|                 .        .        .                                         |
|               <S1[N]>  <S2[N]>  <S3[N]>   (Nth Stokes param. triplet)       |
|               q [e <pos> <TeX labelstring at end point of trajectory>]      |
|           where <pos> is any of                                             |
|               top   (puts label in top relative current point)              |
|               ulft  (puts label in upper left relative current point)       |
|               lft   (puts label in left relative current point)             |
|               llft  (puts label in lower left relative current point)       |
|               bot   (puts label in bottom relative current point)           |
|               lrt   (puts label in lower right relative current point)      |
|               rt    (puts label in right relative current point)            |
|               urt   (puts label in upper right relative current point)      |
|           This syntax is now also incorporated into the data preprocessor   |
|           'stokeproc', which now accepts the command line options           |
|           '--beginlabel' and '--endlabel' for simplifying the end point     |
|           labeling. See comments in the C source code of 'stokeproc' for    |
|           further information.                                              |
|                                                                             |
|  031217:  (Wuppertal) Added a few blocks to ensure that any spaces,         |
| [v.1.11]  linefeeds, or other auxiliary characters are properly read        |
|           away at the end of every Stokes-parameter path that is supplied   |
|           as input to the program. The breaking of a path into subpaths     |
|           works properly whenever the path is passing from the visible to   |
|           the hidden hemisphere of the Poincare sphere; however, there      |
|           still seem to remain some bug that prohibits "makapath makepen"   |
|           to be written to the output file whenever a new path (beginning   |
|           with the character 'p') starts after the very first path is       |
|           finished. In addition, endlabel statements (e.g. 'q e $t=10$')    |
|           seem to fail in the positioning, so there still seem to be plenty |
|           to fix before considering all bugs to be removed.                 |
|                                                                             |
|  040318:  (Wuppertal) Added the '--draw_paths_as_arrows option', enabling   |
| [v.1.12]  the poincare program to draw multiple paths as arrows, rather     |
|           than just as solid curves. Nice option to use if one wish to show |
|           a general direction of polarization state evolution on the sphere |
|           irregardless of the orientation of the polarization ellipse.      |
|           The source code was also transferred to my new Mac OS X machine   |
|           and compiled with the Mac port of GCC without any complaints even |
|           when fed with the '-Wall -pedantic' option.                       |
|                                                                             |
|  050330:  (Cork) Today started the catharsis of the poincare program by     |
| [v.1.13]  collecting all variables related to the Poincare map generation   |
|           into one single C struct, and using this struct for initializa-   |
|           tion and command-line parsing of variables. This is the first     |
|           step towards obtaining a more readable program, and doing so I    |
|           separated the code into the following, more self-explanatory      |
|           routines:                                                         |
|               parse_command_line(argc,argv);                                |
|               display_arrow_specs(map);                                     |
|               open_outfile(map);                                            |
|               write_header(outfileptr,map,argc,argv);                       |
|               write_euler_angle_specs(outfileptr,map);                      |
|               write_sphere_shading_specs(outfileptr,map);                   |
|               write_shaded_sphere(outfileptr,map);                          |
|               write_equators(outfileptr,map);                               |
|               write_scanned_trajectories(outfileptr,map);                   |
|               write_additional_arrows(outfileptr,map);                      |
|               write_coordinate_axes(outfileptr,map);                        |
|               write_additional_coordinate_axes(outfileptr,map);             |
|               write_included_auxiliary_source(outfileptr,map);              |
|               generate_eps_image(map);                                      |
|           There still remains to be something done about the huge           |
|           write_scanned_trajectories() routine, which deals with flushing   |
|           the Stokes trajectories to the MetaPost file. This routine        |
|           involves the splitting of the trajectories into visible and       |
|           hidden parts, as well as all labeling and tick marking, and is    |
|           at the time of writing quite hard to read. This is the first      |
|           thing that needs attention in the progress of cleaning up the     |
|           poincare program.                                                 |
|           In the verification of todays version I ran the 'examples' block  |
|           of the Makefile enclosed with the source code, without any        |
|           deviations from the expected result. However, there still seem    |
|           to be lurking a bug around which causes unpredictable generation  |
|           of multiple Stokes trajectories in the same Poincare map,         |
|           something that needs to be attended in the future.                |
|                                                                             |
|  050402:  (Cork) Today I took the giant leap forward and entirely rewrote   |
| [v.1.14]  the algorithms for the generation of trajectory maps. In the form |
|           as now employed, the entire work is performed with the following  |
|           newly written routines:                                           |
|               short new_trajectory()                                        |
|               short end_of_trajectory()                                     |
|               short beginlabel()                                            |
|               short endlabel()                                              |
|               void readaway_comments_and_blanks()                           |
|               void scan_label()                                             |
|               void scan_beginlabel()                                        |
|               void scan_endlabel()                                          |
|               void scan_for_stokes_triplet()                                |
|               short visible()                                               |
|               short point_just_became_hidden()                              |
|               short point_just_became_visible()                             |
|               void get_screen_coordinates()                                 |
|               void add_subtrajectory()                                      |
|               void sort_out_visible_and_hidden()                            |
|               void add_hidden_subtrajectories()                             |
|               void add_visible_subtrajectories()                            |
|               void add_scanned_trajectory()                                 |
|               void add_scanned_labels()                                     |
|               void write_scanned_trajectories()                             |
|           Checked multiple path specifications within one single map with   |
|           success, using the {\tt example-c} block of the Makefile enclosed |
|           with the poincare source. Label positioning and multiple labels   |
|           functionality works as expected.                                  |
|                                                                             |
|  050403:  (Cork) Added the --hiddengraytone option, and also added the      |
| [v.1.15]  functionality that one in the input trajectory file anywhere      |
|           after a Stokes parameter triplet can specify that this point      |
|           should get a tick mark (simply using a 't' letter after the       |
|           triplet (s1,s2,s3). This functionality is highly useful whenever  |
|           one wish to put the scale of evolution of the Stokes vector       |
|           explicitly in the map. (For example with the Stokes vector as     |
|           function of wavelength or strain, as I currently am in the stage  |
|           of investigating for the sinusoidal gratings with phase           |
|           discontinuities.)                                                 |
|                                                                             |
|  050404:  (Cork) Added the option of including tick mark labeling in the    |
| [v.1.16]  input file syntax, so that tick marks may be given labels if      |
|           wanted. This adds to yesterdays implemented tick marks, so that   |
|           one now can have the absolute scale of measure explicitly present |
|           in the figure, with numbers and all. The current input file       |
|           syntax after this inclusion yields (assuming M points forming the |
|           trajectory):                                                      |
|               p [b <pos> "<TeX label string at begin point>"]               |
|               <s1(1)> <s2(1)> <s3(1)> [options]                             |
|               <s1(2)> <s2(2)> <s3(2)> [options]                             |
|                  .       .       .       .                                  |
|               <s1(M)> <s2(M)> <s3(M)> [options]                             |
|               q [e <pos> "<TeX label string at end point>"]                 |
|           where each of the rows can have individually specified options of |
|           the form                                                          |
|               options = [t [l <pos> "<text>"]] [% <comments>]               |
|                                                                             |
|  050419:  (Southampton) Today fixed a bug in the program which in the case  |
| [v.1.17]  of multiple sets of Stokes trajectories caused previously written |
|           visible parts of earlier trajectories to be crossed by hidden     |
|           parts of trajectories added later on. The issue was simply solved |
|           by adding the |viewtype| flag as input variable to the            |
|           |add_scanned_trajectory()|, |add_scanned_tickmarks()|, and        |
|           |write_scanned_trajectories()| routines, so as to be able to call |
|           |write_scanned_trajectories()| twice, causing the routine to      |
|           perform the parsing of all trajectories twice and adding the      |
|           visible parts only after all hidden parts have been written to    |
|           file. Also fixed a minor bug which under some circumstances       |
|           caused the program to write any additional Stokes trajectories    |
|           with a thinner pen stroke than otherwise specified by the         |
|           numerical value of |map.paththickness|.                           |
|                                                                             |
|  050420:  (Southampton) Fixed a bug which potentially would cause the       |
| [v.1.18]  program to crash for a very large number of tickmarks in the      |
|           Stokes trajectory.                                                |
|                                                                             |
|  050514:  (Southampton) Added a routine which in verbose mode displays      |
| [v.1.19]  the command-line options as they are parsed. This is useful in    |
|           debugging batch-mode calls from, for example, Makefiles.          |
|                                                                             |
|  050515:  (Southampton) Added the --reverse_arrow_paths option, which       |
| [v.1.20]  is used to change the direction of traversal in drawing paths     |
|           as arrows using the --draw_paths_as_arrows option. Useful when    |
|           sampled data have an order which do not coincide with the         |
|           natural order of traversal in the Poincare map.                   |
|                                                                             |
|  050810:  (Southampton) Wrote the strip_away_path() routine in order to     |
| [v.1.21]  finally solve the problem with long path strings that appear in   |
|           the program name string whenever poincare is called with an       |
|           explicit path specified at the command line. The call to the      |
|           strip_away_path() routine is located in the beginning of the      |
|           block for parsing the command line.                               |
|                                                                             |
|  111215:  (Stockholm) Fixed a few rather clumsy blocks in the hidden path   |
| [v.1.22]  generation. Updated the Makefile.                                 |
|                                                                             |
|  140124:  (Stockholm) This morning I received an email from a person in     |
| [v.1.23]  Germany who tried out the program but found that for some reason  |
|           all S1 coordinates seemed to have their signs dropped, i.e. as    |
|           if its absolute value had been extracted. It turned out that the  |
|           sign-dropping was due to a change in the way the standard         |
|           isalnum() function interprets signs. Previously--at least in the  |
|           GNU development environment distributed with Linux (1996-2000)    |
|           and OSX 10.3 (2003)--I had no problems whatsoever to parse the    |
|           signs of signed numbers as belonging to the alphanumeric set;     |
|           however, with newer compilers this behaviour of isalnum() seems   |
|           to have changed, despite that we here follow a strict ISO C90     |
|           standard. Anyway, thanks for pointing this out, and the code is   |
|           now fixed and operational again.                                  |
|                                                                             |
|  250110:  (Gotland) Correction of a few minor bugs in the checks for return |
| [v.1.24]  codes. Updated the Makefile. System is now Ubuntu 24.04.1 LTS,    |
|           Linux kernel 6.8.0-51-generic (64-bit).                           |
|                                                                             |
| Example of usage (the figure on the front page of my PhD thesis):           |
|                                                                             |
|    poincare --normalize --verbose  --bezier --draw_hidden_dashed \          |
|               --axislengths 0.3 1.5 0.3 2.7 0.3 1.5 \                       |
|               --axislabels  "s_1"  "s_2"  "s_3,w_3" \                       |
|               --rotatephi 15.0  --rotatepsi -70.0 \                         |
|               --xtracoordsys  7.0181217  0.0 \                              |
|               --xtracoordsys_axislabel_x "w_1" \                            |
|               --xtracoordsys_axislabel_y "w_2" \                            |
|               --xtracoordsys_axislengths 0.3 1.8 0.3 1.9 0.3 1.8 \          |
|               --shading 0.75 0.99 \                                         |
|               --rhodivisor 50  --phidivisor 80  --scalefactor 2.5 \         |
|               --inputfile tfig2.dat  --outputfile tfig2.mp \                |
|               --paththickness 0.8 --arrowthickness 0.4                      |
|                                                                             |
=============================================================================*/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define VERSION_NUMBER "1.24"

/*-----------------------------------------------------------------------------
| PI should have been defined from <math.h>, but let's be on the safe side
| (since I have found strange behaviour on some SPARC:s).
-----------------------------------------------------------------------------*/
#ifndef PI
#define PI (3.14159265358979323846)
#endif

/*-----------------------------------------------------------------------------
| Definitions of maximum number of allowed coordinates and labels per
| trajectory, as used in the allocation of memory. These fixed parameters
| determine the following:
|    MAX_NUM_STOKE_COORDS    Maximum number of coordinates per trajectory
|    MAX_NUM_TICKMARKS       Maximum number of tick marks per trajectory
|    MAX_NUM_LABELS          Maximum number of text labels per trajectory
|    MAX_LABEL_TEXTLENGTH    Maximum number of characters per text label
-----------------------------------------------------------------------------*/
#define MAX_NUM_STOKE_COORDS (5000)
#define MAX_NUM_TICKMARKS ((int)(MAX_NUM_STOKE_COORDS/10))
#define MAX_NUM_LABELS ((int)(MAX_NUM_TICKMARKS/10))
#define MAX_LABEL_TEXTLENGTH (256)
#define MAX_FILENAME_TEXTLENGTH (256)

#define SUCCESS 0  /* Return code for successful program termination */
#define FAILURE 1  /* Return code for unsuccessful program termination */

#define DEFAULT_OUTFILENAME "aout.mp"
#define DEFAULT_EPSJOBNAME "aout"
#define DEFAULT_AXISLABEL_S1 "S_1"
#define DEFAULT_AXISLABEL_S2 "S_2"
#define DEFAULT_AXISLABEL_S3 "S_3"
#define DEFAULT_AXISLABELPOSITION_S1 "urgt"
#define DEFAULT_AXISLABELPOSITION_S2 "urgt"
#define DEFAULT_AXISLABELPOSITION_S3 "urgt"

#define DEFAULT_ROT_PSI   (-40.0*(PI/180))      /* Angle in radians */
#define DEFAULT_ROT_PHI   (15.0*(PI/180))       /* Angle in radians */

#define DEFAULT_PHI_SOURCE   (30.0*(PI/180))    /* Angle in radians */
#define DEFAULT_THETA_SOURCE (30.0*(PI/180))    /* Angle in radians */

#define DEFAULT_MAX_WHITENESS (0.99)  /*  '0.0' <=> black; '1.0' <=> white */
#define DEFAULT_MIN_WHITENESS (0.75)  /*  '0.0' <=> black; '1.0' <=> white */
#define DEFAULT_HIDDEN_GRAYTONE (0.65)

#define DEFAULT_RHO_DIVISOR  (50.0)
#define DEFAULT_PHI_DIVISOR  (80.0)

#define DEFAULT_POSITIVE_AXIS_LENGTH (1.5)
#define DEFAULT_NEGATIVE_AXIS_LENGTH (0.1)

#define DEFAULT_PATH_THICKNESS  (1.0)
#define DEFAULT_ARROW_THICKNESS (0.6)
#define DEFAULT_ARROW_HEADANGLE (30.0)
#define DEFAULT_TICKSIZE (4*DEFAULT_PATH_THICKNESS)

/* Number of coordinates per line in the generated MetaPost code for the map */
#define NUM_COORDS_PER_METAPOST_LINE (3)

/* Definitions of flags used for identifying label positions */
#define NOLABEL         (0)
#define TOPLABEL        (1)
#define UPPERLEFTLABEL  (2)
#define LEFTLABEL       (3)
#define LOWERLEFTLABEL  (4)
#define BOTTOMLABEL     (5)
#define LOWERRIGHTLABEL (6)
#define RIGHTLABEL      (7)
#define UPPERRIGHTLABEL (8)

#define MAKE_VERBOSE_REALLY_VERBOSE (1)  /* Avoid TOO much of ASCII */

/* Definition of values taken by flags determining whether hidden or visible
 * parts of trajectories should be flushed to file.
 */
#define HIDDEN (0)
#define VISIBLE (1)

/*---------------------------------------------------------------------
| The only global variables allowed in my programs are `optarg`, which is
| the string of characters that specified the call from the command line,
| and `progname`, which simply is the string containing the name of the
| program, as it was invoked from the command line.
---------------------------------------------------------------------*/
extern char *optarg;
char *progname;

int isnan(double x);

typedef struct {
   double **arrows;
   int numarrows;
   short verbose;
   short save_memory;
   short use_normalized_stokes_params;
   short use_bezier_curves;
   short user_specified_inputfile;
   short user_specified_auxfile;
   short user_specified_axislabels;
   short user_specified_additional_coordinate_system;
   short user_specified_xtra_axislabel_x;
   short user_specified_xtra_axislabel_y;
   short user_specified_xtra_axislabel_z;
   short draw_hidden_dashed;
   short draw_paths_as_arrows;
   short reverse_arrow_paths;
   short last_point_infront;
   short current_point_is_a_beginlabelpoint,
         current_point_is_an_endlabelpoint;
   short draw_axes_inside_sphere;
   short currently_drawing_path;
   short generate_eps_output;
   char infilename[MAX_FILENAME_TEXTLENGTH];
   char outfilename[MAX_FILENAME_TEXTLENGTH];
   char auxfilename[MAX_FILENAME_TEXTLENGTH];
   char epsjobname[MAX_FILENAME_TEXTLENGTH];
   char axislabel_s1[MAX_LABEL_TEXTLENGTH];
   char axislabel_s2[MAX_LABEL_TEXTLENGTH];
   char axislabel_s3[MAX_LABEL_TEXTLENGTH];
   char axislabelposition_s1[8],
        axislabelposition_s2[8],
        axislabelposition_s3[8];
   char xtra_axislabel_x[MAX_LABEL_TEXTLENGTH];
   char xtra_axislabel_y[MAX_LABEL_TEXTLENGTH];
   char xtra_axislabel_z[MAX_LABEL_TEXTLENGTH];
   double xtra_neg_axis_length_x;
   double xtra_neg_axis_length_y;
   double xtra_neg_axis_length_z;
   double xtra_pos_axis_length_x;
   double xtra_pos_axis_length_y;
   double xtra_pos_axis_length_z;
   char labelstr_beginpoint[MAX_LABEL_TEXTLENGTH];
   char labelstr_endpoint[MAX_LABEL_TEXTLENGTH];
   double scalefactor;
   double rot_psi, rot_phi;
   double delta_rot_psi, delta_rot_phi;
   double phi_source, theta_source;
   double upper_whiteness_value;
   double lower_whiteness_value;
   double hiddengraytone;
   double rho_divisor;
   double phi_divisor;
   double xpos_beginpoint;
   double ypos_beginpoint;
   double xpos_endpoint;
   double ypos_endpoint;
   double neg_axis_length_s1;
   double neg_axis_length_s2;
   double neg_axis_length_s3;
   double pos_axis_length_s1;
   double pos_axis_length_s2;
   double pos_axis_length_s3;
   double paththickness;
   double arrowthickness;
   double arrowheadangle;
   double coordaxisthickness;
   double ticksize;
} pmap;

typedef struct {
   long numcoords;
   double *s1,*s2,*s3;
   short *visible;
   int numtickmarks;
   long *tickmark;
   int numlabels;
   long *label;
   char **labeltext;
   int *labellength;
   short *labelpos;
} stoketraject;

/*----------------------------------------------------------------------------
| The |svector| routine allocates a vector of short integer precision,
| with vector index ranging from |nl| to |nh|.
----------------------------------------------------------------------------*/
short *svector(long nl, long nh) {
   short *v;
   v=(short *)malloc((size_t) ((nh-nl+2)*sizeof(short)));
   if (!v) {
      fprintf(stderr,"Error: Allocation failure in svector()\n");
      exit(1);
   }
   return v-nl+1;
}

/*----------------------------------------------------------------------------
| The |ivector| routine allocates a vector of integer precision,
| with vector index ranging from |nl| to |nh|.
----------------------------------------------------------------------------*/
int *ivector(long nl, long nh) {
   int *v;
   v=(int *)malloc((size_t) ((nh-nl+2)*sizeof(int)));
   if (!v) {
      fprintf(stderr,"Error: Allocation failure in ivector()\n");
      exit(1);
   }
   return v-nl+1;
}

/*----------------------------------------------------------------------------
| The |lvector| routine allocates a vector of long integer precision,
| with vector index ranging from |nl| to |nh|.
----------------------------------------------------------------------------*/
long *lvector(long nl, long nh) {
   long *v;
   v=(long *)malloc((size_t) ((nh-nl+2)*sizeof(long)));
   if (!v) {
      fprintf(stderr,"Error: Allocation failure in lvector()\n");
      exit(1);
   }
   return v-nl+1;
}

/*----------------------------------------------------------------------------
| The |free_svector| routine release the memory occupied by the
| short integer vector |v[nl..nh]|.
----------------------------------------------------------------------------*/
void free_svector(short *v, long nl, long nh) {
   free((char*) (v+nl-1));
}

/*----------------------------------------------------------------------------
| The |free_ivector| routine release the memory occupied by the
| integer vector |v[nl..nh]|.
----------------------------------------------------------------------------*/
void free_ivector(long *v, long nl, long nh) {
   free((char*) (v+nl-1));
}

/*----------------------------------------------------------------------------
| The |free_lvector| routine release the memory occupied by the
| long integer vector |v[nl..nh]|.
----------------------------------------------------------------------------*/
void free_lvector(long *v, long nl, long nh) {
   free((char*) (v+nl-1));
}

/*----------------------------------------------------------------------------
| The |free_dvector| routine release the memory occupied by the
| real-valued vector |v[nl..nh]|.
----------------------------------------------------------------------------*/
void free_dvector(double *v, long nl, long nh) {
   free((char*) (v+nl-1));
}

/*----------------------------------------------------------------------------
| The |dvector| routine allocates a real-valued vector of double precision,
| with vector index ranging from |nl| to |nh|.
----------------------------------------------------------------------------*/
double *dvector(long nl, long nh) {
   double *v;
   v=(double *)malloc((size_t) ((nh-nl+2)*sizeof(double)));
   if (!v) {
      fprintf(stderr,"Error: Allocation failure in dvector()\n");
      exit(1);
   }
   return v-nl+1;
}

/*-------------------------------------------------------------------------
| The scan_for_boundingbox(infilename,llx,lly,urx,ury) routine takes the
| name of a regular ASCII text file (infilename) containing Encapsulated
| PostScript (EPS) as input, and returns the bounding box of the figure,
| in terms of the corner coordinates given by llx (lower left x), lly
| (lower left y), urx (upper right x), and ury (upper right y).
| Typical usage (as a sample program for extraction of bounding box, similar
| to the UNIX standard program psbb):
|
|     #include <stdlib.h>
|     #include <stdio.h>
|     #include <string.h>
|
|     extern char *optarg;
|     char *progname;
|
|     void scan_for_boundingbox(char infilename[256],
|        long int *llx,long int *lly,long int *urx,long int *ury) {
|        ...
|     }
|
|     int main(int argc,char *argv[]) {
|        char infilename[256];
|        int no_arg;
|        long int llx,lly,urx,ury;
|
|        progname=argv[0];
|        no_arg=argc;
|        if (argc<2) {
|           fprintf(stderr,"%s: Error! Please specify input (EPS) filename.\n",
|              progname);
|           fprintf(stderr,"Usage: %s <filename>\n",progname);
|           exit(FAILURE);
|        }
|        --argc;
|        strcpy(infilename,argv[no_arg-argc]);
|        scan_for_boundingbox(infilename,&llx,&lly,&urx,&ury);
|        fprintf(stdout,"%ld %ld %ld %ld\n",llx,lly,urx,ury);
|        exit(0);
|     }
|
-------------------------------------------------------------------------*/
void scan_for_boundingbox(char infilename[256],
   long int *llx,long int *lly,long int *urx,long int *ury) {
   short int done;
   char tmpstr[256];
   int tmpch;
   FILE *infile;

   if ((infile=fopen(infilename,"r"))==NULL) {
      fprintf(stderr,"%s: Error! Could not open file %s for reading.\n",
         progname,infilename);
      exit(FAILURE);
   }
   fseek(infile,0L,SEEK_SET);
   done=0;
   while (((tmpch=getc(infile))!=EOF)&&(!done)) {
      ungetc(tmpch,infile);
      if (fscanf(infile,"%s",tmpstr)==0) {
         exit(FAILURE);
      }
      if (!strcmp(tmpstr,"%%BoundingBox:")) {
         done=1;
         if (fscanf(infile,"%ld",llx)==0) {
            fprintf(stderr,"Failed scanning llx in bounding box!\n");
	    exit(FAILURE);
	 }
         if (fscanf(infile,"%ld",lly)==0) {
            fprintf(stderr,"Failed scanning lly in bounding box!\n");
	    exit(FAILURE);
	 }
         if (fscanf(infile,"%ld",urx)==0) {
            fprintf(stderr,"Failed scanning urx in bounding box!\n");
	    exit(FAILURE);
	 }
         if (fscanf(infile,"%ld",ury)==0) {
            fprintf(stderr,"Failed scanning ury in bounding box!\n");
	    exit(FAILURE);
	 }
      }
   }
   if (tmpch==EOF) {
      fprintf(stderr,"%s: Error! End of file reached without ",progname);
      fprintf(stderr,"finding any %%%%BoundingBox statement!\n");
      fprintf(stderr,"%s: (Does %s really contain ",progname,infilename);
      fprintf(stderr,"Encapsulated PostScript?)\n");
      exit(FAILURE);
   }
} /* end of scan_for_boundingbox() */

double **matrix(long nrl, long nrh, long ncl, long nch) {
   long i, j, nrow=nrh-nrl+1,ncol=nch-ncl+1;
   double **m;
   m=(double **) malloc((size_t)((nrow+1)*sizeof(double*)));
   if (!m) {
      printf("%s: Allocation failure 1 in matrix()\n",progname);
      exit(FAILURE);
   }
   m += 1;
   m -= nrl;
   m[nrl]=(double *) malloc((size_t)((nrow*ncol+1)*sizeof(double)));
   if (!m[nrl]) {
      printf("%s: Allocation failure 2 in matrix()\n",progname);
      exit(FAILURE);
   }
   m[nrl] += 1;
   m[nrl] -= ncl;
   for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
   for(i=nrl;i<=nrh;i++) for(j=ncl;j<=nch;j++) m[i][j]=0.0;
   return m;
}

void free_matrix(double **m, long nrl, long nrh, long ncl, long nch) {
   free((char*) (m[nrl]+ncl-1));
   free((char*) (m+nrl-1));
}

char **cmatrix(long nrl, long nrh, long ncl, long nch) {
   long i, j, nrow=nrh-nrl+1,ncol=nch-ncl+1;
   char **m;
   m=(char **) malloc((size_t)((nrow+1)*sizeof(char*)));
   if (!m) {
      printf("%s: Allocation failure 1 in cmatrix()\n",progname);
      exit(1);
   }
   m += 1;
   m -= nrl;
   m[nrl]=(char *) malloc((size_t)((nrow*ncol+1)*sizeof(char)));
   if (!m[nrl]) {
      printf("%s: Allocation failure 2 in cmatrix()\n",progname);
      exit(1);
   }
   m[nrl] += 1;
   m[nrl] -= ncl;
   for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
   for(i=nrl;i<=nrh;i++) for(j=ncl;j<=nch;j++) m[i][j]=0.0;
   return m;
}

void free_cmatrix(char **m, long nrl, long nrh, long ncl, long nch) {
   free((char*) (m[nrl]+ncl-1));
   free((char*) (m+nrl-1));
}

void show_banner(void) {
   fprintf(stdout,"This is %s v.%s.  ",progname,VERSION_NUMBER);
   fprintf(stdout,"Copyright (C) 1997-2005, Fredrik Jonsson\n");
}

void showsomehelp(void) {
   fprintf(stdout,"Usage: %s [options]\n",progname);
   fprintf(stdout,
 "where [options] include:\n"
 " -h, --help              Show this help-message and exit clean.\n"
 " -v, --verbose           Toggle verbose mode; show beautiful ASCII.\n"
 "                         Default: off.\n"
 " -s, --save_memory       Toggle memory save mode; spare some RAM.\n"
 "                         Default: off.\n"
 " -V, --version           Show version and exit clean.\n\n");
   fprintf(stdout,
 " -f, --inputfile <name>  Read input Stokes-parameters from file <name>.\n"
 "                         The input file may contain an arbitrary number of\n"
 "                         separate paths, and the format of the file is\n");
   fprintf(stdout,
 "                               p\n"
 "                               <s1> <s2> <s3>       [1:st triplet, path 1]\n"
 "                               <s1> <s2> <s3>       [2:nd triplet, path 1]\n"
 "                                  .  .  .\n"
 "                               <s1> <s2> <s3>       [M:th triplet, path 1]\n"
 "                               q\n");
   fprintf(stdout,
 "                               p\n"
 "                               <s1> <s2> <s3>       [1:st triplet, path 2]\n"
 "                                  .  .  .\n"
 "                               <s1> <s2> <s3>       [N:th triplet, path 2]\n"
 "                               q\n"
 "                                  .  .  .                   [etc.]\n");
   fprintf(stdout,
 "                         Thus, each separate path should be defined by an\n"
 "                         initial 'p', after which each following row con-\n"
 "                         tains a triplet of Stokes parameters.  After the\n"
 "                         Stokes parameter triplet, comments and additional\n"
 "                         information may be written (ignored by program)\n"
 "                         until linefeed.  Finally, each separate path is\n"
 "                         is ended with a 'q' on a separate line.\n"
 "\n");
   fprintf(stdout,
 "                         The only exception to the ignoring of the rest of\n"
 "                         the line as described above is if there is a tick\n"
 "                         mark and possibly an associated label with the\n"
 "                         Stokes-triplet. In this case, the triplet will be\n"
 "                         followed by a single \"t\", with an optional label\n"
	   "                         specified by a following\n");
   fprintf(stdout,
 "                               l <position> \"<TeX label>\".\n"
 "                         This way, positions along the mapped trajectories\n"
 "                         can be easily indicated in the input data to the\n"
 "                         Poincare program.\n"
 "\n");
   fprintf(stdout,
 "                         Thus, to summarize, each row of data in the the\n"
 "                         input file has the format\n"
 "                               <s1> <s2> <s3> [t [l <pos> \"<TeX label>\"]]\n"
 "\n");
   fprintf(stdout,
 " --paththickness <val>   Specifies the thickness in PostScript points (pt)\n"
 "                         of the path to draw.  Default: <val> = 1.0 [pt].\n"
 "                         [1 pt == 1/72 inch]\n"
 "\n");
   fprintf(stdout,
 " --draw_hidden_dashed    Toggles between drawing of hidden parts of the\n"
 "                         specified path with dashed and solid lines.\n"
 "                         Default: off. (Solid lines)\n"
 "\n");
   fprintf(stdout,
 " --draw_paths_as_arrows  Draw all specified trajectories as arrowed\n"
 "                         curves, with arrowheads at the and point.\n"
 "                         This option is useful whenever one wish to,\n"
 "                         for example, show on the direction of evolution\n"
 "                         of a certain trajectory, or the direction of\n"
 "                         rotation of the Stokes vector in a circular\n");
   fprintf(stdout,
 "                         path. With this option it is often useful to\n"
 "                         chop up the trajectory of the original input\n"
 "                         file into subtrajectories, so as to create\n"
 "                         multiple arrow heads in the same trajectory.\n"
 "                         See also the --reverse_arrow_paths option.\n");
   fprintf(stdout,
 " --reverse_arrow_paths   Reverse the direction of all arrows drawn using\n"
 "                         the --draw_paths_as_arrows option. This is useful\n"
 "                         if the sampled trajectory data are not ordered in\n"
 "                         the natural direction of trajectory traversal.\n");
   fprintf(stdout,
 " --auxsource <name>      Causes the auxiliary file <name> to be included\n"
 "                         at the end of the generated MetaPost source.\n"
 "                         Useful for including additional comments, labels\n"
 "                         etc. in the figure.\n"
 "\n");
   fprintf(stdout,
 " --arrowthickness <val>  Analogous to the '--paththickness' option, but\n"
 "                         with the difference that this one applies to\n"
 "                         (eventually occuring) the thickness of additional\n"
 "                         arrows to be drawn with the '--arrow' option.\n"
 "                         Default: <val> = 0.6 [pt].\n"
 "\n");
   fprintf(stdout,
 " --arrowheadangle <deg>  Specifies the head angle of any arrows used in\n"
 "                         the mapping of Stokes parameters on the Poincare\n"
 "                         sphere. Notice that this does not affect the\n"
 "                         head angles of the arrows of the coordinate\n"
 "                         axes. Default value: 30 degrees.\n");
   fprintf(stdout,
 " -b, --bezier            Toggle Bezier mode, in which Bezier interpolation\n"
 "                         is used in order to obtain smooth paths for the\n"
 "                         input trajectory(-ies), specified with the '-f'\n"
 "                         option.  Otherwise regular piecewise stright-line\n"
 "                         type lines are used.   Default: off.\n"
 "\n");
   fprintf(stdout,
 " -o, --outputfile <name> Write output MetaPost-code [1] to file <name>.\n"
 "\n");
   fprintf(stdout,
 " -e, --epsoutput <name>  In addition to just generating MetaPost-code for\n"
 "                         the figure, also try to generate a complete EPS\n"
 "                         (Encapsulated PostScript) figure, using <name>\n"
 "                         as the base name for the job. This option relies\n"
 "                         on system calls for TeX, MetaPost, and DVIPS, and\n"
 "                         relies on that they are properly installed in the\n"
 "                         system environment.\n");
   fprintf(stdout,
 "                         The EPS output and the intermediate TeX, DVI, and\n"
 "                         and log files will from the base name be named\n"
 "                         <name>.eps,<name>.tex,<name>.dvi, and <name>.log,\n"
 "                         respectively.\n"
 "\n");
   fprintf(stdout,
 "--psi, --rotatepsi <val> When mapping Poincare-sphere and corresponding\n"
 "                         coordinate-system (S_1,S_2,S_3), first rotate\n"
 "                         angle psi == <val> around the 'z'-axis (S_3).\n"
 "                         Default: -40.0 (Degrees)\n"
 "\n");
   fprintf(stdout,
 "--phi, --rotatephi <val> When mapping Poincare-sphere and corresponding\n"
 "                         coordinate-system (S_1,S_2,S_3), after the first\n"
 "                         rotation (psi above), rotate angle phi == <val>\n"
 "                         around the 'y'-axis (S_2).\n"
 "                         Default: 15.0 (Degrees)\n"
 "\n");
   fprintf(stdout,
 " --rhodivisor  <val>     Number of segments in radial direction of the 2D-\n"
 "                         mapped Poincare sphere.  Default: 50.\n"
 "\n");
   fprintf(stdout,
 " --phidivisor  <val>     Number of segments in tangential direction of the\n"
 "                         2D-mapped Poincare sphere.  Default: 80.\n"
 "\n");
   fprintf(stdout,
 " --scalefactor <val>     Specifies the radius of the printed Poincare\n"
 "                         sphere (Encapsulated PostScript) in millimetres.\n"
 "\n");
   fprintf(stdout,
 " --shading <w1> <w2>     Specifies the minimum (<w1>) and maximum (<w2>)\n"
 "                         whiteness values of the Poincare sphere to draw\n"
 "                         (using the Phong shading algorithm).\n"
 "                         Here:\n"
 "                             <wx> == 0.0  corresponds to 'white'\n"
 "                             <wx> == 1.0  corresponds to 'white'\n"
 "                         Default values:  <w1> == 0.65,  <w2> == 0.99\n"
 "\n");
   fprintf(stdout,
 " --hiddengraytone <w>    Specifies the whiteness to be used in drawing\n"
 "                         trajectory parts that are hidden behind the\n"
 "                         Poincare sphere.\n"
 "                             <w> == 0.0  corresponds to black,\n"
 "                             <w> == 1.0  corresponds to white,\n");
   fprintf(stdout,
 " --axislengths <v>       Specifies the lengths of negative and positive\n"
 "                         parts of the coordinate axes, on the form\n"
 "                           <v> = <xmin> <xmax> <ymin> <ymax> <zmin> <zmax>\n"
 "                         with 'x' as the s1-axis, 'y' as the s2-axis, and\n"
 "                         'z' as the s3-axis. All values are taken relative\n"
 "                         to the radius of the Poincare sphere; thus <v>=1\n");
   fprintf(stdout,
 "                         correspond to the radius, while <v>=1.5 corre-\n"
 "                         spond to an axis length such that 50 percent of\n"
 "                         the axis is showed outside the Poincare sphere.\n"
 "                         Default:  <xmin> = <ymin> = <zmin> = 0.3 (30 %%)\n"
 "                                   <xmax> = <ymax> = <zmax> = 1.5 (150 %%)\n"
 "\n");
   fprintf(stdout,
 " --axislabels <s>        Specifies the labels of the coordinate axes, on\n"
 "                         the form\n"
 "                           <s> = <s1> <p1> <s2> <p2> <s3> <p3>\n"
 "                         where <s1>, <s2>, and <s3> are strings to use for\n"
 "                         the s1-, s2-, and s3-labels, respectively, and\n");
   fprintf(stdout,
 "                         where the strings <p1>, <p2>, <p3> determine the\n"
 "                         position of respective label, relative the end\n"
 "                         point of the arrow of respective axis. The label\n"
 "                         position is determined by the following syntax:\n");
   fprintf(stdout,
 "                              lft    Left\n"
 "                              rt     Right\n"
 "                              top    Top\n"
 "                              bot    Bottom\n"
 "                              ulft   Upper left\n"
 "                              urt    Upper Right\n"
 "                              llft   Lower left\n"
 "                              lrt    Lower right\n");
   fprintf(stdout,
 "                         The label strings should be expressed in plain\n"
 "                         TeX [2] mathmode syntax.\n");
   fprintf(stdout,
 "                         Default: <s1> = $S_1$, <s2> = $S_2$, <s3> = $S_3$\n"
 "                         Important note:  No blank spaces are allowed in\n"
 "                         the label strings.\n"
 "\n");
   fprintf(stdout,
 " --draw_axes_inside      Toggles drawing (with dashed lines) of coordinate\n"
 "                         axes inside Poincare sphere.  Default: off.\n"
 "\n");
   fprintf(stdout,
 " -n, --normalize         Instead of making a trajectory plot of the para-\n"
 "                         meters (s1,s2,s3), contained in the file speci-\n"
 "                         fied by the '-f' option, instead use the norma-\n");
   fprintf(stdout,
 "                         lized parameter (s1/s0,s2/s0,s3/s0), which for\n"
 "                         completely polarized light corresponds to a tra-\n"
 "                         jectory mapped directly on the Poincare sphere,\n"
 "                         without any deviations fromthe spheres surface.\n");
   fprintf(stdout,
 "                         This option is particularly useful when only the\n"
 "                         state of polarization (and not the intensity) of\n"
 "                         the light is of interest.\n"
 "\n");
   fprintf(stdout,
 " --arrow <pa> <pb> <v>   Display an arrow, in Stokes parameter space, from\n"
 "                         point <pa>, at the command-line specified as the\n"
 "                         triple of floats <s1a> <s2a> <s3a>, to the point\n"
 "                         <pb>, similarly specified as <s1b> <s2b> <s3b>.\n");
   fprintf(stdout,
 "                         Useful for pointing out certain operation cycles\n"
 "                         in polarization domain, or just as an easy direct\n"
 "                         way of creating paths on the Poincare sphere\n"
 "                         without having to use external input files.\n");
   fprintf(stdout,
 "                         The arrow is drawn as a circular arc onto the\n"
 "                         Poincare sphere, through the closest path between\n"
 "                         the points.  The '--arrow' statement may appear\n"
 "                         repeated times,for producing multiple arrows.\n"
 "                         Currently there is a limit of 24 arrows in one\n"
 "                         single Poincare map (which should do for most\n"
 "                         people).\n");
   fprintf(stdout,
 "                           The last argument <v> is a pair of float values\n"
 "                         which determines the style of the drawn arrow.\n"
 "                         The pair <v> should be specified as <v1> <v2> on\n"
 "                         the command-line.\n");
   fprintf(stdout,
 "                         The first parameter <v1> determines the line-type\n"
 "                         of the arrow to draw.  The rules are:\n"
 "                            -0.5 <= <v1> < 0.5   -   Solid line\n"
 "                             0.5 <= <v1> < 1.5   -   Dashed line\n"
 "                         The second parameter, <v2>, determines the black-\n"
 "                         ness of the arrow to draw, where <v2> == 0 corre-\n"
 "                         sponds to white and <v2> == 1 to black.\n"
 "\n");
   fprintf(stdout,
 "Suffix conventions of the files:\n\n"
 "    .mp    - MetaPost source code (ASCII) [1]\n"
 "    .tex   - TeX source code (ASCII) [2]\n"
 "    .dvi   - Device independent output file from TeX [2]\n"
 "    .ps    - PostScript [3]\n"
 "    .eps   - Encapsulated PostScript [3]\n\n");
   fprintf(stdout,"References\n\n");
   fprintf(stdout,
 " [1] For information on the MetaPost program for typesetting figures,\n"
 "     see for example John Hobbys page, at\n"
 "     http://cm.bell-labs.com/who/hobby/MetaPost.html.\n\n");
   fprintf(stdout,
 " [2] For information on the TeX typesetting system, as well as references\n"
 "     to the dvips program, see for example the homepage of the TeX Users\n"
 "     Group, at http://www.tug.org.\n\n");
   fprintf(stdout,
 " [3] For information on the PostScript programming language, see for\n"
 "     example the homepage of Adobe Systems Inc., at\n"
 "     http://www.adobe.com/products/postscript/main.html,\n"
 "     or 'PostScript Language - Tutorial and Cookbook' (Addison-Wesley,\n"
 "     Reading, Massachusetts, 1985), ISBN 0-201-10179-3.\n\n");
   fprintf(stdout,
 "Please report bugs to Fredrik Jonsson <fredrik@physics.kth.se>\n"
 "Copyright (C) 1997-2005, Fredrik Jonsson <fredrik@physics.kth.se>\n");
} /* end of showsomehelp() */

void initialize_variables(pmap *map) {
   (*map).arrows=NULL;
   (*map).numarrows=0;
   (*map).save_memory=0;
   (*map).use_normalized_stokes_params=0;
   (*map).use_bezier_curves=0;
   (*map).user_specified_inputfile=0;
   (*map).user_specified_auxfile=0;
   (*map).user_specified_axislabels=0;
   (*map).user_specified_additional_coordinate_system=0;
   (*map).user_specified_xtra_axislabel_x=0;
   (*map).user_specified_xtra_axislabel_y=0;
   (*map).user_specified_xtra_axislabel_z=0;
   (*map).draw_hidden_dashed=0;
   (*map).draw_paths_as_arrows=0;
   (*map).reverse_arrow_paths=0;
   (*map).last_point_infront=1;
   (*map).current_point_is_a_beginlabelpoint=0;
   (*map).current_point_is_an_endlabelpoint=0;
   (*map).draw_axes_inside_sphere=0;
   (*map).currently_drawing_path=0;
   (*map).generate_eps_output=0;
   (*map).xtra_neg_axis_length_x=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).xtra_neg_axis_length_y=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).xtra_neg_axis_length_z=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).xtra_pos_axis_length_x=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).xtra_pos_axis_length_y=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).xtra_pos_axis_length_z=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).scalefactor=6.0;
   (*map).rot_psi=DEFAULT_ROT_PSI;
   (*map).rot_phi=DEFAULT_ROT_PHI;
   (*map).delta_rot_psi=0.0;
   (*map).delta_rot_phi=0;
   (*map).phi_source=DEFAULT_PHI_SOURCE;
   (*map).theta_source=DEFAULT_THETA_SOURCE;
   (*map).upper_whiteness_value=DEFAULT_MAX_WHITENESS;
   (*map).lower_whiteness_value=DEFAULT_MIN_WHITENESS;
   (*map).hiddengraytone=DEFAULT_HIDDEN_GRAYTONE;
   (*map).rho_divisor=DEFAULT_RHO_DIVISOR;
   (*map).phi_divisor=DEFAULT_PHI_DIVISOR;
   (*map).xpos_beginpoint=0.0;
   (*map).ypos_beginpoint=0.0;
   (*map).xpos_endpoint=0.0;
   (*map).ypos_endpoint=0.0;
   (*map).neg_axis_length_s1=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).neg_axis_length_s2=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).neg_axis_length_s3=DEFAULT_NEGATIVE_AXIS_LENGTH;
   (*map).pos_axis_length_s1=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).pos_axis_length_s2=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).pos_axis_length_s3=DEFAULT_POSITIVE_AXIS_LENGTH;
   (*map).paththickness=DEFAULT_PATH_THICKNESS;
   (*map).arrowthickness=DEFAULT_ARROW_THICKNESS;
   (*map).arrowheadangle=DEFAULT_ARROW_HEADANGLE;
   (*map).coordaxisthickness=DEFAULT_ARROW_THICKNESS;
   (*map).ticksize=DEFAULT_TICKSIZE;
   strcpy((*map).outfilename,DEFAULT_OUTFILENAME);
   strcpy((*map).epsjobname,DEFAULT_EPSJOBNAME);
   strcpy((*map).axislabel_s1,DEFAULT_AXISLABEL_S1);
   strcpy((*map).axislabel_s2,DEFAULT_AXISLABEL_S2);
   strcpy((*map).axislabel_s3,DEFAULT_AXISLABEL_S3);
   strcpy((*map).axislabelposition_s1,DEFAULT_AXISLABELPOSITION_S1);
   strcpy((*map).axislabelposition_s2,DEFAULT_AXISLABELPOSITION_S2);
   strcpy((*map).axislabelposition_s3,DEFAULT_AXISLABELPOSITION_S3);
   strcpy((*map).xtra_axislabel_x,"");
   strcpy((*map).xtra_axislabel_y,"");
   strcpy((*map).xtra_axislabel_z,"");
   strcpy((*map).labelstr_beginpoint,"");
   strcpy((*map).labelstr_endpoint,"");
} /* end of initialize_variables() */

/*
 * In the initialization of the |stoketraject| struct, MAX_NUM_LABELS is the
 * maximum number of allowed labels along each trajectory.
 * However, we reserve two elements of the arrays containing the label data
 * for possibly present labels at begin and end points of the trajectory,
 * hence the total number of elements in the label-related arrays are
 * MAX_NUM_LABELS+2.
 */
void initialize_stoke_trajectory(stoketraject *tr) {
   (*tr).numcoords=0;
   (*tr).s1=dvector(1,MAX_NUM_STOKE_COORDS);
   (*tr).s2=dvector(1,MAX_NUM_STOKE_COORDS);
   (*tr).s3=dvector(1,MAX_NUM_STOKE_COORDS);
   (*tr).visible=svector(1,MAX_NUM_STOKE_COORDS);
   (*tr).numtickmarks=0;
   (*tr).tickmark=lvector(1,MAX_NUM_TICKMARKS);
   (*tr).numlabels=0;
   (*tr).label=lvector(1,(MAX_NUM_LABELS+2));
   (*tr).labeltext=cmatrix(1,(MAX_NUM_LABELS+2),1,MAX_LABEL_TEXTLENGTH);
   (*tr).labellength=ivector(1,(MAX_NUM_LABELS+2));
   (*tr).labelpos=svector(1,(MAX_NUM_LABELS+2));
} /* end of initialize_stoke_trajectory() */

void reset_stokes_trajectory_struct(stoketraject *st) {
   long int j,k;
   (*st).numcoords=0;
   for (k=1;k<=MAX_NUM_STOKE_COORDS;k++) {
      (*st).s1[k]=0.0;
      (*st).s2[k]=0.0;
      (*st).s3[k]=0.0;
      (*st).visible[k]=0;
   }
   (*st).numtickmarks=0;
   for (k=1;k<=MAX_NUM_TICKMARKS;k++) (*st).tickmark[k]=0;
   (*st).numlabels=0;
   for (k=1;k<=(MAX_NUM_LABELS+2);k++) {
      (*st).label[k]=0;
      for (j=1;j<=MAX_LABEL_TEXTLENGTH;j++) (*st).labeltext[k][j]=' ';
      (*st).labellength[k]=0;
      (*st).labelpos[k]=0;
   }
}

/*---------------------------------------------------------------------
| The pathcharacter() routine takes one character `ch` as argument,
| and returns 1 ('true') if the character is valid character of a path
| string, otherwise 0 ('false') is returned.
---------------------------------------------------------------------*/
short pathcharacter(int ch) {
   return(isalnum(ch)||(ch=='.')||(ch=='/')||(ch=='\\')||(ch=='_')||
      (ch=='-')||(ch=='+'));
}

/*---------------------------------------------------------------------
| The strip_away_path() routine takes a character string `filename` as
| argument, and returns a pointer to the same string but without any
| preceding path segments. This routine is, for example, useful for
| removing paths from program names parsed from the command line.
---------------------------------------------------------------------*/
char *strip_away_path(char filename[]) {
   int j,k=0;
   while (pathcharacter(filename[k])) k++;
   j=(--k); /* this is the uppermost index of the full path+file string */
   while (isalnum((int)(filename[j]))) j--;
   j++; /* this is the lowermost index of the stripped file name */
   return(&filename[j]);
}

void display_parsed_command_line_option(pmap *map,char *optstr) {
   if ((*map).verbose)
      fprintf(stdout,"%s: Parsing '%s' option.\n",progname,optstr);
}

/*-----------------------------------------------------------------------------
| Parsing for command-line arguments. This routine also takes care of default
| initialization of the variables of the pmap struct.
| For any additionally specified arrows, we allocate a [7x24] matrix for
| (potential) arrow coordinates. The organization is as follows:
|    Each column c=1,2,...,24, of the matrix corresponds to one arrow. The
|    first three elements are the (s1,s2,s3)-coordinates of the first point
|    of the arrow (the arrow base), the next three elements are the
|    (s1,s2,s3)-coordinates of the endpoint of the arrow. The seventh element
|    determines the type of arrow to draw: if  0 < arrows[...][7] < 1, the
|    arrow will be of solid-line type,  if  1 < arrows[...][7] < 2, the arrow
|    will be dashed.
|    Finally, the eighth element determines the blackness of the arrow, where
|    arrows[...][8] == 0 corresponds to white, and arrows[...][8] = 1 to
|    black.
-----------------------------------------------------------------------------*/
pmap parse_command_line(int argc, char *argv[]) {
   int no_arg;
   pmap map;

   initialize_variables(&map);
   progname=strip_away_path(argv[0]); /* Strip from preceeding path string */
   no_arg=argc;
   map.arrows=matrix(1,8,1,24);
   while(--argc) { /* while any arguments still are left */
      if (!strcmp(argv[no_arg-argc],"-v") ||
         !strcmp(argv[no_arg-argc],"--verbose")) {
         map.verbose=(map.verbose?0:1);
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
      } else if (!strcmp(argv[no_arg-argc],"--save_memory")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.save_memory=(map.save_memory?0:1);
      } else if (!strcmp(argv[no_arg-argc],"-n") ||
              !strcmp(argv[no_arg-argc],"--normalize")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.use_normalized_stokes_params
            =(map.use_normalized_stokes_params?0:1);
      } else if (!strcmp(argv[no_arg-argc],"--paththickness")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.paththickness)) {
            fprintf(stderr,"%s: Couldn't get path thickness!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--draw_hidden_dashed")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.draw_hidden_dashed=(map.draw_hidden_dashed?0:1);
      } else if (!strcmp(argv[no_arg-argc],"--draw_paths_as_arrows")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.draw_paths_as_arrows=(map.draw_paths_as_arrows?0:1);
      } else if (!strcmp(argv[no_arg-argc],"--reverse_arrow_paths")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.reverse_arrow_paths=(map.reverse_arrow_paths?0:1);
      } else if (!strcmp(argv[no_arg-argc],"--arrowthickness")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrowthickness)) {
            fprintf(stderr,\
              "%s: Couldn't get arrow thickness in [pt]!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--arrowheadangle")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrowheadangle)) {
            fprintf(stderr,\
              "%s: Couldn't get arrow head angle in [deg]!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"-b") ||
              !strcmp(argv[no_arg-argc],"--bezier")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.use_bezier_curves=(map.use_bezier_curves?0:1);
      } else if (!strcmp(argv[no_arg-argc],"-h") ||
              !strcmp(argv[no_arg-argc],"--help")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         showsomehelp();
         exit(FAILURE);
      } else if (!strcmp(argv[no_arg-argc],"-V") ||
              !strcmp(argv[no_arg-argc],"--version")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         show_banner();
         exit(0);
      } else if (strcmp(argv[no_arg-argc],"-f")==0 ||
             strcmp(argv[no_arg-argc],"--inputfile")==0) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.infilename,argv[no_arg-argc]);
         map.user_specified_inputfile=1;
      } else if (strcmp(argv[no_arg-argc],"-e")==0 ||
             strcmp(argv[no_arg-argc],"--epsoutput")==0) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.epsjobname,argv[no_arg-argc]);
         map.generate_eps_output=1;
      } else if (strcmp(argv[no_arg-argc],"-o")==0 ||
             strcmp(argv[no_arg-argc],"--outputfile")==0) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.outfilename,argv[no_arg-argc]);
      } else if (strcmp(argv[no_arg-argc],"--auxsource")==0) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.auxfilename,argv[no_arg-argc]);
         map.user_specified_auxfile=1;
      } else if (!strcmp(argv[no_arg-argc],"--psi") ||
              !strcmp(argv[no_arg-argc],"--rotatepsi")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.rot_psi)) {
            fprintf(stderr,\
              "%s: Couldn't get value for psi (rotation round z)!\n",progname);
            exit(FAILURE);
         }
         map.rot_psi *= (PI/180);
      } else if (!strcmp(argv[no_arg-argc],"--phi") ||
              !strcmp(argv[no_arg-argc],"--rotatephi")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.rot_phi)) {
            fprintf(stderr,\
              "%s: Couldn't get value for phi (rotation round y)!\n",progname);
            exit(FAILURE);
         }
         map.rot_phi *= (PI/180);
      } else if (!strcmp(argv[no_arg-argc],"--rhodivisor")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.rho_divisor)) {
            fprintf(stderr,\
              "%s: Couldn't get value for rho divisor!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--phidivisor")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.phi_divisor)) {
            fprintf(stderr,\
              "%s: Couldn't get value for phi divisor!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--scalefactor")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.scalefactor)) {
            fprintf(stderr,\
              "%s: Couldn't get value for scalefactor!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--hiddengraytone")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.hiddengraytone)) {
            fprintf(stderr,\
              "%s: Couldn't get whiteness value of hidden parts!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--shading")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.lower_whiteness_value)) {
            fprintf(stderr,\
              "%s: Couldn't get lower value of sphere whiteness!\n",progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.upper_whiteness_value)) {
            fprintf(stderr,\
              "%s: Couldn't get upper value of sphere whiteness!\n",progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--axislengths")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.neg_axis_length_s1)) {
            fprintf(stderr,"%s: Couldn't get minimum for s1 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.pos_axis_length_s1)) {
            fprintf(stderr,"%s: Couldn't get maximum for s1 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.neg_axis_length_s2)) {
            fprintf(stderr,"%s: Couldn't get minimum for s2 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.pos_axis_length_s2)) {
            fprintf(stderr,"%s: Couldn't get maximum for s2 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.neg_axis_length_s3)) {
            fprintf(stderr,"%s: Couldn't get minimum for s3 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.pos_axis_length_s3)) {
            fprintf(stderr,"%s: Couldn't get maximum for s3 axis!\n",progname);
            fprintf(stderr,"%s: Check the '--axislength' option\n", progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--axislabels")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabel_s1,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabelposition_s1,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabel_s2,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabelposition_s2,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabel_s3,argv[no_arg-argc]);
         --argc;
         strcpy(map.axislabelposition_s3,argv[no_arg-argc]);
         map.user_specified_axislabels=1;
      } else if (!strcmp(argv[no_arg-argc],"--draw_axes_inside")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.draw_axes_inside_sphere=(map.draw_axes_inside_sphere?0:1);
      } else if (!strcmp(argv[no_arg-argc],"--xtracoordsys")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.delta_rot_psi)) {
            fprintf(stderr,"%s: Couldn't get delta_rot_psi!\n",progname);
            fprintf(stderr,"%s: Check the '--xtracoordsys' option\n",progname);
            fprintf(stderr,"%s: (first argument)\n",progname);
            exit(FAILURE);
         }
         map.delta_rot_psi *= (PI/180);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.delta_rot_phi)) {
            fprintf(stderr,"%s: Couldn't get delta_rot_phi!\n",progname);
            fprintf(stderr,"%s: Check the '--xtracoordsys' option\n",progname);
            fprintf(stderr,"%s: (second argument)\n",progname);
            exit(FAILURE);
         }
         map.delta_rot_phi *= (PI/180);
         map.user_specified_additional_coordinate_system=1;
      } else if (!strcmp(argv[no_arg-argc],"--xtracoordsys_axislabel_x")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.xtra_axislabel_x,argv[no_arg-argc]);
         map.user_specified_xtra_axislabel_x=1;
      } else if (!strcmp(argv[no_arg-argc],"--xtracoordsys_axislabel_y")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.xtra_axislabel_y,argv[no_arg-argc]);
         map.user_specified_xtra_axislabel_y=1;
      } else if (!strcmp(argv[no_arg-argc],"--xtracoordsys_axislabel_z")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         strcpy(map.xtra_axislabel_z,argv[no_arg-argc]);
         map.user_specified_xtra_axislabel_z=1;
      } else if (!strcmp(argv[no_arg-argc],"--xtracoordsys_axislengths")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_neg_axis_length_x)) {
            fprintf(stderr,"%s: Couldn't get minimum for x axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_pos_axis_length_x)) {
            fprintf(stderr,"%s: Couldn't get maximum for x axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_neg_axis_length_y)) {
            fprintf(stderr,"%s: Couldn't get minimum for y axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_pos_axis_length_y)) {
            fprintf(stderr,"%s: Couldn't get maximum for y axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_neg_axis_length_z)) {
            fprintf(stderr,"%s: Couldn't get minimum for z axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.xtra_pos_axis_length_z)) {
            fprintf(stderr,"%s: Couldn't get maximum for z axis!\n",progname);
            fprintf(stderr,
               "%s: Check the '--xtracoordsys_axislengths' option", progname);
            exit(FAILURE);
         }
      } else if (!strcmp(argv[no_arg-argc],"--arrow")) {
         display_parsed_command_line_option(&map,argv[no_arg-argc]);
         map.numarrows++;
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[1][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S1 coordinate for starting point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[2][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S2 coordinate for starting point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[3][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S3 coordinate for starting point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[4][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S1 coordinate for ending point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[5][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S2 coordinate for ending point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[6][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get S3 coordinate for ending point of arrow"
              " No. %d!\n", progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[7][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get line type of arrow No. %d!\n",\
                                                     progname, map.numarrows);
            exit(FAILURE);
         }
         --argc;
         if (!sscanf(argv[no_arg-argc],"%lf",&map.arrows[8][map.numarrows])) {
            fprintf(stderr,\
              "%s: Couldn't get blackness of arrow No. %d!\n",\
                                                     progname, map.numarrows);
            exit(FAILURE);
         }
      } else { /* if no valid option is to be found... */
         fprintf(stderr,"%s: Error: Specified option '%s' invalid!\n",
            progname,argv[no_arg-argc]);
         showsomehelp();
         exit(FAILURE);
      }
   }
   return map; /* return all parameter values as a struct of type |pmap| */
} /* end of parse_command_line() */

void display_arrow_specs(pmap map) {
   int i;
   if (map.numarrows > 0) {
      if (map.verbose) {
         printf("%s: You specified the following arrows to draw:\n",progname);
         for (i=1;i<=map.numarrows;i++) {
            printf("%s:    Arrow No. %d: (%1.2f,%1.2f,%1.2f) --> "
                   "(%1.2f,%1.2f,%1.2f) [%1.2f,%1.2f]\n", progname, i,
                         map.arrows[1][i], map.arrows[2][i], map.arrows[3][i],
                         map.arrows[4][i], map.arrows[5][i], map.arrows[6][i],
                         map.arrows[7][i], map.arrows[8][i]);
         }
      }
   }
} /* end of display_arrow_specs() */

FILE *open_outfile(pmap map) {
   FILE *outfileptr;
   if ((outfileptr=fopen(map.outfilename,"w")) == NULL) {
      fprintf(stderr,"Couldn't open file %s for output!\n",map.outfilename);
      exit(FAILURE);
   } else {
      if (map.verbose)
         printf("%s: Writing MetaPost code to %s\n",progname,map.outfilename);
   }
   return outfileptr;
} /* end of open_outfile() */

FILE *open_infile(pmap map) {
   FILE *infileptr;
   if (!map.user_specified_inputfile) {
      fprintf(stderr,"%s: No input trajectory file specified.\n",progname);
      infileptr=NULL;
   } else {
   if ((infileptr=fopen(map.infilename,"r")) == NULL) {
      fprintf(stderr,"%s: Couldn't open trajectory file %s for reading\n",
         progname,map.infilename);
      fprintf(stderr,"%s: Please check -f or --inputfile option arguments\n",
                         progname);
      exit(FAILURE);
   } else {
      if (map.verbose) {
         printf("%s: Reading Stokes parameters from %s\n",\
                                           progname, map.infilename);
      }
   }
   }
   return infileptr;
} /* end of open_infile() */

/*
 * Write out the heading comments of the MetaPost file that is about
 * to be generated.
 */
void write_header(FILE *outfileptr,pmap map,int argc, char *argv[]) {
   time_t now=time(NULL);
   int i=0,no_arg=argc;
   fprintf(outfileptr,
 "%% This Filename:  %s   [MetaPost source]\n"
 "%% Creation time:  %s"                   /* ctime() takes care of '\n'. */
 "%%\n"
 "%% Copyright (C) 1997-2005, Fredrik Jonsson <fj@optics.kth.se>\n"
 "%%\n"
 "%% Input Filename [Stokes parameters]:  %s\n"
 "%% This MetaPost source code was automatically generated by %s\n",
   map.outfilename, ctime(&now), map.infilename, progname);

   /*
    * In many cases, it is quite useful to keep the full command-line
    * that generated a certain figure, for future reference.
    */
   argc=no_arg-1;
   fprintf(outfileptr,
      "%% Full set of command line options that generated this code:\n");
   while (argc) {
      if (i==0) fprintf(outfileptr,"%%    ");
      fprintf(outfileptr," %s",argv[no_arg-argc]);
      argc--;
      i++;
      if (i==6) {
         fprintf(outfileptr,"\n");
         i=0;
      }
   }
   fprintf(outfileptr,"\n%%\n");
   fprintf(outfileptr,
 "%% Description:  Map of Stokes parameters, visualized as trajectories\n"
 "%%               onto the Poincare sphere. This file contains MetaPost\n"
 "%%               source code, to be compiled with John Hobby's MetaPost\n"
 "%%               compiler or used with anything that understands MetaPost\n"
 "%%               source code.\n"
 "%%\n");
   fprintf(outfileptr,
 "%% If you want to create PostScript output, or include the resulting\n"
 "%% output in a TeX document, this example illustrates the procedure,\n"
 "%% assuming 'poincaremap.mp' to be the name of the file containing the\n"
 "%% MetaPost code to be visualized: (commands run on command-line)\n"
 "%%\n");
   fprintf(outfileptr,
 "%%       mp poincaremap.mp;\n"
 "%%       echo \042\\input epsf\\centerline{\\epsfbox{poincaremap.1}}\\bye\042 > tmp.tex;\n"
 "%%       tex tmp.tex;\n"
 "%%       dvips tmp.dvi -o poincaremap.ps;\n"
 "%%\n");
   fprintf(outfileptr,
 "%% Here, the first command compiles the MetaPost source code, and leaves\n"
 "%% an Encapsulated PostScript file named 'poincaremap.1', containing TeX\n"
 "%% control codes for characters, etc. This file does not contain any\n"
 "%% definitions for characters or TeX-specific items, and it cannot be\n"
 "%% viewed or printed simply as is stands; it must rather be included into\n"
 "%% TeX code in order to provide something useful.\n");
   fprintf(outfileptr,
 "%%     The second command creates a temporary minimal TeX-file 'tmp.tex',\n"
 "%% that only includes the previously generated Encapsulated PostScript\n"
 "%% code.\n");
   fprintf(outfileptr,
 "%%     The third command compiles the TeX-code into device-independent,\n"
 "%% or DVI, output, stored in the file 'tmp.dvi'.\n"
 "%%     Finally, the last command converts the DVI output into a free-\n"
 "%% standing PostScript file 'poincaremap.ps', to be printed or viewed\n"
 "%% with some PostScript viewer, such as GhostView.\n"
 "%%\n");
} /* end of write_header() */

void write_euler_angle_specs(FILE *outfileptr,pmap map) {
   fprintf(outfileptr,
 "scalefactor := %f mm;\n"
 "rot_psi := %f;  %% Rotation angle round z-axis (first rotation)\n"
 "rot_phi := %f;  %% Rotation angle round y-axis (second rotation)\n"
 "alpha := %f;    %% == arctan(sin(rot_phi)*tan(rot_psi))\n"
 "beta  := %f;    %% == arctan(sin(rot_phi)/tan(rot_psi))\n\n",
   map.scalefactor, (180/PI)*map.rot_psi, (180/PI)*map.rot_phi,
   (180/PI)*atan(sin(map.rot_phi)*tan(map.rot_psi)),
   (180/PI)*atan(sin(map.rot_phi)/tan(map.rot_psi)));
} /* end of write_euler_angle_specs() */

void write_sphere_shading_specs(FILE *outfileptr,pmap map) {
   /*
    * Parameters specifying the location of the light source.
    */
   fprintf(outfileptr,
     "%%\n"
     "%% Parameters specifying the location of the light source; for Phong\n"
     "%% shading of the sphere.\n"
     "%%\n"
     "%%    phi_source:  Angle (in deg.) to light source counterclockwise\n"
     "%%                 'from three o'clock', viewed from the observer.\n"
     "%%\n"
     "%%  theta_source:  Angle (in deg.) between light source and observer,\n"
     "%%                 seen from the centre of the sphere.\n"
     "%%\n");
   fprintf(outfileptr,
     "%% Parameters specifying the shading 'intensity' in terms of maximum\n"
     "%% (for the highlighs) and minimum (for the deep shadowed regions)\n"
     "%% values for the Phong shading.  '0.0' <=> 'black'; '1.0' <=> 'white'\n"
     "%%\n"
     "%%   upper_value:  Maximum value of whiteness.\n"
     "%%   lower_value:  Minimum value of whiteness.\n"
     "%%\n");
   fprintf(outfileptr,
     "phi_source := %f;\n"
     "theta_source := %f;\n"
     "upper_value := %f;\n"
     "lower_value := %f;\n",
       (180/PI)*map.phi_source,(180/PI)*map.theta_source,
       map.upper_whiteness_value,map.lower_whiteness_value);
   fprintf(outfileptr,
     "radius := scalefactor;\n"
     "delta_rho := radius/%f;\n"
     "delta_phi := 360.0/%f;\n"
     "beginfig(1);\n"
     "  path p;\n"
     "  path equator;\n"
     "  transform T;\n"
     "  c1:=lower_value;\n"
     "  c2:=upper_value-lower_value;\n",map.rho_divisor, map.phi_divisor);

/*-----------------------------------------------------------------------------
| Here follows the x-, y- and z-components of the unit normal vector pointing
| from the center of the sphere to the position of the (point-like) light
| source.
-----------------------------------------------------------------------------*/
   fprintf(outfileptr,
     "  nx_source := sind(theta_source)*cosd(phi_source);\n"
     "  ny_source := sind(theta_source)*sind(phi_source);\n"
     "  nz_source := cosd(theta_source);\n"
     "  phistop := 360.0;\n"
     "  rhostop := radius - delta_rho/2.0;\n");
} /* end of write_sphere_shading_specs() */

/*-----------------------------------------------------------------------------
| Now generate the Phong-shaded poincare sphere as it is projected down
| in 2D.  This is done by creating a number of >>parallelltrapetser<<
| mapped centro-symmmetrically around origo.
| The steps involved in this are as follows:
| 1. Take the coordinate of the center of each >>parallelltrapets<< to be
|    the reference for calculating the coordinates of the corners.
| 2. Calculate the coordinates of the corners of >>parallelltrapetserna<<.
| 3. Create the path p of the >>parallelltrapets<<:
|
|          (x3,y3)                       (x2,y2)
|               +------------<------------+
|                \                       /
|                 \                     /
|                  v                   ^
|                   \                 /
|                    \               /
|                     +------>------+
|                 (x4,y4)          (x1,y1)
|
| 4. Check if the >>parallelltrapets<< just created is shaded by the sphere or
|    not, i.e. if the position of the >>parallelltrapets<< is situated on the
|    opposite side of the Poincare sphere, seen from the direction of the light
|    source. If so, make the >>parallelltrapets<< shaded with the darkest tone,
|    specified by c1; otherwise make it shaded with the tone of gray specified
|    by prod, the scalar product of the normal vector to the object with the
|    normal vector to the lightsource.
-----------------------------------------------------------------------------*/
void write_shaded_sphere(FILE *outfileptr,pmap map) {
   fprintf(outfileptr,
     "%%\n"
     "%% Draw the shaded Poincare sphere projected on 2D screen coordinates\n"
     "%%\n");
   fprintf(outfileptr,
     "  for rho=0.0cm step delta_rho until rhostop:\n"
     "    for phi=0.0 step delta_phi until phistop:\n");
   fprintf(outfileptr,
     "      rhomid := rho + delta_rho/2.0;\n"
     "      phimid := phi + delta_phi/2.0;\n");
   fprintf(outfileptr,
     "      x1 := rho*cosd(phi);\n"
     "      y1 := rho*sind(phi);\n"
     "      x2 := (rho+delta_rho)*cosd(phi);\n"
     "      y2 := (rho+delta_rho)*sind(phi);\n"
     "      x3 := (rho+delta_rho)*cosd(phi+delta_phi);\n"
     "      y3 := (rho+delta_rho)*sind(phi+delta_phi);\n"
     "      x4 := rho*cosd(phi+delta_phi);\n"
     "      y4 := rho*sind(phi+delta_phi);\n");
   fprintf(outfileptr,
     "      p:=makepath makepen ((x1,y1)--(x2,y2)--(x3,y3)--(x4,y4)--(x1,y1));\n"
     "      quot := (rhomid/radius);\n"
     "      nx_object := quot*cosd(phimid);\n"
     "      ny_object := quot*sind(phimid);\n"
     "      nz_object := sqrt(1-quot*quot);\n");
   fprintf(outfileptr,
     "      prod:=nx_object*nx_source+ny_object*ny_source\n"
     "            +nz_object*nz_source;\n");
   fprintf(outfileptr,
     "      if prod < 0.0:\n"
     "         value := c1;\n"
     "      else:\n"
     "         value := c1 + c2*prod*prod;\n"
     "      fi\n"
     "      fill p withcolor value[black,white];\n"
     "    endfor\n"
     "  endfor\n\n");
} /*end of write_shaded_sphere() */

/*-----------------------------------------------------------------------------
| Draw the 'equators' $S_1=0$, $S_2=0$ and $S_3=0$ on the sphere.
| If specified by the user, also draw the additional 'equators' (corresponding
| to a coordinate system rotated around the original one by the Euler angles
| delta_psi, delta_phi) on the sphere.
-----------------------------------------------------------------------------*/
void write_equators(FILE *outfileptr,pmap map) {
   fprintf(outfileptr,
     "%%\n"
     "%% Draw the 'equators' of the Poincare sphere\n"
     "%%\n"
     "   equator := halfcircle scaled (2.0*radius);\n"
     "   eqcolval := .45;    %% '0.0' <=> 'white';  '1.0' <=> 'black'\n"
     "\n");
   fprintf(outfileptr,
     "   pickup pencircle scaled %f pt;\n",map.coordaxisthickness);
   fprintf(outfileptr,
     "%%\n"
     "%% Draw equator $S_3=0$...\n"
     "%%\n"
     "   T := identity yscaled sind(rot_phi) rotated 180.0;\n"
     "   draw equator transformed T withcolor eqcolval [white,black];\n"
     "\n");
   fprintf(outfileptr,
     "%%\n"
     "%% ... then equator $S_2=0$...\n"
     "%%\n"
     "   T := identity yscaled (cosd(rot_phi)*sind(rot_psi))\n"
     "                 rotated (270.0 + alpha);\n"
     "   draw equator transformed T withcolor eqcolval [white,black];\n"
     "\n");
   fprintf(outfileptr,
     "%%\n"
     "%% ... and finally equator $S_1=0$.\n"
     "%%\n"
     "   T := identity yscaled (cosd(rot_phi)*cosd(rot_psi))\n"
     "                 rotated (270.0 - beta);\n"
     "   draw equator transformed T withcolor eqcolval [white,black];\n"
     "\n");
   if (map.user_specified_additional_coordinate_system) { /* whoaaaou ... */
      fprintf(outfileptr,
        "%%\n"
        "%% Some handy parameters used in calculations below.\n"
        "%%\n");
      fprintf(outfileptr,
        "delta_rot_psi := %f; %% Additional 1st rotation angle round z-axis\n"
        "delta_rot_phi := %f;  %% Additional 2nd rotation angle round y-axis\n"
        "delta_alpha := %f;    %% == arctan(sin(rot_phi)*tan(rot_psi))\n"
        "delta_beta  := %f;    %% == arctan(sin(rot_phi)/tan(rot_psi))\n\n",
          (180/PI)*(map.delta_rot_psi),(180/PI)*(map.delta_rot_phi),
          (180/PI)*atan(sin(map.rot_phi + map.delta_rot_phi)
            *tan(map.rot_psi + map.delta_rot_psi)),
          (180/PI)*atan(sin(map.rot_phi + map.delta_rot_phi)
            /tan(map.rot_psi + map.delta_rot_psi)));
      fprintf(outfileptr,
        "%%\n"
        "%% Draw the additional 'equators' of the Poincare sphere,\n"
        "%% corresponding to a system rotated by the Euler-angles\n"
        "%%   delta_psi=, delta_phi=\n"
        "%%\n"
        "   equator := halfcircle scaled (2.0*radius);\n"
        "   eqcolval := .45;    %% '0.0' <=> 'white';  '1.0' <=> 'black'\n"
        "\n");
      fprintf(outfileptr,
        "%%\n"
        "%% Draw equator $W_3=0$...\n"
        "%%\n"
        "   T := identity yscaled sind(rot_phi+delta_rot_phi) rotated 180.0;\n"
        "   draw equator transformed T withcolor eqcolval [white,black];\n"
        "\n");
      fprintf(outfileptr,
        "%%\n"
        "%% ... then equator $W_2=0$...\n"
        "%%\n"
        "   T := identity yscaled (cosd(rot_phi + delta_rot_phi)\n"
        "             *sind(rot_psi + delta_rot_psi))\n"
        "             rotated (270.0 + delta_alpha);\n"
        "   draw equator transformed T withcolor eqcolval [white,black];\n"
        "\n");
      fprintf(outfileptr,
        "%%\n"
        "%% ... and finally equator $W_1=0$.\n"
        "%%\n"
        "   T := identity yscaled (cosd(rot_phi + delta_rot_phi)\n"
        "             *cosd(rot_psi + delta_rot_psi))\n"
        "             rotated (270.0 - delta_beta);\n"
        "   draw equator transformed T withcolor eqcolval [white,black];\n"
        "\n");
   }
} /* end of write_equators() */

void display_stokes_trajectory(stoketraject st) {
   long k;

   fprintf(stdout,"Stokes trajectory [%ld coordinates]:\n",st.numcoords);
   for (k=1;k<=st.numcoords;k++) {
      fprintf(stdout," S[%ld]=(%f, %f ,%f), ",k,st.s1[k],st.s2[k],st.s3[k]);
      fprintf(stdout,"[%s]\n",(st.visible[k]?"visible":"hidden"));
   }
}

short new_trajectory(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='p') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

short end_of_trajectory(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='q') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

short beginlabel(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='b') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

short tickmark(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='t') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

short tickmarklabel(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='l') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

short endlabel(FILE *infileptr) {
   int ch;
   if ((ch=getc(infileptr))=='e') {
      return 1;
   } else {
      ungetc(ch,infileptr);
      return 0;
   }
}

/*
 * The readaway_comments_and_blanks(FILE *infileptr,long *linenum) routine,
 * well... , reads away comments and blanks from the input file (character
 * stream) until next alphanumeric character, including any leading sign, or
 * until end-of-file.
 *
 * Important: The "while ((!(isalnum(ch)||(ch=='-')||(ch=='+')))&&(ch!=EOF))"
 * construction is absolutely necessary, since any preceding signs are not
 * covered by isalnum(). Previously, isalnum() used to cover this as well,
 * at least in the GNU development environment distributed with Linux (1996-
 * 2000) and OSX 10.3 (2003), but somehow this seems to have changed, despite
 * that we here follow a strict ISO C90 standard.
 */
void readaway_comments_and_blanks(FILE *infileptr,long *linenum) {
   int ch;
   ch=getc(infileptr);
   while ((!(isalnum(ch)||(ch=='-')||(ch=='+')))&&(ch!=EOF)) {
      if (ch=='\n') {
         (*linenum)++;
      } else if (ch=='%') { /* if comment found, then read away the row */
         while (((ch=getc(infileptr))!='\n')&&(ch!=EOF));
         (*linenum)++;
      }
      ch=getc(infileptr);
   }
   if (ch!=EOF) ungetc(ch,infileptr);
}

void scan_label(FILE *infileptr,stoketraject *st,int lnum,long *linenum,
      pmap *map,long coordnum) {
   char tmpstr[256];
   int ch,k;

   /* Scan for positioning of label */
   readaway_comments_and_blanks(infileptr,linenum);
   if ((*map).verbose) {
      fprintf(stdout,
         "%s: Scanning label text starting at line %ld of trajectory file\n",
            progname,*linenum);
   }
   /* tells at which coordinate index the label position is to be found */
   (*st).label[lnum]=coordnum;
   if (fscanf(infileptr,"%s",tmpstr)==0) {
      fprintf(stderr,"Failed scanning label position!\n");
      exit(FAILURE);
   }
   if (!strcmp(tmpstr,"top")) {
      (*st).labelpos[lnum]=TOPLABEL;
   } else if (!strcmp(tmpstr,"ulft")) {
      (*st).labelpos[lnum]=UPPERLEFTLABEL;
   } else if (!strcmp(tmpstr,"lft")) {
      (*st).labelpos[lnum]=LEFTLABEL;
   } else if (!strcmp(tmpstr,"llft")) {
      (*st).labelpos[lnum]=LOWERLEFTLABEL;
   } else if (!strcmp(tmpstr,"bot")) {
      (*st).labelpos[lnum]=BOTTOMLABEL;
   } else if (!strcmp(tmpstr,"lrgt")) {
      (*st).labelpos[lnum]=LOWERRIGHTLABEL;
   } else if (!strcmp(tmpstr,"rgt")) {
      (*st).labelpos[lnum]=RIGHTLABEL;
   } else if (!strcmp(tmpstr,"urgt")) {
      (*st).labelpos[lnum]=UPPERRIGHTLABEL;
   } else {
      fprintf(stderr,
         "%s: Invalid string '%s' found at line %ld of trajectory file.\n",
            progname,tmpstr,*linenum);
      exit(1);
   }
   if ((*map).verbose) {
      fprintf(stdout,
         "%s: Scanned label positioning '%s' at line %ld of trajectory file\n",
            progname,tmpstr,*linenum);
   }

   /* Scan for label string to be printed */
   while ((ch=getc(infileptr))==' ');
   ungetc(ch,infileptr);
   if ((ch=getc(infileptr))!='\"') {
      fprintf(stderr,"%s: Error in line %ld of trajectory file. [ch=%c]\n",
         progname,*linenum,ch);
      fprintf(stderr,"%s: Use enclosing quote marks (\") around label text.\n",
         progname);
      exit(1);
   }
   k=0;
   while ((ch=getc(infileptr))!='\"') {
      k++;
      if (ch!='\n') {
         tmpstr[k]=ch;
      } else {
         fprintf(stderr,
            "%s: Error: Reached end of line %ld without closing quote mark.\n",
            progname,*linenum);
         fprintf(stderr,"%s: (Check this label statement.)\n",progname);
         exit(1);
      }
   }
   (*st).labellength[lnum]=k;
   while (0<k) {
      (*st).labeltext[lnum][k]=tmpstr[k];
      k--;
   }
}

/*
The scan_beginlabel() scans for a label string immediatelty after a
statement for a new trajectory, and places the string text, string length,
and label position relative the first point in thye data structure st.
The routine uses the more general scan_label() routine for the implementation,
in order to keep a comapct and consistent behaviour of the algorithm.
*/
void scan_beginlabel(FILE *infile,stoketraject *st,long *linenum,pmap *map,
      long coordnum) {
   scan_label(infile,st,1,linenum,map,coordnum);
}

void scan_ticklabel(FILE *infile,stoketraject *st,long *linenum, pmap *map,
      long coordnum) {
   if ((*map).verbose)
      fprintf(stdout,"%s: Scanning label No %d\n",progname,(*st).numlabels);
   if ((1<(*st).numlabels)&&((*st).numlabels<MAX_NUM_LABELS+2)) {
      scan_label(infile,st,(*st).numlabels,linenum,map,coordnum);
   } else {
      fprintf(stderr,"%s: Error in scan_ticklabel routine:\n",progname);
      fprintf(stderr,"%s: Index number %d out of range.\n",progname,
         (*st).numlabels);
   }
}

void scan_endlabel(FILE *infile,stoketraject *st,long *linenum,pmap *map,
      long coordnum) {
   scan_label(infile,st,MAX_NUM_LABELS+2,linenum,map,coordnum);
}

/*------------------------------------------------------------------------
| In this routine we do not only like to access the numerical values of the
| input data structure <st> which contains all information of the Stokes
| vector trajectory, but we also wish to update the values.
| Hence this routine takes a pointer to the structure as input, and internally
| dereferences this pointer whenever a value is to be updated. Typical
| usage of the routine, for example as when called from within the
| write_scanned_trajectories routine, is:
|    stoketraject st;
|     FILE *infileptr;
|    long int linenum=0;
|    st=initialize_stoke_trajectory();
|    infileptr=open_infile(map);
|    scan_for_stokes_triplet(infileptr,&st,&linenum);
|        . . .
------------------------------------------------------------------------*/
void scan_for_stokes_triplet(FILE *infileptr,stoketraject *st,long *linenum) {
   float s1,s2,s3;
   if (!fscanf(infileptr,"%f",&s1)) {
      fprintf(stderr,"%s: Error: Faulty S1 in line %ld of trajectory file.\n",
         progname,*linenum);
      exit(1);
   }
   if (!fscanf(infileptr,"%f",&s2)) {
      fprintf(stderr,"%s: Error: Faulty S2 in line %ld of trajectory file.\n",
         progname,*linenum);
      exit(1);
   }
   if (!fscanf(infileptr,"%f",&s3)) {
      fprintf(stderr,"%s: Error: Faulty S3 in line %ld of trajectory file.\n",
         progname,*linenum);
      exit(1);
   }
   ((*st).numcoords)++;
   (*st).s1[(*st).numcoords]=s1;
   (*st).s2[(*st).numcoords]=s2;
   (*st).s3[(*st).numcoords]=s3;
}

short visible(double s1,double s2,double s3,pmap *map) {
   double vprod;
   vprod=s1*cos((*map).rot_psi)*cos((*map).rot_phi)
           -s2*sin((*map).rot_psi)*cos((*map).rot_phi)
           +s3*sin((*map).rot_phi);
   if (vprod>=0.0) { /* point (s1,s2,s3) located in visible region */
      return 1;
   } else { /* point (s1,s2,s3) located in hidden region */
      return 0;
   }
}

/*
The point_just_became_hidden(&st,k) routine takes an index k as input
and checks if the corresponding Stokes vector (st.s1[k],st.s2[k],st.s3[k])
just became hidden.
The routine returns 1 (true) if (st.s1[k],st.s2[k],st.s3[k]) is hidden
while (st.s1[k-1],st.s2[k-1],st.s3[k-1]) is visible, or, for k=1, if
(st.s1[k],st.s2[k],st.s3[k]) is hidden; otherwise 0 (false) is returned.
This routine is useful for finding the start and end points of sub-trajectories
of hidden and visible parts of a Stokes trajectory scanned from file.
*/
short point_just_became_hidden(stoketraject *st,long int k) {
   if (k==1) {
      if ((*st).visible[k]==0) {
         return 1;
      } else {
         return 0;
      }
   } else if ((2<=k)&&(k<=(*st).numcoords)) {
      if (((*st).visible[k]==0)&&((*st).visible[k-1]==1)) {
         return 1;
      } else {
         return 0;
      }
   } else {
      fprintf(stderr,"%s: Error in routine point_just_became_hidden()!\n",
         progname);
      fprintf(stderr,"%s: Index %ld is out of range of current trajectory.\n",
         progname,k);
      fprintf(stderr,"%s: (Maximum possible index is %ld.)\n",
         progname,(*st).numcoords);
      exit(1);
   }
}

short point_just_became_visible(stoketraject *st,long int k) {
   if (k==1) {
      if ((*st).visible[k]==1) {
         return 1;
      } else {
         return 0;
      }
   } else if ((2<=k)&&(k<=(*st).numcoords)) {
      if (((*st).visible[k]==1)&&((*st).visible[k-1]==0)) {
         return 1;
      } else {
         return 0;
      }
   } else {
      fprintf(stderr,"%s: Error in routine point_just_became_visible()!\n",
         progname);
      fprintf(stderr,"%s: Index %ld is out of range of current trajectory.\n",
         progname,k);
      fprintf(stderr,"%s: (Maximum possible index is %ld.)\n",
         progname,(*st).numcoords);
      exit(1);
   }
}

/*
 * The get_screen_coordinates() routine calculates the projected screen
 * coordinates (x,y) in the image from a Stokes triplet (s1,s2,s3).
 * In order to update the output parameters we use these as pointers,
 * to give a behaviour as when using reference variables in languages
 * as Pascal.
 */
void get_screen_coordinates(double *x,double *y,
      double s1,double s2,double s3,pmap *map) {
   double snorm;
   (*x)= s1*sin((*map).rot_psi)+s2*cos((*map).rot_psi);
   (*y)=-s1*cos((*map).rot_psi)*sin((*map).rot_phi)
          +s2*sin((*map).rot_psi)*sin((*map).rot_phi)+s3*cos((*map).rot_phi);
   if ((*map).use_normalized_stokes_params) {
      snorm=sqrt(s1*s1+s2*s2+s3*s3);
      (*x)=(*x)/snorm;
      (*y)=(*y)/snorm;
   }
}

void add_subtrajectory(FILE *outfileptr,stoketraject *st,
      long int ka,long int kb,pmap *map,char type[256]) {
   long int k;
   short j;
   double x,y; /* trash variables for screen coordinates */
   j=1;
   fprintf(outfileptr,"   pickup pencircle scaled %f pt;\n",
      (*map).paththickness);
   if (ka<kb) { /* only draw paths of two points or more */
      for (k=ka;k<=kb;k++) {
         j++;
         get_screen_coordinates(&x,&y,(*st).s1[k],(*st).s2[k],(*st).s3[k],map);
         if (k==ka) {
   /*
            fprintf(outfileptr,"%%\n%% Drawing path No %d\n%%\n",pathnum);
   */
            fprintf(outfileptr,"   p := makepath makepen ");
         }
         if (j==(NUM_COORDS_PER_METAPOST_LINE+1)) {
            fprintf(outfileptr,"\n    ");
            j=1;
         }
         if (k>ka)
            fprintf(outfileptr,"%s",((*map).use_bezier_curves)?"..":"--");
         fprintf(outfileptr,"(%1.4f,%1.4f)",x,y);
         if (k==kb) {
            fprintf(outfileptr,";\n");
            if ((kb==(*st).numcoords)&&((*map).draw_paths_as_arrows)) {
               if ((*map).reverse_arrow_paths) {
                  fprintf(outfileptr,"   drawarrow reverse p scaled radius");
               } else {
                  fprintf(outfileptr,"   drawarrow p scaled radius");
               }
            } else {
               fprintf(outfileptr,"   draw p scaled radius");
            }
            if (!strcmp(type,"hidden")) { /* end of a hidden segment */
               if ((*map).draw_hidden_dashed) {
                  fprintf(outfileptr," dashed evenly withcolor black;\n");
               } else {
                  fprintf(outfileptr," withcolor %f [black,white];\n",
                     (*map).hiddengraytone);
               }
            } else { /* end of a visible segment */
               fprintf(outfileptr," withcolor black;\n");
            }
         }
      }
   }
}

/*
 * Sort out visible from hidden parts of the trajectory.
 */
void sort_out_visible_and_hidden(FILE *outfile,stoketraject *st,pmap *map) {
   long int k;
   for (k=1;k<=(*st).numcoords;k++) {
      if (visible((*st).s1[k],(*st).s2[k],(*st).s3[k],map)) {
         (*st).visible[k]=1;
      } else {
         (*st).visible[k]=0;
      }
   }
}

/*
 * Write hidden parts of the trajectory to file.
 */
void add_hidden_subtrajectories(FILE *outfile,stoketraject *st,pmap *map) {
   long int k,ka,kb;
   k=ka=kb=1;
   while (k<=(*st).numcoords) {
      if (point_just_became_hidden(st,k)) {
         ka=k; /* first index of hidden sub-trajectory */
         kb=ka-1; /* initialize kb<ka as check for the following while loop */
         while ((k<=(*st).numcoords)&&(kb<ka)) {
            if (point_just_became_visible(st,k)) {
               kb=k-1;
            } else {
               k++;
            }
         }
         kb=k-1; /* last index of hidden sub-trajectory */
         if ((*map).verbose) {
            fprintf(stdout,
               "%s: Adding hidden subtrajectory from ka=%ld to kb=%ld\n",
               progname,ka,kb);
         }
         add_subtrajectory(outfile,st,ka,kb,map,"hidden");
      }
      k++;
   }
}

/*
 * Write visible parts of the trajectory to file.
 */
void add_visible_subtrajectories(FILE *outfile,stoketraject *st,pmap *map) {
   long int k,ka,kb;
   k=ka=kb=1;
   while (k<=(*st).numcoords) {
      if (point_just_became_visible(st,k)) {
         ka=k; /* first index of visible sub-trajectory */
         kb=ka-1; /* initialize kb<ka as check for the following while loop */
         while ((k<=(*st).numcoords)&&(kb<ka)) {
            if (point_just_became_hidden(st,k)) {
               kb=k-1;
            } else {
               k++;
            }
         }
         kb=k-1; /* last index of visible sub-trajectory */
         if ((*map).verbose) {
            fprintf(stdout,
               "%s: Adding visible subtrajectory from ka=%ld to kb=%ld\n",
               progname,ka,kb);
         }
         /*
          * We allow the ends of visible parts to go one coordinate step
          * behind the sphere so as to give a smooth connection to the
          * hidden parts.
          */
         if (ka>1) ka--;
         if (kb<(*st).numcoords) kb++;
         add_subtrajectory(outfile,st,ka,kb,map,"visible");
      }
      k++;
   }
}

void add_scanned_trajectory(FILE *outfileptr,stoketraject *st,pmap *map,
      short viewtype) {
   sort_out_visible_and_hidden(outfileptr,st,map);
   if (viewtype==HIDDEN) {
      add_hidden_subtrajectories(outfileptr,st,map);
   } else if (viewtype==VISIBLE) {
      add_visible_subtrajectories(outfileptr,st,map);
   } else { /* if invalid |viewtype| option */
      fprintf(stderr,"%s: Error in add_scanned_trajectory: ",progname);
      fprintf(stderr,"Invalid viewtype value! (%d)\n",viewtype);
      exit(1);
   }
}

/*
 * The add_scanned_labels() routines the previously scanned text labels
 * of the trajectory |st| to the output MetaPost source file.
 */
void add_scanned_labels(FILE *outfileptr,stoketraject *st,pmap *map) {
   long int j,k;
   double x,y;

   for (k=1;k<=(MAX_NUM_LABELS+2);k++) {
      if ((*st).labellength[k]>0) {
         if ((*st).labelpos[k]==TOPLABEL) {
            fprintf(outfileptr,"   label.top");
         } else if ((*st).labelpos[k]==UPPERLEFTLABEL) {
            fprintf(outfileptr,"   label.ulft");
         } else if ((*st).labelpos[k]==LEFTLABEL) {
            fprintf(outfileptr,"   label.lft");
         } else if ((*st).labelpos[k]==LOWERLEFTLABEL) {
            fprintf(outfileptr,"   label.llft");
         } else if ((*st).labelpos[k]==BOTTOMLABEL) {
            fprintf(outfileptr,"   label.bot");
         } else if ((*st).labelpos[k]==LOWERRIGHTLABEL) {
            fprintf(outfileptr,"   label.lrt");
         } else if ((*st).labelpos[k]==RIGHTLABEL) {
            fprintf(outfileptr,"   label.rt");
         } else if ((*st).labelpos[k]==UPPERRIGHTLABEL) {
            fprintf(outfileptr,"   label.urt");
         } else {
            fprintf(stderr,
               "%s: add_scanned_labels: Invalid labelpos (%d) detected ",
               progname,(*st).labelpos[k]);
            fprintf(stderr,"at label No %ld\n",k);
            fprintf(stderr,
               "%s: add_scanned_labels: Labelstring is \042",progname);
            for (j=1;j<=(*st).labellength[k];j++)
               fprintf(stderr,"%c",(*st).labeltext[k][j]);
            fprintf(stderr,"\044\n");
            exit(1);
         }
         get_screen_coordinates(&x,&y,(*st).s1[(*st).label[k]],
            (*st).s2[(*st).label[k]],(*st).s3[(*st).label[k]],map);
         fprintf(outfileptr,"(btex ");
         for (j=1;j<=(*st).labellength[k];j++)
            fprintf(outfileptr,"%c",(*st).labeltext[k][j]);
         fprintf(outfileptr," etex,(%f,%f)*radius);\n",x,y);
      }
   }
}

void scan_for_tickmark(FILE *infileptr,stoketraject *st,long *linenum) {
   if (tickmark(infileptr)) {
      (*st).numtickmarks++;
      (*st).tickmark[(*st).numtickmarks]=(*st).numcoords;
   }
}

void scan_for_tickmarklabel(FILE *infileptr,stoketraject *st,pmap *map,
      long *linenum) {
   if (tickmarklabel(infileptr)) {
      (*st).numlabels++;
      (*st).label[(*st).numlabels]=(*st).numcoords;
      scan_label(infileptr,st,(*st).numlabels,linenum,map,
         (*st).label[(*st).numlabels]);
   }
}

void get_tickmark_screen_coordinates(double *xa,double *ya,
      double *xb,double *yb,long int k,stoketraject *st,pmap *map) {
   double xt,yt,snorm,s1n,s2n,s3n,s1a,s2a,s3a,s1b,s2b,s3b,
      s0,s1,s2,s3,p1,p2,p3,q1,q2,q3;

   /* Calculate the normalized approximate tangential vector to path
    * at the tickmark point with index k.
    */
   if ((*st).tickmark[k]==1) {
      q1=(*st).s1[(*st).tickmark[k]+1]-(*st).s1[(*st).tickmark[k]];
      q2=(*st).s2[(*st).tickmark[k]+1]-(*st).s2[(*st).tickmark[k]];
      q3=(*st).s3[(*st).tickmark[k]+1]-(*st).s3[(*st).tickmark[k]];
   } else if ((*st).tickmark[k]==(*st).numcoords) {
      q1=(*st).s1[(*st).tickmark[k]]-(*st).s1[(*st).tickmark[k]-1];
      q2=(*st).s2[(*st).tickmark[k]]-(*st).s2[(*st).tickmark[k]-1];
      q3=(*st).s3[(*st).tickmark[k]]-(*st).s3[(*st).tickmark[k]-1];
   } else if ((1<(*st).tickmark[k])&&((*st).tickmark[k]<(*st).numcoords)) {
      q1=(*st).s1[(*st).tickmark[k]+1]-(*st).s1[(*st).tickmark[k]-1];
      q2=(*st).s2[(*st).tickmark[k]+1]-(*st).s2[(*st).tickmark[k]-1];
      q3=(*st).s3[(*st).tickmark[k]+1]-(*st).s3[(*st).tickmark[k]-1];
   } else {
      fprintf(stderr,"%s: In get_tickmark_screen_coordinates:\n",progname);
      fprintf(stderr,"%s: Index k=%ld out of valid range!\n",
         progname,(*st).tickmark[k]);
      exit(1);
   }
   snorm=sqrt(q1*q1+q2*q2+q3*q3);
   q1/=snorm;
   q2/=snorm;
   q3/=snorm;

   /* Get normalized (unitary) Stokes vector. */
   s1=(*st).s1[(*st).tickmark[k]];
   s2=(*st).s2[(*st).tickmark[k]];
   s3=(*st).s3[(*st).tickmark[k]];
   s0=sqrt(s1*s1+s2*s2+s3*s3);
   s1n=s1/s0;
   s2n=s2/s0;
   s3n=s3/s0;

   /* Calculate the normalized vector transverse to the tangent of path,
    * {\bf p}={\bf s}\times{\bf q}/|{\bf s}\times{\bf q}|.
    */
   p1=s2n*q3-s3n*q2;
   p2=s3n*q1-s1n*q3;
   p3=s1n*q2-s2n*q1;
   snorm=sqrt(p1*p1+p2*p2+p3*p3);
   p1=p1/snorm;
   p2=p2/snorm;
   p3=p3/snorm;

   /* Calculate the 1st endpoint of tick mark in Stokes parameter space */
   s1a=s1n+0.028213*p1;
   s2a=s2n+0.028213*p2;
   s3a=s3n+0.028213*p3;

/*   s1a=s1+(*map).ticksize*p1;
   s2a=s2+(*map).ticksize*p2;
   s3a=s3+(*map).ticksize*p3; */

   /* Calculate the 2nd endpoint of tick mark in Stokes parameter space */
   s1b=s1n-0.028213*p1;
   s2b=s2n-0.028213*p2;
   s3b=s3n-0.028213*p3;

   /* Get the screen coordinates of the ends of the tick mark */
   get_screen_coordinates(&xt,&yt,s0*s1a,s0*s2a,s0*s3a,map);
   if (isnan(xt) || isnan(yt)) {
     fprintf(stderr,"%s: [Case A] NaN detected by routine "
	     "get_tickmark_screen_coordinates: xt=%1.4f, yt=%1.4f\n",
	     progname, xt,yt);
     fprintf(stderr,"%s: s0=%1.4f, s1a=%1.4f, s2a=%1.4f, s3a=%1.4f\n",
	     progname, s0, s1a, s2a, s3a);
      exit(1);
   }
   (*xa)=xt;
   (*ya)=yt;
   get_screen_coordinates(&xt,&yt,s0*s1b,s0*s2b,s0*s3b,map);
   if (isnan(xt) || isnan(yt)) {
     fprintf(stderr,"%s: [Case B] NaN detected by routine "
	     "get_tickmark_screen_coordinates: xt=%1.4f, yt=%1.4f\n",
	     progname, xt, yt);
     fprintf(stderr,"%s: s0=%1.4f, s1b=%1.4f, s2b=%1.4f, s3b=%1.4f\n",
	     progname, s0, s1b, s2b, s3b);
      exit(1);
   }
   (*xb)=xt;
   (*yb)=yt;
}

void add_scanned_tickmarks(FILE *outfileptr,stoketraject *st,pmap *map,
      short viewtype) {
   long int k;
   double xa,ya,xb,yb;

   fprintf(outfileptr,"   pickup pencircle scaled %f pt;\n",
      (*map).paththickness/2.0);
   for (k=1;k<=(*st).numtickmarks;k++) {
      get_tickmark_screen_coordinates(&xa,&ya,&xb,&yb,k,st,map);
      fprintf(outfileptr,"   p:=makepath makepen (%f,%f)--(%f,%f);\n",
         xa,ya,xb,yb);
      if (((*st).visible[(*st).tickmark[k]])&&(viewtype==VISIBLE)) {
         fprintf(outfileptr,"   draw p scaled radius;\n");
      } else if ((!((*st).visible[(*st).tickmark[k]]))&&(viewtype==HIDDEN)) {
         fprintf(outfileptr,"   draw p scaled radius");
         fprintf(outfileptr," withcolor %f [black,white];\n",
            (*map).hiddengraytone);
      }
   }
}

/*-----------------------------------------------------------------------------
| (REWRITE THIS COMMENT)
| Draw the trajectories of Stokes parameters on the Poincare sphere (if a
| filename containing the path was specified at startup of the program).
| In order to properly take care of the fact that we might be over-writing
| parts of the 'visible' parts of the path with 'invisible' parts from the
| hidden parts of the Poincare sphere, we must traverse the path-drawing
| routine twice, in the second run only writing the parts that are visible.
| This is only important when the shade of the front, 'visible' paths are
| different from the paths in the back, 'invisible' parts of the sphere.
| Notice that, for example, with hidden parts of the path drawn with black,
| dashed lines, and visible part drawn with solid black lines, it does not
| matter whether this two-pass drawing takes place or not.
|
| This routine also particularly checks for statments of labels to be applied
| at the begin and end points of the trajectories, that is to say, checking
| for statements of type 'b <pos> "TeX label string"' or 'e <pos> "TeX label
| string"' immediately after the 'p' and 'q' statements of each trajectory.
| These labels could equally well be supplied as regular labels after the
| first and last triplets of data, but in some cases it is for example in
| batch mode operation easier to simply concatenate in the Stokes data without
| having to manually go in and add these label statements. This is the very
| reason for this additional feature of begin and end label statements in the
| data syntax as accepted by the program.
|
| Depending on whether the boolean input variable |viewtype| is |HIDDEN| or
| |VISIBLE|, this routine will either flush out the hidden or visible parts
| of a Stokes trajectory. This construction is done in order for the main
| program to call the routine in two passes after each other, one for the
| hidden parts and one for the visible parts (which we do not wish to have
| later overwritten by hidden parts of other trajectories scanned later on.
| This switch is not explicitly present in the algorithm of this routine, but
| rather just used as passed on to the routines |add_scanned_trajectory()| and
| |add_scanned_tickmarks()|, which take care of adding the hidden ans visible
| trajectories with tickmarks in a coherent manner.
|
| As being the key routine in the flushing of the Stokes trajectories to
| file, this routine also takes care of writing any present tick marks or
| labels, by calling the |add_scanned_tickmarks()| and |add_scanned_labels()|
| routines.
|
| Throughout the parsing and writing of the Stokes parameter trajectory to
| file, the entire data set is kept in the structure |st| (short for Stokes
| trajectory), of type |stoketraject| as defined in the definitions section
| of this program.
-----------------------------------------------------------------------------*/
void write_scanned_trajectories(FILE *outfileptr,pmap map,short viewtype) {
   FILE *infileptr = NULL;
   stoketraject st; /* data structure for keeping track of trajectories */
   long int linenum,coord;
   int i=0, k;
/*   int pathnum=0; rrrr */ /* used to keep track on the number of paths scanned so far */

   initialize_stoke_trajectory(&st); /* Allocate memory for arrays etc. */
   reset_stokes_trajectory_struct(&st); /* Make sure all data is cleared */
   infileptr=open_infile(map); /* open file to read Stokes triplets from */
   /*-------------------------------------------------------------------------
   | If the file pointer |infileptr| after opening the file with the
   | |open_infile()| routine points to NULL, this indicates that the filename
   | was empty, and that now Stokes trajectories hence should be read from
   | file. If there was a problem opening the file for reading, due to read
   | permissions, file existence etc, this would automatically have taken
   | care of by the |open_infile()| routine.
   -------------------------------------------------------------------------*/
   if (infileptr!=NULL) {
      fprintf(outfileptr,"  oldahangle:=ahangle;\n");
      fprintf(outfileptr,"  ahangle:=%f;\n",map.arrowheadangle);
      fprintf(outfileptr,"  pickup pencircle scaled %f pt;\n",
         map.paththickness);
      linenum=1; /* counter for keeping track of line numbers in input file */
      coord=0; /* counter for keeping track of trajectory coordinate numbers */
      while (new_trajectory(infileptr)) {
         if (map.verbose) fprintf(stdout,
            "%s: New trajectory detected at line %ld\n",progname,linenum);
         i++; /* increment trajectory counter */
         readaway_comments_and_blanks(infileptr,&linenum);
         if (beginlabel(infileptr)) { /* check for text label at begin point */
            if (map.verbose)
               fprintf(stdout, "%s: Begin-point label detected at line %ld\n",
                  progname,linenum);
            scan_beginlabel(infileptr,&st,&linenum,&map,1);
            readaway_comments_and_blanks(infileptr,&linenum);
            if (map.verbose) {
               fprintf(stdout,"%s: Parsed begin label string '",progname);
               for (k=1;k<=st.labellength[1];k++)
                  fprintf(stdout,"%c",st.labeltext[1][k]);
               fprintf(stdout,"' [%d characters]\n",st.labellength[1]);
            }
         }
         if (map.verbose) fprintf(stdout,
            "%s: Scanning Stokes trajectory starting at line %ld.\n",
               progname,linenum);
         st.numcoords=0; /* reset coordinate counter */
         while (!end_of_trajectory(infileptr)) { /* scan current trajectory */
            scan_for_stokes_triplet(infileptr,&st,&linenum);
            coord++;
            readaway_comments_and_blanks(infileptr,&linenum);
            scan_for_tickmark(infileptr,&st,&linenum);
            readaway_comments_and_blanks(infileptr,&linenum);
            scan_for_tickmarklabel(infileptr,&st,&map,&linenum);
            readaway_comments_and_blanks(infileptr,&linenum);
         }
         if (map.verbose) fprintf(stdout,
            "%s: End of Stokes trajectory detected at line %ld.\n",
               progname,linenum);
         readaway_comments_and_blanks(infileptr,&linenum);
         if (endlabel(infileptr)) { /* check for text label at end point */
            if (map.verbose) fprintf(stdout,
               "%s: End-point label detected at line %ld\n",progname,linenum);
            scan_endlabel(infileptr,&st,&linenum,&map,coord);
            readaway_comments_and_blanks(infileptr,&linenum);
            if (map.verbose) {
               fprintf(stdout,"%s: Parsed end label string '",progname);
               for (k=1;k<=st.labellength[MAX_NUM_LABELS+2];k++)
                  fprintf(stdout,"%c",st.labeltext[MAX_NUM_LABELS+2][k]);
               fprintf(stdout,"' [%d characters]\n",
                  st.labellength[MAX_NUM_LABELS+2]);
            }
         }
         add_scanned_trajectory(outfileptr,&st,&map,viewtype);
         add_scanned_tickmarks(outfileptr,&st,&map,viewtype);
         add_scanned_labels(outfileptr,&st,&map);
         reset_stokes_trajectory_struct(&st);
      } /* End of "while (new_trajectory(infileptr)) ..." */
      fclose(infileptr);
      fprintf(outfileptr,"  ahangle:=oldahangle;\n");
   } else {
/*
 *      fprintf(stderr,
 *         "%s: Sorry, trajectory scanning from stdin not implemented yet.",
 *         progname);
 *      exit(1);
 */
   }
}

/*-----------------------------------------------------------------------------
| Draw any additional arrows (if specified by user) onto the Poincare sphere.
| This feature is useful for example whenever there is a distinct transition
| between two polarization states that need to be pointed out, or whenever
| just a principal change is to be pointed out.
-----------------------------------------------------------------------------*/
void write_additional_arrows(FILE *outfileptr,pmap map) {
   int i;
   double s1,s2,s3,s1a,s2a,s3a,s1b,s2b,s3b,t,dt=0.02,xtmp,ytmp;
   if (map.numarrows > 0) {
      fprintf(outfileptr,
         "%%\n"
         "%% Draw the paths of the arrows specified by the user.\n"
         "%%\n");
      fprintf(outfileptr,"   pickup pencircle scaled 0.5pt;\n");
      for (i=1;i<=map.numarrows;i++) {
         s1a=map.arrows[1][i];
         s2a=map.arrows[2][i];
         s3a=map.arrows[3][i];
         s1b=map.arrows[4][i];
         s2b=map.arrows[5][i];
         s3b=map.arrows[6][i];
         if (map.use_normalized_stokes_params) {
            xtmp=sqrt(s1a*s1a+s2a*s2a+s3a*s3a);
            s1a=s1a/xtmp;
            s2a=s2a/xtmp;
            s3a=s3a/xtmp;
            xtmp=sqrt(s1b*s1b+s2b*s2b+s3b*s3b);
            s1b=s1b/xtmp;
            s2b=s2b/xtmp;
            s3b=s3b/xtmp;
         }
         fprintf(outfileptr,"   p := makepath makepen\n");
         fprintf(outfileptr,"      ");
         for (t=0.0;t<=0.5+dt/1000.0;t+=dt) {
            if (t > dt) fprintf(outfileptr,"                  ");
            if (t > 0.0) fprintf(outfileptr,"..");
            s1=(1.0 - t)*s1a + t*s1b;
            s2=(1.0 - t)*s2a + t*s2b;
            s3=(1.0 - t)*s3a + t*s3b;
            xtmp=sqrt(s1*s1+s2*s2+s3*s3);
            s1 /= xtmp;
            s2 /= xtmp;
            s3 /= xtmp;
            xtmp= s1*sin(map.rot_psi)+s2*cos(map.rot_psi);
            ytmp=-s1*cos(map.rot_psi)*sin(map.rot_phi) + \
                    s2*sin(map.rot_psi)*sin(map.rot_phi) + \
                    s3*cos(map.rot_phi);
            fprintf(outfileptr,"(%f,%f)",xtmp,ytmp);
            if ((t > (0.0+dt/1000.0)) && (t < (0.5-dt/1000.0)))
               fprintf(outfileptr,"\n");
         }
         fprintf(outfileptr,";\n");
         if ((-0.5 <= map.arrows[7][i]) && (map.arrows[7][i] < 0.5)) {
            fprintf(outfileptr,\
               "   drawarrow p scaled radius withcolor %f [white,black];\n",\
                                                          map.arrows[8][i]);
         } else if ((0.5 <= map.arrows[7][i]) && (map.arrows[7][i] < 1.5)) {
            fprintf(outfileptr,\
               "   drawarrow p scaled radius dashed evenly withcolor %f"
               " [white,black];\n", map.arrows[8][i]);
         }
         fprintf(outfileptr,"   p := makepath makepen\n");
         fprintf(outfileptr,"      ");
         for (t=0.5;t<=1.0+dt/1000.0;t+=dt) {
            if (t > 0.5+dt) fprintf(outfileptr,"                  ");
            if (t > 0.5) fprintf(outfileptr,"..");
            s1=(1.0 - t)*s1a + t*s1b;
            s2=(1.0 - t)*s2a + t*s2b;
            s3=(1.0 - t)*s3a + t*s3b;
            xtmp=sqrt(s1*s1+s2*s2+s3*s3);
            s1 /= xtmp;
            s2 /= xtmp;
            s3 /= xtmp;
            xtmp= s1*sin(map.rot_psi)+s2*cos(map.rot_psi);
            ytmp=-s1*cos(map.rot_psi)*sin(map.rot_phi) + \
                    s2*sin(map.rot_psi)*sin(map.rot_phi) + \
                    s3*cos(map.rot_phi);
            fprintf(outfileptr,"(%f,%f)",xtmp,ytmp);
            if ((t > (0.5+dt/1000.0)) && (t < (1.0-dt/1000.0)))
               fprintf(outfileptr,"\n");
         }
         fprintf(outfileptr,";\n");
         if ((-0.5 <= map.arrows[7][i]) && (map.arrows[7][i] < 0.5)) {
            fprintf(outfileptr,\
               "   draw p scaled radius withcolor %f [white,black];\n",\
                                                           map.arrows[8][i]);
         } else if ((0.5 <= map.arrows[7][i]) && (map.arrows[7][i] < 1.5)) {
            fprintf(outfileptr,\
               "   draw p scaled radius dashed evenly withcolor %f"
               " [white,black];\n", map.arrows[8][i]);
         }
      }
   }
}

/*-----------------------------------------------------------------------------
| Draw the coordinate axes of the $(S_1,S_2,S_3)$-space.
-----------------------------------------------------------------------------*/
void write_coordinate_axes(FILE *outfileptr,pmap map) {
fprintf(outfileptr,
 "%%\n"
 "%% Draw the $S_1$-, $S_2$- and $S_3$-axis of the Poincare sphere.\n"
 "%% First of all, calculate the transformations of the intersections\n"
 "%% for the unity sphere.\n"
 "%%\n");
fprintf(outfileptr,
 "%% Used variables:\n"
 "%%\n"
 "%%    behind_distance : Specifies the relative distance of the coordi-\n"
 "%%                      axes to be plotted behind origo (in negative di-\n"
 "%%                      rection of respective axis.\n"
 "%%\n");
fprintf(outfileptr,
 "%%   outside_distance_s1 : The relative distance from origo to the point\n"
 "%%                         of the arrow head of the coordinate axis S1.\n"
 "%%                         If this is set to 1.0, the arrow head will\n"
 "%%                         point directly at the Poincare sphere.\n"
 "%%\n");
fprintf(outfileptr,
 "%%   outside_distance_s2 : Same as above, except that this one controls\n"
 "%%                         the S2 coordinate axis instead.\n"
 "%%\n");
fprintf(outfileptr,
 "%%   outside_distance_s3 : Same as above, except that this one controls\n"
 "%%                         the S3 coordinate axis instead.\n"
 "%%\n");
fprintf(outfileptr,
 "%%    insidecolval :    Specifies the shade of gray to use for the parts\n"
 "%%                      of the coordinate axes that are inside the Poin-\n"
 "%%                      care sphere. Values must be between 0 and 1,\n"
 "%%                      where:  '0.0' <=> 'white';  '1.0' <=> 'black'\n"
 "%%\n");
fprintf(outfileptr,
 "   behind_distance_s1  := -%f;\n"
 "   behind_distance_s2  := -%f;\n"
 "   behind_distance_s3  := -%f;\n"
 "   outside_distance_s1 :=  %f;\n"
 "   outside_distance_s2 :=  %f;\n"
 "   outside_distance_s3 :=  %f;\n"
 "   insidecolval := .85;    %% '0.0' <=> 'white';  '1.0' <=> 'black'\n"
 "\n",
   map.neg_axis_length_s1, map.neg_axis_length_s2, map.neg_axis_length_s3,
   map.pos_axis_length_s1, map.pos_axis_length_s2, map.pos_axis_length_s3);
   fprintf(outfileptr,
     "   pickup pencircle scaled %f pt;\n",map.coordaxisthickness);

fprintf(outfileptr,
 "%%\n"
 "%% Start with drawing the x-axis...\n"
 "%%\n"
 "   x_bis_start :=  radius*behind_distance_s1*cosd(rot_psi)*cosd(rot_phi);\n"
 "   y_bis_start :=  radius*behind_distance_s1*sind(rot_psi);\n"
 "   z_bis_start := -radius*behind_distance_s1*cosd(rot_psi)*sind(rot_phi);\n"
 "   x_bis_intersect :=  radius*cosd(rot_psi)*cosd(rot_phi);\n"
 "   y_bis_intersect :=  radius*sind(rot_psi);\n"
 "   z_bis_intersect := -radius*cosd(rot_psi)*sind(rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (outside_distance_s1*y_bis_intersect,\n"
 "              outside_distance_s1*z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.%s(btex $%s$ etex,\n"
 "             (outside_distance_s1*y_bis_intersect,\n"
 "              outside_distance_s1*z_bis_intersect));\n"
 "\n",map.axislabelposition_s1,
(map.user_specified_axislabels ? map.axislabel_s1
   : (map.use_normalized_stokes_params ? "S_1/S_0" : "S_1")));

fprintf(outfileptr,
 "%%\n"
 "%% ... then draw the y-axis ...\n"
 "%%\n"
 "   x_bis_start := -radius*behind_distance_s2*sind(rot_psi)*cosd(rot_phi);\n"
 "   y_bis_start :=  radius*behind_distance_s2*cosd(rot_psi);\n"
 "   z_bis_start :=  radius*behind_distance_s2*sind(rot_psi)*sind(rot_phi);\n"
 "   x_bis_intersect := -radius*sind(rot_psi)*cosd(rot_phi);\n"
 "   y_bis_intersect :=  radius*cosd(rot_psi);\n"
 "   z_bis_intersect :=  radius*sind(rot_psi)*sind(rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (outside_distance_s2*y_bis_intersect,\n"
 "              outside_distance_s2*z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.%s(btex $%s$ etex,\n"
 "             (outside_distance_s2*y_bis_intersect,\n"
 "              outside_distance_s2*z_bis_intersect));\n"
 "\n",map.axislabelposition_s2,
(map.user_specified_axislabels ? map.axislabel_s2
   : (map.use_normalized_stokes_params ? "S_2/S_0" : "S_2")));

fprintf(outfileptr,
 "%%\n"
 "%% ... then, finally, draw the z-axis.\n"
 "%%\n"
 "   x_bis_start := radius*behind_distance_s3*sind(rot_phi);\n"
 "   y_bis_start := 0.0;\n"
 "   z_bis_start := radius*behind_distance_s3*cosd(rot_phi);\n"
 "   x_bis_intersect := radius*sind(rot_phi);\n"
 "   y_bis_intersect := 0.0;\n"
 "   z_bis_intersect := radius*cosd(rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (outside_distance_s3*y_bis_intersect,\n"
 "              outside_distance_s3*z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.%s(btex $%s$ etex,\n"  /* For z-label to the right of S_3 axis */
 "             (outside_distance_s3*y_bis_intersect,\n"
 "              outside_distance_s3*z_bis_intersect));\n"
 "\n",map.axislabelposition_s3,
(map.user_specified_axislabels ? map.axislabel_s3
   : (map.use_normalized_stokes_params ? "S_3/S_0" : "S_3")));
}

/*-----------------------------------------------------------------------------
| In case the user has specified that an additional coordinate system should
| be drawn, draw the coordinate axes of the additional $(x,y,z)$-space.
| The rule is that only coordinate axes that have explicitly declared
| axis labels should be drawn.  The reason for that I have chosen this
| way of operation is that the rotated, additional coordinate system might
| still have one axis in common with the original one, potentially making
| ugly double-drawn axes with corresponding labels.
-----------------------------------------------------------------------------*/
void write_additional_coordinate_axes(FILE *outfileptr,pmap map) {
if (map.user_specified_additional_coordinate_system)  /* whoaaaou again ... */
{
fprintf(outfileptr,
 "%%\n"
 "%% Draw the $S_1$-, $S_2$- and $S_3$-axis of the Poincare sphere.\n"
 "%% First of all, calculate the transformations of the intersections\n"
 "%% for the unity sphere.\n"
 "%%\n");
fprintf(outfileptr,
 "%% Used variables are similar to the ones described for\n"
 "%% drawing the original coordinate system.\n"
 "%%\n");
fprintf(outfileptr,
 "   xtra_behind_distance_x  := -%f;\n"
 "   xtra_behind_distance_y  := -%f;\n"
 "   xtra_behind_distance_z  := -%f;\n",
   map.xtra_neg_axis_length_x,
   map.xtra_neg_axis_length_y,
   map.xtra_neg_axis_length_z);
fprintf(outfileptr,
 "   xtra_outside_distance_x :=  %f;\n"
 "   xtra_outside_distance_y :=  %f;\n"
 "   xtra_outside_distance_z :=  %f;\n",
   map.xtra_pos_axis_length_x,
   map.xtra_pos_axis_length_y,
   map.xtra_pos_axis_length_z);
fprintf(outfileptr,
 "   insidecolval := .85;    %% '0.0' <=> 'white';  '1.0' <=> 'black'\n\n");

if (map.user_specified_xtra_axislabel_x)
{
fprintf(outfileptr,
 "%%\n"
 "%% Start with drawing the x-axis...\n"
 "%%\n");
fprintf(outfileptr,
 "   x_bis_start :=  radius * xtra_behind_distance_x\n"
 "                          * cosd(rot_psi + delta_rot_psi)\n"
 "                          * cosd(rot_phi + delta_rot_phi);\n"
 "   y_bis_start :=  radius * xtra_behind_distance_x\n"
 "                          * sind(rot_psi + delta_rot_psi);\n"
 "   z_bis_start := -radius * xtra_behind_distance_x\n"
 "                          * cosd(rot_psi + delta_rot_psi)\n"
 "                          * sind(rot_phi + delta_rot_phi);\n");
fprintf(outfileptr,
 "   x_bis_intersect :=  radius * cosd(rot_psi + delta_rot_psi)\n"
 "                              * cosd(rot_phi + delta_rot_phi);\n"
 "   y_bis_intersect :=  radius * sind(rot_psi + delta_rot_psi);\n"
 "   z_bis_intersect := -radius * cosd(rot_psi + delta_rot_psi)\n"
 "                              * sind(rot_phi + delta_rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (xtra_outside_distance_x * y_bis_intersect,\n"
 "              xtra_outside_distance_x * z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.bot(btex $%s$ etex,\n"
 "             (xtra_outside_distance_x * y_bis_intersect,\n"
 "              xtra_outside_distance_x * z_bis_intersect));\n"
 "\n", map.xtra_axislabel_x);
}

if (map.user_specified_xtra_axislabel_y)
{
fprintf(outfileptr,
 "%%\n"
 "%% ... then draw the y-axis ...\n"
 "%%\n");
fprintf(outfileptr,
 "   x_bis_start := -radius * xtra_behind_distance_y\n"
 "                          * sind(rot_psi + delta_rot_psi)\n"
 "                          * cosd(rot_phi + delta_rot_phi);\n"
 "   y_bis_start :=  radius * xtra_behind_distance_y\n"
 "                          * cosd(rot_psi + delta_rot_psi);\n"
 "   z_bis_start :=  radius * xtra_behind_distance_y\n"
 "                          * sind(rot_psi + delta_rot_psi)\n"
 "                          * sind(rot_phi + delta_rot_phi);\n");
fprintf(outfileptr,
 "   x_bis_intersect := -radius * sind(rot_psi + delta_rot_psi)\n"
 "                              * cosd(rot_phi + delta_rot_phi);\n"
 "   y_bis_intersect :=  radius * cosd(rot_psi + delta_rot_psi);\n"
 "   z_bis_intersect :=  radius * sind(rot_psi + delta_rot_psi)\n"
 "                              * sind(rot_phi + delta_rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (xtra_outside_distance_y * y_bis_intersect,\n"
 "              xtra_outside_distance_y * z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.bot(btex $%s$ etex,\n"
 "             (xtra_outside_distance_y * y_bis_intersect,\n"
 "              xtra_outside_distance_y * z_bis_intersect));\n"
 "\n", map.xtra_axislabel_y);
}

if (map.user_specified_xtra_axislabel_z)
{
fprintf(outfileptr,
 "%%\n"
 "%% ... then, finally, draw the z-axis.\n"
 "%%\n"
 "   x_bis_start := radius * xtra_behind_distance_z\n"
 "                         * sind(rot_phi + delta_rot_phi);\n"
 "   y_bis_start := 0.0;\n"
 "   z_bis_start := radius * xtra_behind_distance_z\n"
 "                         * cosd(rot_phi + delta_rot_phi);\n"
 "   x_bis_intersect := radius * sind(rot_phi + delta_rot_phi);\n"
 "   y_bis_intersect := 0.0;\n"
 "   z_bis_intersect := radius * cosd(rot_phi + delta_rot_phi);\n");
if (map.draw_axes_inside_sphere) fprintf(outfileptr,
 "   p := makepath makepen \n"
 "             (y_bis_start,z_bis_start)--(y_bis_intersect,z_bis_intersect);\n"
 "   draw p dashed evenly withcolor insidecolval [white,black];\n");
fprintf(outfileptr,
 "   p := makepath makepen (y_bis_intersect,z_bis_intersect)--\n"
 "             (xtra_outside_distance_z * y_bis_intersect,\n"
 "              xtra_outside_distance_z * z_bis_intersect);\n"
 "   drawarrow p;\n");
fprintf(outfileptr,
 "   label.top(btex $%s$ etex,\n"
 "             (xtra_outside_distance_z * y_bis_intersect,\n"
 "              xtra_outside_distance_z * z_bis_intersect));\n"
 "\n", map.xtra_axislabel_z);
}
}
}

void write_included_auxiliary_source(FILE *outfileptr,pmap map) {
   if (map.user_specified_auxfile) {
      fprintf(outfileptr,
         "%%\n"
         "%% The following external file is included"
         " (using the --auxsource option):\n%%    %s  [MetaPost source]\n"
         "%%\n"
         "   input %s\n",map.auxfilename, map.auxfilename);
   }
   fprintf(outfileptr,
      "   endfig;\n"
      "end\n"
   );
}

/*-----------------------------------------------------------------------
| Generate Encapsulated PostScript output from the MetaPost-source.
-----------------------------------------------------------------------*/
void generate_eps_image(pmap map) {
   char tmpstr[1024];
   long int llx,lly,urx,ury;

   /*--------------------------------------------------------------------
   | Compile the MetaPost code into EPS with control codes for TeX.
   --------------------------------------------------------------------*/
   sprintf(tmpstr,"mpost -job-name %s %s;",map.epsjobname,map.outfilename);
   if (map.verbose)
      fprintf(stdout,"%s: Executing system command: %s\n",progname,tmpstr);
   if (system(tmpstr)) {  /* Anything but 0 in return is a failure */
      fprintf(stderr,"Failed executing %s!\n", tmpstr);
   }

   /*--------------------------------------------------------------------
   | Use TeX for generating a self-containing DVI output of the figure.
   --------------------------------------------------------------------*/
   sprintf(tmpstr,"tex -job-name %s \'\\input epsf\\nopagenumbers"
      "\\centerline{\\epsfbox{%s.1}}\\bye\';",map.epsjobname,map.epsjobname);
   if (map.verbose)
      fprintf(stdout,"%s: Executing system command: %s\n",progname,tmpstr);
   if (system(tmpstr)) {  /* Anything but 0 in return is a failure */
      fprintf(stderr,"Failed executing %s!\n", tmpstr);
   }

   /*--------------------------------------------------------------------
   | Use DVIPS for generating a self-containing EPS output with a tight
   | bounding box from the DVI.
   --------------------------------------------------------------------*/
   sprintf(tmpstr,"dvips -D1200 -E %s.dvi -o %s.eps",
      map.epsjobname,map.epsjobname);
   if (map.verbose)
      fprintf(stdout,"%s: Executing system command: %s\n",progname,tmpstr);
   if (system(tmpstr)) {  /* Anything but 0 in return is a failure */
      fprintf(stderr,"Failed executing %s!\n", tmpstr);
   }

   /*--------------------------------------------------------------------
   | Finally, extract the bounding box of the generated Encapsulated
   | PostScript file, in order to get an idea of the natural physical
   | size of the figure. This is useful in order to enter correct
   | settings as the resulting image is included in for example TeX
   | documents, in which case the typical inclusion yields
   |    \input epsf
   |    \centerline{\epsfxsize=<reported x-size>\epsfbox{<filename>.eps}}
   --------------------------------------------------------------------*/
   sprintf(tmpstr,"%s.eps",map.epsjobname);
   scan_for_boundingbox(tmpstr,&llx,&lly,&urx,&ury);
   fprintf(stdout,"%s: Bounding box of %s:\n"
      "     width=%-4.2f mm (%ld pts), height=%-4.2f mm (%ld pts)\n",
      progname,tmpstr,(urx-llx)*(25.4/72.27),urx-llx,
               (ury-lly)*(25.4/72.27),ury-lly);
}

int main(int argc, char *argv[]) {
   pmap map;              /* The data structure containing input parameters */
   FILE *outfileptr=NULL; /* The destination file for MetaPost code */

   map=parse_command_line(argc,argv);
   if (map.verbose) show_banner();
   display_arrow_specs(map);
   outfileptr=open_outfile(map);
   write_header(outfileptr,map,argc,argv);
   write_euler_angle_specs(outfileptr,map);
   write_sphere_shading_specs(outfileptr,map);
   write_shaded_sphere(outfileptr,map); /* Generate the background sphere */
   write_equators(outfileptr,map); /* Generate the equators S_k=0, k=1,2,3 */
   write_scanned_trajectories(outfileptr,map,HIDDEN);
   write_scanned_trajectories(outfileptr,map,VISIBLE);
   write_additional_arrows(outfileptr,map);
   write_coordinate_axes(outfileptr,map);
   write_additional_coordinate_axes(outfileptr,map);
   write_included_auxiliary_source(outfileptr,map);
   fclose(outfileptr);
   if (map.generate_eps_output) generate_eps_image(map);
   return(0);  /* exit clean from error codes if successful execution */
}
