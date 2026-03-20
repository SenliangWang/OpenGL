CXX      := g++
CXXFLAGS := -Wall -g
LDFLAGS  := -lglut -lGLU -lGL

BUILDDIR := build

.PHONY: all clean

all: $(BUILDDIR)/fixedPipelineStudy $(BUILDDIR)/matrices_and_coloring_polygons

$(BUILDDIR)/fixedPipelineStudy: fixedPipelineStudy/fixedPipelineStudy.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(BUILDDIR)/matrices_and_coloring_polygons: matrices_and_coloring_polygons/main.cpp matrices_and_coloring_polygons/LUtil.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I matrices_and_coloring_polygons -o $@ $^ $(LDFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
