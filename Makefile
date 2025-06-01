PYTHON=python3
VENV_DIR=.venv
PIP=$(VENV_DIR)/bin/pip
PYTEST=$(VENV_DIR)/bin/pytest

CC = gcc
CFLAGS = -fPIC -Wall -Werror
LDFLAGS = -shared

LOGIC_SRC = components/src/calculate.c
LOGIC_SO = components/src/calculate.so

COMPONENTS = components/src/calculate.comp components/src/mh400e_spindle.comp

# Check en bouw logic.so
$(LOGIC_SO): $(LOGIC_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: venv
venv:
	@if [ ! -d $(VENV_DIR) ]; then \
		$(PYTHON) -m venv $(VENV_DIR); \
	fi
	@$(PIP) install --upgrade pip > /dev/null
	@$(PIP) show pytest > /dev/null 2>&1 || $(PIP) install pytest

.PHONY: test
test: venv $(LOGIC_SO)
	$(PYTEST) components/tests

.PHONY: install
install:
	@for comp in $(COMPONENTS); do \
		echo "Compiling $$comp..."; \
		comp --install $$comp; \
	done

.PHONY: clean
clean:
	rm -f $(LOGIC_SO)