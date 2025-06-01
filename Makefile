PYTHON=python3
VENV_DIR=.venv
PIP=$(VENV_DIR)/bin/pip
PYTEST=$(VENV_DIR)/bin/pytest

CC = gcc
CFLAGS = -fPIC -Wall -Werror
LDFLAGS = -shared

# Vind alle .c files in subfolders van components/src
C_SOURCES := $(shell find components/src -type f -name '*.c')
SO_TARGETS := $(C_SOURCES:.c=.so)

# Regels om .so te bouwen vanuit .c
%.so: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: build-sos
build-sos: $(SO_TARGETS) ## Compileer alle .so bestanden

.PHONY: help
help: ## Toon dit help-overzicht
	@echo ""
	@echo "📦  Beschikbare commando's:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
	@echo ""

.PHONY: venv
venv: ## Maak virtuele omgeving en installeer pytest als die nog niet bestaat
	@if [ ! -d $(VENV_DIR) ]; then \
		$(PYTHON) -m venv $(VENV_DIR); \
	fi
	@$(PIP) install --upgrade pip > /dev/null
	@$(PIP) show pytest > /dev/null 2>&1 || $(PIP) install pytest

.PHONY: test
test: venv $(SO_TARGETS) ## Compileer alle .so bestanden en draai tests
	$(PYTEST) components/tests

.PHONY: install
install: ## Compileer en installeer alle .comp bestanden met comp --install
	@for comp in $(shell find components/src -name '*.comp'); do \
		echo "Compiling $$comp..."; \
		sudo halcompile --install $$comp; \
	done

.PHONY: clean
clean: ## Verwijder alle .so bestanden
	rm -f $(SO_TARGETS)