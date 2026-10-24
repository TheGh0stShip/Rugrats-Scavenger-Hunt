PYTHON ?= python3
ROM ?= baseroms/us/baserom.z64
N64DK_ROOT ?= ../n64-decomp-kit
N64DK_PYTHON ?= $(abspath $(N64DK_ROOT))/.venv/bin/python
N64DK_TOOLCHAIN ?= ../BattleTanx/.toolchain
N64DK := PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) -m n64dk

.PHONY: check report report-check rom-audit deep-rom-audit verify-rom verify-matching survey

check: verify-rom verify-matching report-check

rom-audit: verify-rom
	$(PYTHON) tools/audit_rom.py "$(ROM)" --json analysis/rom_map.json --markdown analysis/rom_map.md --ledger analysis/rom_inventory.tsv

deep-rom-audit: rom-audit report
	$(PYTHON) tools/generate_deep_rom_audit.py

report:
	$(PYTHON) tools/generate_decomp_report.py

report-check:
	$(PYTHON) -m unittest discover -s tests

verify-rom:
	$(PYTHON) tools/verify_rom.py "$(ROM)"

verify-matching: verify-rom
	$(N64DK) match-batch config/us/matching_units.tsv=kmc --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_kmc_normalized.py config/us/matching_kmc_normalized_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_kmc_as_units.tsv=kmc+as --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_kmc_padded.py config/us/matching_kmc_padded_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_sn_units.tsv=sn --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_sn_normalized.py config/us/matching_sn_normalized_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_ido_units.tsv=ido5.3 --project . --symbols analysis/survey/symbol_addrs.txt

survey: verify-rom
	$(N64DK) survey "$(ROM)" --out analysis/survey

