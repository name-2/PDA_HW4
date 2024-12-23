# 編譯器選項
CXX = g++
CXXFLAGS = -std=c++11 -Wall
DEBUGFLAGS = -g -fsanitize=address
RELEASEFLAGS = -O2

# 檔案名稱
TARGET = D2DGRter
SRC = main.cpp
HEADERS = inc/info.h
OBJ = $(SRC:.cpp=.o)

# 編譯目標
all: debug

# 調試模式編譯
debug: CXXFLAGS := $(CXXFLAGS) $(DEBUGFLAGS)
debug: $(TARGET)

# 發佈模式編譯
release: CXXFLAGS := $(CXXFLAGS) $(RELEASEFLAGS)
release: $(TARGET)

# 生成最終可執行檔案
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 生成每個 .o 文件
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的檔案
clean:
	rm -f $(TARGET) $(OBJ)

run:
	./D2DGRter publicCase/publicCase/testcase0/testcase0.gmp publicCase/publicCase/testcase0/testcase0.gcl publicCase/publicCase/testcase0/testcase0.cst publicCase/publicCase/testcase0/testcase0.lg
	./D2DGRter publicCase/publicCase/testcase1/testcase1.gmp publicCase/publicCase/testcase1/testcase1.gcl publicCase/publicCase/testcase1/testcase1.cst publicCase/publicCase/testcase1/testcase1.lg
	./D2DGRter publicCase/publicCase/testcase2/testcase2.gmp publicCase/publicCase/testcase2/testcase2.gcl publicCase/publicCase/testcase2/testcase2.cst publicCase/publicCase/testcase2/testcase2.lg

test:
	./Evaluator /mnt/c/Users/alexe/OneDrive/桌面/Physical_Design_Automation/Lab4/publicCase/publicCase/testcase0 testcase0
	./Evaluator /mnt/c/Users/alexe/OneDrive/桌面/Physical_Design_Automation/Lab4/publicCase/publicCase/testcase1 testcase1
	./Evaluator /mnt/c/Users/alexe/OneDrive/桌面/Physical_Design_Automation/Lab4/publicCase/publicCase/testcase2 testcase2