#!/usr/bin/env bash
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

if ! command -v poetry &> /dev/null; then
  echo -e "${RED}❌ Error:${RESET} 'poetry' is not installed or not in PATH."
  echo -e "👉 ${YELLOW}Install it via https://python-poetry.org/docs/#installation${RESET}"
  exit 1
fi

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LUBRICATION_PKG_DIR="$PROJECT_DIR/lubrication"
LUBRICATION_DIST_DIR="$LUBRICATION_PKG_DIR/dist"

echo -e "${CYAN}🔁 Bumping patch version...${RESET}"
cd "$LUBRICATION_PKG_DIR"
poetry version patch

echo -e "${CYAN}📦 Building package in:${RESET} $LUBRICATION_PKG_DIR"
poetry build

LUBRICATION_WHL_FILE=$(ls "$LUBRICATION_DIST_DIR"/*.whl | sort -V | tail -n 1)

echo -e "${CYAN}⬆️  Installing package:${RESET} ${BOLD}$LUBRICATION_WHL_FILE${RESET}"
cd "$PROJECT_DIR"
sudo python3 -m pip install --upgrade "$LUBRICATION_WHL_FILE" --break-system-packages

echo -e "${GREEN}✅ Done.${RESET}"
