# The UltiSoC Project
# Macro definitions


# Used to create a build directory in build rules.
create-dir-rule = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi


# Generate dependency files.
# $(call make-depend, src_file, obj_file, dep_file, flags)
define make-depend
	$(GCC_PREFIX)gcc	\
		-MM		\
		-MF $3		\
		-MP		\
		-MT $2		\
		$4		\
		$1
endef


# Returns 1 if two lists intersects otherwise empty string
# $(call lists-intersect, list1, list2)
lists-intersect = $(sort $(foreach ITM,$1,$(if $(filter $(ITM),$2),1,)))
