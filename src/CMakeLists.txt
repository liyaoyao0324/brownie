add_executable(brownie  kmeroverlaptable.cpp readcorrection.cpp alignment.cpp bubble.cpp coverage.cpp library.cpp kmernode.cpp kmertable.cpp cliptips.cpp dsnode.cpp nucleotide.cpp nodeendstable.cpp settings.cpp util.cpp tstring.cpp kmeroverlap.cpp graph.cpp brownie.cpp solutioncomp.cpp suffix_tree.c)

target_link_libraries(brownie readfile essaMEM pthread)

if (ZLIB_FOUND)
   target_link_libraries(brownie ${ZLIB_LIBRARY})
endif (ZLIB_FOUND)

install(TARGETS brownie RUNTIME DESTINATION bin)
add_subdirectory(readfile)
add_subdirectory(essaMEM-master)
