gcov -b ../*.cpp  ../../../src/*.cpp ../../../src/gcanvas/*cpp   -o CMakeFiles/gcanvasTest.dir/
lcov --capture --directory .  --output-file coverage.info
genhtml coverage.info --output-directory covOut