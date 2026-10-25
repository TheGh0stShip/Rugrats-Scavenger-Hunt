PYTHON ?= python3
ROM ?= baseroms/us/baserom.z64
N64DK_ROOT ?= tools/n64dk
N64DK_PYTHON ?= $(PYTHON)
N64DK_TOOLCHAIN ?= .toolchain
ARMIPS ?= armips
N64DK := PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) -m n64dk

.PHONY: check report report-check rom-audit deep-rom-audit verify-rom verify-matching verify-executable-inventory verify-executable-data verify-rsp-microcode survey

check: verify-rom verify-matching verify-executable-inventory verify-executable-data report-check

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

verify-executable-inventory: verify-rom
	$(PYTHON) tools/generate_executable_inventory.py "$(ROM)"

verify-executable-data: verify-rom verify-executable-inventory
	$(PYTHON) tools/verify_executable_data.py "$(ROM)"

verify-rsp-microcode: verify-rom
	$(PYTHON) tools/verify_rsp_microcode.py "$(ROM)" --armips "$(ARMIPS)"

verify-matching: verify-rom
	$(N64DK) match-batch config/us/matching_units.tsv=kmc --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_kmc_normalized.py config/us/matching_kmc_normalized_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_kmc_as_units.tsv=kmc+as --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_kmc_padded.py config/us/matching_kmc_padded_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_sn_units.tsv=sn --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_sn_normalized.py config/us/matching_sn_normalized_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	$(N64DK) match-batch config/us/matching_ido_units.tsv=ido5.3 --project . --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_ido_normalized.py config/us/matching_ido_normalized_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt
	PYTHONPATH=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(N64DK_PYTHON) tools/verify_asm_units.py config/us/matching_asm_units.tsv --project . --rom "$(ROM)" --symbols analysis/survey/symbol_addrs.txt

survey: verify-rom
	$(N64DK) survey "$(ROM)" --out analysis/survey


.PHONY: data-roundtrip data-tests
data-roundtrip: verify-rom
	ROM=$(ROM) PROJECT_ROOT=. N64DK_ROOT=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(PYTHON) assets/tools/rebuild_rom.py $(ROM)

data-tests: verify-rom
	ROM=$(ROM) PROJECT_ROOT=. N64DK_ROOT=$(abspath $(N64DK_ROOT)) N64DK_TOOLCHAIN=$(abspath $(N64DK_TOOLCHAIN)) $(PYTHON) -m unittest discover -s assets/tests
