# Enumerates possible build architectures so that we can restrict targets to
# onily build for certain ones.

environment(
  name = "x86",
	visibility = ["//visibility:public"],
)

environment(
  name = "arm",
	visibility = ["//visibility:public"],
)

environment_group(
	name = "cpus",
	environments = [":x86", ":arm"],
	defaults = [":x86", ":arm"],
)
