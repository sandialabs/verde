CONFIG += warn_on opengl qt verde_EXE
TARGET = verde_exe
TEMPLATE += app

DEFINES += QT_CLEAN_NAMESPACE 'NAME="verde"'
DEPENDPATH += verdict GUI


# use verdict library
DEFINES += USE_VERDICT
INCLUDEPATH += verdict
LIBS += -Lverdict -lverdict100


include(platforms.pro.inc)
include(verde.local)

da_vplatform {
   LIBS += -LGUI -lverde_gui -L$(CUBIT)/lib -lexoIIv2c -lnetcdf
   INCLUDEPATH += $(CUBIT)/include
}

hp_vplatform {
   LIBS += -LGUI -lverde_gui -L$(CUBIT)/lib -lexoIIv2c -lnetcdf -lld
   INCLUDEPATH += $(CUBIT)/include
}

lin_vplatform {
   LIBS += -LGUI -lverde_gui -L$(CUBIT)/lib -lexoIIv2c -lnetcdf
   INCLUDEPATH += $(CUBIT)/include
}

sg_vplatform {
   # directories to these libs are defined in platforms.pro.inc
   LIBS += -LGUI -lverde_gui -lexoIIv2c -lnetcdf
   INCLUDEPATH += $(CUBIT)/exodus/exodus3.07/include
}

ss5_vplatform {
   LIBS += -LGUI -lverde_gui -L$(CUBIT)/lib -lexoIIv2c -lnetcdf -ldl
   INCLUDEPATH += $(CUBIT)/include
}


SOURCES = \
	AllocMemManagers.cpp \
        ArrayBasedContainer.cpp \
        BucketSort.cpp \
        BoundingBox.cpp \
        CpuTimer.cpp \
        EdgeElem.cpp \
        EdgeTool.cpp \
        EdgeMetric.cpp \
        EdgeRef.cpp \
        ElementBlock.cpp \
        ElementBC.cpp \
        ElemRef.cpp \
        ErrorTool.cpp \
        EulerTool.cpp \
        ExodusTool.cpp \
        GetLongOpt.cpp \
        GridSearch.cpp \
        HexMetric.cpp \
        HexRef.cpp \
        KnifeMetric.cpp \
        KnifeRef.cpp \
        MDBClient.cpp \
        MemoryBlock.cpp \
        MemoryManager.cpp \
        Mesh.cpp \
        MeshContainer.cpp \
        Metric.cpp \
        NodeRef.cpp \
        NodeBC.cpp \
        PyramidMetric.cpp \
        PyramidRef.cpp \
        QuadElem.cpp \
        QuadMetric.cpp \
        QuadRef.cpp \
        QualitySummary.cpp \
        SkinTool.cpp \
        TetMetric.cpp \
        TetRef.cpp \
        TriElem.cpp \
        TriMetric.cpp \
        TriRef.cpp \
        VerdeApp.cpp \
        VerdeMatrix.cpp \
        VerdeString.cpp \
        VerdeUtil.cpp \
        VerdeVector.cpp \
        WedgeMetric.cpp \
        WedgeRef.cpp \
	aprepro_init.cpp \
	builtin.cpp \
	UserInterface.cpp \
	VerdeMessage.cpp \
	CommandHandler.cpp \
	idr.cpp \
	idr_parser.cpp

HEADERS = \
	AllocMemManagers.hpp \
        ArrayBasedContainer.hpp \
        BucketSort.hpp \
        BoundingBox.hpp \
        CpuTimer.hpp \
        EdgeElem.hpp \
        EdgeTool.hpp \
        EdgeMetric.hpp \
        EdgeRef.hpp \
        ElementBlock.hpp \
        ElementBC.hpp \
        ElemRef.hpp \
        ErrorTool.hpp \
        EulerTool.hpp \
        ExodusTool.hpp \
        GetLongOpt.hpp \
        GridSearch.hpp \
        HexMetric.hpp \
        HexRef.hpp \
        KnifeMetric.hpp \
        KnifeRef.hpp \
        MDBClient.hpp \
        MemoryBlock.hpp \
        MemoryManager.hpp \
        Mesh.hpp \
	MeshOutput.hpp \
        MeshContainer.hpp \
        Metric.hpp \
        NodeRef.hpp \
        NodeBC.hpp \
        PyramidMetric.hpp \
        PyramidRef.hpp \
        QuadElem.hpp \
        QuadMetric.hpp \
        QuadRef.hpp \
        QualitySummary.hpp \
        SkinTool.hpp \
        TetMetric.hpp \
        TetRef.hpp \
        TriElem.hpp \
        TriMetric.hpp \
        TriRef.hpp \
        VerdeApp.hpp \
        VerdeMatrix.hpp \
        VerdeString.hpp \
        VerdeUtil.hpp \
        VerdeVector.hpp \
        WedgeMetric.hpp \
        WedgeRef.hpp \
        aprepro_init.hpp \
	builtin.h \
	VerdeVersion.h \
	VerdeDefines.hpp \
	UserInterface.hpp \
	VerdeMessage.hpp \
	CommandHandler.hpp \
	idr.hpp \
	allwords.h \
	keywords.h \
	CommandTable.h


