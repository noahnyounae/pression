CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11


SERIALISER_SOURCES = src/serialiser/Serialiser.cc
DESERIALISER_SOURCES = src/deserialiser/Deserialiser.cc
COMPRESSOR_SOURCES = src/compressor/Compressor.cc

all: serialiser deserialiser compressor

serialiser: $(SERIALISER_SOURCES)
	$(CXX) $(CXXFLAGS) -o serialiser $(SERIALISER_SOURCES)

deserialiser: $(DESERIALISER_SOURCES)
	$(CXX) $(CXXFLAGS) -o deserialiser $(DESERIALISER_SOURCES)

compressor: $(COMPRESSOR_SOURCES)
	$(CXX) $(CXXFLAGS) -o compressor $(COMPRESSOR_SOURCES)

clean:
	rm -f serialiser deserialiser compressor