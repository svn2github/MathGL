AM_CXXFLAGS += -I$(top_srcdir)/include -I$(top_builddir)/include

lib_LTLIBRARIES = libmgl.la

WIDGETS =
if USE_QT
WIDGETS += -DWITH_QT
endif
if USE_WX
WIDGETS += -DWITH_WX
endif
if USE_FLTK
WIDGETS += -DWITH_FLTK
endif
if USE_GLUT
WIDGETS += -DWITH_GLUT
endif

libmgl_la_LIBADD = $(PNG_LIBS) $(GL_LIBS) $(LOPENMP)
libmgl_la_LDFLAGS = -no-undefined -version-info $(MGL_CURRENT):$(MGL_REVISION):$(MGL_AGE)

libmgl_la_CXXFLAGS = $(WIDGETS) -DMGL_FONT_PATH=\"$(datadir)/$(PACKAGE)/fonts/\" \
    -DMOD_LIB_DIR=\"$(libdir)/mgl/\" $(HDF5_FLAGS) $(HDF4_FLAGS) $(GIF_FLAGS) $(JPEG_FLAGS) \
    $(TIFF_FLAGS) $(PNG_FLAGS) $(FLTK_FLAGS) $(ADDON_FLAGS) $(AM_CXXFLAGS) \
    $(PTHREAD_FLAGS)

if USE_PTHREAD
libmgl_la_LIBADD += $(PTHREAD_LIBS)
endif

if USE_GSL
libmgl_la_LIBADD += $(GSL_LIBS)
endif

if USE_HDF4
libmgl_la_LIBADD += $(HDF4_LIBS)
endif

if USE_HDF5
libmgl_la_LIBADD += $(HDF5_LIBS)
endif

if USE_GIF
libmgl_la_LIBADD += $(GIF_LIBS)
endif

if USE_JPEG
libmgl_la_LIBADD += $(JPEG_LIBS)
endif

if USE_U3D
libmgl_la_LIBADD += $(U3D_LIBS)
endif

libmgl_la_SOURCES = \
mgl_data_cf.cpp	   mgl_axis.cpp     mgl_data_png.cpp  mgl_zb2.cpp \
mgl_c.cpp          mgl_eps.cpp      mgl_font.cpp      mgl_zb.cpp \
mgl_combi.cpp      mgl_eval.cpp     mgl_gl.cpp        mgl_def_font.cpp\
mgl_1d.cpp         mgl_cont.cpp     mgl_exec.cpp      mgl_tex_table.cpp\
mgl_2d.cpp         mgl_crust.cpp    mgl_export.cpp    mgl_main.cpp  \
mgl_3d.cpp         mgl_data.cpp     mgl_f.cpp         mgl_idtf.cpp \
mgl_ab.cpp         mgl_data_io.cpp  mgl_flow.cpp      mgl_vect.cpp \
mgl_parse.cpp      mgl_fit.cpp      mgl_pde.cpp       mgl_evalc.cpp \
mgl_addon.cpp      mgl_evalp.cpp
