# Компилятор
CXX = g++
# Флаги
CXXFLAGS = -Wall -O2 -std=c++11 \
	-DAPP_VERSION=\"1.0\" \
	-DROOT=\"/var/www/images/\" \
	-Iinclude

# Добавляем флаг _DEBUG для отладки
ifdef DEBUG
	CXXFLAGS += -D_DEBUG
endif

# Цель
TARGET = cgi-bin/merge.exe

# Исходные файлы
SOURCES = src/main.cpp \
	$(wildcard src/image/*.cpp) \
	$(wildcard src/number/*.cpp) \
	$(wildcard src/util/*.cpp)

# Объектные файлы (с заменой .cpp на .o)
OBJECTS = $(SOURCES:.cpp=.o)

# Библиотеки
LIBS = ./lib/Image.a -ljpeg -lpng -lz -lm

# Пути к объектным файлам
vpath %.cpp src src/image src/number src/util

# Сборка
$(TARGET): $(OBJECTS)
	@mkdir -p cgi-bin
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Правило для компиляции .cpp в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Отладочная сборка
debug: CXXFLAGS += -D_DEBUG
debug: $(TARGET)
	@echo "Отладочная сборка завершена."

# Очистка
clean:
	del $(OBJECTS) $(TARGET) 2>nul
	rmdir /q /s cgi-bin 2>nul
	@echo "Очистка завершена."

.PHONY: clean debug