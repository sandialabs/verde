// Common string definitions for *Table.h command syntax files
// Note: one identifier per line for making allwords.h via sed
#define TOGGLES \
"off", \
"on", \
"yes", \
"no", \
"true", \
"false"
  
#define RANGE \
"all",\
"and", \
"bottom", \
"by", \
"equal", \
"except",\
"expand", \
"high", \
"low", \
"in",\
"of",\
"or", \
"out",\
"step",\
"through", \
"thru",\
"to", \
"top", \
"valence", \
"with"

#define LOCATION_DIRECTION \
"location", \
"direction", \
"fraction", \
"position", \
"close_to", \
"segment", \
"vector", \
"rotate", \
"swing", \
"revolve", \
"about", \
"move", \
"ax", \
"ay", \
"az", \
"angle", \
"length", \
"distance", \
"tangent", \
"at", \
"from", \
"last", \
"reverse", \
"between", \
"origin", \
"cross", \
"towards"
// Add "on" if TOGGLES not included

#define ABAQUS_SET_PARAMETERS \
"materialfile", \
"abaqus",\
"header",\
"readflag",\
"writeflag",\
"readstartstep",\
"readendstep",\
"readincrement",\
"writefrequency",\
"writeoverlayflag",\
"modifysetflag",\
"solutiontype",\
"elementoutputstring",\
"nodeoutputstring",\
"preprintstring",\
"elementoutputfrequency",\
"nodeoutputfrequency",\
"stepincrement",\
"steploadtype",\
"stepamplitudetype",\
"axissymmetryflag",\
"heattransferincrement",\
"heattransfertimeperiod",\
"heattransfermininumtime",\
"heattransfermaximumtime",\
"heattransfertempchangerate",\
"heattransfermode",\
"materialset"

#define ABAQUS_ANALYSIS_TYPES \
"static",\
"buckle",\
"frequency",\
"dynamic",\
"steadystate",\
"modal",\
"geostatic",\
"ramp",\
"pertubation",\
"heattransfer",\
"nlgeom"

#define BOUNDARY_CONDITION_PARAMETERS \
"bc_pixellength", \
"bc_analysistype", \
"drawvertexforcecomponents", \
"drawactiverestraintset", \
"drawactiveloadset"

