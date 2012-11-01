 cppcheck -j 4 --xml-version=2 --enable=all -ishared/lemon/ -ishared/logog/ -ishared/parserYAML/ -ibuild/ ./ 2>err.log

