TARGET=ftrScanAPI_Ex

CFLAGS=-m32 -Wall -O2 -I./       

FTRSCANAPI_DLIB=libScanAPI-intel.so libusb-0.1.so.4.4.4



all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(FTRSCANAPI_DLIB)

clean:
	rm -f $(TARGET)
