find src -regex '.*\(cpp\|hpp\|cc\|c\|h\)' -exec clang-format -i {} \;
find tests -regex '.*\(cpp\|hpp\|cc\|c\|h\)' -exec clang-format -i {} \;