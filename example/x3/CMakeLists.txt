IF (MSGPACK_USE_X3_PARSE)
    LIST (APPEND exec_PROGRAMS
        unpack.cpp
        parse.cpp
    )
    IF (MSGPACK_BOOST AND CMAKE_THREAD_LIBS_INIT)
        LIST (APPEND with_boost_PROGRAMS
            stream_unpack.cpp
        )
    ENDIF ()
    FOREACH (source_file ${exec_PROGRAMS})
        GET_FILENAME_COMPONENT (source_file_we ${source_file} NAME_WE)
        ADD_EXECUTABLE (
            ${source_file_we}
            ${source_file}
        )
        TARGET_LINK_LIBRARIES (${source_file_we}
            MsgPack::msgpack-cpp
        )
    ENDFOREACH ()
    FOREACH (source_file ${with_boost_PROGRAMS})
        GET_FILENAME_COMPONENT (source_file_we ${source_file} NAME_WE)
        ADD_EXECUTABLE (
            ${source_file_we}
            ${source_file}
        )
        TARGET_INCLUDE_DIRECTORIES (${source_file_we}
            ${Boost_INCLUDE_DIRS}
        )
        TARGET_LINK_LIBRARIES (${source_file_we}
            MsgPack::msgpack-cpp
            ${Boost_CONTEXT_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${CMAKE_THREAD_LIBS_INIT}
        )
    ENDFOREACH ()
ENDIF ()
