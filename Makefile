# Компилятор
CXX = g++
# Флаги
CXXFLAGS = -Wall -O2 -std=c++11 \
	-DAPP_VERSION=\"1.0\" \
	-DROOT=\"/var/www/images/\" \
	-Iinclude

# Цель
TARGET = cgi-bin/merge

# Объектные файлы
OBJ = src/main.o

# Библиотеки
LIBS = ./lib/Image.a -ljpeg -lpng -lz -lm

# Сборка
$(TARGET): $(OBJ)
	@mkdir -p cgi-bin
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Тестовая сборка с _DEBUG
debug: CXXFLAGS += -D_DEBUG
debug: $(TARGET)
	@echo "Сборка с тестовыми данными (запуск без QUERY_STRING)"

# Очистка
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean debug