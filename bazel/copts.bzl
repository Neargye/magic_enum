COPTS = select({
    "@bazel_tools//tools/cpp:msvc": ["/std:c++17", "/permissive-"],
    "//conditions:default": ["-std=c++17"],
})
