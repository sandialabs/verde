# BOF:  Makefile
#
# Makefile to convert jpg to eps files for latex
# Assumes that the utility jpeg2ps is in the path
#
# usage: make        {to generate eps files}
#        make clean  {to clean directory}
#

FILES = file_open.eps       \
        nodesets.eps        \
        results_window.eps  \
        blank_screen.eps    \
        metric_ranges.eps   \
        nonmanifold.eps     \
        sidesets.eps        \
        coincident_nodes.eps\
        model_screen.eps    \
        normals.eps         \
        color_dlg.eps       \
        model_window.eps    \
        preferences.eps     \
        command_window.eps  \
	quads_on_hexes.eps  \
	element_metrics.eps \
	edge_only.eps       \
	quad_3_nodes.eps    \
	mouse_buttons.eps


.SUFFIXES: .jpg .eps
.jpg.eps:
	jpeg2ps $< > $@

all: $(FILES)

clean:
	rm -f *.eps
#
# EOF:  Makefile
