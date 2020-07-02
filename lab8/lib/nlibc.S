.section ".text.entry"

.global _entry
_entry:
	bl main
	bl exit

