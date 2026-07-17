# Makefile for QSemOS check_tests
#
# Usage:
#   make           - 编译所有测试
#   make run       - 运行所有测试
#   make task=N    - 编译并运行指定任务
#   make clean     - 清理所有生成的文件

.PHONY: all run clean tasks

# 目录
CHECK_TESTS_DIR := $(shell pwd)
QSemOS_DIR := $(CHECK_TESTS_DIR)
SRC_DIR := $(QSemOS_DIR)/src
INCLUDE_DIR := $(SRC_DIR)/include

# GCC 设置
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra \
	-I$(CHECK_TESTS_DIR)/include \
	-I$(QSemOS_DIR) \
	-I$(SRC_DIR) \
	-I$(INCLUDE_DIR) \
	-I$(SRC_DIR)/core/kernel/include \
	-I$(SRC_DIR)/core/ipc/include \
	-I$(SRC_DIR)/core/extend/include \
	-I$(SRC_DIR)/extended \
	-I$(SRC_DIR)/extended/include \
	-I$(SRC_DIR)/extended/perf \
	-I$(SRC_DIR)/extended/perf/include \
	-I$(SRC_DIR)/extended/perf/pmu \
	-I$(SRC_DIR)/extended/perf/pmu/include \
	-I$(SRC_DIR)/utility \
	-I$(SRC_DIR)/utility/include \
	-I$(SRC_DIR)/utility/lib \
	-I$(SRC_DIR)/utility/lib/include \
	-I$(SRC_DIR)/utility/log \
	-I$(SRC_DIR)/utility/log/include \
	-I$(SRC_DIR)/core/kernel/task \
	-I$(SRC_DIR)/core/kernel/task/smp \
	-I$(SRC_DIR)/core/kernel/sched \
	-I$(SRC_DIR)/core/kernel/sched/smp \
	-I$(SRC_DIR)/core/kernel/spinlock \
	-I$(SRC_DIR)/core/kernel/timer \
	-I$(SRC_DIR)/core/kernel/timer/include \
	-I$(SRC_DIR)/core/kernel/timer/swtmr \
	-I$(SRC_DIR)/core/kernel/timer/swtmr/include \
	-I$(SRC_DIR)/core/kernel/irq \
	-I$(SRC_DIR)/core/kernel/irq/include \
	-I$(SRC_DIR)/core/kernel/sys \
	-I$(SRC_DIR)/core/kernel/sys/include \
	-I$(SRC_DIR)/core/extend/dlmodule \
	-I$(SRC_DIR)/core/extend/dlmodule/include \
	-I$(SRC_DIR)/core/ipc/sem \
	-I$(SRC_DIR)/core/ipc/sem/include \
	-I$(SRC_DIR)/core/ipc/queue \
	-I$(SRC_DIR)/core/ipc/queue/include \
	-I$(SRC_DIR)/core/ipc/rwlock \
	-I$(SRC_DIR)/core/ipc/rwlock/include \
	-I$(SRC_DIR)/core/ipc/signal \
	-I$(SRC_DIR)/core/ipc/signal/include \
	-DOS_OPTION_SMP=0

# 任务列表
TASKS := $(shell ls -d tasks/task_* 2>/dev/null | sort -V)

all: tasks

tasks:
	@for task in $(TASKS); do \
		$(MAKE) -C $$task || exit 1; \
	done

run: tasks
	@echo "======================================"
	@echo "Running all check_tests..."
	@echo "======================================"
	@failed=0; passed=0; total=0; \
	for task in $(TASKS); do \
		task_id=$$(echo $$task | sed 's/task_//'); \
		echo ""; \
		echo ">>> Task $$task_id"; \
		echo "--------------------------------------"; \
		if [ -f $$task/test ]; then \
			./$$task/test; \
			if [ $$? -eq 0 ]; then \
				passed=$$((passed+1)); \
			else \
				failed=$$((failed+1)); \
			fi; \
			total=$$((total+1)); \
		else \
			echo "Binary not found, run 'make' first"; \
			failed=$$((failed+1)); \
			total=$$((total+1)); \
		fi; \
	done; \
	echo ""; \
	echo "======================================"; \
	echo "Final Results:"; \
	echo "  Total : $$total"; \
	echo "  Passed: $$passed"; \
	echo "  Failed: $$failed"; \
	echo "======================================"

# 单独运行某个任务
task:
ifndef TASK_ID
	@echo "Usage: make task TASK_ID=001"
else
	$(MAKE) -C task_$(TASK_ID)
	./task_$(TASK_ID)/test
endif

clean:
	@echo "Cleaning..."
	@for task in $(TASKS); do \
		$(MAKE) -C $$task clean; \
	done
	@echo "Done."
